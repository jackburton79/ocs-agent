/*
 * RunningProcesses.h
 *
 *  Created on: 16/lug/2013
 *      Author: stefano
 */

#ifndef RUNNINGPROCESSES_H_
#define RUNNINGPROCESSES_H_

#include <list>
#include <string>

struct process_info {
	std::string cmdline;
	int cpu_usage;
	int memory;
	int pid;
	// int started;
	// int tty;
	std::string user;
	int virtualmem;
};


class RunningProcessesList {
public:
	RunningProcessesList();
	~RunningProcessesList();

	void Rewind();
	bool GetNext(process_info& info);

private:

	void _ReadProcessInfo(process_info& info, std::string pid);

	std::list<process_info> fProcesses;
	std::list<process_info>::iterator fIterator;
};

#endif /* RUNNINGPROCESSES_H_ */
