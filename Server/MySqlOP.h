#pragma once
#include <string>
#include <mysql/mysql.h>
#include "SecKeyShm.h"
using namespace std;

class MySqlOP
{
public:
    MySqlOP();
    ~MySqlOP();

    //初始化环境连接数据库
    bool connectMySql(string user, string passwd);
    //得到keyID
    int getKeyID();
    bool updateKeyID(int keyID);
    bool writeSecKey(NodeSHMInfo *pNode);
    void closeDB();

private:
    MYSQL *m_mysql;
};
