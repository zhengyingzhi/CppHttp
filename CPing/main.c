#include <stdio.h>

#include <Windows.h>

#include "cping.h"


int main(int argc, char* argv[])
{
    const char*  lpDestIP = "180.97.33.107";
    cping_t*     lPing;
    ping_reply_t lReply;
    int32_t      lMaxTimes = 1024;

    if (argc > 1)
    {
        lpDestIP = argv[1];
        if (strlen(lpDestIP) < 8 || !strstr(lpDestIP, "."))
        {
            fprintf(stderr, "invalid ip:%s\n", lpDestIP);
            return -1;
        }
    }

    lPing = cping_create();

    fprintf(stderr, "Ping %s with %d bytes of data:\n", lpDestIP, DEF_PACKET_SIZE);
    while (--lMaxTimes)
    {
        cping_ip(lPing, lpDestIP, &lReply, 5000);
        fprintf(stderr, "reply from %s: bytes=%d time=%dms ttl=%d\n",
            lpDestIP, lReply.m_dwBytes, lReply.m_dwRoundTripTime, lReply.m_dwTTL);
        Sleep(1000);
    }

    return 0;
}
