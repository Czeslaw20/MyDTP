#include "Response.h"

RespondCodec::RespondCodec()
{
}

RespondCodec::RespondCodec(string enc)
{
    initMessage(enc);
}

RespondCodec::RespondCodec(RespondInfo *info)
{
    initMessage(info);
}

void RespondCodec::initMessage(string enc)
{
    m_encStr = enc;
}

void RespondCodec::initMessage(RespondInfo *info)
{
    m_msg.set_status(info->status);
    m_msg.set_seckeyid(info->seckeyID);
    m_msg.set_clientid(info->clientID);
    m_msg.set_serverid(info->serverID);
    m_msg.set_data(info->data);
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
    // cout << "status: " << m_msg.status()
    //      << ", seckeyID: " << m_msg.seckeyid()
    //      << ", clientID: " << m_msg.clientid()
    //      << ", serverID: " << m_msg.serverid()
    //      << ", data: " << m_msg.data() << endl;
    return &m_msg;
}

RespondCodec::~RespondCodec()
{
}