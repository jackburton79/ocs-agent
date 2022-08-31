/*
 * Inventory.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#include "Inventory.h"

#include "Agent.h"
#include "Configuration.h"
#include "Logger.h"
#include "inventoryformat/InventoryFormatOCS.h"

#include <iostream>

Inventory::Inventory()
	:
	fFormat(NULL)
{
	fFormat = new InventoryFormatOCS();
}


Inventory::~Inventory()
{
	delete fFormat;
}


bool
Inventory::Initialize()
{
	Logger& logger = Logger::GetDefault();

	fFormat->Clear();

    std::string deviceID = Configuration::Get()->DeviceID();
    if (deviceID.empty()) {
    	deviceID = fFormat->GenerateDeviceID();
    	Configuration::Get()->SetDeviceID(deviceID.c_str());
    }

	logger.LogFormat(LOG_INFO, "Inventory::Initialize(): Device ID: %s...", deviceID.c_str());

	bool result = fFormat->Initialize();

	logger.LogFormat(LOG_INFO, "Inventory::Initialize(): Device ID: %s... OK!", deviceID.c_str());

	return result;
}


void
Inventory::Clear()
{
	fFormat->Clear();
}


bool
Inventory::Build(bool noSoftware)
{
	Logger& logger = Logger::GetDefault();

	logger.Log(LOG_INFO, "Building inventory...");

	bool result = fFormat->Build(noSoftware);

	logger.Log(LOG_INFO, "Building inventory... Done!");
	return result;
}


bool
Inventory::Save(const char* fileName)
{
	if (fileName == NULL)
		return false;

	Logger& logger = Logger::GetDefault();

	logger.LogFormat(LOG_INFO, "Saving %s inventory as %s", Configuration::Get()->DeviceID().c_str(), fileName);

	bool result = fFormat->Save(fileName);
	if (result)
		logger.Log(LOG_INFO, "Inventory saved correctly!");
	else
		logger.Log(LOG_INFO, "Failed to save inventory!");

	return result;
}


bool
Inventory::Send(const char* serverUrl)
{
	//Logger& logger = Logger::GetDefault();

	// TODO: Move some stuff from InventoryFormat implementation to here
	return fFormat->Send(serverUrl);
/*
	// Prepare prolog
	logger.LogFormat(LOG_INFO, "Inventory::Send(): server URL: %s", serverUrl);
	tinyxml2::XMLDocument prolog;
	_WriteProlog(prolog);
	char* prologData = NULL;
	size_t prologLength = 0;
	if (!XML::Compress(prolog, prologData, prologLength)) {
		logger.Log(LOG_ERR, "Error while compressing XML prolog!");
		return false;
	}

	HTTPRequestHeader requestHeader;
	requestHeader.SetRequest("POST", inventoryUrl.URLString());
	requestHeader.SetValue("Pragma", "no-cache");
	requestHeader.SetValue("Keep-Alive", "300");
	requestHeader.SetValue("Connection", "Keep-Alive, TE");
	requestHeader.SetValue("TE", "deflate, gzip");
	requestHeader.SetContentType("application/x-compress");
	requestHeader.SetContentLength(prologLength);

	// TODO: Improve.
	if (inventoryUrl.Username() != "") {
		requestHeader.SetAuthentication(HTTP_AUTH_TYPE_BASIC,
				inventoryUrl.Username(), inventoryUrl.Password());
	}

	HTTP httpObject;
	std::string userAgentString;
	for (int c = 0; c < 2; c++) {
		// Try first with the new agent. If it fails, try with the old,
		// safe one.
		userAgentString = (c == 0) ? Agent::AgentString()
			: Agent::LegacyAgentString();
		requestHeader.SetUserAgent(userAgentString);

		logger.Log(LOG_INFO, "Inventory::Send(): Prolog prepared!");
		logger.LogFormat(LOG_DEBUG, "%s", requestHeader.ToString().c_str());
		if (httpObject.Request(requestHeader, prologData, prologLength) != 0) {
			delete[] prologData;
			logger.LogFormat(LOG_INFO, "Inventory::Send(): Failed to send prolog: %s",
						httpObject.ErrorString().c_str());
			return false;
		}

		logger.Log(LOG_INFO, "Inventory::Send(): Prolog Sent!");
		const HTTPResponseHeader& responseHeader = httpObject.LastResponse();
		if (responseHeader.StatusCode() == HTTP_BAD_REQUEST) {
			if (c == 0) {
				logger.LogFormat(LOG_INFO, "Server didn't accept prolog. Try again with standard agent string.");
				continue;
			}
		}

		delete[] prologData;

		if (responseHeader.StatusCode() != HTTP_OK
				|| !responseHeader.HasContentLength()) {
			logger.LogFormat(LOG_ERR, "Server replied %s", responseHeader.StatusString().c_str());
			logger.LogFormat(LOG_ERR, "%s", responseHeader.ToString().c_str());
			return false;
		}

		size_t contentLength = ::strtol(responseHeader.Value(HTTPContentLength).c_str(), NULL, 10);
		char* resultData = new char[contentLength];
		if (httpObject.Read(resultData, contentLength) < (int)contentLength) {
			delete[] resultData;
			logger.LogFormat(LOG_ERR, "Inventory::Send(): failed to read XML response: %s",
				httpObject.ErrorString().c_str());

			return false;
		}

		logger.Log(LOG_INFO, "Inventory::Send(): Decompressing XML... ");
		tinyxml2::XMLDocument document;
		bool uncompress = XML::Uncompress(resultData, contentLength, document);
		delete[] resultData;
		if (!uncompress) {
			logger.Log(LOG_ERR, "failed to decompress XML");
			return false;
		}

		std::string serverResponse = XML::GetElementText(document, "RESPONSE");
		logger.LogFormat(LOG_INFO, "Inventory::Send(): server replied %s", serverResponse.c_str());
		if (serverResponse == "SEND")
			break;
		logger.LogFormat(LOG_ERR, "Server not ready to accept inventory: %s", serverResponse.c_str());
		return false;
	}

	logger.Log(LOG_INFO, "Inventory::Send(): Compressing XML inventory data... ");
	char* compressedData = NULL;
	size_t compressedSize;
	if (!XML::Compress(*fDocument, compressedData, compressedSize)) {
		logger.Log(LOG_ERR, "Inventory::Send(): error while compressing inventory XML data!");
		return false;
	}

	requestHeader.Clear();
	requestHeader.SetRequest("POST", inventoryUrl.URLString());
	requestHeader.SetValue("Pragma", "no-cache");
	requestHeader.SetValue("Keep-Alive", "300");
	requestHeader.SetValue("Connection", "Keep-Alive, TE");
	requestHeader.SetValue("TE", "deflate, gzip");
	requestHeader.SetContentType("application/x-compress");
	requestHeader.SetContentLength(compressedSize);
	requestHeader.SetUserAgent(userAgentString);
	if (inventoryUrl.Username() != "") {
		requestHeader.SetAuthentication(HTTP_AUTH_TYPE_BASIC,
						inventoryUrl.Username(), inventoryUrl.Password());
	}

	logger.LogFormat(LOG_DEBUG, "%s", requestHeader.ToString().c_str());
	if (httpObject.Request(requestHeader, compressedData, compressedSize) != 0) {
		delete[] compressedData;
		logger.LogFormat(LOG_ERR, "Inventory::Send(): error while sending inventory: %s",
				httpObject.ErrorString().c_str());
		return false;
	}

	logger.Log(LOG_INFO, "Inventory::Send(): Inventory sent correctly!");

	delete[] compressedData;
*/
	return true;
}


void
Inventory::Print()
{
	std::cout << fFormat->ToString() << std::endl;
}
