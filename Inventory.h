/*
 * Inventory.h
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#ifndef INVENTORY_H_
#define INVENTORY_H_

class TiXmlDocument;
class Inventory {
public:
	Inventory();
	~Inventory();

	bool Build(const char* deviceID);
	bool Save(const char* name);
	void Send(const char* serverUrl);

private:
	TiXmlDocument* fDocument;

};

#endif /* INVENTORY_H_ */
