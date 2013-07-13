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
	Configuration();
	~Configuration();

	std::string ServerURL() const;
	bool LocalInventory() const;
};

#endif /* CONFIGURATION_H_ */
