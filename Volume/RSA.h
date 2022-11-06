#pragma once
#include "BigInt.h"
#include <vector>
using namespace std;

class RSA_pwd
{
private:
	BigInt p = 7;
	BigInt q = 19;
	BigInt n = p * q;
	BigInt e = 17;
	BigInt d = 53;
public:
	RSA_pwd();
	~RSA_pwd();
	BigInt getN();
	BigInt getE();
	BigInt getD();
public:
	BigInt pow_modify(BigInt a, BigInt b);
	static string encryptPassword(string pwd);
	static string decryptPassword(string pwd);
};
