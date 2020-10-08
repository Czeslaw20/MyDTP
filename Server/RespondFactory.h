#pragma once
#include "CodecFactory.h"
#include "RespondCodec.h"
#include "Codec.h"

class RespondFactory : public CodecFactory
{
public:
    //创建解码对象
    RespondFactory(string enc);
    //创建编码对象
    RespondFactory(RespondInfo *info);
    Codec *createCodec();

    ~RespondFactory();

private:
    bool m_flag;
    string m_encStr;
    RespondInfo *m_info;
};