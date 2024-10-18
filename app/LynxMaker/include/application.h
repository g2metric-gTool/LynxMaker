#pragma once

#include <QtQml>
#include <QGuiApplication>

#include "settings.h"
#include "system/infos/appInfo.h"
#include "ui/xConfigureController.h"
#include "system/infos/machineInfo.h"

// gLib
#include "log/gLogManager.h"
#include "log/listener/gStdStreamLog.h"
#include "log/listener/gG2mLogListener.h"

class Application
{
public:
    Application(int &argc, char **argv);
    ~Application();

    int run();

    ApplicationSettings* settings() const;
    QQmlApplicationEngine *qmlEngine() const;
private:
    void _addFonts();
    void _registerQmlTypes();
    void _installTranslators();
    void _installLogListener();
    
    QGuiApplication* _application;
    QQmlApplicationEngine* _engine;
    
    AppInfo* _appInfo;
    MachineInfo* _machineInfo;
    ApplicationSettings* _settings;

    ConfigureController* _configureController;

    std::unique_ptr<gLog::listener::CoutLog> _coutLog;
    std::unique_ptr<gLog::listener::G2mLogListener> _g2mLog;
};
