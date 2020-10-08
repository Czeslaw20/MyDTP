#include "RsaCrypto.h"
#include <openssl/bio.h>
#include <openssl/err.h>

RsaCrypto::RsaCrypto()
{
    m_publicKey = RSA_new();
    m_privateKey = RSA_new();
}

RsaCrypto::RsaCrypto(string filename, bool isPrivate)
{
    m_publicKey = RSA_new();
    m_privateKey = RSA_new();
    if (isPrivate)
    {
        initPrivateKey(filename);
    }
    else
    {
        initPublicKey(filename);
    }
}

RsaCrypto::~RsaCrypto()
{
    RSA_free(m_publicKey);
    RSA_free(m_privateKey);
}

//生成RSA密钥对
void RsaCrypto::generateRsaKey(int bits, string pub, string pri)
{
    RSA *r = RSA_new();
    //创将bignum对象
    BIGNUM *e = BN_new();
    BN_set_word(e, 456787);
    RSA_generate_key_ex(r, bits, e, NULL);

    //创建bio对象
    BIO *pubIO = BIO_new_file(pub.data(), "w");
    PEM_write_bio_RSAPublicKey(pubIO, r);
    //强制将内存中的数据刷到磁盘上
    BIO_flush(pubIO);
    BIO_free(pubIO);

    BIO *priIO = BIO_new_file(pri.data(), "w");
    PEM_write_bio_RSAPrivateKey(priIO, r, NULL, NULL, 0, NULL, NULL);
    //强制将内存中的数据刷到磁盘上
    BIO_flush(priIO);
    BIO_free(priIO);

    //得到公钥和私钥
    m_privateKey = RSAPrivateKey_dup(r);
    m_publicKey = RSAPublicKey_dup(r);

    //释放资源
    BN_free(e);
    RSA_free(r);
}

//公钥加密
string RsaCrypto::rsaPubKeyEncrypt(string data)
{
    //计算公钥长度
    int keyLen = RSA_size(m_publicKey);
    cout << "pubKey len: " << keyLen << endl;
    //申请内存空间
    char *encode = new char[keyLen + 1];
    //使用公钥加密
    int ret = RSA_public_encrypt(data.size(),
                                 (const unsigned char *)data.data(),
                                 (unsigned char *)encode,
                                 m_publicKey, RSA_PKCS1_PADDING);
    string retStr = string();
    if (ret >= 0)
    {
        //加密成功
        retStr = string(encode, ret);
    }
    //释放资源
    delete[] encode;
    return retStr;
}

//私钥解密
string RsaCrypto::rsaPriKeyDecrypt(string encData)
{
    //计算私钥长度
    int keyLen = RSA_size(m_privateKey);
    char *decode = new char[keyLen + 1];
    //使用私钥解密
    int ret = RSA_private_encrypt(encData.size(),
                                  (const unsigned char *)encData.data(),
                                  (unsigned char *)decode,
                                  m_privateKey, RSA_PKCS1_PADDING);
    string retStr = string();
    if (ret >= 0)
    {
        retStr = string(decode, ret);
    }
    //释放资源
    delete[] decode;
    return retStr;
}

//使用RSA签名
string RsaCrypto::rsaSign(string data, SignLevel level)
{
    unsigned int len;
    char *signBuf = new char[RSA_size(m_privateKey) + 1];
    RSA_sign(level, (const unsigned char *)data.data(),
             data.size(), (unsigned char *)signBuf,
             &len, m_privateKey);
    cout << "sign len: " << len << endl;
    string retStr = string(signBuf, len);
    delete[] signBuf;
    return retStr;
}

//使用RSA验证签名
bool RsaCrypto::rsaVerify(string data, string signData,
                          SignLevel level)
{
    int ret = RSA_verify(level, (const unsigned char *)data.data(),
                         data.size(),
                         (const unsigned char *)signData.data(),
                         signData.size(), m_publicKey);
    if (ret != 1)
    {
        return false;
    }
    return true;
}

bool RsaCrypto::initPublicKey(string pubfile)
{
    //通过bio读文件
    BIO *pubBio = BIO_new_file(pubfile.data(), "r");
    //将bio文件中的pem数据读出
    if (PEM_read_bio_RSAPublicKey(pubBio, &m_publicKey, NULL, NULL) == NULL)
    {
        ERR_print_errors_fp(stdout);
        return false;
    }
    BIO_free(pubBio);
    return true;
}

bool RsaCrypto::initPrivateKey(string prifile)
{
    //通过bio读文件
    BIO *priBio = BIO_new_file(prifile.data(), "r");
    //将bio文件中的pem数据读出
    if (PEM_read_bio_RSAPrivateKey(priBio, &m_privateKey, NULL, NULL) == NULL)
    {
        ERR_print_errors_fp(stdout);
        return false;
    }
    BIO_free(priBio);
    return true;
}