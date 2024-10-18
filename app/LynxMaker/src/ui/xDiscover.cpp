#include <regex>
#include <random>
#include <iomanip>
#include <iostream>

#include "log/gLog.h"
#include "gMamba.h"
#include "ui/xDiscover.h"
#include "gPromotion.h"
#include "gMambaDiscovery.h"
#include "gPromotionDiscovery.h"
#include "network/multicastSocket/gMulticastSocket.h"

static gLog::Log _log("LynxDiscover");

// Cette fonction prend beaucoup de temps, nous devons donc trouver une autre fonction équivalente!
static std::string regexSearch(const std::string& reponse, const std::string& pattern) {
	std::smatch matches;
	if (std::regex_search(reponse, matches, std::regex(pattern))) return matches[1].str();
	else return "";
}

static std::string socketRequest()
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

std::list<gCore::Parameters> Connection::discover()
{
	std::list<gCore::Parameters> lynxInfos;
	std::list<MambaInfo> cameraList = gMamba::Connection::discover();
	std::list<PromotionInfo> promotionList = gPromotion::Connection::discover();

	for (const auto& camera : cameraList) try {
		std::string serialNumber = regexSearch(camera.details, R"(onvif://www.onvif.org/Lynx/(\w+))");
		std::string promotionUid = regexSearch(camera.details, R"(onvif://www.onvif.org/Promotion/([a-fA-F0-9:]+))");

		if (!promotionUid.empty() && !serialNumber.empty())
		{
			bool found = false;
			for (const auto& promotion : promotionList)
				if (promotion.uid == promotionUid)
				{
					gCore::Parameters infos;
					infos["promotionInfos"] = promotion;
					infos["mambaInfos"] = camera;
					infos["serialNumber"] = serialNumber;
					lynxInfos.push_back(infos);
					found = true;
					break;
				}
			if (!found)
				_log << "Promotion " << promotionUid << " (associated with camera " << camera.name << ") not found." << gLog::error;
		}
		else _log << "Camera " << camera.name << " badly configured : no serial number or Promotion association." << gLog::error;
	}
	catch (gCore::Exception& e) { _log << e.what() << e.comment(); }

	return lynxInfos;
}