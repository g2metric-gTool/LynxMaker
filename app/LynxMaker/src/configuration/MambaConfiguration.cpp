#include <chrono>
#include <thread>
#include <iostream>

#include <qDebug>

#include <tinyxml2.h>

#include "configuration/mambaConfiguration.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

// TODO recuperer le log lors de l'exécution de la fonction scp pour voir s'il y a une erreur par exempledf de type No space left on device

MambaConfiguration::MambaConfiguration(const std::string& hostname,
                                       const std::string& username, 
                                       const std::string& password, 
                                       int port)
    : _hostname(hostname)
    , _username(username)
    , _password(password)
    , _port(port)
    , _session(nullptr) 
    , _configFilePath("./onvif_config.xml")
{
    _establishSSHSession();
    _authenticateWithPassword();
}

MambaConfiguration::~MambaConfiguration()
{
    //_shutdown();
}


bool MambaConfiguration::configureDevice(const std::string& lynxSerialNumber, const std::string& promotionMacAddress)
{
    return _run(lynxSerialNumber, promotionMacAddress);
}


void MambaConfiguration::_shutdown()
{
    if (_session) {
        libssh2_session_disconnect(_session, "Normal Shutdown");
        libssh2_session_free(_session);
    }

    if (_sock != LIBSSH2_INVALID_SOCKET) {
        shutdown(_sock, 2);
        closesocket(_sock);
    }

    fprintf(stderr, "All done\n");
    libssh2_exit(); // TODO vérifier si je peux faire libssh2_exit()
}

bool MambaConfiguration::_establishSSHSession() {
    int rc;
    WSADATA wsadata;
    LIBSSH2_CHANNEL* channel;

    rc = WSAStartup(MAKEWORD(2, 0), &wsadata);
    if (rc) {
        qCritical("[EstablishSSHSession] WSAStartup failed with error: %d", rc);
        return false;
    }

    // Initialiser la bibliothèque libssh2
    rc = libssh2_init(0);
    if (rc != 0) {
        qCritical("[EstablishSSHSession] error during the initialization of libssh2");
        return false;
    }

    // Créer une session SSH
    // Désactiver la vérification de l'hôte
    _session = libssh2_session_init();
    if (!_session) {
        qCritical("[EstablishSSHSession] Unable to create an SSH session");
        return false;
    }
    return true;
}

bool MambaConfiguration::_authenticateWithPassword() {
    int rc;
    // Établir une connexion non-bloquante
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sin{};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(_port);
    sin.sin_addr.s_addr = inet_addr(_hostname.c_str());

    if (connect(_sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) {
        qCritical("[AuthenticateWithPassword] failed to connect to the SSH server");
        return false;
    }

    // Configurer la session SSH
    rc = libssh2_session_handshake(_session, _sock);
    if (rc) {
        qCritical("[AuthenticateWithPassword] failed to establish the SSH session");
        return false;
    }

    // Authentifier avec le mot de passe
    rc = libssh2_userauth_password(_session, _username.c_str(), _password.c_str());
    if (rc) {
        qCritical("[AuthenticateWithPassword] authentication failed");
        return false;
    }
    else {
        qDebug("[AuthenticateWithPassword] session successfully connected");
        return true;
    }
}

bool MambaConfiguration::_backupOnvifConfig() {
    const char* scp_command;
    char check_file_result[2] = { 0 };
    LIBSSH2_CHANNEL* channel;
    const char* check_file_command = "test -e /etc/onvif/onvif_config.recovery && echo \"1\" || echo \"0\"";
    channel = libssh2_channel_open_session(_session);
    if (!channel) {
        qCritical("[BackupOnvifConfig] unable to open a session");
        _shutdown();
        return false;
    }

    if (libssh2_channel_exec(channel, check_file_command)) {
        qCritical("[BackupOnvifConfig] unable to execute file existence check command on channel");
        _shutdown();
        return false;
    }

    // Attendre la fin de la commande
    ssize_t nbytes;
    do {
        nbytes = libssh2_channel_read(channel, check_file_result, sizeof(check_file_result) - 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Attendre 50 ms
    } while (nbytes == LIBSSH2_ERROR_EAGAIN);

    // Si le résultat est "1", le fichier existe déjà
    if (nbytes > 0 && check_file_result[0] == '1') {
        qDebug("[BackupOnvifConfig] recovery file already exists");
    }
    else {
        qWarning("[BackupOnvifConfig] recovery file does not exist. Executing the SCP command to copy it.");

        // Fermer le canal précédent
        libssh2_channel_free(channel);

        // Créer un nouveau canal
        channel = libssh2_channel_open_session(_session);
        if (!channel) {
            qCritical("[BackupOnvifConfig] unable to open a session");
            _shutdown();
            return false;
        }

        // Exécuter la commande scp pour copier le fichier
        scp_command = "scp /etc/onvif/onvif_config.xml /etc/onvif/onvif_config.recovery";
        if (libssh2_channel_exec(channel, scp_command)) {
            qCritical("[BackupOnvifConfig] unable to execute scp command on channel: %s", scp_command);
            _shutdown();
            return false;
        }
    }

    return true;
}

bool MambaConfiguration::_updateOnvifConfig(const std::string& lynxSerialNumber, const std::string& promotionMacAddress)
{
    // Construction de la commande
    std::string command = "pscp -scp -pw " + _password + " " + _username + "@" + _hostname + ":/etc/onvif/onvif_config.recovery " + _configFilePath;
    if (!_executeCommandSystem(command)) return false;

    // Charger le fichier XML existant
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(_configFilePath.c_str()) != tinyxml2::XML_SUCCESS) {
        qCritical("[UpdateOnvifConfig] error loading the XML file");
        return false;
    }

    // Trouver l'élément <scope> après lequel nous voulons ajouter les nouveaux scopes (SN, MAC)
    tinyxml2::XMLElement* currentScope = doc.FirstChildElement("config")->FirstChildElement("scope");
    while (currentScope) {
        const char* scopeText = currentScope->GetText();
        if (scopeText && std::string(scopeText) == "onvif://www.onvif.org/hardware/MambaA") {
            break; // On a trouvé l'élément <scope> souhaité
        }
        currentScope = currentScope->NextSiblingElement("scope");
    }

    if (!currentScope) {
        qCritical("[UpdateOnvifConfig] unable to find the specified <scope> element");
        return false;
    }

    // Créer et ajouter les nouveaux scopes en utilisant les arguments lynxSerialNumber et promotionMacAddress
    tinyxml2::XMLElement* newScope1 = doc.NewElement("scope");
    newScope1->SetText(("onvif://www.onvif.org/Lynx/" + lynxSerialNumber).c_str());
    currentScope->Parent()->InsertAfterChild(currentScope, newScope1);

    tinyxml2::XMLElement* newScope2 = doc.NewElement("scope");
    newScope2->SetText(("onvif://www.onvif.org/Promotion/" + promotionMacAddress).c_str());
    currentScope->Parent()->InsertAfterChild(newScope1, newScope2);

    // Enregistrer les modifications dans le fichier
    if (doc.SaveFile(_configFilePath.c_str()) != tinyxml2::XML_SUCCESS) {
        qCritical("[UpdateOnvifConfig] error while saving changes to the XML file");
        return false;
    }

    qDebug("[UpdateOnvifConfig] scopes added successfully");
    
    // Construction de la commande
    command = "pscp -scp -pw " + _password + " " + _configFilePath + " " + _username + "@" + _hostname + ":/etc/onvif/onvif_config.xml";
    if (!_executeCommandSystem(command)) return false;

    return true;
}

bool MambaConfiguration::_restartOnvifServer()
{
    // Créer un nouveau canal
    LIBSSH2_CHANNEL* channel;
    channel = libssh2_channel_open_session(_session);
    if (!channel) {
        qCritical("[RestartOnvifServer] unable to open a session");
        _shutdown();
        return false;
    }

    if (libssh2_channel_exec(channel, "systemctl restart twiga-onvifserver")) {
        qCritical("[RestartOnvifServer] unable to request command on channel");
        _shutdown();
        return false;
    }

    // Read and print the output of the command
    char buffer[4096];
    ssize_t nbytes;

    do {
        nbytes = libssh2_channel_read(channel, buffer, sizeof(buffer));
        if (nbytes > 0) {
            // Assuming the output is text, print it to the console
            fwrite(buffer, 1, nbytes, stdout);
        }
    } while (nbytes > 0);

	return true;
}

bool MambaConfiguration::_run(const std::string& serialNumber, const std::string& promotionMacAddress)
{
    if (!_backupOnvifConfig()) return false;

    if (!_updateOnvifConfig(serialNumber, promotionMacAddress)) return false;

    // Suppression du fichier temporaire onvif config
    if (std::remove(_configFilePath.c_str()) != 0) {
        // La suppression a échoué
        qDebug() << "[UpdateOnvifConfig] error deleting file:" << QString::fromStdString(_configFilePath);
    }
    else {
        // La suppression a réussi
        qDebug() << "[UpdateOnvifConfig] file deleted successfully:" << QString::fromStdString(_configFilePath);
    }

    if (!_restartOnvifServer()) return false;

    _shutdown();

    return true;
}


bool MambaConfiguration::_executeCommandSystem(const std::string& command)
{
    // Exécution de la commande
    int result = system(command.c_str());
    // Vérification du résultat
    if (result == 0) {
        qDebug("[ExecuteCommandSystem] the command '%s' executed successfully.", command.c_str());
        return true;
    }
    else
    {
        // La commande a échoué
        qCritical("[ExecuteCommandSystem] failed to execute the command '%s'", command.c_str());
        return false;
    }
}