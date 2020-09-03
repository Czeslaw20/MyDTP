#pragma once
#include <iostream>
using namespace std;

class Codec
{
public:
    Codec();
    virtual string encodeMsg();
    virtual void *decodeMsg();
    virtual ~Codec();
};