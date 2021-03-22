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
		bios.release_date = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_date").ReadLine());
		bios.vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_vendor").ReadLine());
		bios.version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_version").ReadLine());
		gComponents["BIOS"].MergeWith(bios);

		Component system;
		system.name = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_name").ReadLine());
		system.version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_version").ReadLine());
		system.uuid = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_uuid").ReadLine());
		system.serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_serial").ReadLine());
		system.vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/sys_vendor").ReadLine());
		gComponents["SYSTEM"].MergeWith(system);

		Component chassis;
		chassis.asset_tag = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_asset_tag").ReadLine());
		chassis.serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_serial").ReadLine());
		chassis.vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_vendor").ReadLine());
		chassis.version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_version").ReadLine());
		// TODO: This is a numeric "type", so no.
		//chassis.type = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_type").ReadLine());
		gComponents["CHASSIS"].MergeWith(chassis);

		Component board;
		board.asset_tag = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_asset_tag").ReadLine());
		board.name = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_name").ReadLine());
		board.serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_serial").ReadLine());
		board.vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_vendor").ReadLine());
		board.version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_version").ReadLine());
		gComponents["BOARD"].MergeWith(board);
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
		gComponents["GRAPHICS"].MergeWith(videoInfo);
	return 0;
}
