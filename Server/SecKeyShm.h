#pragma once
#include "BaseShm.h"

class NodeSHMInfo
{
public:
    NodeSHMInfo() : status(0), seckeyID(0)
    {
        bzero(clientID, sizeof(clientID));
        bzero(serverID, sizeof(serverID));
        bzero(secKey, sizeof(secKey));
    }
    int status;
    int seckeyID;
    char clientID[12];
    char serverID[12];
    char secKey[128];
};

class SecKeyShm : public BaseShm
{
public:
    SecKeyShm(int key, int maxNode);
    SecKeyShm(string pathname, int maxNode);

    void shmInit();
    int shmWrite(NodeSHMInfo *pNodeInfo);
    NodeSHMInfo shmRead(string clientID, string serverID);
    ~SecKeyShm();

private:
    int m_maxNode;
};
