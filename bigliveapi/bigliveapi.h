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
    HIE_Success = 0,        //成功
    HIE_InitErr,            //初始化失败
    HIE_ConnectErr,         //连接HTTP服务器失败
    HIE_SendErr,            //发送请求失败
    HIE_QueryErr,           //查询HTTP请求头失败
    HIE_404,                //页面不存在
    HIE_IllegalUrl,         //无效的URL
    HIE_CreateFileErr,      //创建文件失败
    HIE_DownloadErr,        //下载失败
    HIE_QueryIPErr,         //获取域名对应的地址失败
    HIE_SocketErr,          //套接字错误
    HIE_UserCancel,         //用户取消下载
    HIE_BufferErr,          //文件太大，缓冲区不足
    HIE_HeaderErr,          //HTTP请求头错误
    HIE_ParamErr,           //参数错误，空指针，空字符
    HIE_UnknowErr,
}ApiStatusError;

/* post http request
 * apOutData:       结果数据
 * apURL:           请求的域名，如 https://abc.com
 * apInterface:     请求的接口：如 /api/live_trading/xxx
 * apHeader:        请求头部
 * aHeaderLength:   请求头部长度
 * apReqData:       请求数据
 * aReqLength:      请求数据的长度
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
