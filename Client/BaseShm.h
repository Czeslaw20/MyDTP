#pragma once
#include <string>
//#include <iostream>
using namespace std;

class BaseShm
{
public:
    //通过key打开共享内存
    BaseShm(int key);
    //通过key创建/打开共享内存
    BaseShm(int key, int size);

    //通过路径打开共享内存
    BaseShm(string name);
    //通过路径创建/打开共享内存

    BaseShm(string name, int size);

    void *mapShm();
    int unmapShm();
    int delShm();

    ~BaseShm();

private:
    int getShmID(int key, int shmSize, int flag);

private:
    int m_shmID;
    void *m_shmAddr = NULL;
};