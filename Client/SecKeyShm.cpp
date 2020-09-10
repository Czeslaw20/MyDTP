#include "SecKeyShm.h"

SecKeyShm::SecKeyShm(int key)
{
}
SecKeyShm::SecKeyShm(int key, int m_maxNode)
{
}
SecKeyShm::SecKeyShm(string name)
{
}
SecKeyShm::SecKeyShm(string name, int m_maxNode)
{
}

int shmWrite(string name, int maxnode)
{
}
int shmRead(string clientID, string serverID, NodeSHMInfo *pNodeInfo)
{
}

SecKeyShm::~SecKeyShm()
{
}