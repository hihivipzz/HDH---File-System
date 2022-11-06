#include "Caesar.h"

Caesar::Caesar() {
}

Caesar::~Caesar() {
}

int Caesar::getKey() {
	return this->key;
}

string Caesar::encryptPassword(string pwd) {
	Caesar a;
	string result = "";

	int key = a.getKey();
	vector<int> listNumber;

	for (int i = 0; i < pwd.length(); i++) {
		int temp = (pwd[i] + key);
		listNumber.push_back(temp);
	}

	for (long i = 0; i < listNumber.size() - 1; i++) {
		result += char(listNumber[i]);
	}

	return result;
}

string Caesar::decryptPassword(string pwd) {
	Caesar a;
	string result = "";

	int key = a.getKey();
	vector<int> listNumber;

	for (int i = 0; i < pwd.length(); i++) {
		int temp = (pwd[i] - key);
		listNumber.push_back(temp);
	}

	for (long i = 0; i < listNumber.size() - 1; i++) {
		result += char(listNumber[i]);
	}

	return result;
}

void Caesar::encryptFile(string filename) {
	Caesar a;
	int key = a.getKey();
	ifstream fin;
	fin.open(filename.c_str(), ios::binary | ios::in);
	if (!fin.is_open()) {
		cout << "Khong the mo tep tin!" << endl;
		return;
	}

	char c; // đọc từng ký tự cho vào c
	vector<int> listNumber;

	while (!fin.eof())
	{
		fin >> noskipws >> c;
		cout << c << "\t";
		int temp = (c + key);
		listNumber.push_back(temp);
	}

	fin.close();

	ofstream fout;
	fout.open(filename.c_str(), ios::binary | ios::out);

	if (!fout.is_open()) {
		cout << "Khong the mo tep tin!" << endl;
		return;
	}

	for (long i = 0; i < listNumber.size() - 1; i++) {
		fout << char(listNumber[i]);
	}

	fout.close();
}

void Caesar::decryptFile(string filename) {
	Caesar a;
	int key = a.getKey();

	ifstream fin;
	fin.open(filename.c_str(), ios::binary | ios::in);
	if (!fin.is_open()) {
		cout << "Khong the mo tep tin!" << endl;
		return;
	}

	char c; // đọc từng ký tự cho vào c
	vector<int> listNumber;

	while (!fin.eof())
	{
		fin >> noskipws >> c;
		int temp = (c - key);
		listNumber.push_back(temp);
	}

	fin.close();

	ofstream fout;
	fout.open(filename.c_str(), ios::binary | ios::out);

	if (!fout.is_open()) {
		cout << "Khong the mo tep tin!" << endl;
		return;
	}

	for (long i = 0; i < listNumber.size() - 1; i++) {
		fout << char(listNumber[i]);
	}

	fout.close();
}