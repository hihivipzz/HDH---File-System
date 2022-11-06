#pragma once
#include <iostream>
#include "RSA.h"
#include <ctime>
using namespace std;

const int BUFFER_SIZE = 512;
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
	string getFileName();
	int getDataSize();
	void read(FILE* f);
	void Display(vector<Entry> listEntry);// In ten cac file
	bool checkPassword(string password);
};

