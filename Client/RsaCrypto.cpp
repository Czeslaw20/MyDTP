#include "RsaCrypto.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <iostream>
#include <openssl/buffer.h>
#include <string.h>

RsaCrypto::RsaCrypto()
{
    m_publicKey = RSA_new();
    m_privateKey = RSA_new();
}

RsaCrypto::RsaCrypto(string fileName, bool isPrivate)
{
    m_publicKey = RSA_new();
    m_privateKey = RSA_new();
    if (isPrivate)
    {
        initPrivateKey(fileName);
    }
    else
    {
        initPublicKey(fileName);
    }
}

RsaCrypto::~RsaCrypto()
{
    RSA_free(m_publicKey);
    RSA_free(m_privateKey);
}

void RsaCrypto::parseKeyString(string keystr, bool pubKey)
{
    //字符串数据 -> BIO对象中
    BIO *bio = BIO_new_mem_buf(keystr.data(), keystr.size());
    //公钥字符串
    if (pubKey)
    {
        PEM_read_bio_RSAPublicKey(bio, &m_publicKey, NULL, NULL);
    }
    //私钥字符串
    else
    {
        PEM_read_bio_RSAPrivateKey(bio, &m_privateKey, NULL, NULL);
    }
    BIO_free(bio);
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

//公钥加密
string RsaCrypto::rsaPubKeyEncrypt(string data)
{
    cout << "加密数据长度: " << data.size() << endl;
    // 计算公钥长度
    int keyLen = RSA_size(m_publicKey);
    cout << "pubKey len: " << keyLen << endl;
    //申请内存空间
    char *encode = new char[keyLen + 1];
    //使用公钥加密
    int ret = RSA_public_encrypt(data.size(), (const unsigned char *)data.data(),
                                 (unsigned char *)encode, m_publicKey, RSA_PKCS1_PADDING);
    string retStr = string();
    if (ret >= 0)
    {
        //加密成功
        cout << "ret: " << ret << ", keyLen: " << keyLen << endl;
        retStr = toBase64(encode, ret);
    }
    else
    {
        ERR_print_errors_fp(stdout);
    }
    //释放资源
    delete[] encode;
    return retStr;
}

//私钥解密
string RsaCrypto::rsaPriKeyDecrypt(string encData)
{
    //base64解码
    char *text = fromBase64(encData);
    //计算私钥长度
    int keyLen = RSA_size(m_privateKey);
    char *decode = new char[keyLen + 1];
    //使用私钥解密
    //rsa加密数据最大长度 密钥长度-11  加密完成后密文长度==密钥长度
    int ret = RSA_private_decrypt(keyLen, (const unsigned char *)text,
                                  (unsigned char *)decode, m_privateKey, RSA_PKCS1_PADDING);
    string retStr = string();
    if (ret >= 0)
    {
        retStr = string(decode, ret);
    }
    else
    {
        cout << "私钥解密失败..." << endl;
        ERR_print_errors_fp(stdout);
    }
    //释放资源
    delete[] decode;
    delete[] text;
    return retStr;
}

//使用RSA签名
string RsaCrypto::rsaSign(string data, SignLevel level)
{
    unsigned int len;
    char *signBuf = new char[1024];
    memset(signBuf, 0, 1024);
    int ret = RSA_sign(level, (const unsigned char *)data.data(), data.size(), (unsigned char *)signBuf,
                       &len, m_privateKey);
    if (ret == -1)
    {
        ERR_print_errors_fp(stdout);
    }
    // cout << "使用base64编码前的sign" << signBuf << endl;

    cout << "sign len: " << len << ", ret: " << ret << endl;

    string retStr = toBase64(signBuf, len);
    // string retStr = string(signBuf, len);

    delete[] signBuf;
    return retStr;
}

//使用RSA验证签名
bool RsaCrypto::rsaVerify(string data, string signData, SignLevel level)
{
    // cout << "data:" << data << endl;
    int keyLen = RSA_size(m_publicKey);

    char *sign = fromBase64(signData);
    // const char *sign = signData.c_str();

    // cout << "base64解码后的sign: " << sign << endl;
    // cout << "校验的data：" << data.data() << endl;

    int ret = RSA_verify(level, (const unsigned char *)data.data(), data.size(),
                         (const unsigned char *)sign, keyLen, m_publicKey);
    delete[] sign;
    cout << " 校验结果ret的值： " << ret << endl;

    if (ret == -1)
    {
        ERR_print_errors_fp(stdout);
    }
    if (ret != 1)
    {
        return false;
    }
    return true;
}

//base64编码
string RsaCrypto::toBase64(const char *str, int len)
{
    //准备一个 内存的 BIO对象
    BIO *mem = BIO_new(BIO_s_mem());
    //准备一个base64 BIO对象
    BIO *bs64 = BIO_new(BIO_f_base64());

    //将base64和内存BIO对象 -> BIO链
    bs64 = BIO_push(bs64, mem);
    //数据通过base64进行编码之后->存储到内存
    //往BIO链中写数据
    //调用bio_write,base64对象默认做编码操作
    BIO_write(bs64, str, len);
    BIO_flush(bs64);
    //将编码数据从bio内存对象中读出
    BUF_MEM *memPtr;
    BIO_get_mem_ptr(bs64, &memPtr);

    string retStr = string(memPtr->data, memPtr->length - 1);
    BIO_free_all(bs64);

    return retStr;
}

//base64解码
char *RsaCrypto::fromBase64(string str)
{
    //准备一个base64 BIO对象
    int length = str.size();
    BIO *bs64 = BIO_new(BIO_f_base64());
    //准备一个 内存的 BIO对象
    BIO *mem = BIO_new_mem_buf(str.data(), length);
    //将base64和内存BIO对象 -> BIO链
    BIO_push(bs64, mem);
    // BIO_push(mem, b64);
    //解码
    char *buffer = new char[length];
    memset(buffer, 0, length);
    BIO_read(bs64, buffer, length);
    BIO_free_all(bs64);
    // BIO_read(mem, buf, length);
    // BIO_free_all(mem);
    return buffer;
}
