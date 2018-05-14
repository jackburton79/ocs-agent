/*
 * Screens.h
 *
 *  Created on: 15/lug/2015
 *      Author: Stefano Ceccherini
 */
 
#ifndef __SCREENS_H
#define __SCREENS_H

#include "ItemsList.h"

#include <string>

struct screen_info {
  std::string name;
  std::string description;
  std::string manufacturer;
  std::string model;
  std::string serial_number;  
};

class Screens : public ItemsList<screen_info> {
public:
  Screens();

};


#endif

