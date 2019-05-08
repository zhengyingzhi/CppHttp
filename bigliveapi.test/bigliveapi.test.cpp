#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

#include <bigliveapi/bigliveapi.h>

#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;


void ParseJsonInfo(const std::string& aInterface, const std::string& aJsonInfo);

int main(int argc, char* argv[])
{
    fprintf(stderr, "hello biglive api\n");

    int rv;
    const char* lpURL = "https://bigquant.com";
    const char* lpHeader = "Content-Type: application/x-www-form-urlencoded";
    char        lReqData[1000] = "";
    uint32_t    lReqLength = 0;

    const char* lSenderCompId  = "xczq";
    const char* lSenderVersion = "1.0.0";
    const char* lUserName = "xczq002";
    const char* lToken = "";
    const char* lLiveId = "886ed1066fa411e9b5650a580a80040c";

    lReqLength = snprintf(lReqData, sizeof(lReqData) - 1, "sender_comp_id=%s&sender_version=%s&user_name=%s&live_id=%s&token=%s",
        lSenderCompId, lSenderVersion, lUserName, lLiveId, lToken);

    const char* lpInterface;
    big_live_data_t lData;

    lpInterface = "/api/live_trading/info";
    fprintf(stderr, "requesting %s...\n", lpInterface);
    rv = big_live_request_post(&lData, lpURL, lpInterface, lpHeader, strlen(lpHeader), lReqData, lReqLength);
    if (rv != 0)
    {
        fprintf(stderr, "info error:%d\n", rv);
    }
    else
    {
        fprintf(stderr, "%s\n", lData.data);
        std::string lJsonData(lData.data, lData.size);
        ParseJsonInfo("info", lJsonData);

        lData.cleanup(&lData);
    }

    lpInterface = "/api/live_trading/strategy_info";
    fprintf(stderr, "requesting %s...\n", lpInterface);
    rv = big_live_request_post(&lData, lpURL, lpInterface, lpHeader, strlen(lpHeader), lReqData, lReqLength);
    if (rv != 0)
    {
        fprintf(stderr, "strategy_info error:%d\n", rv);
    }
    else
    {
        fprintf(stderr, "%s\n", lData.data);
        std::string lJsonData(lData.data, lData.size);
        ParseJsonInfo("strategy_info", lJsonData);

        lData.cleanup(&lData);
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
        // std::string ln_local = UtfToGbk(ln);

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
