#include "ClientOP.h"
#include "RequestFactory.h"
#include "RespondFactory.h"
#include "RsaCrypto.h"
#include <fstream>
#include <sstream>
#include "TcpSocket.h"
#include <json/json.h>
#include "Hash.h"

ClientOP::ClientOP(string fileName)
{
    //1.读磁盘的json文件

    //1.3 Value -> m_info
    //1.1 创建流对象->读文件 ifstream
    ifstream ifs(fileName);
    //1.2 使用CJson Reader -> Value
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifs, root);
    //将数据从Value中取出
    m_info.clientID = root["clientID"].asString();
    m_info.serverID = root["serverID"].asString();
    m_info.serverIP = root["serverIP"].asString();
    m_info.shmKey = root["shmKey"].asString();
    m_info.port = root["serverPort"].asInt();
    m_info.maxNode = root["maxNode"].asInt();

    //实例化共享内存对象
    m_shm = new SecKeyShm(m_info.shmKey, m_info.maxNode);
    //清空共享内存
    m_shm->shmInit();
}

ClientOP::~ClientOP()
{
    delete m_shm;
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
    //序列化的类对象通过 -> 工厂类的工厂函数创建
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
    ifstream ifs("public.pem");
    //通过文件流 -> 字符串流
    stringstream strBuf;
    strBuf << ifs.rdbuf();

    //创建哈希对象
    Hash hash(T_SHA1);
    hash.addData(strBuf.str());
    string sh1 = hash.result();

    reqInfo.sign = crypto.rsaSign(sh1);
    cout << "签名成功..." << endl;
    cout << "公钥信息: " << endl;
    // cout << strBuf.str() << endl;

    // cout << "发送的公钥hash后：" << sh1 << endl;
    // cout << "发送的签名：" << reqInfo.sign << endl;

    //获取给服务器发送的数据 -> rsa公钥
    reqInfo.data = strBuf.str();
    cout << reqInfo.data << endl;
    CodecFactory *factory = new RequestFactory(&reqInfo);
    Codec *codec = factory->createCodec();
    string encodeMsg = codec->encodeMsg();
    cout << "数据编码完成..." << endl;
    delete factory;
    delete codec;

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
    bool ret = true;
    if (!resMsg->status())
    {
        // return false;
        cout << "服务器处理请求失败..." << endl;
        ret = false;
    }
    else
    {
        //5. 通过私钥将data中的密钥解析出来
        string aeskey = crypto.rsaPriKeyDecrypt(resMsg->data());
        cout << "对称加密的秘钥: " << aeskey << endl;

        //6. 将aeskey存储到共享内存中
        NodeSHMInfo shmNode;
        //data()和c_str()的区别：c_str()遇到\0即停止
        strcpy(shmNode.clientID, m_info.clientID.data());
        strcpy(shmNode.serverID, m_info.serverID.data());
        strcpy(shmNode.secKey, aeskey.data());
        //可用1，不可用0，秘钥注销之后设置为不可用
        shmNode.status = 0; //秘钥是否可用
        shmNode.seckeyID = resMsg->seckeyid();
        m_shm->shmWrite(&shmNode);
    }

    //7. 释放资源
    delete factory;
    delete codec;

    //8. 关闭套接字
    socket->disconnect();
    delete socket;

    return ret;
}

//2.密钥校验
void ClientOP::seckeyCheck()
{
}

//3.密钥注销
void ClientOP::seckeyCancel()
{
}
