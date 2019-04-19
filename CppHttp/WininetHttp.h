#pragma once

#include <string>
using namespace std;

enum HttpInterfaceError
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
    HIE_ParamErr,            //�������󣬿�ָ�룬���ַ�
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
    /* ͨ��HTTP����Get��Post��ʽ��ȡ��Ϣ
     */
    HttpInterfaceError RequestInfo(
        std::string& aRetData,
        const std::string& aReqURL,
        const std::string& aReqInterface,
        HttpRequest aReqType,
        const std::string& aHeader = "",
        const std::string& aReqData = "");

    /* ������������
     */
    static void ParseRequestURL(const std::string& aReqUrl, std::string& aHostName, int& aPort, int& aIsHttps);

    /* �ַ���ת��
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
