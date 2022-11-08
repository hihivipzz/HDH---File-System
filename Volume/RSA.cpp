#include "RSA.h"

RSA_pwd::RSA_pwd() {

}

RSA_pwd::~RSA_pwd() {

}

BigInt RSA_pwd::getN() {
	return this->n;
}

BigInt RSA_pwd::getE() {
	return this->e;
}

BigInt RSA_pwd::getD() {
	return this->d;
}

BigInt RSA_pwd::pow_modify(BigInt a, BigInt b)
{
    if (b == 0)
    {
        return 1;
    }
    return a * pow_modify(a, b - 1);
}

string RSA_pwd::encryptPassword(string pwd) {
    RSA_pwd a;
    string result = "";
    vector<BigInt> enstr;

    BigInt n = a.getN();
    BigInt e = a.getE();

    for (long long i = 0; i < pwd.length(); i++)
    {
        BigInt m = pwd[i];
        m = a.pow_modify(m, e) % n;
        enstr.push_back(m);
    }
    
    for (int i = 0; i < enstr.size(); i++) {
        result += char(enstr[i].to_int());
    }

    return result;
}

string RSA_pwd::decryptPassword(string pwd) {
    RSA_pwd a;
    string result = "";
    BigInt n = a.getN();
    BigInt d = a.getD();

    for (int i = 0; i < pwd.length(); i++)
    {
        BigInt m = pwd[i];
        m = a.pow_modify(m, d) % n;
        if (m < 0) {
            m = -m;
        }
        result += char(m.to_int());
    }

    return result;
}
