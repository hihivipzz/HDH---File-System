#pragma once
#include <iostream>
#include "RSA.h"
#include <ctime>
using namespace std;


class Entry
{
private:
	uint8_t size; // kích thước cluster
	char nameSize;
	char passSize;
	uint16_t timeCreate;
	uint16_t dateCreate;
	char type;
	uint16_t startCluster;
	int dataSize;
	string name;
	string password;

public:
	void createEntry(string name, string password, int dataSize, int type, uint16_t startCluster);
	char* toBytes();
	void readEntry(char* data);
	string getFileName();
	int getDataSize();
	uint16_t getStartCluster();
	bool checkPassword(string password);
	string getPassword();
	void setPassword(string pwd);
	char getType();
	uint8_t getSize();
	char getPassSize();
};

