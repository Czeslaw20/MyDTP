#include "MySqlOP.h"
#include <iostream>

static const char *DB_HOST = "127.0.0.1";
static const char *DB_NAME = "SECMNG";
static const unsigned short DB_PORT = 3306;

MySqlOP::MySqlOP()
{
    m_mysql = NULL;
}

MySqlOP::~MySqlOP()
{
}

bool MySqlOP::connectMySql(string user, string passwd)
{
    //1.初始化
    m_mysql = mysql_init(NULL);
    if (!m_mysql)
    {
        fprintf(stderr, "Error in mysql_init()\n");
        return false;
    }
    //2.建立连接
    if (!mysql_real_connect(
            m_mysql,       //mysql_init得到的句柄
            DB_HOST,       //mysql服务器地址
            user.data(),   //mysql服务器登陆的用户名
            passwd.data(), //mysql用户登陆的密码
            DB_NAME,       //登陆之后默认使用哪个数据库
            DB_PORT,       //mysql服务器端口号 3306
            NULL,          //如果服务器是以本地套接字部署的，这里填本地套接字地址
            0              //默认填0
            ))
    {
        fprintf(stderr, "Error in mysql_real_connect():%s\n",
                mysql_error(m_mysql));
        return false;
    }
    else
    {
        printf("连接数据库成功\n");
    }
    return true;
}

int MySqlOP::getKeyID()
{
    int keyID = -1;
    //for update ：临时对数据表加锁
    string sql = "select ikeysn from KEYSN for update";

    //执行sql，返回结果集
    if (mysql_query(m_mysql, sql.c_str()))
    //成功则返回0，失败返回非零值
    {
        fprintf(stderr, "Error in mysql_query :%s\n", mysql_error(m_mysql));
        return keyID;
    }

    printf("Sql execute successed\n");

    //获取结果集
    MYSQL_RES *res = mysql_store_result(m_mysql);
    MYSQL_ROW row = NULL;
    row = mysql_fetch_row(res);
    keyID = atoi(row[0]);

    mysql_free_result(res);

    return keyID;
}

bool MySqlOP::updateKeyID(int keyID)
{
    string sql = "UPDATE KEYSN set ikeysn = " + to_string(keyID);

    //执行sql，返回结果集
    if (mysql_query(m_mysql, sql.c_str()))
    //成功则返回0，失败返回非零值
    {
        fprintf(stderr, "Error in mysql_query :%s\n", mysql_error(m_mysql));
        return false;
    }
    printf("Sql execute successed\n");
    return true;
}

bool MySqlOP::writeSecKey(NodeSHMInfo *pNode)
{
    //组织待插入的sql语句
    char sql[1024] = {0};
    cout << "clientid: " << pNode->clientID << endl;
    cout << "serverID: " << pNode->serverID << endl;

    sprintf(sql, "INSERT INTO SECKEYINFO VALUE('%s','%s',%d,now(),%d,'%s')",
            pNode->clientID, pNode->serverID, pNode->seckeyID, 1, pNode->secKey);
    cout << "insert sql: " << sql << endl;
    //执行sql，返回结果集
    if (mysql_query(m_mysql, sql))
    //成功则返回0，失败返回非零值
    {
        fprintf(stderr, "Error in mysql_query :%s\n", mysql_error(m_mysql));
        return false;
    }
    printf("Sql execute successed\n");
    return true;
}

void MySqlOP::closeDB()
{
    mysql_close(m_mysql);
}
