#include "ServerOP.h"
#include <pthread.h>
#include "RequestFactory.h"
#include "RespondFactory.h"
#include <json/json.h>
#include <fstream>
#include <unistd.h>
#include "RsaCrypto.h"
#include <fstream>

using namespace std;

ServerOP::ServerOP(string jsonfile)
{
    //加载配置文件
    //1.从磁盘读文件内容
    ifstream ifs(jsonfile);
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifs, root);
    //将数据从Value中取出
    m_info.dbPasswd = root["dbPasswd"].asString();
    m_info.dbSID = root["dbSID"].asString();
    m_info.dbUser = root["dbUser"].asString();
    m_info.serverID = root["serverID"].asString();
    m_info.shmkey = root["shmkey"].asString();
    m_info.port = root["port"].asInt();
    m_info.maxnode = root["maxnode"].asInt();
}

ServerOP::~ServerOP()
{
    delete m_server;
}

void ServerOP::startServer()
{
    //创建tcpserver对象
    m_server = new TcpServer();
    m_server->setListen(m_info.port);
    //接收连接请求
    while (true)
    {
        TcpSocket *pSocket = m_server->acceptConn();
        if (pSocket == NULL)
        {
            cout << "accept 错误, 继续检测……" << endl;
            continue;
        }
        cout << "accept success" << endl;
        //通信
        pthread_t tid;
        pthread_create(&tid, NULL, working, this);
        pthread_detach(tid);
        //通信的套接字放到容器中
        m_socket.insert(make_pair(tid, pSocket));
    }
}

string ServerOP::seckeyAgree(RequestMsg *msg)
{
    //2 将得到的公钥写入服务器磁盘
    //写文件，文件名：客户端ID
    string filename = msg->clientid() + ".pem";
    ofstream ofs(filename);
    ofs << msg->data();
    cout << "磁盘上生成了公钥文件……" << endl;
    RsaCrypto rsa(filename, false);
    //1 校验签名
    //保证rsa对象中公钥是可用的
    RespondInfo info;
    bool bl = rsa.rsaVerify(msg->data(), msg->sign());
    if (bl == false)
    {
        cout << "签名校验失败……" << endl;
        info.status = false;
    }
    else
    {
        info.status = true;
        cout << "签名校验成功……" << endl;
        //3 生成一个随机字符串 -> 16/24/32byte -> 对称加密的密钥
        string seckey = getRandStr(LenNormal);
        //4 回复数据 结构体
        //5 数据初始化->密钥使用公钥加密
        info.clientID = msg->clientid();
        info.serverID = m_info.serverID;
        //需要数据操作，目前直接赋值即可
        info.seckeyID = 123; //对称加密的密钥的ID
        info.data = rsa.rsaPubKeyEncrypt(seckey);

        //生成序列化对象
        CodecFactory *factory = new RespondFactory(&info);
        Codec *codec = factory->createCodec();
        //6 通过工厂函数创建编码对象 -> 编码
        string sendMsg = codec->encodeMsg();
        //7 要回复给客户端的数据得到了
    }
    //生成序列化对象
    CodecFactory *factory = new RespondFactory(&info);
    Codec *codec = factory->createCodec();
    //6 通过工厂函数创建编码对象 -> 编码
    string sendMsg = codec->encodeMsg();
    //7 要回复给客户端的数据得到了

    return sendMsg;
}

//参数：num == 16 24 32
//改称枚举类型
//字符串中都哟i什么？ 1-9 a-z A-Z 特殊字符
string ServerOP::getRandStr(KeyLen num)
{
    srand(time(NULL));
    string retStr = string();
    char *buf = "~@#$%&*(){}[];-=+':";
    for (int i = 0; i < num; i++)
    {
        int flag = rand() % 4;
        switch (flag)
        {
        case 0: //0-9
            retStr.append(1, rand() % 10 + '0');
            break;
        case 1: //a-z
            retStr.append(1, rand() % 26 + 'a');
            break;
        case 2: //A-Z
            retStr.append(1, rand() % 26 + 'A');
            break;
        case 3: //特殊字符
            retStr.append(1, buf[rand() % strlen(buf)]);
            break;
        }
    }
    return retStr;
}

void *working(void *arg)
{
    usleep(100); //防止子线程运行时socket还没被写入容器内
    //得到通信的套接字
    ServerOP *op = (ServerOP *)arg;
    //得到通信的套接字对象
    TcpSocket *pSocket = op->m_socket[pthread_self()];
    // TcpSocket *pSocket = (TcpSocket *)arg;
    //接收数据->编码之后的
    string recvMsg = pSocket->recvMsg();
    //对接收的数据解码
    CodecFactory *factory = new RequestFactory(recvMsg);
    Codec *codec = factory->createCodec();
    RequestMsg *reqMsg = (RequestMsg *)codec->decodeMsg();

    //根据cmd判断客户端的请求
    string str;
    switch (reqMsg->cmdtype())
    {
    case 1:
        //密钥协商
        str = op->seckeyAgree(reqMsg);
        break;
    case 2:
        //密钥校验

        break;
    case 3:
        //密钥注销

        break;
    }
    pSocket->sendMsg(str);
    cout << "数据已经发送给了客户端" << endl;
    pSocket->disconnect();
    //将pSocket对象从map删除
    //资源释放

    return nullptr;
}
