#pragma once

#include <QObject>

#include "xMaker.h"
#include "gMamba.h"
#include "log/gLog.h"
#include "gPromotion.h"
#include "gMambaDiscovery.h"
#include "gMulticastSocket.h"
#include "gPromotionDiscovery.h"
#include "system/xSingleton.h"


class ConfigureController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList promotionList READ promotionList NOTIFY promotionListChanged)
    Q_PROPERTY(QStringList cameraList READ cameraList NOTIFY cameraListChanged)

public:
    ConfigureController(QObject* parent = nullptr);
    ~ConfigureController();

    QStringList cameraList();
    QStringList promotionList();
    
    Q_INVOKABLE void run(const QString& cameraIpAddress, const QString& lynxSerialNumber, const QString& promotionIpAddress, const double& resetDuration);
    Q_INVOKABLE void discover();
    Q_INVOKABLE QString deviceDetails(QString ipCamera);

signals:
    void cameraListChanged();
    void promotionListChanged();

private:
    xMaker::Maker* _maker;
    mutable gLog::Log _log;

    std::list<MambaInfo> _cameras;
    std::list<PromotionInfo> _promotions;

    QString _promotionUid(QString ip);
};
