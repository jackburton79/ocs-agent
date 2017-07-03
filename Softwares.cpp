/*
 * Softwares.cpp
 *
 *      Author: Jonathan ORSEL
 */

#include "Softwares.h"
#include "ProcReader.h"
#include "Support.h"

#include <iostream>
#include <istream>



Softwares::Softwares()
{
	_ReadSoftwaresInfo();	
}

Softwares::~Softwares()
{
}


int
Softwares::Count() const
{
	return fSoftwares.size();
}


software_info
Softwares::SoftwareAt(int i) const
{
	return fSoftwares[i];
}


void
Softwares::_ReadSoftwaresInfo()
{
	try {
		/*if (!CommandExists("rpm"))
			return;*/
		popen_streambuf rpms("rpm -qai", "r");
		std::istream iStreamRpms(&rpms);
		std::string string;
		std::string strtmp;

		std::string comments = "";
		std::string size = "";
		std::string from = "";
		std::string installdate = "";
		std::string name = "";
		std::string version = "";

		while (std::getline(iStreamRpms, string)) {
			if (string.find("Name        :") != std::string::npos ) {
				// new rpm, add last in defined
				if (name != "") {
					software_info info;
					info.comments = comments;
					info.size = size;
					info.from = from;
					info.installdate = installdate;
					info.name = name;
					info.version = version;
					fSoftwares.push_back(info);
				}
				// then update name
				name = string.substr(13, 35);
				trim(name);
			} else if (string.find("Version     :") != std::string::npos) {
				version = string.substr(13, 35);
				trim(version);
			} else if (string.find("Architecture:") != std::string::npos) {
				strtmp = string.substr(13, 35);
				trim(strtmp);
				name = name + '.' + strtmp;
			} else if (string.find("Release     :") != std::string::npos) {
				strtmp = string.substr(13, 35);
				trim(strtmp);
				version = version + '-' + strtmp;
			} else if (string.find("Install Date:") != std::string::npos) {
				installdate = string.substr(13, 50);
				trim(installdate);
			} else if (string.find("Size        :") != std::string::npos) {
				size = string.substr(13, 35);
				trim(size);
			} else if (string.find("Summary     :") != std::string::npos) {
				comments = string.substr(13, 100);
				trim(comments);
			} else if (string.find("Vendor      :") != std::string::npos) {
				strtmp = string.substr(13, 35);
				trim(strtmp);
				from = "RPM-" + strtmp;
			}
		}
		//last one ?
		if (name != "") {
			software_info info;
			info.comments = comments;
			info.size = size;
			info.from = from;
			info.installdate = installdate;
			info.name = name;
			info.version = version;
			fSoftwares.push_back(info);
		}
         
	} catch (...) {
		std::cout << "No software info" << std::endl;
	}
}
