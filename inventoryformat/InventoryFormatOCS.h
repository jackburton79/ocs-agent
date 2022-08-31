/*
 * InventoryFormatOCS.h
 *
 *  Created on: 31/aug/2022
 *      Author: Stefano Ceccherini
 */

#ifndef INVENTORYFORMATOCS_H_
#define INVENTORYFORMATOCS_H_

#include "inventoryformat/InventoryFormat.h"

#include <string>

namespace tinyxml2 {
    class XMLDocument;
    class XMLElement;
}

class Machine;
class InventoryFormatOCS : public InventoryFormat {
public:
	InventoryFormatOCS();
	virtual ~InventoryFormatOCS();

	virtual bool Initialize();
	virtual void Clear();

	virtual std::string GenerateDeviceID() const;

	virtual bool Build(bool noSoftware = false);
	virtual bool Save(const char* fileName);
	virtual bool Send(const char* serverUrl);

	virtual std::string ToString();

private:
	int Checksum() const;

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

    tinyxml2::XMLDocument* fDocument;
    tinyxml2::XMLElement* fContent;
};

#endif /* INVENTORYFORMATOCS_H_ */
