#pragma once

#include <string>
using namespace std;

enum HttpInterfaceError
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
    HIE_ParamErr,            //参数错误，空指针，空字符
    HIE_UnknowErr,
};

enum HttpRequest
{
    HR_Get,
    HR_Post
};

class WininetHttp
{
public:
    WininetHttp(void);
    ~WininetHttp(void);

public:
    /* 通过HTTP请求：Get或Post方式获取信息
     */
    HttpInterfaceError RequestInfo(
        std::string& aRetData,
        const std::string& aReqURL,
        const std::string& aReqInterface,
        HttpRequest aReqType,
        const std::string& aHeader = "",
        const std::string& aReqData = "");

    /* 解析请求域名
     */
    static void ParseRequestURL(const std::string& aReqUrl, std::string& aHostName, int& aPort, int& aIsHttps);

    /* 字符集转换
     */
    static std::string UtfToGbk(const std::string& utf8);

protected:
    void ReleaseAll();

    void ReleaseHandle(void*& hInternet);

private:
    void*   m_hSession;     // HINTERNET type
    void*   m_hConnect;
    void*   m_hRequest;
};
