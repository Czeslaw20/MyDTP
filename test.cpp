#include <iostream>
#include "Codec.h"
#include "Request.h"
#include "Response.h"
#include "Codec.h"

using namespace std;

string encode(Codec *code)
{
    Codec *c = code;
    return c->encodeMsg();
}

void *decode(Codec *code)
{
    return code->decodeMsg();
}

int main()
{
    RequestCodec req(1, "client", "server", "x00911",
                     "hello, world");
    string str = encode(&req);

    RequestCodec req1(str);
    RequestMsg *r1 = (RequestMsg *)decode(&req1);
    cout << "cmdtype: " << r1->cmdtype()
         << ", clientID: " << r1->clientid()
         << ", serverID: " << r1->serverid()
         << ", sign: " << r1->sign()
         << ", data: " << r1->data() << endl;

    cout << endl
         << "=============================" << endl;

    RespondCodec res(1, 999, "luffy", "zoro",
                     "change world 666 !");
    str = encode(&res);

    RespondCodec res1(str);
    RespondMsg *r2 = (RespondMsg *)decode(&res1);
    cout << "status: " << r2->status()
         << ", seckeyID: " << r2->seckeyid()
         << ", clientID: " << r2->clientid()
         << ", serverID: " << r2->serverid()
         << ", data: " << r2->data() << endl;

    return 0;
}