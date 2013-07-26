/*
 * Configuration.h
 *
 *  Created on: 13/lug/2013
 *      Author: stefano
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>

class Configuration {
public:
	static Configuration* Get();

	bool Load(const char* fileName);

	std::string DeviceID() const;
	std::string ServerURL() const;
	bool LocalInventory() const;

private:
	Configuration();
	~Configuration();

};

#endif /* CONFIGURATION_H_ */
