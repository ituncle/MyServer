/*
 * test.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: kingTang
 */
#include "../dataAccess/db/connectionPool.h"
#include "../dataAccess/db/DbHandler.h"
#include <memory>
#include "../log/ftlog.h"
#include "../utils/cJSON.h"

using namespace ft::mysql;


void testjson(){
	float temprature = 36.5;
	std::string weather = "阴天";
	float airQuality = 1.5;
	std::string solarTerm = "小满";
	std::string festival = "植树节";

	char * jsonStr = "{\"temprature\":null, \"weather\":null, \"airQuality\":1.5, \"solarTerm\":\"小满\","
			" \"festival\":\"植树节\"}";

	if(!jsonStr){
			LogInfo << "jsonStr is nullptr";
	}
	LogInfo << "jsonStr = " << jsonStr;

	cJSON* pJson = cJSON_Parse(jsonStr);
	if(!pJson)
	{
		LogInfo << "pJson is nullptr";
		return ;
	}

	cJSON * pSub = cJSON_GetObjectItem(pJson, "temprature");
	if(!pSub)return ;
	temprature = pSub->valuedouble;

	pSub = cJSON_GetObjectItem(pJson, "weather");
	if(!pSub)return ;
	if(pSub->valuestring)
		weather = pSub->valuestring;

	pSub = cJSON_GetObjectItem(pJson, "airQuality");
	if(!pSub)return ;
	airQuality = pSub->valuedouble;

	pSub = cJSON_GetObjectItem(pJson, "solarTerm");
	if(!pSub)return ;
	if(pSub->valuestring)
		solarTerm = pSub->valuestring;

	pSub = cJSON_GetObjectItem(pJson, "festival");
	if(!pSub)return ;
	festival = pSub->valueint;

	cJSON_Delete(pJson);

	LogInfo << "json解析完成";
}

void testdb_handler(){
	int ret = 0;
//	ret = DbHandler::instance()->TBL_QLIFESEG_DAY_insert("ud", "hd","un",1, 23, "2016-02-23",599, "wt","es", "s","t","2016-02-23 10:10:10");
//	LogInfo << "TBL_QLIFESEG_DAY_insert ret = " << ret;
//
//	ret = DbHandler::instance()->TBL_Qlifeseg_insert("ud", "hd","un",1, 23,20, "2016-03-06", "t", 600, "rl","sq", "bm",
//					"rs", "et","ph", "s","es", "2016-03-06 10:10:10", 1);
//	LogInfo << "TBL_Qlifeseg_insert ret = " << ret;
//
//	ret = DbHandler::instance()->TBL_QlifesegSumdata_insert("ud", "hd", 1, "2016-03-06", 600, "hhhh", 1, DbHandler::INSERT);
//	LogInfo << "TBL_QlifesegSumdata_insert ret = " << ret;

	ret = DbHandler::instance()->TBL_QlifesegSumdata_insert("37cc8ae0-8ae0-37cc-adf8-65c62cc265c6", "88985258-6c94-432c-bb79-15d007806daa", 1, "2016-04-11", 99, "health_data", 1, DbHandler::UPDATE);
	LogInfo << "ret = " << ret ;
}

void testdb_date() {
	const char *format = "select id, user_id, host_id,user_name,sex,age, date_format(day,'%%Y-%%m-%%d') as ddd, "
								"score_num, weather_relate, expert_seg, suggestion from YFQ_DATABASE.TBL_QLIFESEG_DAY "
								"where DATE_FORMAT(day, '%%Y-%%m-%%d') = '%s' and host_id = '%s'";
//	const char *format = "select * from YFQ_DATABASE.TBL_QLIFESEG_DAY";
	const int len = 2000;
	std::unique_ptr<char> sql_query(new char[len]);
	snprintf(sql_query.get(), len, format, "2016-03-10", "ccccd");

	LogInfo << "sql_query = " << sql_query.get();

	auto mysqlPool = ft::mysql::ConnectionPool::instance();
	auto conn = mysqlPool->getConnection();
	if (!conn) {
		LogFatal << "failed to getConnection()";
		return ;
	}
	std::string error;
	auto results = conn->executeQuery(sql_query.get(), error);
	if (results == nullptr) {
		LogError << "results == nullptr" << error;
		return ;
	}
	while (results->next()) {
		LogInfo << "1111";
		LogInfo << "有数据" << "user_id = " << results->getString("user_id");
		LogInfo		<< ", host_id = " << results->getString("host_id");
		LogInfo	<< ", user_name = " << results->getString("user_name") ;
		LogInfo	<< ", sex = " << results->getInt("sex") ;
		LogInfo	<< ", age = " << results->getInt("age");
		LogInfo	<< ", day = " << results->getString("ddd") ;
		LogInfo	<< ", score_num = " << results->getInt("score_num");
		LogInfo	<< ", weather_relate = " << results->getString("weather_relate") ;
		LogInfo	<< ", expert_seg = " << results->getString("expert_seg");
		LogInfo	<< ", suggestion = " << results->getString("suggestion");
	}
	mysqlPool->releaseConnection(conn);
}


