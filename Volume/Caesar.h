#pragma once
#include <fstream>
#include <conio.h>
#include <iostream>
#include <vector>

using namespace std;

class Caesar
{
private:
	int key = 245; // số này sẽ được dùng để mã hóa cũng như giải mã file
public:
	Caesar();
	~Caesar();
	int getKey();
public: 
	static void encryptFile(string filename); 
	static void decryptFile(string filename);
	static string encryptPassword(string pwd);
	static string decryptPassword(string pwd);
};
