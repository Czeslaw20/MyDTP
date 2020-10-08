#include <iostream>
#include "ClientOP.h"

using namespace std;

int usage()
{
    int nSel = -1;
    printf("\n  /**************************************/");
    printf("\n  /**************************************/");
    printf("\n  /*****          1.密钥协商         *****/");
    printf("\n  /*****          2.密钥校验         *****/");
    printf("\n  /*****          3.密钥注销         *****/");
    printf("\n  /*****          4.密钥查看         *****/");
    printf("\n  /*****          0.退出系统         *****/");
    printf("\n  /**************************************/");
    printf("\n  /**************************************/");
    printf("\n\n 选择：");

    scanf("%d", &nSel);
    while (getchar() != '\n')
    {
    }
    return nSel;
}

int main()
{
    ClientOP op("client.json");
    //循环，接收用户的输入
    while (1)
    {
        int sel = usage();
        switch (sel)
        {
        case 1:
            //1.密钥协商
            op.seckeyAgree();
            break;
        case 2:
            //2.密钥校验
            op.seckeyCheck();
            break;
        case 3:
            //3.密钥注销
            op.seckeyCancel();
            break;
        }
    }

    return 0;
}