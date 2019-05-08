#include <stdio.h>
#include <iostream>

#include <nlohmann/json.hpp>

#include "WininetHttp.h"

using json = nlohmann::json;

void ParseJsonInfo(const std::string& aJsonInfo);

int main(int argc, char* argv[])
{
    std::string lUrl, lInterface, lResult, lPostData;
    lUrl = "https://bigquant.com";      // 192.168.1.142:9000
    lInterface = "/api/live_trading/info";

    json lJsonData;
    lJsonData["sender_comp_id"] = "xczq";
    lJsonData["sender_version"] = "1.0.0";
    lJsonData["user_name"] = "xczq002";
    lJsonData["token"] = "";
    lJsonData["live_id"] = "886ed1066fa411e9b5650a580a80040c";
    std::string lReqData = lJsonData.dump();
    std::cout << "post_data:" << lReqData << std::endl;

    std::string lHeader;
    WininetHttp winhttp;
    HttpInterfaceError lErrorID = winhttp.RequestInfo(lResult, lUrl, lInterface, HR_Post, lHeader, lReqData);
    std::cout << "result:" << lResult << std::endl;

    if (lResult.length())
    {
        ParseJsonInfo(lResult);
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

void ParseJsonInfo(const std::string& aJsonInfo)
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
