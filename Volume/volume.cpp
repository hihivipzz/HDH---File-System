#include "volume.h"

char* Volume::readBlock(int num) {
	char* result = new char[512];
	this->data.seekg(num* this->bytePerSector);
	this->data.read(result, this->bytePerSector);


	return result;
};

void Volume::writeBlock(int num, char* data) {
	this->data.seekg(num * this->bytePerSector);
	this->data.write(data, this->bytePerSector);
};