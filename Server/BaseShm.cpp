#include "BaseShm.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>

using namespace std;

const char RandX = 'x';
BaseShm::BaseShm(int key)
{
    getShmID(key, 0, 0);
}

BaseShm::BaseShm(int key, int size)
{
    getShmID(key, size, IPC_CREAT | 0664);
}

BaseShm::BaseShm(string name)
{
    //string转char*有两种方法 name.data()  name.c_str()
    key_t key = ftok(name.data(), RandX);
    getShmID(key, 0, 0);
}

BaseShm::BaseShm(string name, int size)
{
    key_t key = ftok(name.data(), RandX);
    getShmID(key, size, IPC_CREAT | 0664);
}

void *BaseShm::mapShm()
{
    m_shmAddr = shmat(m_shmID, NULL, 0);
    if (m_shmAddr == (void *)(-1))
    {
        return NULL;
    }
    return m_shmAddr;
}

int BaseShm::unmapShm()
{
    int ret = shmdt(m_shmAddr);
    return ret;
}

int BaseShm::delShm()
{
    int ret = shmctl(m_shmID, IPC_RMID, NULL);
    return ret;
}

int BaseShm::getShmID(int key, int shmsize, int flag)
{
    cout << "shared memory size:" << shmsize << endl;
    m_shmID = shmget(key, shmsize, flag);
    if (m_shmID == -1)
    {
        //写log日志
        cout << "shmget 失败" << endl;
    }
    return m_shmID;
}

BaseShm::~BaseShm()
{
}