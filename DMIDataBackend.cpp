/*
 * DMIDataBackend.cpp
 *
 *  Created on: 22 mar 2021
 *      Author: stefano
 */

#include "DMIDataBackend.h"

#include "Machine.h"
#include "ProcReader.h"
#include "Support.h"


DMIDataBackend::DMIDataBackend()
{

}


DMIDataBackend::~DMIDataBackend()
{
}


/* virtual */
int
DMIDataBackend::Run()
{
	try {
		gComponents["BIOS"].release_date = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_date").ReadLine());
		gComponents["BIOS"].vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_vendor").ReadLine());
		gComponents["BIOS"].version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_version").ReadLine());

		gComponents["SYSTEM"].name = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_name").ReadLine());
		gComponents["SYSTEM"].version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_version").ReadLine());
		gComponents["SYSTEM"].uuid = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_uuid").ReadLine());
		gComponents["SYSTEM"].serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_serial").ReadLine());
		gComponents["SYSTEM"].vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/sys_vendor").ReadLine());

		gComponents["CHASSIS"].asset_tag = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_asset_tag").ReadLine());
		gComponents["CHASSIS"].serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_serial").ReadLine());
		gComponents["CHASSIS"].vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_vendor").ReadLine());
		gComponents["CHASSIS"].version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_version").ReadLine());
		// TODO: This is a numeric "type", so no.
		//gComponents["CHASSIS"].type = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_type").ReadLine());

		gComponents["BOARD"].asset_tag = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_asset_tag").ReadLine());
		gComponents["BOARD"].name = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_name").ReadLine());
		gComponents["BOARD"].serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_serial").ReadLine());
		gComponents["BOARD"].vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_vendor").ReadLine());
		gComponents["BOARD"].version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_version").ReadLine());
	} catch (...) {
		return -1;
	}

	// TODO: Does not work with multiple video cards. And does not work well in general
	Component videoInfo;
	try {
		videoInfo.name = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_product_name").ReadLine());
	} catch (...) {
	}
	try {
		videoInfo.vendor = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_vendor").ReadLine());
	} catch (...) {
	}
	try {
		videoInfo.type = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_string").ReadLine());
	} catch (...) {
	}
	try {
		videoInfo.specific = trimmed(ProcReader("/sys/class/graphics/fb0/virtual_size").ReadLine());
	} catch (...) {
	}
	// try this other path
	try {
		videoInfo.specific = trimmed(ProcReader("/sys/class/graphics/fb0/device/graphics/fb0/virtual_size").ReadLine());
	} catch (...) {
	}
	std::replace(videoInfo.specific.begin(), videoInfo.specific.end(), ',', 'x');
	if (!videoInfo.specific.empty() || !videoInfo.name.empty() || !videoInfo.type.empty())
		gComponents["GRAPHICS"] = videoInfo;
	return 0;
}
