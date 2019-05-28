#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>

#include <bigliveapi/bigliveapi.h>

#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;


void ParseJsonInfo(const std::string& aInterface, const std::string& aJsonInfo);

int main(int argc, char* argv[])
{
    fprintf(stderr, "hello biglive c api\n");

    std::string lFile = "config.json";
    int lIsJson = 1;
    if (argc > 1)
        lFile = argv[1];

    std::string lURL = "https://bigquant.com";
    std::string lSenderCompId = "xczq";
    std::string lSenderVersion = "1.0.0";
    std::string lUserName = "xczq002";
    std::string lToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjozMTA5MiwidXNlcm5hbWUiOiJ4Y3pxMDAyIiwidXBkYXRlX3RpbWUiOiIyMDE5LTA0LTMwIDE2OjQ0OjExIiwiZXhwIjo3MjU4MDg5NjAwfQ.WxEmPfWzcP6KATtwMq3e2dDfLffZJfI32YOxcJTWPz0";
    std::string lLiveId = "7b2ea4407b7911e9ae5c0a580a800506";

    std::ifstream in(lFile);
    if (in.is_open())
    {
        char lConfBuffer[1020] = "";
        in.read(lConfBuffer, sizeof(lConfBuffer) - 1);
        json lConfReader = json::parse(lConfBuffer);
        std::string _lSenderCompId = lConfReader["sender_comp_id"];
        std::string _lSenderVersion = lConfReader["sender_version"];
        std::string _lUserName = lConfReader["user_name"];
        std::string _lToken = lConfReader["token"];
        std::string _lLiveId = lConfReader["live_id"];
        if (lConfReader.contains("url"))
        {
            std::string _lURL = lConfReader["url"];
            lURL = _lURL;
        }
        if (lConfReader.contains("IsJson"))
        {
            int _lIsJson = lConfReader["IsJson"];
            lIsJson = _lIsJson;
        }
        lSenderCompId = _lSenderCompId;
        lSenderVersion = _lSenderVersion;
        lUserName = _lUserName;
        lToken = _lToken;
        lLiveId = _lLiveId;
        in.close();
    }
    else
    {
        fprintf(stderr, "read config json file [%s] failed\n", lFile.c_str());
    }
    fprintf(stderr, "<<%s isjson:%d>>\n", lURL.c_str(), lIsJson);
    fprintf(stderr, "<<test username:%s, live_id:%s>>\n", lUserName.c_str(), lLiveId.c_str());

    int rv;
    const char* lpHeader = NULL;
    char        lReqData[1000] = "";
    uint32_t    lReqLength = 0;

    if (!lIsJson)
    {
        lpHeader = "Content-Type: application/x-www-form-urlencoded";
        lReqLength = snprintf(lReqData, sizeof(lReqData) - 1, "sender_comp_id=%s&sender_version=%s&user_name=%s&live_id=%s&token=%s",
            lSenderCompId.c_str(), lSenderVersion.c_str(), lUserName.c_str(), lLiveId.c_str(), lToken.c_str());
    }
    else
    {
        lpHeader = "Content-Type: application/json";
        json lJsonData;
        lJsonData["sender_comp_id"] = lSenderCompId;
        lJsonData["sender_version"] = lSenderVersion;
        lJsonData["user_name"] = lUserName;
        lJsonData["token"] = lToken;
        lJsonData["live_id"] = lLiveId;
        std::string lDumpData = lJsonData.dump();
        strncpy(lReqData, lDumpData.c_str(), sizeof(lReqData) - 1);
        lReqLength = (int)lDumpData.length();
    }

    const char* lpInterface;
    big_live_data_t lData;

    lpInterface = "/api/live_trading/info";
    fprintf(stderr, "requesting %s...\n", lpInterface);
    rv = big_live_request_post(&lData, lURL.c_str(), lpInterface, lpHeader, strlen(lpHeader), lReqData, lReqLength);
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
    rv = big_live_request_post(&lData, lURL.c_str(), lpInterface, lpHeader, strlen(lpHeader), lReqData, lReqLength);
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

    lpInterface = "/api/live_trading/strategy_orders";
    fprintf(stderr, "requesting %s...\n", lpInterface);
    rv = big_live_request_post(&lData, lURL.c_str(), lpInterface, lpHeader, strlen(lpHeader), lReqData, lReqLength);
    if (rv != 0)
    {
        fprintf(stderr, "strategy_orders error:%d\n", rv);
    }
    else
    {
        fprintf(stderr, "%s\n", lData.data);
        std::string lJsonData(lData.data, lData.size);
        ParseJsonInfo("strategy_orders", lJsonData);

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
    else if (aInterface == "strategy_orders")
    {
        auto live_id = data["live_id"];
        auto plan_orders = data["planned_orders"];
        // std::vector<json> lv = data["planned_orders"];

        for (size_t i = 0; i < plan_orders.size(); ++i)
        {
            json elem = plan_orders[i];
            auto algo_params = elem["algo_params"];
            auto sub_orders = elem["sub_orders"];
            string sid = elem["symbol"];
            std::cout << "algo_params type is:" << algo_params.type_name() << std::endl;
            if (algo_params.is_null())
            {
                std::cout << "algo_params is null" << std::endl;
            }
            int y = 0;
        }

        // auto plan_order = plan_orders.at(0);
        // auto algo_params = plan_order["algo_params"];
        int x = 0;
    }
}
