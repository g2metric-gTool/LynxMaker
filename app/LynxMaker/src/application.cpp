#include "version.h"
#include "application.h"

// LynxHorizon
#include "xUI.h"

#include "utils/pointer/gPointer.h"

#include <QTranslator>
#include <QApplication>

#ifdef WIN32
#include "system/app_dmp.h"
#endif

gLog::Log _log("Application");

static QString appData = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + APPLICATION_NAME;

static bool limitLogsFromDir(const QString& dirPath, const QStringList& filters, int limit, gLog::Log& log)
{
    if (!filters.isEmpty())
    {
        log << "[LimitingFiles] Type: " << filters.join(" | ").toStdString() << " - Directory: " << dirPath.toStdString() << " - Limit: " << QString::number(limit).toStdString() << " ->";
        QDir dir(dirPath);
        if (dir.exists())
        {
            QFileInfoList infoList(dir.entryInfoList(filters, QDir::NoDotAndDotDot | QDir::Files, QDir::Time));
            if (!infoList.isEmpty())
            {
                if (infoList.size() > limit)
                {
                    bool out(true); int total(infoList.size());
                    for (int i(limit); i < total; ++i)
                        out &= QFile::remove(infoList.at(i).absoluteFilePath());

                    if (out)
                        log << " Success - " << QString::number(total - limit).toStdString() << " removed." << gLog::info;
                    else
                        log << " Failure - Unable to remove some file(s). It might be used by another program." << gLog::info;
                    return out;
                }
                else log << " Number of found files is below the limit." << gLog::info;
            }
            else log << " No files found." << gLog::info;
        }
        else log << " Failure - directory does not exist." << gLog::info;
    }
    return false;
}

static QGuiApplication *createApplication(int &argc, char **argv)
{
    // La mise à l'échelle pour les écrans haute résolution est toujours activée dans Qt 6
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#ifdef WIN32
    ::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=2");
#endif

    QGuiApplication::setApplicationName(APPLICATION_NAME);
	QGuiApplication::setOrganizationName(COMPANY_NAME);
	QGuiApplication::setOrganizationDomain(ORGANIZATION_DOMAINE);
	QGuiApplication::setApplicationVersion(APPLICATION_VERSION);
    
    QApplication *app = new QApplication(argc, argv);
    return app;
}

Application::Application(int &argc, char **argv)
{
    _installLogListener();

    _application        = createApplication(argc, argv);
    _engine             = new QQmlApplicationEngine;

    _appInfo            = new AppInfo;
    _machineInfo        = new MachineInfo;
    _settings           = new ApplicationSettings(appData + ".ini", QSettings::IniFormat);

    _configureController = new ConfigureController;

    _addFonts();
    _registerQmlTypes();
    _installTranslators();

    const QUrl url(QStringLiteral("qrc:/qml/App.qml"));
    _engine->load(url);
    if (_engine->rootObjects().isEmpty())
        _log << "... failed to load application" << gLog::error ;
}

Application::~Application()
{
    _log << "~Application" << gLog::debug;

    _log << "... Clean engine" << gLog::debug; gPointerUtils::clearPointer(_engine);
    _log << "... Clean application" << gLog::debug; gPointerUtils::clearPointer(_application);
    _log << "... Clean configureController" << gLog::debug; gPointerUtils::clearPointer(_configureController);
    _log << "... Clean appInfo" << gLog::debug; gPointerUtils::clearPointer(_appInfo);
    _log << "... Clean machineInfo" << gLog::debug; gPointerUtils::clearPointer(_machineInfo);
    _log << "... Clean settings" << gLog::debug; gPointerUtils::clearPointer(_settings);

    // Retirer les listeners avant la destruction de l'objet _coutLog and _g2mLog
    _log << "... Remove cout log listeners" << gLog::debug; gLog::LogManager::instance().removeLogListener(*_coutLog); 
    _log << "... Remove g2m log listeners" << gLog::debug; gLog::LogManager::instance().removeLogListener(*_g2mLog);
}

int Application::run()
{
    return _application->exec();
}

ApplicationSettings* Application::settings() const
{
    return _settings;
}

QQmlApplicationEngine *Application::qmlEngine() const
{
    return _engine;
}

void Application::_registerQmlTypes()
{
    FluentUI::getInstance()->registerTypes();

    _engine->rootContext()->setContextProperty("AppInfo", _appInfo);
    _engine->rootContext()->setContextProperty("MachineInfo", _machineInfo);
    _engine->rootContext()->setContextProperty("SettingsHelper", _settings);

    _engine->rootContext()->setContextProperty("ConfigureController", _configureController);
}

void Application::_addFonts()
{
    /*
        // Il est particulièrement important de s'assurer que toutes les polices que nous utilisons
        // sont disponibles, sinon Qt devra rechercher des "alias de famille de polices",
        // ce qui peut prendre 1 seconde (observable avec qt.qpa.fonts.warning = true).
        // Pour ajouter un ou plusieurs fonts, il suffit d'utiliser QFontDatabase::addApplicationFont(fontPath)
    */
}

void Application::_installTranslators()
{
    // Install translators for the current language.
    const QString locale(_settings->language());

    QTranslator* translator = new QTranslator(_application);
    qApp->installTranslator(translator);
    QString translatorPath = QGuiApplication::applicationDirPath() + "/translations";
    QString filename = QString::fromStdString("%1/%2_%3.qm").arg(translatorPath, QCoreApplication::applicationName(), locale);
    if (translator->load(filename)) _engine->retranslate();
    else _log << "Failed to load translate file '" << filename.toStdString() << "'" << gLog::error;
}

void Application::_installLogListener()
{
    QString logsDirPath = appData + "/logs";

    // Crée le dossier logs s'il n'existe pas
    QDir logsDir(logsDirPath);
    if (!logsDir.exists()) {
        if (!logsDir.mkpath(".")) {
            _log << "Unable to create the logs directory at " << logsDirPath.toStdString() << gLog::warning; return;
        }
    }

    // Crée la chaîne avec la date et l'heure actuelle
    QString dateTimeStr = QDateTime::currentDateTime().toString("yyyy-MM-d_hh-mm-ss");
    QString logFilePath = logsDirPath + "/log_" + dateTimeStr + ".g2mLog";

    // Initialisation de CoutLog et G2mLogListener avant la création de l'application
    _coutLog = std::make_unique<gLog::listener::CoutLog>();
    _g2mLog = std::make_unique<gLog::listener::G2mLogListener>(logFilePath.toStdString());
    gLog::LogManager::instance().addLogListener(*_coutLog);
    gLog::LogManager::instance().addLogListener(*_g2mLog);

    limitLogsFromDir(logsDirPath, QStringList("*.g2mlog"), 10, _log);

}
