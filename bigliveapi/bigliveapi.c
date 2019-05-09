#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <stdio.h>

#include "bigliveapi.h"

#pragma comment(lib, "Wininet.lib")

#define READ_BUFFER_SIZE        4096


static void data_cleanup(big_live_data_t* livedata)
{
    if (livedata->data)
    {
        free(livedata->data);
        livedata->data = NULL;
        livedata->size = 0;
    }
}

LIVE_API int LIVE_STDCALL big_live_request_post(
    big_live_data_t* apOutData,
    const char* apURL,
    const char* apInterface,
    const char* apHeader,
    uint32_t aHeaderLength,
    const char* apReqData,
    uint32_t aReqLength)
{
    void*   hSession;     // HINTERNET type
    void*   hConnect;
    void*   hRequest;
    ApiStatusError   rv;

    int  lOpenReqFlags;
    int  lIsHttps = 0;
    int  lPort = 0;
    char lHostName[256] = "";

    char lBuffer[READ_BUFFER_SIZE] = { 0 };
    DWORD lBufferSize = READ_BUFFER_SIZE - 1;

    hSession = hConnect = hRequest = NULL;

    if (!apURL || !apURL[0])
    {
        return HIE_ParamErr;
    }

    hSession = InternetOpen(_T("Http-connect"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hSession)
    {
        rv = HIE_InitErr;
        goto REQUEST_END;
    }

    /* parse request url */
    if (strstr(apURL, "https://"))
    {
        strncpy(lHostName, apURL + sizeof("https://") - 1, sizeof(lHostName) - 1);
        lIsHttps = 1;
    }
    else if (strstr(apURL, "http://"))
    {
        strncpy(lHostName, apURL + sizeof("http://") - 1, sizeof(lHostName) - 1);
        lIsHttps = 0;
    }
    else
    {
        strncpy(lHostName, apURL, sizeof(lHostName) - 1);
    }

    const char* lpPortStr = strstr(lHostName, ":");
    if (lpPortStr)
    {
        lPort = atoi(lpPortStr + 1);
        int lHostNameLen = (int)(lpPortStr - lHostName);
        lHostName[lHostNameLen] = '\0';
    }
    else
    {
        if (lIsHttps)
            lPort = 443;
        else
            lPort = 80;
    }

#if _DEBUG
    fprintf(stdout, "request url: %s%s\n", apURL, apInterface);
    fprintf(stdout, "host name: %s, port:%d\n", lHostName, lPort);
#endif

    DWORD_PTR dwContext = 0;
    hConnect = InternetConnectA(hSession, lHostName, lPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, dwContext);
    if (!hConnect)
    {
        rv = HIE_ConnectErr;
        goto REQUEST_END;
    }

    lOpenReqFlags = 0;
    if (apHeader && strstr(apHeader, "form-"))
        lOpenReqFlags |= INTERNET_FLAG_FORMS_SUBMIT;
    if (lIsHttps)
        lOpenReqFlags |= INTERNET_FLAG_SECURE;

    hRequest = HttpOpenRequestA(hConnect, "POST", apInterface, "HTTP/1.1", NULL, NULL, lOpenReqFlags, dwContext);
    if (!hRequest)
    {
        rv = HIE_InitErr;
        goto REQUEST_END;
    }

    BOOL lRet = FALSE;
    lRet = HttpSendRequestA(hRequest, apHeader, aHeaderLength, (LPVOID)apReqData, aReqLength);
    if (!lRet)
    {
        rv = HIE_SendErr;
        goto REQUEST_END;
    }

    if (!HttpQueryInfoA(hRequest, HTTP_QUERY_RAW_HEADERS, lBuffer, &lBufferSize, NULL))
    {
        rv = HIE_QueryErr;
        goto REQUEST_END;
    }
    if (strstr(lBuffer, "404"))
    {
        rv = HIE_404;
        goto REQUEST_END;
    }

    apOutData->capacity = READ_BUFFER_SIZE;
    apOutData->size = 0;
    apOutData->data = malloc(apOutData->capacity);

    while (1)
    {
        memset(lBuffer, 0, sizeof(lBuffer));
        lRet = InternetReadFile(hRequest, lBuffer, READ_BUFFER_SIZE - 1, &lBufferSize);
        if (!lRet || (0 == lBufferSize))
        {
            break;
        }
        lBuffer[lBufferSize] = '\0';

        if (apOutData->size + lBufferSize > apOutData->capacity)
        {
            apOutData->capacity *= 2;
            apOutData->data = realloc(apOutData->data, apOutData->capacity);
        }

        memcpy(apOutData->data + apOutData->size, lBuffer, lBufferSize);
        apOutData->size += lBufferSize;
    }

    apOutData->data[apOutData->size] = 0;
    apOutData->cleanup = data_cleanup;
    rv = HIE_Success;

REQUEST_END:
    if (hRequest)
        InternetCloseHandle(hRequest);
    if (hSession)
        InternetCloseHandle(hSession);
    if (hConnect)
        InternetCloseHandle(hConnect);
    return rv;
}
