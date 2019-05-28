#include <stdio.h>
#include <iostream>

#include <nlohmann/json.hpp>

#include "WininetHttp.h"

using json = nlohmann::json;

void ParseJsonInfo(const std::string& aInterface, const std::string& aJsonInfo);


int main(int argc, char* argv[])
{
    std::string lUrl, lInterface, lResult, lPostData;
    lUrl = "https://bigquant.com";      // 192.168.1.142:9000

    std::string lSenderCompId   = "xczq";
    std::string lSenderVersion  = "1.0.0";
    std::string lUserName        = "xczq002";
    std::string lToken           = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjozMTA5MiwidXNlcm5hbWUiOiJ4Y3pxMDAyIiwidXBkYXRlX3RpbWUiOiIyMDE5LTA0LTMwIDE2OjQ0OjExIiwiZXhwIjo3MjU4MDg5NjAwfQ.WxEmPfWzcP6KATtwMq3e2dDfLffZJfI32YOxcJTWPz0";
    std::string lLiveId          = "886ed1066fa411e9b5650a580a80040c";

    std::string lHeader, lReqData;
#if 0
    lHeader = "Content-Type: application/x-www-form-urlencoded";
    char lReqBuffer[4096] = "";
    snprintf(lReqBuffer, sizeof(lReqBuffer) - 1, "sender_comp_id=%s&sender_version=%s&user_name=%s&live_id=%s&token=%s",
        lSenderCompId.c_str(), lSenderVersion.c_str(), lUserName.c_str(), lLiveId.c_str(), lToken.c_str());
    lReqData = std::string(lReqBuffer);
#else
    lHeader = "Content-Type: application/json";
    json lJsonData;
    lJsonData["sender_comp_id"] = lSenderCompId.c_str();
    lJsonData["sender_version"] = lSenderVersion.c_str();
    lJsonData["user_name"] = lUserName.c_str();
    lJsonData["token"] = lToken.c_str();
    lJsonData["live_id"] = lLiveId.c_str();
    lReqData = lJsonData.dump();
#endif
    std::cout << "post_data:" << lReqData << std::endl;

    WininetHttp winhttp;
    HttpInterfaceError lErrorID;

    // lResult = "{'status_code': 200}";
    // ParseJsonInfo(lResult);


    lResult = "";
    lInterface = "/api/live_trading/info";
    lErrorID = winhttp.RequestInfo(lResult, lUrl, lInterface, HR_Post, lHeader, lReqData);
    std::cout << "# info result:" << lResult << std::endl;
    if (lResult.length())
    {
        ParseJsonInfo("info", lResult);
    }

    lResult = "";
    lInterface = "/api/live_trading/strategy_info";
    lErrorID = winhttp.RequestInfo(lResult, lUrl, lInterface, HR_Post, lHeader, lReqData);
    std::cout << "# strategy_info result:" << lResult << std::endl;
    if (lResult.length())
    {
        ParseJsonInfo("strategy_info", lResult);
    }

    lResult = "";
    lInterface = "/api/live_trading/strategy_orders";
    lErrorID = winhttp.RequestInfo(lResult, lUrl, lInterface, HR_Post, lHeader, lReqData);
    std::cout << "# strategy_orders result:" << lResult << std::endl;
    if (lResult.length())
    {
        ParseJsonInfo("strategy_orders", lResult);
    }

    return 0;
}

typedef struct stStrategyInfo
{
    char    CreateTime[32];
    char    RunDate[16];
    double  PositionsValue;
    double  MaximumDrawdown;
    double  Sharpe;
    double  Volatility;
    double  CumulativeIncome;
    double  AnnualIncome;
    double  CapitalBase;
    char    StrategyIds[128];
    char    LiveName[64];
    int     RunStatus;
    int     TradeMode;
}StrategyInfo;

void ParseJsonInfo(const std::string& aInterface, const std::string& aJsonInfo)
{
    // std::cout << aJsonInfo << endl;
    StrategyInfo si = { 0 };

    json reader;
    reader = json::parse(aJsonInfo.c_str());
    int status_code = reader["status_code"];
    std::cout << "status_code:" << status_code << endl;
    if (status_code != 200)
    {
        return;
    }
    json data = reader["data"];

    if (aInterface == "info")
    {
        string ct = data["create_time"];
        string rd = data["run_date"];
        string ln = data["live_name"];
        std::string ln_local = WininetHttp::UtfToGbk(ln);

        strcpy(si.CreateTime, ct.c_str());
        strcpy(si.RunDate, rd.c_str());
        strcpy(si.LiveName, ln.c_str());
        si.RunStatus = data["run_status"];
        si.CapitalBase = data["capital_base"];
        si.MaximumDrawdown = data["maximum_drawdown"];
        si.CumulativeIncome = data["cumulative_income"];
        si.AnnualIncome = data["annual_income"];
        // si.Volatility = data["volatility"];
    }
}
