/*
 * Inventory.h
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#ifndef INVENTORY_H_
#define INVENTORY_H_

#include <string>


class InventoryFormat;
class Inventory {
public:
	Inventory();
	~Inventory();

	bool Initialize();
	void Clear();

	bool Build(bool noSoftware = false);
	bool Save(const char* fileName);
	bool Send(const char* serverUrl);
	void Print();

private:
	InventoryFormat* fFormat;
};

#endif /* INVENTORY_H_ */
