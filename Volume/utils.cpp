#include<iostream>
#include<fstream>
#include<string>
#include"utils.h"
using namespace std;

void Menu1() {
	int choice;
	while (1) {
		cout << "-------------------------------" << endl << endl;
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
		cout << "-------------------------------" << endl << endl;

		switch (choice)
		{
		case 1:
			createVolume();
			break;
		case 2:
			MenuPasswordVolume();
			break;
		case 3:
			viewListMenu();
			break;
		case 4:
			changeFilePassword();
			break;
		case 5:
			importFileToVolume();
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
	if (!f.is_open()) {
		cout << "Volume da duoc tao ban co muon format lai Volume" << endl;
	}

	int size;
	cout << "Nhap kich thuoc Volume (Don vi byte, kich thuoc vol < 4 GB va > 1 KB ): ";
	cin >> size;
	string password;
	cout << "Nhap mat khau cho Volume: ";
	cin >> password;

	Volume v;
	v.init(size, password);

	v.close();
}

void MenuPasswordVolume() {
	string passWord;
	cout << "Nhap mat khau de truy suat Volume: ";
	cin >> passWord;
	Volume v;
	if (!v.open(passWord)) {
		return;
	}

	string passOld, passNew1, passNew2;
	int choice;
	while (1) {
		cout << "1. Doi Mat Khau" << endl;
		cout << "2. Kiem tra mat khau" << endl;
		cout << "0.Tro lai" << endl;
		cout << "Chon chuc nang muon thuc hien: ";
		cin >> choice;

		switch (choice)
		{
		case 1:
			
			cout << "Nhap mat khau cu: ";
			cin >> passOld;
			if (passOld != passWord) {
				cout << "Mat khau khong dung!" << endl;
				break;
			}
			cout << "Nhap mat khau moi: ";
			cin >> passNew1;
			cout << "Nhap lai mat khau moi: ";
			cin >> passNew2;
			if (passNew1 == passNew2) {
				v.resetPassWord(passNew1);
				cout << "Doi mat khau thanh cong" << endl;
			}
			else {
				cout << "Mat khau khong trung khop" << endl;
			}
			break;
		case 2:
			cout << "Mat khau hien tai: " << passWord << endl;
			break;
		case 0:
			return;
		default:
			cout << "Khong hop le" << endl;
			break;
		}
	}
}

void viewListMenu() {
	string passWord;
	cout << "Nhap mat khau de truy suat Volume: ";
	cin >> passWord;
	Volume v;
	if (!v.open(passWord)) {
		return;
	}

	vector<Entry> listEntry = v.readRDET();
	cout << "Danh sach file: " << endl;
	for (int i = 0; i < listEntry.size(); i++) {
		Entry e = listEntry[i];
		string name = e.getFileName();
		cout << name<<endl;
	}
	cout << "\n";
	system("pause");

	v.close();
}

void importFileToVolume() {
	string passWord;
	cout << "Nhap mat khau de truy suat Volume: ";
	cin >> passWord;
	Volume v;
	if (!v.open(passWord)) {
		cout << "Volume chua duoc tao hoac mat khau khong dung" << endl;
		return;
	}

	string path;
	cout << "Nhap duong dan file: ";
	cin >> path;
	v.import(path);

	v.close();
}

void changeFilePassword() {
	string passWord;
	cout << "Nhap mat khau de truy suat Volume: ";
	cin >> passWord;
	Volume v;
	if (!v.open(passWord)) {
		cout << "Volume chua duoc tao hoac mat khau khong dung" << endl;
		return;
	}

	string filename;
	cout << "Nhap ten file muon cap nhat/thay doi mat khau: ";
	cin >> filename;
	v.updatePassword(filename);

	v.close();
}

void writeOffset(char* buffer, int offSet, char* data, int n) {
	for (int i = 0; i < n; i++) {
		buffer[offSet + i] = data[i];
	}
}


