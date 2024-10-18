#pragma once

#include <QObject>
#include <QQmlApplicationEngine>

#include "system/xStdafx.h"
#include "system/xSingleton.h"

class AppInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY_AUTO(QString,version)
    Q_PROPERTY_AUTO(QString,commitHash)
    Q_PROPERTY_AUTO(QString,appFullVersion)
    Q_PROPERTY_AUTO(QString,commitAuthor)
    Q_PROPERTY_AUTO(QString,commitDate)
    Q_PROPERTY_AUTO(QString,gitBranch)
    Q_PROPERTY_AUTO(QString,compilationAuthor)
    Q_PROPERTY_AUTO(QString,compilationDate)
    Q_PROPERTY_AUTO(QString,complierName)
public:
    explicit AppInfo(QObject *parent = nullptr);
    //SINGLETON(AppInfo)
};

