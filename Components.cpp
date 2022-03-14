/*
 * Components.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */


#include <Components.h>



components_map gComponents;


void
Component::MergeWith(Component& component)
{
	string_map::iterator i;
	for (i = component.fields.begin(); i != component.fields.end(); i++) {
		if (!i->second.empty() && i->second != "") {
			string_map::iterator c = fields.find(i->first);
			if (c == fields.end())
				fields.insert(*i);
			else if (c->second.empty())
				c->second = i->second;
		}
	}
}

