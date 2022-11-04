#pragma once
#include <string>
#include <vector>

#include"Entry.h"
using namespace std;
class Volume
{
private:
	uint16_t bytePerSector; // SỐ byte mỗi cluster
	unsigned int fatStartSector; // Sector bắt đầu bảng FAT
	unsigned int sectorPerCluster; //Số sector mỗi cluster
	unsigned int rdetCluster; // Cluster bắt đầu RDET
	unsigned int sectorPerFat; //So Sector mỗi bảng FAT
	unsigned int numberOfCluters; //So luong cluster

public:
	bool init(string path, int size); // Khởi tạo Volume mới
	void open(string path); //Mở volume đã khởi tạo

	int* readFat(); // đọc thông tin bảng FAT
	vector<Entry> readRDET(); // đọc bảng RDET

	bool resetPassWord();

	void import(string path);
	void outport(string file, string outportPath);

	void deleteFile(string file);

};

