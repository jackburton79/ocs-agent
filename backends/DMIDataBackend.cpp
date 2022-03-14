/*
 * DMIDataBackend.cpp
 *
 *  Created on: 22 mar 2021
 *      Author: stefano
 */

#include "DMIDataBackend.h"

#include "Components.h"
#include "ProcReader.h"
#include "Support.h"

#include <iostream>

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
	Component bios;
	try {
		bios.fields["release_date"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_date").ReadLine());
	} catch (...) {}
	try {
		bios.fields["vendor"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_vendor").ReadLine());
	} catch (...) {}
	try {
		bios.fields["version"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_version").ReadLine());
	} catch (...) {}	
	gComponents.Merge("BIOS", bios);

	Component system;
	try {
		system.fields["name"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_name").ReadLine());
	} catch (...) {}
	try {
		system.fields["version"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_version").ReadLine());
	} catch (...) {}
	try {
		system.fields["uuid"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_uuid").ReadLine());
	} catch (...) {}
	try {
		system.fields["serial"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_serial").ReadLine());
	} catch (...) {}
	try {
		system.fields["vendor"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/sys_vendor").ReadLine());
	} catch (...) {}		
	gComponents.Merge("SYSTEM", system);
		
	Component chassis;
	try {
		chassis.fields["asset_tag"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_asset_tag").ReadLine());
	} catch (...) {}
	try {
		chassis.fields["serial"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_serial").ReadLine());
	} catch (...) {}
	try {
		chassis.fields["vendor"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_vendor").ReadLine());
	} catch (...) {}
	try {
		chassis.fields["version"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_version").ReadLine());
	} catch (...) {}
		// TODO: This is a numeric "type", so no.
	try {
		//chassis.type = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_type").ReadLine());
	} catch (...) {}
	gComponents.Merge("CHASSIS", chassis);

	Component board;
	try {
		board.fields["asset_tag"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_asset_tag").ReadLine());
	} catch (...) {}
	try {
		board.fields["name"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_name").ReadLine());
	} catch (...) {}
	try {
		board.fields["serial"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_serial").ReadLine());
	} catch (...) {}
	try {
		board.fields["vendor"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_vendor").ReadLine());
	} catch (...) {}
	try {
		board.fields["version"] = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_version").ReadLine());
	} catch (...) {}

	gComponents.Merge("BOARD", board);
	
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
		videoInfo.fields["resolution"] = trimmed(ProcReader("/sys/class/graphics/fb0/virtual_size").ReadLine());
	} catch (...) {
	}
	// try this other path
	try {
		videoInfo.fields["resolution"] = trimmed(ProcReader("/sys/class/graphics/fb0/device/graphics/fb0/virtual_size").ReadLine());
	} catch (...) {
	}
	std::replace(videoInfo.fields["resolution"].begin(), videoInfo.fields["resolution"].end(), ',', 'x');
	if (!videoInfo.fields["resolution"].empty()
			|| !videoInfo.fields["name"].empty()
			|| !videoInfo.fields["type"].empty())
		gComponents.Merge("GRAPHICS", videoInfo);
	return 0;
}
