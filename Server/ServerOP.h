#pragma once
#include "TcpServer.h"
#include "TcpSocket.h"
#include <vector>
#include <map>
#include "msg.pb.h"
#include "SecKeyShm.h"
#include "MySqlOP.h"

//存储磁盘配置文件信息
struct ServerInfo
{
    string serverID;
    string dbUser;
    string dbPasswd;
    string dbSID;
    unsigned short port;
    int maxnode;
    string shmkey;
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
    ServerOP(string jsonfile);
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
    map<pthread_t, TcpSocket *> m_socket;
    ServerInfo m_info;
    TcpServer *m_server;
    SecKeyShm *m_shm;
    MySqlOP m_sqlop;
};

//子线程的回调函数
void *working(void *arg);
//回调不能写成类成员，因为与pthread_create参数类型不符
