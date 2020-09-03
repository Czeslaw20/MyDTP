#pragma once
#include "CodecFactory.h"
#include "Response.h"
#include "Codec.h"

class RespondFactory : public CodecFactory
{
public:
    RespondFactory(string enc);
    RespondFactory(RespondInfo *info);
    Codec *createCodec();

    ~RespondFactory();

private:
    bool m_flag;
    string m_encStr;
    RespondInfo *m_info;
};