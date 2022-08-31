/*
 * InventoryFormat.h
 *
 *  Created on: 31 ago 2022
 *      Author: Stefano Ceccherini
 */

#ifndef INVENTORYFORMAT_H_
#define INVENTORYFORMAT_H_

#include <string>


class InventoryFormat {
public:
	InventoryFormat();
	virtual ~InventoryFormat();

	virtual bool Initialize() = 0;
	virtual void Clear() = 0;

	virtual std::string GenerateDeviceID() const = 0;

	virtual bool Build(bool noSoftware = false) = 0;
	virtual bool Save(const char* fileName) = 0;
	virtual bool Send(const char* serverUrl) = 0;

	virtual std::string ToString() = 0;
};

#endif /* INVENTORYFORMAT_H_ */
