#include "Response.h"

RespondCodec::RespondCodec()
{
}

RespondCodec::RespondCodec(string enc)
{
    initMessage(enc);
}

RespondCodec::RespondCodec(int status, int seckeyID, string clientID,
                           string serverID, string data)
{
    initMessage(status, seckeyID, clientID, serverID, data);
}

void RespondCodec::initMessage(string enc)
{
    m_encStr = enc;
}

void RespondCodec::initMessage(int status, int seckeyID, string clientID,
                               string serverID, string data)
{
    m_msg.set_status(status);
    m_msg.set_seckeyid(seckeyID);
    m_msg.set_clientid(clientID);
    m_msg.set_serverid(serverID);
    m_msg.set_data(data);
}

string RespondCodec::encodeMsg()
{
    string output;
    m_msg.SerializeToString(&output);
    return output;
}

void *RespondCodec::decodeMsg()
{
    m_msg.ParseFromString(m_encStr);
    cout << "status: " << m_msg.status()
         << ", seckeyID: " << m_msg.seckeyid()
         << ", clientID: " << m_msg.clientid()
         << ", serverID: " << m_msg.serverid()
         << ", data: " << m_msg.data() << endl;
    return &m_msg;
}

RespondCodec::~RespondCodec()
{
}