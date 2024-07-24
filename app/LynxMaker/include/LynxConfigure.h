#ifndef LYNXCONFIGURE_H
#define LYNXCONFIGURE_H

#include <QObject>
#include "system/xSingleton.h"

#include "gMamba.h"
#include "log/gLog.h"
#include "gPromotion.h"
#include "gMulticastSocket.h"
#include "gMambaDiscovery.h"
#include "gPromotionDiscovery.h"


class LynxConfigure : public QObject
{
    Q_OBJECT
    SINGLETON(LynxConfigure)

    Q_PROPERTY(QStringList promotionList READ promotionList NOTIFY promotionListChanged)
    Q_PROPERTY(QStringList cameraList READ cameraList NOTIFY cameraListChanged)

public:
    LynxConfigure(QObject* parent = nullptr);

    QStringList promotionList();
    QStringList cameraList();

    Q_INVOKABLE void updateList();
    Q_INVOKABLE void configure(QString ipCamera, QString ipPromotion, QString serielNumber);
    Q_INVOKABLE QString configInfo(QString ipCamera);
    Q_INVOKABLE QString promotionUid(QString ip);

private:
    std::string _socketRequest();
    std::string _regexSearch(const std::string& reponse, const std::string& pattern);
    gLog::Log _log;

signals:
    void promotionListChanged();
    void cameraListChanged();
};

#endif // LYNXCONFIGURE
