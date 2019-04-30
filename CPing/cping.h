#ifndef _CPING_H_INCLUDED_
#define _CPING_H_INCLUDED_

#include <stdint.h>

#define DEF_PACKET_SIZE     32
#define ECHO_REQUEST        8
#define ECHO_REPLY          0

struct IPHeader
{
    uint8_t     m_byVerHLen;        //4λ�汾+4λ�ײ�����
    uint8_t     m_byTOS;            //��������
    uint16_t    m_usTotalLen;       //�ܳ���
    uint16_t    m_usID;             //��ʶ
    uint16_t    m_usFlagFragOffset; //3λ��־+13λƬƫ��
    uint8_t     m_byTTL;            //TTL
    uint8_t     m_byProtocol;       //Э��
    uint16_t    m_usHChecksum;      //�ײ������
    uint32_t    m_ulSrcIP;          //ԴIP��ַ
    uint32_t    m_ulDestIP;         //Ŀ��IP��ַ
};

struct ICMPHeader
{ 
    uint8_t   m_byType;             //����
    uint8_t   m_byCode;             //����
    uint16_t m_usChecksum;          //����� 
    uint16_t m_usID;                //��ʶ��
    uint16_t m_usSeq;               //���
    uint32_t  m_ulTimeStamp;        //ʱ������Ǳ�׼ICMPͷ����
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
