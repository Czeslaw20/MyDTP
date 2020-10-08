#pragma once
#include <string>

struct ClientInfo
{
    std::string serverID;
    std::string clientID;
    std::string serverIP;
    unsigned short port;
    int maxNode;
    std::string shmKey;
};

class ClientOP
{
public:
    //初始化数据，从磁盘的配置文件中
    ClientOP(std::string filename);
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