#include <regex>
#include <random>
#include <iomanip>
#include <iostream>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "ui/xDiscover.h"
#include "utils/pointer/gPointer.h"
#include "ui/xConfigureController.h"

static int S_PORT = 22;
static std::string S_USERNAME = "root";
static std::string S_password = "Twig@31!";

using LynxInfo = gCore::Parameters;

ConfigureController::ConfigureController(QObject* parent) 
	: QObject(parent)
	, _log("ConfigureController")
	, _cameras(gMamba::Connection::discover())
	, _promotions(gPromotion::Connection::discover())
	, _maker(nullptr)
{
}

ConfigureController::~ConfigureController()
{
	gPointerUtils::clearPointer(_maker);
}

QStringList ConfigureController::promotionList()
{
	QStringList promotionList;
	for (const auto& promotion : _promotions) {
		promotionList.append(promotion.ftpIp.c_str());
	}
	return promotionList;
}

QStringList ConfigureController::cameraList()
{
	QStringList cameraList;
	for (const auto& camera : _cameras) {
		cameraList.append(camera.ip.c_str());
	}
	return cameraList;
}

void ConfigureController::discover()
{
	std::list<MambaInfo> cameras = gMamba::Connection::discover();
	//if (_cameras != cameras) {
		_cameras = cameras;
		emit cameraListChanged();
	//}

	std::list<PromotionInfo> promotions = gPromotion::Connection::discover();
	//if (_promotions != promotions) {
		_promotions = promotions;
		emit promotionListChanged();
	//}
}

void ConfigureController::run(const QString& cameraIpAddress, const QString& lynxSerialNumber, const QString& promotionIpAddress, const double& resetDuration)
{
	_log << "Configure: " << cameraIpAddress.toStdString() << " | " << promotionIpAddress.toStdString() << " | " << resetDuration << "(s) | " << lynxSerialNumber.toStdString() << gLog::info;
	try {
		_maker = new xMaker::Maker(cameraIpAddress.toStdString(), S_USERNAME, S_password, S_PORT);
		std::string promotionMacAddress = _promotionUid(promotionIpAddress).toStdString();
		if (promotionMacAddress.empty()) {
			_log << "Error: Unable to find MAC address for promotion with IP address '" << promotionIpAddress.toStdString() << "'" << gLog::error;
			return;
		}
		if (_maker->configureDevice(lynxSerialNumber.toStdString(), promotionMacAddress, 40.5))
			_log << "Configuration successful for device: " << lynxSerialNumber.toStdString() << gLog::info;
		else
			_log << "Failed to configure device: " << lynxSerialNumber.toStdString() << gLog::error;
	}
	catch (const g2::core::Exception& e) {
		_log << "Configuration failed: Exception occurred: " << e.comment() << gLog::error;
	}
	catch (const std::exception& e) {
		_log << "Configuration failed: Standard exception occurred: " << e.what() << gLog::error;
	}
	catch (...) {
		_log << "Configuration failed: Unknown exception occurred." << gLog::error;
	}
}

QString ConfigureController::deviceDetails(QString ipCamera)
{
	if (ipCamera.isEmpty()) return "";


	// Découverte des LynxInfo
	std::list<LynxInfo> lynxList = Connection::discover();

	// Variables pour stocker les informations filtrées
	QString serialNumber;
	QJsonObject cameraObject;
	QJsonArray promotionsArray;

	for (const LynxInfo& lynx : lynxList) {
		// Récupération des caméras

		MambaInfo mambaInfos = lynx.value<MambaInfo>("mambaInfos");

		if (mambaInfos.ip == ipCamera.toStdString()) {
			// Si l'IP correspond, on récupère les informations de la caméra
			cameraObject["name"] = QString::fromStdString(mambaInfos.name);
			cameraObject["ip"] = QString::fromStdString(mambaInfos.ip);
			cameraObject["port"] = static_cast<int>(mambaInfos.port);
			cameraObject["uid"] = QString::fromStdString(mambaInfos.macAddress);

			// Récupération du serialNumber
			serialNumber = QString::fromStdString(lynx.value<std::string>("serialNumber", ""));

			// Récupération des promotions
			PromotionInfo promotionInfos = lynx.value<PromotionInfo>("promotionInfos");
			QJsonObject promotionObject;
			promotionObject["name"] = QString::fromStdString(promotionInfos.name);
			promotionObject["hostIp"] = QString::fromStdString(promotionInfos.hostIp);
			promotionObject["ftpIp"] = QString::fromStdString(promotionInfos.ftpIp);
			promotionObject["uid"] = QString::fromStdString(promotionInfos.uid);
			promotionObject["startChannel"] = static_cast<int>(promotionInfos.startChannel);
			promotionObject["universe"] = static_cast<int>(promotionInfos.universe);

			promotionsArray.append(promotionObject);

			// On sort de la boucle dès qu'on a trouvé la caméra
			break;
		}
	}

	// Ajouter les informations au résultat JSON
	QJsonObject jsonResult;
	jsonResult["camera"] = cameraObject;
	jsonResult["promotion"] = promotionsArray;
	jsonResult["serialNumber"] = serialNumber;

	// Convertir en JSON formaté
	QJsonDocument jsonDoc(jsonResult);
	return QString(jsonDoc.toJson(QJsonDocument::Indented));
}

QString ConfigureController::_promotionUid(QString ip)
{
	for (const auto& promotion : _promotions) {
		if (promotion.ftpIp == ip.toStdString())
			return promotion.uid.c_str();
	}
	return "";
}