#include "Request.h"

RequestCodec::RequestCodec()
{
}

RequestCodec::RequestCodec(string encstr)
{
    initMessage(encstr);
}

RequestCodec::RequestCodec(int cmd, string clientID,
                           string serverID, string sign, string data)
{
    initMessage(cmd, clientID, serverID, sign, data);
}

void RequestCodec::initMessage(string encstr)
{
    m_encStr = encstr;
}

void RequestCodec::initMessage(int cmd, string clientID,
                               string serverID, string sign, string data)
{
    m_msg.set_cmdtype(cmd);
    m_msg.set_clientid(clientID);
    m_msg.set_serverid(serverID);
    m_msg.set_sign(sign);
    m_msg.set_data(data);
}

string RequestCodec::encodeMsg()
{
    string output;
    m_msg.SerializeToString(&output);
    return output;
}

void *RequestCodec::decodeMsg()
{
    m_msg.ParseFromString(m_encStr);
    cout << "cmdtype: " << m_msg.cmdtype()
         << ", clientID: " << m_msg.clientid()
         << ", serverID: " << m_msg.serverid()
         << ", sign: " << m_msg.sign()
         << ", data: " << m_msg.data() << endl;
    return &m_msg;
}

RequestCodec::~RequestCodec()
{
}