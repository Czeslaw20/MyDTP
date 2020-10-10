#pragma once
#include <string>
using namespace std;

struct ClientInfo
{
    string serverID;
    string clientID;
    string serverIP;
    unsigned short port;
    int maxNode;
    string shmKey;
};

class ClientOP
{
public:
    //初始化数据，从磁盘的配置文件中
    ClientOP(string fileName);
    ~ClientOP();

    //1.密钥协商
    bool seckeyAgree();

    //2.密钥校验
    void seckeyCheck();

    //3.密钥注销
    void seckeyCancel();

private:
    ClientInfo m_info;
};