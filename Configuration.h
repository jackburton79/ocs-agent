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

	void Print() const ;

	std::string ServerURL() const;
	bool SetServer(const char* serverUrl);

	std::string OutputFileName() const;
	bool SetOutputFileName(const char* fileName);

	bool SetKeyValue(const char* key, const char* value);
	bool SetVolatileKeyValue(const char* key, const char* value);
	std::string KeyValue(const char* key) const;
	
	std::string DeviceID() const;	
	bool LocalInventory() const;

	void SetUseCurrentTimeInDeviceID(bool use);
	bool UseCurrentTimeInDeviceID() const;

	void SetUseBaseBoardSerialNumber(bool use);
	bool UseBaseBoardSerialNumber() const;
private:
	Configuration();
	~Configuration();

	void _GenerateDeviceID();

	std::map<std::string, std::string> fValues;
	std::map<std::string, std::string> fVolatileValues;
	std::string fConfigFileName;

};

#endif /* CONFIGURATION_H_ */
