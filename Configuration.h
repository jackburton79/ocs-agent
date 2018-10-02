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

#define CONF_AGENT_STRING "agent-string"
#define CONF_NO_SOFTWARE "nosoftware"
#define CONF_OUTPUT_STDOUT "stdout"
#define CONF_WAIT_TIME "waittime"

#define CONF_VALUE_TRUE "true"
#define CONF_VALUE_FALSE "false"

class Configuration {
public:
	static Configuration* Get();

	bool Load(const char* fileName);
	bool Save(const char* fileName);
	bool Save();

	void Print() const;

	std::string ServerURL() const;
	void SetServer(const char* serverUrl);

	std::string OutputFileName() const;
	void SetOutputFileName(const char* fileName);

	void SetKeyValueBoolean(const char* key, bool value);
	void SetVolatileKeyValueBoolean(const char* key, bool value);
	bool KeyValueBoolean(const char* key) const;

	void SetKeyValue(const char* key, const char* value);
	void SetVolatileKeyValue(const char* key, const char* value);
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

	static std::string _BooleanToString(bool value);
	static bool _StringToBoolean(const std::string& string);

	std::map<std::string, std::string> fValues;
	std::map<std::string, std::string> fVolatileValues;
	std::string fConfigFileName;

};

#endif /* CONFIGURATION_H_ */
