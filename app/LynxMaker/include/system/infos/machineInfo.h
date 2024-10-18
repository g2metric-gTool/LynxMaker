#pragma once

#include <QObject>
#include <QQmlApplicationEngine>

#include "system/xStdafx.h"
#include "system/xSingleton.h"

class MachineInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY_AUTO(QString,deviceId)
    Q_PROPERTY_AUTO(QString,manufacturer)
    Q_PROPERTY_AUTO(QString,machineHostName)
    Q_PROPERTY_AUTO(QString,cpuArchitecture)
    Q_PROPERTY_AUTO(QString,username)
public:
    explicit MachineInfo(QObject *parent = nullptr);
    //SINGLETON(MachineInfo)
};

