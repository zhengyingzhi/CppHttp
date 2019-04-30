#include <stdlib.h>
#include <string.h>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "cping.h"

#pragma comment(lib, "Ws2_32.lib")

struct cping_s
{
    SOCKET      m_sockRaw;
    WSAEVENT    m_event;
    USHORT      m_usCurrentProcID;
    char*       m_szICMPData;
    BOOL        m_bIsInitSucc;
};

static uint16_t s_PacketSeq = 0;

static int cping_core(cping_t* ping, uint32_t aDestAddr, ping_reply_t* apReply, uint32_t aTimeoutMS);
static uint16_t calc_check_sum(uint16_t* apBuffer, uint32_t aSize);
static uint32_t get_tick_count_calibrate();

cping_t* cping_create()
{
    cping_t* ping;
    ping = (cping_t*)malloc(sizeof(cping_t));
    memset(ping, 0, sizeof(cping_t));

    WSADATA WSAData;
    WSAStartup(MAKEWORD(1, 1), &WSAData);

    ping->m_event = WSACreateEvent();
    ping->m_usCurrentProcID = (USHORT)GetCurrentProcessId();

    if ((ping->m_sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, 0)) != SOCKET_ERROR)
    {
        WSAEventSelect(ping->m_sockRaw, ping->m_event, FD_READ);
        ping->m_bIsInitSucc = TRUE;

        ping->m_szICMPData = (char*)malloc(DEF_PACKET_SIZE + sizeof(struct ICMPHeader));

        if (ping->m_szICMPData == NULL)
        {
            ping->m_bIsInitSucc = FALSE;
        }
    }

    return ping;
}

void cping_release(cping_t* ping)
{
    // WSACleanup();

    if (NULL != ping->m_szICMPData)
    {
        free(ping->m_szICMPData);
        ping->m_szICMPData = NULL;
    }
    free(ping);
}

int cping_ip(cping_t* ping, const char* ip, ping_reply_t* apReply, uint32_t aTimeoutMS)
{
    if (ip)
    {
        uint32_t lAddr = 0;
        inet_pton(AF_INET, ip, &lAddr);
        return cping_core(ping, lAddr, apReply, aTimeoutMS);
    }
    return -1;
}


static int cping_core(cping_t* ping, uint32_t aDestAddr, ping_reply_t* apReply, uint32_t aTimeoutMS)
{
    //判断初始化是否成功
    if (!ping->m_bIsInitSucc)
    {
        return FALSE;
    }

    //配置SOCKET
    struct sockaddr_in lAddrDest;
    lAddrDest.sin_family = AF_INET;
    lAddrDest.sin_addr.s_addr = aDestAddr;
    int lAddrSize = sizeof(lAddrDest);

    //构建ICMP包
    uint32_t lICMPDataSize = DEF_PACKET_SIZE + sizeof(struct ICMPHeader);
    uint32_t ulSendTimestamp = get_tick_count_calibrate();
    uint16_t usSeq = ++s_PacketSeq;
    memset(ping->m_szICMPData, 0, lICMPDataSize);
    struct ICMPHeader* lpICMPHeader = (struct ICMPHeader*)ping->m_szICMPData;
    lpICMPHeader->m_byType  = ECHO_REQUEST;
    lpICMPHeader->m_byCode  = 0;
    lpICMPHeader->m_usID    = ping->m_usCurrentProcID;
    lpICMPHeader->m_usSeq   = usSeq;
    lpICMPHeader->m_ulTimeStamp = ulSendTimestamp;
    lpICMPHeader->m_usChecksum = calc_check_sum((uint16_t*)ping->m_szICMPData, lICMPDataSize);

    //发送ICMP报文
    if (sendto(ping->m_sockRaw, ping->m_szICMPData, lICMPDataSize, 0, (struct sockaddr*)&lAddrDest, lAddrSize) == SOCKET_ERROR)
    {
        return FALSE;
    }

    //判断是否需要接收相应报文
    if (!apReply)
    {
        return 0;
    }

    char lRecvBuf[256] = { 0 };
    while (1)
    {
        //接收响应报文
        if (WSAWaitForMultipleEvents(1, &ping->m_event, FALSE, 100, FALSE) != WSA_WAIT_TIMEOUT)
        {
            WSANETWORKEVENTS netEvent;
            WSAEnumNetworkEvents(ping->m_sockRaw, ping->m_event, &netEvent);

            if (netEvent.lNetworkEvents & FD_READ)
            {
                ULONG nRecvTimestamp = get_tick_count_calibrate();
                int nPacketSize = recvfrom(ping->m_sockRaw, lRecvBuf, 256, 0, (struct sockaddr*)&lAddrDest, &lAddrSize);
                if (nPacketSize != SOCKET_ERROR)
                {
                    struct IPHeader *pIPHeader = (struct IPHeader*)lRecvBuf;
                    uint16_t lIPHeaderLen = (USHORT)((pIPHeader->m_byVerHLen & 0x0f) * 4);
                    struct ICMPHeader *pICMPHeader = (struct ICMPHeader*)(lRecvBuf + lIPHeaderLen);

                    if (pICMPHeader->m_usID == ping->m_usCurrentProcID //是当前进程发出的报文
                        && pICMPHeader->m_byType == ECHO_REPLY //是ICMP响应报文
                        && pICMPHeader->m_usSeq == usSeq //是本次请求报文的响应报文
                        )
                    {
                        apReply->m_usSeq = usSeq;
                        apReply->m_dwRoundTripTime = nRecvTimestamp - pICMPHeader->m_ulTimeStamp;
                        apReply->m_dwBytes = nPacketSize - lIPHeaderLen - sizeof(struct ICMPHeader);
                        apReply->m_dwTTL = pIPHeader->m_byTTL;
                        return TRUE;
                    }
                }
            }
        }
        //超时
        if (get_tick_count_calibrate() - ulSendTimestamp >= aTimeoutMS)
        {
            return -1;
        }
    }
}

static uint16_t calc_check_sum(uint16_t* apBuffer, uint32_t aSize)
{
    uint32_t lCheckSum = 0;
    while (aSize > 1)
    {
        lCheckSum += *apBuffer++;
        aSize -= sizeof(uint16_t);
    }
    if (aSize)
    {
        lCheckSum += *(uint8_t*)apBuffer;
    }

    lCheckSum = (lCheckSum >> 16) + (lCheckSum & 0xffff);
    lCheckSum += (lCheckSum >> 16);

    return (uint16_t)(~lCheckSum);
}

static uint32_t get_tick_count_calibrate()
{
    static uint64_t lFirstCallTick = 0;
    static uint64_t lFirstCallTickMS = 0;

    SYSTEMTIME systemtime;
    FILETIME filetime;
    GetLocalTime(&systemtime);
    SystemTimeToFileTime(&systemtime, &filetime);
    LARGE_INTEGER liCurrentTime;
    liCurrentTime.HighPart = filetime.dwHighDateTime;
    liCurrentTime.LowPart = filetime.dwLowDateTime;
    LONGLONG llCurrentTimeMS = liCurrentTime.QuadPart / 10000;

    if (lFirstCallTick == 0)
    {
        lFirstCallTick = GetTickCount();
    }
    if (lFirstCallTickMS == 0)
    {
        lFirstCallTickMS = llCurrentTimeMS;
    }

    return (uint32_t)(lFirstCallTick + (uint64_t)(llCurrentTimeMS - lFirstCallTickMS));
}
