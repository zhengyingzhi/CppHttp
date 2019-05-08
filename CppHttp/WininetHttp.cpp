#include <windows.h>
#include <wininet.h>
#include <tchar.h>

#include <nlohmann/json.hpp>

#include "WininetHttp.h"

#pragma comment(lib, "Wininet.lib")

using namespace std;
using json = nlohmann::json;

#define READ_BUFFER_SIZE        4096


WininetHttp::WininetHttp(void)
    : m_hSession()
    , m_hConnect()
    , m_hRequest(NULL)
{
}

WininetHttp::~WininetHttp(void)
{
    ReleaseAll();
}


HttpInterfaceError WininetHttp::RequestInfo(
    std::string& aRetData,
    const std::string& aReqURL,
    const std::string& aReqInterface,
    HttpRequest aReqType,
    const std::string& aHeader,
    const std::string& aReqData)
{
    if (aReqURL.empty())
    {
        throw HIE_ParamErr;
    }

    ReleaseAll();
    m_hSession = InternetOpen(_T("Http-connect"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
    if (NULL == m_hSession)
    {
        return HIE_InitErr;
    }

    int lIsHttps = 0;
    int lPort = 0;
    std::string lHostName;
    ParseRequestURL(aReqURL, lHostName, lPort, lIsHttps);
#if 1
    fprintf(stdout, "request url: %s%s\n", aReqURL.c_str(), aReqInterface.c_str());
    fprintf(stdout, "host name: %s, port:%d\n", lHostName.c_str(), lPort);
#endif

    m_hConnect = InternetConnectA(m_hSession, lHostName.c_str(), lPort, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
    if (NULL == m_hConnect)
    {
        return HIE_ConnectErr;
    }

    std::string lRequestType = (HR_Get == aReqType) ? "GET" : "POST";
    DWORD flags = INTERNET_FLAG_FORMS_SUBMIT;// INTERNET_FLAG_RELOAD;
    if (lIsHttps)
        flags |= INTERNET_FLAG_SECURE;

    m_hRequest = HttpOpenRequestA(m_hConnect, lRequestType.c_str(), aReqInterface.c_str(), "HTTP/1.1", NULL, NULL,  flags, NULL);
    if (NULL == m_hRequest)
    {
        throw HIE_InitErr;
    }

    // HttpAddRequestHeadersA(m_hRequest, aHeader.c_str(), aHeader.length(), HTTP_ADDREQ_FLAG_COALESCE);

    DWORD lHeaderLength = aHeader.length();
    BOOL lRet = FALSE;
    if (HR_Get == aReqType)
    {
        lRet = HttpSendRequestA(m_hRequest, aHeader.c_str(), lHeaderLength, NULL, 0);
    }
    else
    {
        lRet = HttpSendRequestA(m_hRequest, aHeader.c_str(), lHeaderLength, (LPVOID)aReqData.c_str(), (DWORD)aReqData.length());
    }
    if (!lRet)
    {
        return HIE_SendErr;
    }

    char lBuffer[READ_BUFFER_SIZE] = { 0 };
    DWORD lBufferSize = READ_BUFFER_SIZE - 1;
    if (!HttpQueryInfoA(m_hRequest, HTTP_QUERY_RAW_HEADERS, lBuffer, &lBufferSize, NULL))
    {
        return HIE_QueryErr;
    }
    if (strstr(lBuffer, "404"))
    {
        return HIE_404;
    }

    while (true)
    {
        memset(lBuffer, 0, sizeof(lBuffer));
        lRet = InternetReadFile(m_hRequest, lBuffer, READ_BUFFER_SIZE, &lBufferSize);
        if (!lRet || (0 == lBufferSize))
        {
            break;
        }
        lBuffer[lBufferSize] = '\0';
        aRetData.append(lBuffer);
    }

    return HIE_Success;
}

void WininetHttp::ParseRequestURL(const std::string& aReqUrl, std::string& aHostName, int& aPort, int& aIsHttps)
{
    aPort = 80;
    std::string lTemp = aReqUrl;
    size_t offset = strlen("http://");
    size_t lPos = lTemp.find("http://");
    if (lPos == std::string::npos)
    {
        lPos = lTemp.find("https://");
        if (lPos != std::string::npos)
        {
            aPort = 443;
            offset += 1;
            aIsHttps = 1;
        }
    }

    if (lPos != std::string::npos)
    {
        lTemp = lTemp.substr(lPos + offset, lTemp.length() - lPos - offset);
    }

    lPos = lTemp.find('/');
    if (lPos == std::string::npos)
    {
        aHostName = lTemp;
    }
    else
    {
        aHostName = lTemp.substr(0, lPos);
    }

    std::size_t lPortPos = aHostName.find(':');
    if (lPortPos != std::string::npos)
    {
        std::string strPort = lTemp.substr(lPortPos + 1, aHostName.length() - lPortPos - 1);
        aHostName = aHostName.substr(0, lPortPos);
        aPort = atoi(strPort.c_str());
    }
}

void WininetHttp::ReleaseAll()
{
    ReleaseHandle(m_hRequest);
    ReleaseHandle(m_hConnect);
    ReleaseHandle(m_hSession);
}

void WininetHttp::ReleaseHandle(void*& hInternet)
{
    if (hInternet)
    {
        InternetCloseHandle(hInternet);
        hInternet = NULL;
    }
}

std::string WininetHttp::UtfToGbk(const std::string& utf8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);

    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* dst = new char[len + 1];
    memset(dst, 0, len + 1);

    WideCharToMultiByte(CP_ACP, 0, wstr, -1, dst, len, NULL, NULL);

    std::string lRet = dst;

    delete[] wstr;
    delete[] dst;
    return lRet;
}

std::string WininetHttp::GbkToUtf(const std::string& gbk)
{
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, NULL, 0);

    wchar_t * pwBuf = new wchar_t[nwLen + 1];
    ZeroMemory(pwBuf, nwLen * 2 + 2);

    ::MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), (int)gbk.length(), pwBuf, nwLen);

    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char * pBuf = new char[nLen + 1];
    ZeroMemory(pBuf, nLen + 1);

    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string lRet(pBuf);

    delete[]pwBuf;
    delete[]pBuf;

    pwBuf = NULL;
    pBuf = NULL;
    return lRet;
}
