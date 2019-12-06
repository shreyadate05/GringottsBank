/*
 * GringottsClient.cpp
 *
 *  Created on: Oct 1, 2019
 *      Author: shreya
 */

#include "GringottsClient.h"

using std::cout;
using std::string;
using std::vector;
using std::map;

void* GringottsClient::connection_handler(void* args)
{
	ClientData* client = static_cast<ClientData*>(args);

	SocketData newSocket;
	if (!GringottsClient::initClient(newSocket, *client))
	{
		cout << "\nError while creating client in thread " << pthread_self() << "";
	}

	cout << "\nClient " << pthread_self() << " :: transaction "  << client->sTransaction << "\n";
	Socket::sendData(newSocket, (client->sTransaction).c_str());

	return NULL;
}

bool GringottsClient::initClient(SocketData& socketData, ClientData& client)
{
	bool res = true;

	socketData.iPortNum = client.iPortNum;
	socketData.sHostname = client.sHostname;

	if (!Socket::createSocket(socketData))
	{
		cout << "\nError in " << __func__;
		res =  false;
	}

	if (!Socket::connect(socketData))
	{
		cout << "\nError in " << __func__;
		res =  false;
	}
	return res;
}

bool GringottsClient::runClient(int iPortNum, const string sHostname)
{
	bool res = true;

	// create 1 threads per transaction
	pthread_t threadIds[vTransactions.size()];
	ClientData clients[vTransactions.size()];

	for (int iClientThread = 0; iClientThread < vTransactions.size(); iClientThread++)
	{
		clients[iClientThread].iPortNum     = iPortNum;
		clients[iClientThread].sHostname    = sHostname;
		clients[iClientThread].sTransaction = vTransactions[iClientThread];
	}

	for (int iClientThread = 0; iClientThread < vTransactions.size(); iClientThread++)
	{
		if(pthread_create(&threadIds[iClientThread], NULL, GringottsClient::connection_handler, (void*)(&clients[iClientThread])))
		{
			cout << "\nError in " << __func__;
			return false;
		}
		sleep(0.5);
	}

	for (int i = 0; i < vTransactions.size(); i++)
	{
		pthread_join(threadIds[i],NULL);
	}

	return res;
}

bool GringottsClient::readTransactions()
{
	bool res = true;

	std::ifstream file("Transactions.txt");
	std::string   line;
	try
	{
		while(std::getline(file, line))
		{
			std::stringstream lineStream(line);
			string value;
			string sTransaction = "";
			// Read single row of space separated vector
			while(lineStream >> value)
			{
				sTransaction += " " + value;
			}
			vTransactions.push_back(sTransaction);
		}
	}
	catch(...)
	{
		std::cout << "\nException thrown while reading client transactions in " << __func__;
		res = false;
	}

	return res;

}

bool GringottsClient::printTransactions()
{
	for (int i = 0; i < vTransactions.size(); i++)
	{
		cout << "\n" << vTransactions[i];
	}

	return true;
}
