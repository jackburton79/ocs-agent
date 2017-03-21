/*
 * Configuration.h
 *
 *  Created on: 13/lug/2013
 *      Author: Stefano Ceccherini
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <map>
#include <string>

class Configuration {
public:
	static Configuration* Get();

	bool Load(const char* fileName);
	bool Save(const char* fileName);
	bool Save();

	bool SetServer(const char* serverUrl);
	bool SetOutputFileName(const char* fileName);
	
	std::string DeviceID() const;
	std::string ServerURL() const;
	bool LocalInventory() const;
	std::string OutputFileName() const;

private:
	Configuration();
	~Configuration();

	void _GenerateDeviceID();

	std::map<std::string, std::string> fValues;
	std::string fConfigFileName;

};

#endif /* CONFIGURATION_H_ */
