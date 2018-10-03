/*
 * Inventory.h
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#ifndef INVENTORY_H_
#define INVENTORY_H_

#include <string>

namespace tinyxml2 {
    class XMLDocument;
    class XMLElement;
}

class Machine;
class Inventory {
public:
	Inventory();
	~Inventory();

	bool Initialize(const std::string& deviceID);
	void Clear();

	bool Build(bool noSoftware = false);
	bool Save(const char* fileName);
	bool Send(const char* serverUrl);
	void Print();

	int Checksum() const;

private:

    void _AddAccountInfo(tinyxml2::XMLElement* parent);
    void _AddBIOSInfo(tinyxml2::XMLElement* parent);
    void _AddCPUsInfo(tinyxml2::XMLElement* parent);
    void _AddStoragesInfo(tinyxml2::XMLElement* parent);
    void _AddMemoriesInfo(tinyxml2::XMLElement* parent);
    void _AddDrivesInfo(tinyxml2::XMLElement* parent);
    void _AddHardwareInfo(tinyxml2::XMLElement* parent);
    void _AddNetworksInfo(tinyxml2::XMLElement* parent);
    void _AddProcessesInfo(tinyxml2::XMLElement* parent);
    void _AddSoftwaresInfo(tinyxml2::XMLElement* parent);
    void _AddUsersInfo(tinyxml2::XMLElement* parent);
    void _AddVideosInfo(tinyxml2::XMLElement* parent);
    void _AddMonitorsInfo(tinyxml2::XMLElement* parent);

    bool _WriteProlog(tinyxml2::XMLDocument& document) const;

    tinyxml2::XMLDocument* fDocument;
    tinyxml2::XMLElement* fContent;
	Machine* fMachine;
	std::string fDeviceID;
};

#endif /* INVENTORY_H_ */
