#pragma once
#include "Codec.h"
#include "msg.pb.h"

class RequestCodec : public Codec
{
public:
    RequestCodec();
    RequestCodec(string encstr);
    RequestCodec(int cmd, string clientID, string serverID,
                 string sign, string data);
    void initMessage(string encstr);
    void initMessage(int cmd, string clientID, string serverID,
                     string sign, string data);
    string encodeMsg();
    void *decodeMsg();
    ~RequestCodec();

private:
    string m_encStr;
    RequestMsg m_msg;
};