#include<iostream>
#include<fstream>
#include<string>
#include"utils.h"
#include <windows.h>
#include <iomanip>
using namespace std;

void Menu1() {
	int choice;
	while (1) {
		system("cls");
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, 10);
		cout << "                  888                                      888    888 88888888888 88888888888 " << endl;
		SetConsoleTextAttribute(hConsole, 11);
		cout << "                  888                                      888    888     888         888     " << endl;
		SetConsoleTextAttribute(hConsole, 10);
		cout << "                  888                                      888    888     888         888     " << endl;
		SetConsoleTextAttribute(hConsole, 11);
		cout << "888  888  .d88b.  888 888  888 88888b.d88b.   .d88b.       8888888888     888         888     " << endl;
		SetConsoleTextAttribute(hConsole, 10);
		cout << "888  888 d88\"\"88b 888 888  888 888 \"888 \"88b d8P  Y8b      888    888     888         888     " << endl;
		SetConsoleTextAttribute(hConsole, 11);
		cout << "Y88  88P 888  888 888 888  888 888  888  888 88888888      888    888     888         888     " << endl;
		SetConsoleTextAttribute(hConsole, 10);
		cout << " Y8bd8P  Y88..88P 888 Y88b 888 888  888  888 Y8b.          888    888     888         888     " << endl;
		SetConsoleTextAttribute(hConsole, 11);
		cout << "  Y88P    \"Y88P\"  888  \"Y88888 888  888  888  \"Y8888       888    888     888         888     " << endl;
		SetConsoleTextAttribute(hConsole, 14);
		cout << "---------------------------------------------------------------------------------------------" << endl << endl;
		cout << "\t\t1. Tao Volume MyFS.dat" << endl;
		cout << "\t\t2. Thiet lap/ Doi/ Kiem tra mat khau truy suat MyFS" << endl;
		cout << "\t\t3. Liet ke danh sach tap tin trong MyFS" << endl;
		cout << "\t\t4. Dat/Doi mat khau cho mot tap tin trong MyFS" << endl;
		cout << "\t\t5. Chep (import) 1 tap tin tu ngoai vao MyFS" << endl;
		cout << "\t\t6. Chep (outport) 1 tap tin tu MyFS ra ngoai" << endl;
		cout << "\t\t7. Xoa 1 tap tin trong MyFS" << endl;
		cout << "\t\t0. Thoat" << endl;
		cout << "Chon chuc nang muon thuc hien: ";
		cin >> choice;
		cout << "---------------------------------------------------------------------------------------------" << endl << endl;

		switch (choice)
		{
		case 1:
			createVolume();
			system("pause");
			break;
		case 2:
			MenuPasswordVolume();
			system("pause");
			break;
		case 3:
			viewListMenu();
			system("pause");
			break;
		case 4:
			changeFilePassword();
			system("pause");
			break;
		case 5:
			importFileToVolume();
			system("pause");
			break;
		case 6:
			exportFile();
			system("pause");
			break;
		case 7:
			deleteFile();
			system("pause");
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
		cout <<"     " << name << endl;
	}
	cout << "\n";

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

void exportFile() {
	string passWord;
	cout << "Nhap mat khau de truy suat Volume: ";
	cin >> passWord;
	Volume v;
	if (!v.open(passWord)) {
		cout << "Volume chua duoc tao hoac mat khau khong dung" << endl;
		return;
	}

	vector<Entry> listEntry = v.readRDET(); // Hien danh sach file de chon
	cout << "Danh sach file: " << endl;
	for (int i = 0; i < listEntry.size(); i++) {
		Entry e = listEntry[i];
		if (e.getFileName() != "") {
			string name = e.getFileName();
			cout << name << endl;
		}
	}
	cout << "\n------";

	string filename;
	cout << "Nhap file muon export: ";
	cin >> filename;

	string outpath;
	cout << "Nhap duong dan muon export file den: ";
	cin >> outpath;

	if (v.outport(filename, outpath)) cout << "Export file thanh cong";
	else cout << "Export file that bai";
	v.close();
}

void deleteFile() {
	string passWord;
	cout << "Nhap mat khau de truy suat Volume: ";
	cin >> passWord;
	Volume v;
	if (!v.open(passWord)) {
		cout << "Volume chua duoc tao hoac mat khau khong dung" << endl;
		return;
	}

	string path;
	cout << "Nhap file muon xoa: ";
	cin >> path;
	v.deleteFile(path);

	v.close();
}

void writeOffset(char* buffer, int offSet, char* data, int n) {
	for (int i = 0; i < n; i++) {
		buffer[offSet + i] = data[i];
	}
}
