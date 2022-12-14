#include "Entry.h"
#include "utils.h"

void Entry::createEntry(string name, string password, int dataSize, int type, uint16_t startCluster) {
	this->name = name; // tên file 
	this->nameSize = name.length(); // kích thước tên file

	// mã hóa mật khẩu và kích thước mật khẩu
	if (password != "") {
		this->password = RSA_pwd::encryptPassword(password);
		this->passSize = password.length();
	}
	else {
		this->password = "";
		this->passSize = 0;
	}

	// Thiết lập ngày import vào
	struct tm newtime;
	time_t now = time(0);
	localtime_s(&newtime, &now);
	int Month = 1 + newtime.tm_mon; // tháng
	int Day = newtime.tm_mday; // ngày
	int Year = 1900 + newtime.tm_year - 2022; // năm sở dĩ - 2022 vì các thư mục được lưu trên vol được tính từ 2022 trở đi
	this->dateCreate = (Year << 9) | (Month << 5) | Day;

	// Thiết lập giờ import vào
	int sec = newtime.tm_sec; // giây
	int minute = newtime.tm_min; // phút
	int hour = newtime.tm_hour; // giờ
	this->timeCreate = (hour << 1) | (minute << 5) | (sec >> 1);

	this->type = type;
	this->startCluster = startCluster;
	this->dataSize = dataSize;

	this->size = 14 + nameSize + passSize;
}

char* Entry::toBytes() {
	char* data = new char[size];

	writeOffset(data, 0, (char*)&size, sizeof(size));
	writeOffset(data, 1, (char*)&nameSize, sizeof(nameSize));
	writeOffset(data, 2, (char*)&passSize, sizeof(passSize));
	writeOffset(data, 3, (char*)&timeCreate, sizeof(timeCreate));
	writeOffset(data, 5, (char*)&dateCreate, sizeof(dateCreate));
	writeOffset(data, 7, (char*)&type, sizeof(type));
	writeOffset(data, 8, (char*)&startCluster, sizeof(startCluster));
	writeOffset(data, 10, (char*)&dataSize, sizeof(dataSize));
	writeOffset(data, 14, (char*)name.c_str(), nameSize);
	writeOffset(data, 14+nameSize, (char*)password.c_str(), passSize);

	return data;
}

string Entry::getFileName() {
	return name;
}

int Entry::getDataSize() {
	return dataSize;
}

uint16_t Entry::getStartCluster() {
	return startCluster;
}

bool Entry::checkPassword(string password) {
	string pwd_giaima = RSA_pwd::decryptPassword(this->password);

	if (pwd_giaima != password) {
		return false;
	}

	return true;
}

string Entry::getPassword() {
	return password;
}

void Entry::setPassword(string pwd) {
	password = RSA_pwd::encryptPassword(pwd);
}

char Entry::getType() {
	return type;
}

uint8_t Entry::getSize() {
	return size;
}

char Entry::getPassSize() {
	return passSize;
}

void Entry::readEntry(char* data) {
	size = *(uint8_t*)&data[0];
	nameSize = *(char*)&data[1];
	passSize = *(char*)&data[2];
	timeCreate = *(uint16_t*)&data[3];
	dateCreate = *(uint16_t*)&data[5];
	type = *(char*)&data[7];
	startCluster = *(uint16_t*)&data[8];
	dataSize = *(int*)&data[10];

	for (int i = 0; i < nameSize; i++) {
		name += data[14 + i];
	}
	for (int i = 0; i < passSize; i++) {
		password += data[14 + nameSize + i];
	}
}
