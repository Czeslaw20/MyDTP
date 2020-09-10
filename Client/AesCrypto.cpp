#include "AesCrypto.h"

AesCrypto::AesCrypto(string key)
{
    if (key.size() == 16 || key.size() == 24 || key.size() == 32)
    {
        m_key = key;
        const unsigned char *aesKey = (const unsigned char *)key.data();
        AES_set_encrypt_key(aesKey, key.size() * 8, &m_encKey);
        AES_set_decrypt_key(aesKey, key.size() * 8, &m_decKey);
    }
}

AesCrypto::~AesCrypto()
{
}

//加密
string AesCrypto::aesCBCEncrypt(string text)
{
    return aesCrypto(text, AES_ENCRYPT);
}

//解密
string AesCrypto::aesCBCDecrypt(string encStr)
{
    return aesCrypto(encStr, AES_DECRYPT);
}

void AesCrypto::generateIvec(unsigned char *ivec)
{
    for (int i = 0; i < AES_BLOCK_SIZE; i++)
    {
        ivec[i] = m_key.at(AES_BLOCK_SIZE - i - 1);
    }
}

string AesCrypto::aesCrypto(string data, int crypto)
{
    AES_KEY *key = crypto == AES_ENCRYPT ? &m_encKey : &m_decKey;

    unsigned char ivec[AES_BLOCK_SIZE];
    //加密
    int length = data.size() + 1; //加的是\0
    if (length % 16)
    {
        length = (length / 16 + 1) * 16; //必须是16的整数倍
    }
    char *out = new char[length];
    generateIvec(ivec);

    AES_cbc_encrypt((const unsigned char *)data.c_str(),
                    (unsigned char *)out, length,
                    key, ivec, crypto);

    string retStr = string(out);
    delete[] out;
    return string(retStr);
}
