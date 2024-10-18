#include <QDebug>
#include <QQmlContext>
#include <QGuiApplication>

#include "system/infos/machineInfo.h"

MachineInfo::MachineInfo(QObject *parent) : QObject(parent)
{
    deviceId(QSysInfo::machineUniqueId());
    manufacturer(QSysInfo::prettyProductName());
    machineHostName(QSysInfo::machineHostName());
    cpuArchitecture(QSysInfo::currentCpuArchitecture());
    username(QString(qgetenv("USERNAME")));
}
