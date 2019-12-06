/*
 * GringottDefs.h
 *
 *  Created on: Oct 1, 2019
 *      Author: shreya
 */

#ifndef GRINGOTTDEFS_H_
#define GRINGOTTDEFS_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

typedef struct _Account
{
	std::string sAccountName;
	long long llBalance;

} Account;

typedef struct _ClientData
{
	std::string sHostname;
	int         iPortNum;
	std::string sTransaction;
} ClientData;

#endif /* GRINGOTTDEFS_H_ */
