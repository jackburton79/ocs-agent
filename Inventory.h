/*
 * Inventory.h
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#ifndef INVENTORY_H_
#define INVENTORY_H_

class Machine;
class TiXmlDocument;
class TiXmlElement;
class Inventory {
public:
	Inventory();
	~Inventory();

	bool Build(const char* deviceID);
	bool Save(const char* name);
	bool Send(const char* serverUrl);

	int Checksum() const;

private:
	void _AddAccountInfo(TiXmlElement* parent);
	void _AddBIOSInfo(TiXmlElement* parent);
	void _AddCPUsInfo(TiXmlElement* parent);
	void _AddDrivesInfo(TiXmlElement* parent);
	void _AddHardwareInfo(TiXmlElement* parent);
	void _AddNetworksInfo(TiXmlElement* parent);
	void _AddProcessesInfo(TiXmlElement* parent);
	void _AddSoftwaresInfo(TiXmlElement* parent);
	void _AddUsersInfo(TiXmlElement* parent);
	void _AddVideosInfo(TiXmlElement* parent);

	bool _WriteProlog(TiXmlDocument& document) const;

	TiXmlDocument* fDocument;
	Machine* fMachine;
};

#endif /* INVENTORY_H_ */
