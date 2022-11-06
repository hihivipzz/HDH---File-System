#include<iostream>
#include<string>
#include"utils.h"
#include "RSA.h"
using namespace std;

int main() {
	/*char* a = new char[18];
	int i = 123123;
	char* arr = new char[4];
	arr[0] = 'a';
	arr[1] = 'b';
	arr[2] = 'c';
	arr[3] = 'd';
	string s = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	fstream f;
	f.open("Test.txt", ios::binary|ios::out|ios::in);
	if (f.is_open()) {
		cout << 1;
	}
	else {
		cout << 0;
	}
	f.seekp(1);
	f.write((char*)arr, 4);
	f.seekg(1);
	f.read(a, 4);
	for (int i = 0; i < 5; i++) {
		cout << a[i] << endl;
	}*/
	
	/*Menu1();

	Volume v;
	v.open("asdasd");*/
	string pwd_mahoa = RSA_pwd::encryptPassword("xyzeqrt+-092");
	cout << pwd_mahoa << endl;

	string pwd_giaima = RSA_pwd::decryptPassword(pwd_mahoa);
	cout << pwd_giaima << endl;
	return 0;
}