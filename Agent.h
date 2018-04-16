/*
 * Agent.h
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#ifndef AGENT_H_
#define AGENT_H_

#include <string>

class Agent {
public:
	Agent();
	~Agent();

	void Run();

	static const char* Version();
	static const char* LegacyAgentString();
	static const char* AgentString();

private:

	void _RetrieveInventory();
	void _PrintInventory();
	void _SendInventory();

	static std::string sAgentString;
};

#endif /* AGENT_H_ */
