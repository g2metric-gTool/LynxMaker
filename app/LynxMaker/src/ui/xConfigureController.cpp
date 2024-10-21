#include <regex>
#include <random>
#include <iomanip>
#include <iostream>

#include <QFuture>
#include <QJsonArray>
#include <QJsonObject>
#include <QtConcurrent>
#include <QJsonDocument>
#include <QFutureWatcher>

#include "ui/xDiscover.h"
#include "utils/gUtils.h"
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
	gUtils::safeDelete(_maker);
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
	if (_cameras != cameras) {
		_cameras = cameras;
		emit cameraListChanged();
	}

	std::list<PromotionInfo> promotions = gPromotion::Connection::discover();
	if (_promotions != promotions) {
		_promotions = promotions;
		emit promotionListChanged();
	}
}

void ConfigureController::run(const QString& cameraIpAddress, const QString& lynxSerialNumber, const QString& promotionIpAddress, const double& resetDuration)
{
	// Cr�ez un QFuture pour ex�cuter la t�che en arri�re-plan
	auto future = QtConcurrent::run([this, cameraIpAddress, lynxSerialNumber, promotionIpAddress, resetDuration]() -> bool {
		try {
			_maker = new xMaker::Maker(cameraIpAddress.toStdString(), S_USERNAME, S_password, S_PORT);
			std::string promotionMacAddress = _promotionUid(promotionIpAddress).toStdString();
			if (promotionMacAddress.empty()) {
				handleErrorMessage("Unable to find MAC address");
				_log << "Error: Unable to find MAC address for promotion with IP address '" << promotionIpAddress.toStdString() << "'" << gLog::error;
				return false;
			}

			if (std::isnan(resetDuration)) {
				handleErrorMessage("Reset duration is not valid");
				_log << "Reset duration is not valid" << gLog::info;
				return false;
			}
			if (_maker->configureDevice(lynxSerialNumber.toStdString(), promotionMacAddress, resetDuration)) {
				_log << "Configuration successful for device: " << lynxSerialNumber.toStdString() << gLog::info;
				return true;
			}
			else {
				handleErrorMessage("Failed to configure device");
				_log << "Failed to configure device: " << lynxSerialNumber.toStdString() << gLog::error;
				return false;
			}
		}
		catch (const g2::core::Exception& e) {
			handleErrorMessage("Configuration failed: Exception occurred");
			_log << "Configuration failed: Exception occurred: " << e.comment() << gLog::error;
			return false;
		}
		catch (const std::exception& e) {
			handleErrorMessage("Configuration failed: Standard exception occurred");
			_log << "Configuration failed: Standard exception occurred: " << e.what() << gLog::error;
			return false;
		}
		catch (...) {
			handleErrorMessage("Configuration failed: Unknown exception occurred");
			_log << "Configuration failed: Unknown exception occurred." << gLog::error;
			return false;
		}
		});

	// Cr�ez un QFutureWatcher pour surveiller l'�tat de l'op�ration
	QFutureWatcher<bool>* watcher = new QFutureWatcher<bool>(this);
	connect(watcher, &QFutureWatcher<bool>::finished, this, &ConfigureController::onRunFinished);
	watcher->setFuture(future);
}

void ConfigureController::onRunFinished() {
	QFutureWatcher<bool>* watcher = static_cast<QFutureWatcher<bool>*>(sender());
	if (watcher) {
		// V�rifiez le r�sultat de l'op�ration
		bool success = watcher->result();
		if (success) {
			emit executionSuccessful();
		}
		else {
			emit errorOccurred("Configuration failed");
		}
		watcher->deleteLater(); // Nettoyez le watcher
	}
}

QString ConfigureController::deviceDetails(QString ipCamera)
{
	if (ipCamera.isEmpty()) return "";

	// D�couverte des LynxInfo
	std::list<LynxInfo> lynxList = Connection::discover();

	// Variables pour stocker les informations filtr�es
	QString resetDuration;
	QString serialNumber;
	QJsonObject cameraObject;
	QJsonArray promotionsArray;

	for (const LynxInfo& lynx : lynxList) {
		// R�cup�ration des cam�ras
		MambaInfo mambaInfos = lynx.value<MambaInfo>("mambaInfos");

		if (mambaInfos.ip == ipCamera.toStdString()) {
			// Si l'IP correspond, on r�cup�re les informations de la cam�ra
			cameraObject["name"] = QString::fromStdString(mambaInfos.name);
			cameraObject["ip"] = QString::fromStdString(mambaInfos.ip);
			cameraObject["port"] = static_cast<int>(mambaInfos.port);
			cameraObject["uid"] = QString::fromStdString(mambaInfos.macAddress);

			resetDuration = QString::number(lynx.value<double>("resetDuration", 0.0));
			// R�cup�ration du serialNumber
			serialNumber = QString::fromStdString(lynx.value<std::string>("serialNumber", ""));

			// R�cup�ration des promotions
			PromotionInfo promotionInfos = lynx.value<PromotionInfo>("promotionInfos");
			QJsonObject promotionObject;
			promotionObject["name"] = QString::fromStdString(promotionInfos.name);
			promotionObject["hostIp"] = QString::fromStdString(promotionInfos.hostIp);
			promotionObject["ftpIp"] = QString::fromStdString(promotionInfos.ftpIp);
			promotionObject["uid"] = QString::fromStdString(promotionInfos.uid);
			promotionObject["startChannel"] = static_cast<int>(promotionInfos.startChannel);
			promotionObject["universe"] = static_cast<int>(promotionInfos.universe);

			promotionsArray.append(promotionObject);

			// On sort de la boucle d�s qu'on a trouv� la cam�ra
			break;
		}
	}

	// Ajouter les informations au r�sultat JSON
	QJsonObject jsonResult;
	jsonResult["camera"]		= cameraObject;
	jsonResult["promotion"]		= promotionsArray;
	jsonResult["resetDuration"]	= resetDuration;
	jsonResult["serialNumber"]	= serialNumber;

	// Convertir en JSON format�
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

void ConfigureController::handleErrorMessage(const QString& message) {
	emit errorOccurred(message);
}