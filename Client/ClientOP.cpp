#include "ClientOP.h"
#include "RequestFactory.h"
#include "RespondFactory.h"
#include "RsaCrypto.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "TcpSocket.h"

using namespace std;

ClientOP::ClientOP(string filename)
{
    //1.读磁盘的json文件
    //1.1 创建流对象->读文件 ifstream
    //1.2 使用CJson Reader -> Value
    //1.3 Value -> m_info
}

//1.密钥协商
/*
    准备：对称加密的密钥，通过非对称加密的方式完成密钥交换
    数据发送：通过protobuf序列化之后的结构体 -> A
    数据的接收：通过protobuf序列化之后的结构体 -> B
*/
bool ClientOP::seckeyAgree()
{
    //1.准备数据，并序列化
    //序列化的类处理什么类型，就准备什么类型的数据
    //序列化的类对象通过 -> 工厂类的工厂函数创建的
    //1.1 创建工厂对象
    RequestInfo reqInfo;
    reqInfo.clientID = m_info.clientID;
    reqInfo.serverID = m_info.serverID;
    reqInfo.cmd = 1;
    //1.2 生成rsa密钥对
    RsaCrypto crypto;
    crypto.generateRsaKey(1024);
    //获取签名数据 -> 对rsa公钥签名
    //1.3 将磁盘中的公钥文件内容读出
    ifstream ifs;
    //通过文件流 -> 字符串流
    stringstream strBuf;
    strBuf << ifs.rdbuf();
    reqInfo.sign = crypto.rsaSign(strBuf.str());
    cout << "公钥信息:" << endl;
    cout << strBuf.str() << endl;

    //获取给服务器发送的数据 -> rsa公钥
    reqInfo.data = strBuf.str();
    CodecFactory *factory = new RequestFactory(&reqInfo);
    Codec *codec = factory->createCodec();
    string encodeMsg = codec->encodeMsg();
    cout << "数据编码完成……" << endl;

    //2.将编码之后的数据发送
    //2.1 连接服务器
    TcpSocket *socket = new TcpSocket();
    //2.2连接
    socket->connectToHost(m_info.serverIP, m_info.port);
    //2.3 发送数据
    socket->sendMsg(encodeMsg);
    //2.4等待并接收服务器数据
    string recvMsg = socket->recvMsg();

    //3.解码服务器回复的数据
    //3.1 创建解码数据的对象
    factory = new RespondFactory(recvMsg);
    codec = factory->createCodec();
    RespondMsg *resMsg = (RespondMsg *)codec->decodeMsg();

    //4. 判断状态
    if (!resMsg->status())
    {
        return false;
    }

    //5. 通过私钥将data中的密钥解析出来
    string aesKey = crypto.rsaPriKeyDecrypt(resMsg->data());

    //6. 将aeskey存储到共享内存中

    //7. 释放资源

    //8. 关闭套接字

    return true;
}

//2.密钥校验
void ClientOP::seckeyCheck()
{
}

//3.密钥注销
void ClientOP::seckeyCancel()
{
}

ClientOP::~ClientOP()
{
}