#pragma once
#include "BaseShm.h"
#include <string.h>

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
    char clientID[128];
    char serverID[128];
    char secKey[128];
};

class SecKeyShm : public BaseShm
{
public:
    SecKeyShm(int key, int maxNode);
    SecKeyShm(string pathName, int maxNode);
    ~SecKeyShm();

    void shmInit();
    int shmWrite(NodeSHMInfo *pNodeInfo);
    NodeSHMInfo shmRead(string clientID, string serverID);

private:
    int m_maxNode;
};
