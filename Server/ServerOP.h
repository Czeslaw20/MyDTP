#pragma once
#include "TcpServer.h"
#include "TcpSocket.h"
#include <string>
#include <map>
#include "msg.pb.h"

//存储磁盘配置文件信息
struct ServerInfo
{
    std::string serverID;
    std::string dbUser;
    std::string dbPasswd;
    std::string dbSID;
    unsigned short port;
    int maxnode;
    std::string shmkey;
};

enum KeyLen
{
    LenSmall = 16,
    LenNormal = 24,
    LenLarge = 32
};

class ServerOP
{
public:
    ServerOP(std::string jsonfile);
    ~ServerOP();

    //操作
    void startServer();
    string seckeyAgree(RequestMsg *msg);

    friend void *working(void *arg);
    //线程的回调函数
    //static void *hardWork(void *arg) {}
private:
    string getRandStr(KeyLen num);

private:
    ServerInfo m_info;
    map<pthread_t, TcpSocket *> m_socket;
    TcpServer *m_server;
};

//子线程的回调函数
void *working(void *arg);
//回调不能写成类成员，因为与pthread_create参数类型不符
