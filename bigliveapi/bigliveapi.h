#ifndef _BIG_LIVE_API_H_
#define _BIG_LIVE_API_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_MSC_VER)
#ifdef BIGLIVEAPI_EXPORTS
#define LIVE_API __declspec(dllexport)
#else
#define LIVE_API __declspec(dllimport)
#endif
#define LIVE_STDCALL    __stdcall       /* ensure stdcall calling convention on NT */

#else
#define LIVE_API 
#define LIVE_STDCALL                   /* leave blank for other systems */
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct big_live_data_s big_live_data_t;
struct big_live_data_s
{
    char*       data;
    uint32_t    size;
    uint32_t    capacity;
    void      (*cleanup)(big_live_data_t*);
};

typedef enum 
{
    HIE_Success = 0,        //�ɹ�
    HIE_InitErr,            //��ʼ��ʧ��
    HIE_ConnectErr,         //����HTTP������ʧ��
    HIE_SendErr,            //��������ʧ��
    HIE_QueryErr,           //��ѯHTTP����ͷʧ��
    HIE_404,                //ҳ�治����
    HIE_IllegalUrl,         //��Ч��URL
    HIE_CreateFileErr,      //�����ļ�ʧ��
    HIE_DownloadErr,        //����ʧ��
    HIE_QueryIPErr,         //��ȡ������Ӧ�ĵ�ַʧ��
    HIE_SocketErr,          //�׽��ִ���
    HIE_UserCancel,         //�û�ȡ������
    HIE_BufferErr,          //�ļ�̫�󣬻���������
    HIE_HeaderErr,          //HTTP����ͷ����
    HIE_ParamErr,           //�������󣬿�ָ�룬���ַ�
    HIE_UnknowErr,
}ApiStatusError;

/* post http request
 * apOutData:       �������
 * apURL:           ������������� https://abc.com
 * apInterface:     ����Ľӿڣ��� /api/live_trading/xxx
 * apHeader:        ����ͷ��
 * aHeaderLength:   ����ͷ������
 * apReqData:       ��������
 * aReqLength:      �������ݵĳ���
 */
LIVE_API int LIVE_STDCALL big_live_request_post(
    big_live_data_t* apOutData,
    const char* apURL,
    const char* apInterface,
    const char* apHeader,
    uint32_t aHeaderLength,
    const char* apReqData,
    uint32_t aReqLength);

#ifdef __cplusplus
}
#endif

#endif//_BIG_LIVE_API_H_
