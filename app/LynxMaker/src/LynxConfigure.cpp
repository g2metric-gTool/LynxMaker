#include <regex>
#include <random>
#include <iomanip>
#include <iostream>

#include "LynxConfigure.h"
#include <QDebug>

#include "configuration/MambaConfiguration.h"

LynxConfigure::LynxConfigure(QObject* parent) : QObject(parent), _log("Discover")
{

}

QStringList LynxConfigure::promotionList()
{
	gPromotion::DeviceInfoList promotionInfoList = gPromotion::PromotionDiscovery::discover();
	QStringList promotion;

	for (const gPromotion::DeviceInfo& promotionInfo : promotionInfoList) 
	{
		promotion.append(QString::fromStdString(promotionInfo.value<std::string>("ftpIp")));
	}
	return promotion;
}

QStringList LynxConfigure::cameraList()
{
	gMamba::DeviceInfoList cameraInfoList = gMamba::CameraDiscovery::discover();
	QStringList camera;
	for (const gPromotion::DeviceInfo& cameraInfo : cameraInfoList)
	{
		camera.append(QString::fromStdString(cameraInfo.value<std::string>("ip")));
	}
	return camera;
}

void LynxConfigure::updateList()
{
	cameraList();
	promotionList();
	emit cameraListChanged();
	emit promotionListChanged();
}

void LynxConfigure::configure(QString ipCamera, QString ipPromotion, QString serialNumber)
{
	_log << "CONFIGURE: " << ipCamera.toStdString() << " | " << ipPromotion.toStdString() << " | " << serialNumber.toStdString() << " | " << gLog::info;
	if(promotionUid(ipPromotion) == "") return;

	int port = 22;
	const std::string username = "root";
	const std::string password = "Twig@31!";
	MambaConfiguration mambaConfiguration = MambaConfiguration(ipCamera.toStdString(), username, password, port);

	mambaConfiguration.configureDevice(serialNumber.toStdString(), promotionUid(ipPromotion).toStdString());
	_log << "validate" << gLog::info;
}

QString LynxConfigure::configInfo(QString ipCamera)
{
	int multicastPort = 3702;
	std::string multicastAddr = "239.255.255.250";
	gMulticastSocket::MulticastSocket socket(multicastAddr, multicastPort, 150);

	std::string request = _socketRequest();
	if (request.empty()) return "";

	if (socket.write(request)) {
		auto responses = socket.readAll();
		if (!responses.empty()) {
			for (const auto& response : responses) try {

				gMamba::DeviceInfo cameraInfo;
				std::string ip = _regexSearch(response, R"(<d:XAddrs>http://([\d\.]+):)");
				if (ip.empty()) continue;
				
				if(ipCamera.toStdString() == ip);
				{
					std::string name = _regexSearch(response, R"(onvif://www\.onvif\.org/name/([^\s]+))");
					if (name.empty()) continue;
					std::string serialNumber = _regexSearch(response, R"(onvif://www.onvif.org/Lynx/(\w+))");
					std::string promotionUid = _regexSearch(response, R"(onvif://www.onvif.org/Promotion/([a-fA-F0-9:]+))");

					return QString::fromStdString("Camera Name: " + name + "\n Lynx Serial Number: " + serialNumber + "\n Promotion: " + promotionUid);
				}
			}
			catch (gCore::Exception& e) { _log << e.what() << e.comment(); }
		}
	}
	return "";
}

QString LynxConfigure::promotionUid(QString ip)
{
	gPromotion::DeviceInfoList promotionInfoList = gPromotion::PromotionDiscovery::discover();
	for (const gPromotion::DeviceInfo& promotionInfo : promotionInfoList)
	{
		if (ip == QString::fromStdString(promotionInfo.value<std::string>("ftpIp")))
			return QString::fromStdString(promotionInfo.value<std::string>("uid"));
	}
	return "";
}

std::string LynxConfigure::_socketRequest()
{
	// Génère un identifiant de message unique au format UUID (Universally Unique Identifier).
	std::string messageId;
	{
		std::ostringstream oss;
		oss << "urn:uuid:";
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 255);
		for (int i = 0; i < 16; i++) {
			oss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
			if (i == 3 || i == 5 || i == 7 || i == 9)
				oss << "-";
		}
		messageId = oss.str();
	}

	std::string request = R"(<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://www.w3.org/2003/05/soap-envelope" xmlns:a="http://schemas.xmlsoap.org/ws/2004/08/addressing">
					<SOAP-ENV:Header>
						<a:Action SOAP-ENV:mustUnderstand="1">http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</a:Action>
						<a:MessageID>)" + messageId + R"(</a:MessageID>
						<a:ReplyTo>
							<a:Address>http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous</a:Address>
						</a:ReplyTo>
						<a:To SOAP-ENV:mustUnderstand="1">urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To>
					</SOAP-ENV:Header>
					<SOAP-ENV:Body>
						<p:Probe xmlns:p="http://schemas.xmlsoap.org/ws/2005/04/discovery">
							<d:Types xmlns:d="http://schemas.xmlsoap.org/ws/2005/04/discovery" xmlns:dp0="http://www.onvif.org/ver10/network/wsdl">dp0:NetworkVideoTransmitter</d:Types>
						</p:Probe>
					</SOAP-ENV:Body>
				</SOAP-ENV:Envelope>
			)";
	return request;
}

std::string LynxConfigure::_regexSearch(const std::string& reponse, const std::string& pattern) {
	std::smatch matches;
	if (std::regex_search(reponse, matches, std::regex(pattern))) return matches[1].str();
	else return "";
}