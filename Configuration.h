/*
 * Configuration.h
 *
 *  Created on: 13/lug/2013
 *      Author: stefano
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <map>
#include <string>

class Configuration {
public:
	static Configuration* Get();

	bool Load(const char* fileName);
	bool SetServer(const char* fileName);

	std::string DeviceID() const;
	std::string ServerURL() const;
	bool LocalInventory() const;

private:
	Configuration();
	~Configuration();

	std::map<std::string, std::string> fValues;

};

#endif /* CONFIGURATION_H_ */
