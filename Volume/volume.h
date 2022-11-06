﻿#pragma once
#include <string>
#include <vector>
#include <fstream>

#include"Entry.h"
using namespace std;
class Volume
{
private:
	fstream data;
	char signature[4];
	uint16_t bytePerSector; // SỐ byte mỗi cluster
	uint8_t numberOfFat;// Số lượng bảng FAT
	uint8_t fatStartSector; // Sector bắt đầu bảng FAT
	uint8_t sectorPerCluster; //Số sector mỗi cluster
	unsigned int rdetCluster; // Cluster bắt đầu RDET
	uint16_t sectorPerFat; //So Sector mỗi bảng FAT
	unsigned int numberOfCluters; //So luong cluster
	char publicKey;
	uint8_t passWordLenght;
	string passWord;

public:

	void init(int size, string password); // Khởi tạo Volume mới
	bool open(string password); //Mở volume đã khởi tạo, đọc thông tin bootsector

	char* readBlock(int num); //Doc sector thu num (Tra ve day 512b)
	void writeBlock(int num, char* data); //ghi data vao sector ( data la day 512 byte)

	//int* readFat(); // đọc thông tin bảng FAT
	//vector<Entry> readRDET(); // đọc bảng RDET

	//bool resetPassWord();

	//void import(string path);
	//void outport(string file, string outportPath);

	//void deleteFile(string file);

};

