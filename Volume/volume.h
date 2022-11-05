#pragma once
#include <string>
#include <vector>
#include <fstream>

#include"Entry.h"
using namespace std;
class Volume
{
private:
	fstream data;
	uint16_t bytePerSector; // SỐ byte mỗi cluster
	unsigned int fatStartSector; // Sector bắt đầu bảng FAT
	unsigned int sectorPerCluster; //Số sector mỗi cluster
	unsigned int rdetCluster; // Cluster bắt đầu RDET
	unsigned int sectorPerFat; //So Sector mỗi bảng FAT
	unsigned int numberOfCluters; //So luong cluster

public:

	//bool init(int size, string password); // Khởi tạo Volume mới
	//void open(string path); //Mở volume đã khởi tạo, đọc thông tin bootsector

	char* readBlock(int num); //Doc sector thu num (Tra ve day 512b)
	void writeBlock(int num, char* data); //ghi data vao sector ( data la day 512 byte)

	//int* readFat(); // đọc thông tin bảng FAT
	//vector<Entry> readRDET(); // đọc bảng RDET

	//bool resetPassWord();

	//void import(string path);
	//void outport(string file, string outportPath);

	//void deleteFile(string file);

};

