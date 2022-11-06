#include<iostream>
#include<fstream>
#include<string>
#include"utils.h"
using namespace std;

void Menu1() {
	int choice;
	while (1) {
		cout << "1. Tao Volume MyFS.dat" << endl;
		cout << "2. Thiet lap/ Doi/ Kiem tra mat khau truy suat MyFS" << endl;
		cout << "3. Liet ke danh sach tap tin trong MyFS" << endl;
		cout << "4. Dat/Doi mat khau cho mot tap tin trong MyFS" << endl;
		cout << "5. Chep (import) 1 tap tin tu ngoai vao MyFS" << endl;
		cout << "6. Chep (outport) 1 tap tin tu MyFS ra ngoai" << endl;
		cout << "7. Xoa 1 tap tin trong MyFS" << endl;
		cout << "0. Thoat" << endl;
		cout << "Chon chuc nang muon thuc hien: ";
		cin >> choice;

		switch (choice)
		{
		case 1:
			createVolume();
			break;
		case 2:
			break;
		case 0:
			return;
		default:
			cout << "Khong hop le" << endl;
			break;
		}
	}
}

void createVolume() {
	fstream f("MyFS.dat");
	if (f.is_open()) {
		cout << "True";
	}
	else {
		cout << "false";
	}

	int size;
	cout << "Nhap kich thuoc Volume (Don vi byte, kich thuoc vol < 4 GB va > 1 KB ): ";
	cin >> size;
	string password;
	cout << "Nhap mat khau cho Volume: ";
	cin >> password;

	Volume v;
	v.init(size, password);

}

void importFileToVolume() {
	string passWord;
	cout << "Nhap mat khau cho Volume: ";
	cin >> passWord;
	Volume v;
	if (!v.open(passWord)) {
		cout << "Volume chua duoc tao hoac mat khau khong dung" << endl;
		return;
	}
	
}

void writeOffset(char* buffer, int offSet, char* data, int n) {
	for (int i = 0; i < n; i++) {
		buffer[offSet + i] = data[i];
	}
}


