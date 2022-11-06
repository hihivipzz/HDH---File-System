#include "Entry.h"

void Entry::createEntry(string name, string password, int dataSize, int type, uint16_t startCluster) {
	this->name = name; // tên file 
	this->nameSize = name.length(); // kích thước tên file

	// mã hóa mật khẩu và kích thước mật khẩu
	if (password != "") {
		this->password = RSA_pwd::encryptPassword(password);
		this->passSize = password.length();
	}

	this->password = "";
	this->passSize = 0;

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

	this->startCluster = startCluster;
	this->dataSize = dataSize;

	this->size = 14 + nameSize + passSize;
}