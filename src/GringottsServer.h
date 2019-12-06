/*
 * GringottsServer.h
 *
 *  Created on: Oct 1, 2019
 *      Author: shreya
 */

#ifndef GRINGOTTSSERVER_H_
#define GRINGOTTSSERVER_H_

#include "SocketDefs.h"
#include "GringottDefs.h"
#include "Socket.h"
#include <pthread.h>
#include <vector>

class GringottsServer
{
	SocketData socketData;
	static std::map<long long, Account> mAccountData;
	static pthread_mutex_t lock;

	public:

		bool setHostname(std::string sHostname);
		bool setPortNum(int iPortNum);
		bool initServerData();
		bool startServer();
		bool runServer();
		bool exitServer();

		static void* connection_handler(void* args);
		static bool transact(std::string sRecord);
		static bool withdraw(long long llAccountNumber, long long llAmount);
		static bool deposit(long long llAccountNumber, long long llAmount);
		static std::vector<std::string> StringSplitByDelim(std::string str, char& cDeLim);
		static bool printAccountData();
};


#endif /* GRINGOTTSSERVER_H_ */
