#pragma once
#include "Codec.h"
#include "msg.pb.h"

class RespondCodec : public Codec
{
public:
    RespondCodec();
    RespondCodec(string enc);
    RespondCodec(int status, int seckeyID, string clientID,
                 string serverID, string data);
    void initMessage(string enc);
    void initMessage(int status, int seckeyID, string clientID,
                     string serverID, string data);
    string encodeMsg();
    void *decodeMsg();
    ~RespondCodec();

private:
    string m_encStr;
    RespondMsg m_msg;
};