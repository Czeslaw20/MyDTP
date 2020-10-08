#pragma once
#include <iostream>
#include <openssl/aes.h>
using namespace std;

class AesCrypto
{
public:
    //可使用16byte，24byte，32byte的密钥
    AesCrypto(string key);
    ~AesCrypto();

    //加密
    string aesCBCEncrypt(string text);

    //解密
    string aesCBCDecrypt(string encStr);

private:
    string aesCrypto(string data, int crypto);
    void generateIvec(unsigned char *ivec);

private:
    AES_KEY m_encKey;
    AES_KEY m_decKey;

    string m_key; //密钥
};
