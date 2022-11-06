#include "volume.h"
#include "utils.h"
#include "RSA.h"

int fileEnd = UINT32_MAX;

void Volume::init(int size, string password) {
	data.open("MyFS.dat", ios::out | ios::binary);

	//Tạo MyFS.dat kích thước size
	data.seekp(size);
	char temp = 0;
	data.write(&temp, 1);

	//Tính số Sector
	int sectors = size / 512;

	signature[0]='H';
	signature[1] = 'T';
	signature[2] = 'T';
	bytePerSector = 512;
	numberOfFat = 2;
	fatStartSector = 1;
	sectorPerCluster = 4;
	rdetCluster = 3;

	//Tính kích thước bảng FAT và số cluster
	numberOfCluters = (sectors - fatStartSector - numberOfFat) / (4 * numberOfFat / 512 + sectorPerCluster);//4 là số byte quản lý 1 cluster
	sectorPerFat = (4 * numberOfCluters) / 512 + 1;

	publicKey = 17;
	string pwd_mahoa = RSA_pwd::encryptPassword(password);
	passWordLenght = pwd_mahoa.length();
	passWord = pwd_mahoa;

	//Ghi các thông tin vào offset tương ứng
	char* buffer = new char[512];
	for (int i = 0; i < 512; i++) {
		buffer[i] = 0;
	}
	writeOffset(buffer, 0, signature, 4);
	writeOffset(buffer, 4, (char*)&bytePerSector, sizeof(bytePerSector));
	writeOffset(buffer, 6, (char*)&numberOfFat, sizeof(numberOfFat));
	writeOffset(buffer, 7, (char*)&fatStartSector, sizeof(fatStartSector));
	writeOffset(buffer, 8, (char*)&sectorPerFat, sizeof(sectorPerFat));
	writeOffset(buffer, 10, (char*)&sectorPerCluster, sizeof(sectorPerCluster));
	writeOffset(buffer, 11, (char*)&rdetCluster, sizeof(rdetCluster));
	writeOffset(buffer, 15, (char*)&numberOfCluters, sizeof(numberOfCluters));
	writeOffset(buffer, 19, (char*)&publicKey, sizeof(publicKey));
	writeOffset(buffer, 20, (char*)&passWordLenght, sizeof(passWordLenght));
	writeOffset(buffer, 21, (char*)(passWord.c_str()), passWordLenght);

	writeBlock(0, buffer); // ghi sector 0

	for (int i = 0; i < 512; i++) {
		buffer[i] = 0;
	}
	
	//Ghi File RDET vào bảng FAT
	writeOffset(buffer, 4 * rdetCluster, (char*)&fileEnd, 4);
	writeBlock(1, buffer);

	for (int i = 0; i < 512; i++) {
		buffer[i] = 0;
	}

	//Ghi các sector còn lại 
	for (int i = 2; i < sectors; i++) {
		writeBlock(i, buffer);
	}

	data.close();
}

bool Volume::open(string passwordInput) {
	data.open("MyFS.dat", ios::binary | ios::out | ios::in);
	if (!data.is_open()) {
		cout << "Volume chua khoi tao" << endl;
		return false;
	}

	//Đọc sector 0 lấy thông tin bootsector
	char* buffer = readBlock(0);
	//Lấy các thông số từ sector 0
	bytePerSector = *(uint16_t*)&buffer[4];
	numberOfFat = *(uint8_t*)&buffer[6];
	fatStartSector = *(uint8_t*)&buffer[7];
	sectorPerFat = *(uint16_t*) &buffer[8];
	sectorPerCluster = *(uint8_t*)&buffer[10];
	rdetCluster = *(unsigned int*)&buffer[11];
	numberOfCluters = *(unsigned int*)&buffer[15];
	publicKey = buffer[19];
	passWordLenght = buffer[20];
	for (int i = 0; i < passWordLenght; i++) {
		passWord += buffer[21 + i];
	}
	string pwd_giaima = RSA_pwd::decryptPassword(passWord);
	if (pwd_giaima != passwordInput) {
		cout << "Mat khau khong dung" << endl;
		return false;
	}

	delete buffer;
	return true;
};

char* Volume::readBlock(int num) {
	char* result = new char[512];

	this->data.seekg(num* 512);
	this->data.read(result, 512);


	return result;
};

void Volume::writeBlock(int num, char* data) {
	this->data.seekg(num * 512);
	this->data.write(data, 512);
};

unsigned int* Volume::readFat() {
	unsigned int* result = new unsigned int (numberOfCluters + 3);
	int index = 0;

	for (int n = 0; n < sectorPerFat; n++) {
		char*buffer = readBlock(fatStartSector + n);
		for (int i = 0; i < 512 / 4; i+=4) {
			result[index] = *(unsigned int*)&buffer[i];
			index++;
			if (index = numberOfCluters + 3 - 1) {
				break;
			}
		}
		delete [] buffer;
	}
	
	return result;
};

char* Volume::readCluster(int num) {
	char* result = new char[512 * sectorPerCluster];

	this->data.seekg(512*(fatStartSector + sectorPerFat * numberOfFat + (num - 3) * sectorPerCluster));
	data.read(result, 512 * sectorPerCluster);

	return result;
};

void Volume::writeCluster(int num, char* data) {
	this->data.seekp(512*(fatStartSector + sectorPerFat * numberOfFat + (num - 3) * sectorPerCluster));
	this->data.write(data, 512 * sectorPerCluster);
};

bool Volume::resetPassWord(string newPW) {
	char* buffer = readBlock(0);
	//Xoa mat khau cu tren bootsector
	for (int i = 0; i < passWordLenght; i++) {
		buffer[21 + i] = 0;
	}

	//Luu mat khau moi vao bootsector
	string pwd_mahoa = RSA_pwd::encryptPassword(newPW);
	passWordLenght = pwd_mahoa.length();
	passWord = pwd_mahoa;
	writeOffset(buffer, 20, (char*)&passWordLenght, sizeof(passWordLenght));
	writeOffset(buffer, 21, (char*)(passWord.c_str()), passWordLenght);


	writeBlock(0, buffer);
	delete[] buffer;

	return true;
}

void Volume::import(string path) {

	// Bước 2
	fstream readFile;
	readFile.open(path.c_str(), ios::binary | ios::in);

	// lỗi: không thể mở file 
	if (!readFile.is_open()) {
		cout << "Duong dan khong hop le" << endl;
		return;
	}

	// tên file
	string filename = path.substr(path.find_last_of('/') + 1); // xác định tên file từ đường dẫn cung cấp
	// kích thước tên file
	uint8_t sizeOfFilename = sizeof(filename.c_str()) - 1; // tính theo bytes

	// mật khẩu
	string password = "";
	// kích thước mật khẩu
	uint8_t sizeOfPassword = 0; // mặc định ban đầu không có password

	// tính kích thước file <=> kích thước dữ liệu 
	int begin = readFile.tellg();
	readFile.seekg(0, ios::end);
	int end = readFile.tellg();
	int sizeOfFile = end - begin; // sizeOfFile là kích thước của file cần import (đơn vị byte)
	// vì kích thước volume tối đa là 4GB nên khi sizeOfFile > 4GB = 4,294,967,296 sẽ không import 
	if (sizeOfFile > 4294967296) {
		cout << "Kich thuoc file qua lon de import!" << endl;
		return;
	}

	uint16_t dateImport, timeImport;

	//// Thiết lập ngày import vào
	//struct tm newtime;
	//time_t now = time(0);
	//localtime_s(&newtime, &now);
	//int Month = 1 + newtime.tm_mon; // tháng
	//int Day = newtime.tm_mday; // ngày
	//int Year = 1900 + newtime.tm_year - 2022; // năm sở dĩ -2022 vì các thư mục được lưu trên vol được tính từ 2022 trở đi
	//dateImport = (Year << 9) | (Month << 5) | Day;

	//// Thiết lập giờ import vào
	//int sec = newtime.tm_sec; // giây
	//int minute = newtime.tm_min; // phút
	//int hour = newtime.tm_hour; // giờ
	//timeImport = (hour << 1) | (minute << 5) | (sec >> 1);

	// thiết lập trạng thái là file hay folder
	int type = 0;

	// tính kích thước entry của tệp tin này
	uint16_t sizeOfEntry = 2 + 1 + 1 + 2 + 2 + 1 + 2 + 4 + sizeOfFilename + sizeOfPassword; // N

	// số cluster mà Entry chiếm
	uint16_t amountOfCluster = sizeOfEntry / bytePerSector + 1;  // M

	// Bước 3: đọc bảng FAT vào bộ nhớ
	unsigned int* FAT_table = this->readFat();

	// Bước 4:

	// truy xuất RDET
	vector<int> RDET_cluster;
	RDET_cluster.push_back(rdetCluster);
	while (1) {
		if (FAT_table[RDET_cluster[RDET_cluster.size() - 1]] == fileEnd) {
			break;
		}

		RDET_cluster.push_back(FAT_table[RDET_cluster[RDET_cluster.size() - 1]]);
	}

	char* temp = NULL;
	int countByteEmpty = 0;
	bool flag = false;

	int i = 0, j = 0;
	while (i < RDET_cluster.size()) {
		temp = readCluster(i);
		while (j < sectorPerCluster * 512) {
			char val = *(char*)(&temp[j]);
			if (val == 0) {
				countByteEmpty++;
				if (countByteEmpty == sizeOfEntry) {
					flag = true;
					break;
				}
				j++;
			}
			else {
				j = j + val;
			}
		}
		i++;
	}

	// không đủ N byte liên tiếp
	if (!flag) {
		int indexClusterEmpy;
		for (int i = 0; i < sizeof(FAT_table); i++) {
			if (FAT_table[i] == 0) {
				indexClusterEmpy = i;
				break;
			}
		}

		// mở rộng bảng RDET

	}

	// thêm filename vào entry mới

	// kiểm tra tên file đã tồn tại hay chưa

	//

	readFile.close();
}