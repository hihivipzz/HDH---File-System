#pragma once
class Entry
{
private:
	uint16_t size;
	char nameSize;
	char passSize;
	uint16_t timeCreate;
	uint16_t dateCreate;
	char type;
	uint16_t startCluster;
	int dataSize;
	string name;
	string password;
	char* data;

public:
	void createEntry(string name, string password, int dataSize, int type, uint16_t startCluster);
	char* toBytes();
};

