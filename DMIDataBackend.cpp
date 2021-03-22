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
		Component bios;
		bios.fields["release_date"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_date").ReadLine());
		bios.fields["vendor"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_vendor").ReadLine());
		bios.fields["version"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_version").ReadLine());
		gComponents["BIOS"].MergeWith(bios);

		Component system;
		system.fields["name"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_name").ReadLine());
		system.fields["version"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_version").ReadLine());
		system.fields["uuid"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_uuid").ReadLine());
		system.fields["serial"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_serial").ReadLine());
		system.fields["vendor"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/sys_vendor").ReadLine());
		gComponents["SYSTEM"].MergeWith(system);

		Component chassis;
		chassis.fields["asset_tag"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_asset_tag").ReadLine());
		chassis.fields["serial"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_serial").ReadLine());
		chassis.fields["vendor"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_vendor").ReadLine());
		chassis.fields["version"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_version").ReadLine());
		// TODO: This is a numeric "type", so no.
		//chassis.type = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_type").ReadLine());
		gComponents["CHASSIS"].MergeWith(chassis);

		Component board;
		board.fields["asset_tag"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_asset_tag").ReadLine());
		board.fields["name"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_name").ReadLine());
		board.fields["serial"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_serial").ReadLine());
		board.fields["vendor"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_vendor").ReadLine());
		board.fields["version"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_version").ReadLine());
		gComponents["BOARD"].MergeWith(board);
	} catch (...) {
		return -1;
	}

	// TODO: Does not work with multiple video cards. And does not work well in general
	Component videoInfo;
	try {
		videoInfo.fields["name"] = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_product_name").ReadLine());
	} catch (...) {
	}
	try {
		videoInfo.fields["vendor"] = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_vendor").ReadLine());
	} catch (...) {
	}
	try {
		videoInfo.fields["type"] = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_string").ReadLine());
	} catch (...) {
	}
	try {
		videoInfo.fields["specific"] = trimmed(ProcReader("/sys/class/graphics/fb0/virtual_size").ReadLine());
	} catch (...) {
	}
	// try this other path
	try {
		videoInfo.fields["specific"] = trimmed(ProcReader("/sys/class/graphics/fb0/device/graphics/fb0/virtual_size").ReadLine());
	} catch (...) {
	}
	std::replace(videoInfo.fields["specific"].begin(), videoInfo.fields["specific"].end(), ',', 'x');
	if (!videoInfo.fields["specific"].empty()
			|| !videoInfo.fields["name"].empty()
			|| !videoInfo.fields["type"].empty())
		gComponents["GRAPHICS"].MergeWith(videoInfo);
	return 0;
}
