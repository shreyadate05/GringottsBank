/*
 * GringottsServer.cpp
 *
 *  Created on: Oct 1, 2019
 *      Author: shreya
 */
#include "GringottsServer.h"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <algorithm>

using std::vector;
using std::string;
using std::stringstream;

using std::cout;
using std::string;
using std::vector;
using std::map;
using std::endl;
using std::setw;

std::map<long long, Account> GringottsServer::mAccountData;
pthread_mutex_t GringottsServer::lock;
vector<string> transactionQueue;

std::string & ltrim(std::string & str)
{
  auto it2 =  std::find_if( str.begin() , str.end() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
  str.erase( str.begin() , it2);
  return str;
}

std::string & rtrim(std::string & str)
{
  auto it1 =  std::find_if( str.rbegin() , str.rend() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
  str.erase( it1.base() , str.end() );
  return str;
}

std::string & trim(std::string & str)
{
   return ltrim(rtrim(str));
}

void* GringottsServer::connection_handler(void* args)
{
	string* sTransaction = static_cast<string*>(args);

//	if (transactionQueue[0][transactionQueue[0].size()-1] == ' ')
//		transactionQueue[0] = transactionQueue[0].substr(0, transactionQueue[0].size()-2);
//	if(transactionQueue[0][0] == ' ')
//		transactionQueue[0] = transactionQueue[0].substr(1, transactionQueue[0].size()-1);


	transactionQueue[0].erase(std::remove(transactionQueue[0].begin(), transactionQueue[0].end(), '\n'), transactionQueue[0].end());
	transactionQueue[0].erase(std::remove(transactionQueue[0].begin(), transactionQueue[0].end(), '\r'), transactionQueue[0].end());
	transactionQueue[0] = trim(transactionQueue[0]);
	if (transactionQueue[0][0] == ' ')
		transactionQueue[0] = transactionQueue[0].substr(0, transactionQueue[0].size());

	pthread_mutex_lock(&lock);
	GringottsServer::transact(transactionQueue[0]);
	transactionQueue.erase(transactionQueue.begin());
	cout << "\n-------------------------------------------\n\n";
	pthread_mutex_unlock(&lock);

	return NULL;
}

bool GringottsServer::transact(string sRecord)
{
	bool res = true;
	char cDelim = ' ';
	sRecord = transactionQueue[0];
	cout << "\nIn transact for record  :: " << sRecord;
	vector<string> vTokens = GringottsServer::StringSplitByDelim(sRecord, cDelim);

	if (vTokens.size() != 4)
	{
		cout << "\nIgnoring invalid record \"" << sRecord << "\" in Transactions.txt\n";
		return false;
	}

	string sTimestamp         = vTokens[0];
	long long llAccountNumber = stoll(vTokens[1]);
	string sTransaction       = vTokens[2];
	long long llAmount        = stoll(vTokens[3]);

	if (mAccountData.count(llAccountNumber) <= 0)
	{
		cout << "\nAccount " << llAccountNumber << "does not exist" << endl;
		return false;
	}

	if (sTransaction.compare("w") == 0)
	{
		withdraw(llAccountNumber, llAmount);
	}
	else if (sTransaction.compare("d") == 0)
	{
		deposit(llAccountNumber, llAmount);
	}
	else
	{
		cout << "\nTransaction type is invalid for " << sRecord << endl;
		return false;
	}

	return res;
}

bool GringottsServer::withdraw(long long llAccountNumber, long long llAmount)
{
	cout << "\nTransaction initialized :: Withdraw " << llAmount << " from account " << llAccountNumber;
	cout << "\nCurrent balance         :: " << mAccountData[llAccountNumber].llBalance;

	if (mAccountData[llAccountNumber].llBalance < llAmount)
	{
		cout << "\nAccount balance for " << llAccountNumber << " is not enough to withdraw " << llAmount << endl;
		return false;
	}

	mAccountData[llAccountNumber].llBalance = mAccountData[llAccountNumber].llBalance - llAmount;
	cout << "\nTransaction completed   :: Withdrew " << llAmount << " from account " << llAccountNumber;
	cout << "\nUpdated balance         :: " << mAccountData[llAccountNumber].llBalance;

	//GringottsServer::printAccountData();
	return true;
}

bool GringottsServer::deposit(long long llAccountNumber, long long llAmount)
{
	cout << "\nTransaction initialized :: Deposit " << llAmount << " in account " << llAccountNumber;
	cout << "\nCurrent balance         :: " << mAccountData[llAccountNumber].llBalance;

	mAccountData[llAccountNumber].llBalance = mAccountData[llAccountNumber].llBalance + llAmount;
	cout << "\nTransaction completed   :: Deposited " << llAmount << " in account " << llAccountNumber;
	cout << "\nUpdated balance         :: " << mAccountData[llAccountNumber].llBalance;

	//GringottsServer::printAccountData();
	return true;
}

bool GringottsServer::setHostname(string sHostname)
{
	socketData.sHostname = sHostname;
	return true;
}

bool GringottsServer::setPortNum(int iPortNum)
{
	socketData.iPortNum = iPortNum;
	return true;
}

bool GringottsServer::initServerData()
{
	bool res = true;

	std::ifstream file("Records.txt");
	std::string   line;
	try
	{
		while(std::getline(file, line))
		{
			std::vector<string> lineData;
			std::stringstream   lineStream(line);
			string value;

			// Read single row of space separated vector
			while(lineStream >> value)
			{
				lineData.push_back(value);
			}

			// Add it as Account Data
			Account acc;
			acc.sAccountName = lineData[1];
			acc.llBalance = stoll(lineData[2]);
			mAccountData[stoll(lineData[0])] = acc;
		}
		printAccountData();
	}
	catch(...)
	{
		std::cout << "\nException thrown while creating initial client records in " << __func__;
		res = false;
	}

	return res;
}

bool GringottsServer::startServer()
{
	bool res = true;

	if ( ! Socket::createSocket(socketData) )
	{
		cout << "\nError in " << __func__;
		return false;
	}

	if ( ! Socket::bind(socketData) )
	{
		cout << "\nError in " << __func__;
		return false;
	}

	if ( ! Socket::listen(socketData) )
	{
		cout << "\nError in " << __func__;
		return false;
	}

	return res;
}

bool GringottsServer::runServer()
{
	bool res = true;
	int i = 0;
	while(true)
	{
		SocketData* newSocket = new SocketData();
		newSocket->iPortNum = socketData.iPortNum;
		newSocket->sHostname = socketData.sHostname;
		pthread_t threadId;
		string sClientData;

		Socket::accept(socketData, *newSocket);
		Socket::recvData(*newSocket, sClientData);

//		cout << "\n---------------------------------------------\n";
//		cout << "\nServer connected to a client for transaction: " << sClientData.c_str();
		transactionQueue.push_back(sClientData);
		if(pthread_create(&threadId, NULL, GringottsServer::connection_handler, (void*)(&sClientData)))
		{
			cout << "\nError in " << __func__;
			return false;
		}
		i++;
	}
	return res;
}

bool GringottsServer::exitServer()
{
	bool res = true;

	return res;
}

bool GringottsServer::printAccountData()
{
	bool res = true;
	int n = 8;

	cout << "\n----------------------------------------------";
	cout << "\n             CLIENT ACCOUNT DATA              ";
	cout << "\n----------------------------------------------";
	for (auto elem: mAccountData)
	{
		cout << "\nAccount Number : " << elem.first;
		cout << "\nAccount Name   : " << elem.second.sAccountName;
		cout << "\nAccount Balance: " << elem.second.llBalance;
		cout << "\n\n";
	}
	cout << "\n----------------------------------------------\n";

	return res;
}


vector<string> GringottsServer::StringSplitByDelim(std::string str, char& cDeLim)
{
	vector<string> vTokens;
	stringstream ss(str);
	string item;

	while (std::getline(ss, item, cDeLim))
	{
		if(item != " " || item != "\n" || item!= "\r")
			vTokens.push_back (item);
	}

//	for (int i=0; i< vTokens.size(); i++)
//	{
//		cout << "\nvToken: " << vTokens[i];
//	}

	return vTokens;
}




