#pragma once
#include "BaseShm.h"

struct NodeSHMInfo
{
    int status;
    int seckeyID;
    string clientID;
    string serverID;
    string secKey;
    string pubkey;
};

class SecKeyShm
{
public:
    SecKeyShm(int key);
    SecKeyShm(int key, int m_maxNode);
    SecKeyShm(string name);
    SecKeyShm(string name, int m_maxNode);

    int shmWrite(string name, int maxnode);
    int shmRead(string clientID, string serverID, NodeSHMInfo *pNodeInfo);

    ~SecKeyShm();

private:
    int m_maxNode;
};
