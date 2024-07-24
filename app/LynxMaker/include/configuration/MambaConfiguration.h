#pragma once

#include <string>
#include <libssh2.h>

class MambaConfiguration {
public:
    MambaConfiguration(const std::string& hostname, const std::string& username, const std::string& password, int port);
    ~MambaConfiguration();

    bool configureDevice(const std::string& LynxSerialNumber, const std::string& promotionMacAddress);

    void setHostname(const std::string& hostname) { _hostname = hostname; }
    void setUsername(const std::string& username) { _username = username; }
    void setPassword(const std::string& password) { _password = password; }
    void setConfigFilePath(const std::string& configFilePath) { _configFilePath = configFilePath; }

    std::string hostname() const { return _hostname; }
    std::string username() const { return _username; }
    std::string password() const { return _password; }
    std::string configFilePath() const { return _configFilePath; }

private:
    int _port;
    std::string _hostname;
    std::string _username;
    std::string _password;
    libssh2_socket_t _sock;
    LIBSSH2_SESSION* _session;

    std::string _configFilePath;

    void _shutdown();
    bool _backupOnvifConfig();
    bool _restartOnvifServer();
    bool _establishSSHSession();
    bool _authenticateWithPassword();
    bool _executeCommandSystem(const std::string& command);
    bool _updateOnvifConfig(const std::string& lynxSerialNumber, const std::string& promotionMacAddress);
    bool _run(const std::string& lynxSerialNumber, const std::string& promotionMacAddress);
};