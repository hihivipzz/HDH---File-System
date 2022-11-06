#include<iostream>
#include<string>
#include"utils.h"
#include "RSA.h"
using namespace std;

int main() {
	
	Menu1();

	Volume v;
	v.open("abc123");
	char* a = new char[3];
	a[0] = 1;
	a[1] = 2;
	a[2] = 3;
	v.writeCluster(3, a);
	char* b = v.readCluster(3);
	//cout << b[0];
	/*string pwd_mahoa = RSA_pwd::encryptPassword("xyzeqrt+-092");
	cout << pwd_mahoa << endl;

	string pwd_giaima = RSA_pwd::decryptPassword(pwd_mahoa);
	cout << pwd_giaima << endl;
	return 0;*/

}