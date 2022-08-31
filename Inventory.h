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

	bool Initialize();
	void Clear();

	bool Build(bool noSoftware = false);
	bool Save(const char* fileName);
	bool Send(const char* serverUrl);
	void Print();

	int Checksum() const;

private:
    void _AddAccountInfo();
    void _AddBIOSInfo();
    void _AddCPUsInfo();
    void _AddStoragesInfo();
    void _AddMemoriesInfo();
    void _AddDrivesInfo();
    void _AddHardwareInfo();
    void _AddNetworksInfo();
    void _AddProcessesInfo();
    void _AddSoftwaresInfo();
    void _AddUsersInfo();
    void _AddVideosInfo();
    void _AddMonitorsInfo();

    bool _WriteProlog(tinyxml2::XMLDocument& document) const;

    std::string _GenerateDeviceID() const;

    tinyxml2::XMLDocument* fDocument;
    tinyxml2::XMLElement* fContent;
};

#endif /* INVENTORY_H_ */
