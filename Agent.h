/*
 * Agent.h
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#ifndef AGENT_H_
#define AGENT_H_

class Agent {
public:
	Agent();
	~Agent();

	void Run();
private:

	void _RetrieveInventory();
	void _PrintInventory();
	void _SendInventory();
};

#endif /* AGENT_H_ */
