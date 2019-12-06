/*
 * ClientMain.cpp
 *
 *  Created on: Oct 1, 2019
 *      Author: shreya
 */

#include "GringottsClient.h"

int main(int argc, char* argv[])
{
	GringottsClient client;
	client.readTransactions();
	client.runClient(atoi(argv[1]),"localhost");

	std::cout << std::endl;
	return 0;
}
