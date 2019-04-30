#ifndef _CPING_H_INCLUDED_
#define _CPING_H_INCLUDED_

#include <stdint.h>

#define DEF_PACKET_SIZE     32
#define ECHO_REQUEST        8
#define ECHO_REPLY          0

struct IPHeader
{
    uint8_t     m_byVerHLen;        //4位版本+4位首部长度
    uint8_t     m_byTOS;            //服务类型
    uint16_t    m_usTotalLen;       //总长度
    uint16_t    m_usID;             //标识
    uint16_t    m_usFlagFragOffset; //3位标志+13位片偏移
    uint8_t     m_byTTL;            //TTL
    uint8_t     m_byProtocol;       //协议
    uint16_t    m_usHChecksum;      //首部检验和
    uint32_t    m_ulSrcIP;          //源IP地址
    uint32_t    m_ulDestIP;         //目的IP地址
};

struct ICMPHeader
{ 
    uint8_t   m_byType;             //类型
    uint8_t   m_byCode;             //代码
    uint16_t m_usChecksum;          //检验和 
    uint16_t m_usID;                //标识符
    uint16_t m_usSeq;               //序号
    uint32_t  m_ulTimeStamp;        //时间戳（非标准ICMP头部）
};

struct ping_reply_s
{
    uint16_t    m_usSeq;
    uint32_t    m_dwRoundTripTime;
    uint32_t    m_dwBytes;
    uint32_t    m_dwTTL;
};
typedef struct ping_reply_s ping_reply_t;

typedef struct cping_s cping_t;

cping_t* cping_create();

void cping_release(cping_t* ping);

int cping_ip(cping_t* ping, const char* ip, ping_reply_t* apReply, uint32_t aTimeoutMS);

#endif//_CPING_H_INCLUDED_
