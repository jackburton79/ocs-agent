
#include "Screens.h"
#include "edid-decode.h"
#include "Support.h"

#include <iostream>
#include <string>
#include <vector>

Screens::Screens()
{
    std::vector<std::string> outputs;
    popen_streambuf buf("find /sys/devices/ -name edid", "r");
    std::istream stream(&buf);

    std::string line;
    while (std::getline(stream, line)) {
        std::cout << line << std::endl;
        outputs.push_back(line);
    }

    std::vector<std::string>::const_iterator i;
    for (i = outputs.begin(); i != outputs.end(); i++) {
        screen_info info;
        edid_info edidInfo;
        info.name = *i;
        if (get_edid_info((char*)i->c_str(), &edidInfo) == 0) {
            info.manufacturer = edidInfo.manufacturer;
            info.model = edidInfo.model;
            info.serial_number = edidInfo.serial_number;
            fItems.push_back(info);
        }
    }

    Rewind();
}


