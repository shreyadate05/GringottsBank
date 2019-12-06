/*
 * ServerMain.cpp
 *
 *  Created on: Oct 1, 2019
 *      Author: shreya
 */

#include "GringottsServer.h"
#include <string.h>

int main(int argc, char* argv[])
{
	GringottsServer server;

	server.setPortNum(atoi(argv[1]));

	if(!server.initServerData())
	{
		std::cout << "\nError in " << __func__;
		return -1;
	}

	if(!server.startServer())
	{
		std::cout << "\nError in " << __func__;
		return -1;
	}

	if(!server.runServer())
	{
		std::cout << "\nError in " << __func__;
		return -1;
	}

	if (!server.exitServer())
	{
		std::cout << "\nError in " << __func__;
		return -1;
	}

	std::cout << std::endl;
	return 0;
}
