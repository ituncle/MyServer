//
// Created by root on 3/2/16.
//

#include "Time.h"
#include <cstring>
#include <stdio.h>

namespace ft {
    namespace utils {
        std::string Time::nextDate(char * &pstr){
			tm tm_;
			tm * ptr = &tm_;
			strptime(pstr, "%Y-%m-%d", ptr); //将字符串转换为tm时间
			char tmp[20];
			snprintf(tmp, 20,"%d-%d-%d %d:%d:%d",ptr->tm_year + 1900, ptr->tm_mon + 1, ptr->tm_mday, ptr->tm_hour, ptr->tm_min, ptr->tm_sec );
		//	printf("datetime:%s\n", tmp);

			char tmq[20];
			if(ptr->tm_mon < 9)
					snprintf(tmq, 20, "%d-0%d-", ptr->tm_year + 1900, ptr->tm_mon + 1);
			else
					snprintf(tmq, 20, "%d-%d-", ptr->tm_year + 1900, ptr->tm_mon + 1);
		//	printf("ym:%s\n", tmq);
			char tmo[5];
			if(ptr->tm_mday < 10)
					snprintf(tmo, 5, "0%d ", ptr->tm_mday);
			else
					snprintf(tmo, 5, "%d ", ptr->tm_mday);
			strcat(tmq, tmo);
		//	printf("ymd:%s\n", tmq);
			strcpy(pstr, tmq);
            return "";
        }

        std::string Time::getCurrentDate(char *pstr){
            struct tm *ptr;
            time_t it;
            it = time(nullptr);
            //	printf("The Calendar Time now is %d\n", it);
            ptr = localtime(&it);
            //	printf("year = %d, month = %d, day = %d, hour = %d, min = %d, sec = %d\n", ptr->tm_year, ptr->tm_mon, ptr->tm_mday, ptr->tm_hour, ptr->tm_min, ptr->tm_sec);
            char tmp[20];
            snprintf(tmp, 20,"%d-%d-%d %d:%d:%d",ptr->tm_year + 1900, ptr->tm_mon + 1, ptr->tm_mday, ptr->tm_hour, ptr->tm_min, ptr->tm_sec );
            //	printf("datetime:%s\n", tmp);

            char tmq[20];
            if(ptr->tm_mon < 9)
                snprintf(tmq, 20, "%d-0%d-", ptr->tm_year + 1900, ptr->tm_mon + 1);
            else
                snprintf(tmq, 20, "%d-%d-", ptr->tm_year + 1900, ptr->tm_mon + 1);
            //	printf("ym:%s\n", tmq);
            char tmo[5];
            if(ptr->tm_mday < 10)
                snprintf(tmo, 5, "0%d ", ptr->tm_mday);
            else
                snprintf(tmo, 5, "%d ", ptr->tm_mday);
            strcat(tmq, tmo);
            //	printf("ymd:%s\n", tmq);
            strcpy(pstr, tmq);
            return pstr;
        }
        std::string Time::getLocalFormatTime(char *pstr){
            struct tm *ptr;
            time_t it;
            it = time(nullptr);
            //	printf("The Calendar Time now is %d\n", it);
            ptr = localtime(&it);
            //	printf("year = %d, month = %d, day = %d, hour = %d, min = %d, sec = %d\n", ptr->tm_year, ptr->tm_mon, ptr->tm_mday, ptr->tm_hour, ptr->tm_min, ptr->tm_sec);
            char tmp[20];
            snprintf(tmp, 20,"%d-%d-%d %d:%d:%d",ptr->tm_year + 1900, ptr->tm_mon + 1, ptr->tm_mday, ptr->tm_hour, ptr->tm_min, ptr->tm_sec );
            //	printf("datetime:%s\n", tmp);

            char tmq[20];
            if(ptr->tm_mon < 9)
                snprintf(tmq, 20, "%d-0%d-", ptr->tm_year + 1900, ptr->tm_mon + 1);
            else
                snprintf(tmq, 20, "%d-%d-", ptr->tm_year + 1900, ptr->tm_mon + 1);
            //	printf("ym:%s\n", tmq);
            char tmo[5];
            if(ptr->tm_mday < 10)
                snprintf(tmo, 5, "0%d ", ptr->tm_mday);
            else
                snprintf(tmo, 5, "%d ", ptr->tm_mday);
            strcat(tmq, tmo);
            //	printf("ymd:%s\n", tmq);

            if(ptr->tm_hour < 10)
                snprintf(tmo, 5, "0%d:", ptr->tm_hour);
            else
                snprintf(tmo, 5, "%d:", ptr->tm_hour);
            strcat(tmq, tmo);

            if(ptr->tm_min < 10)
                snprintf(tmo, 5, "0%d:", ptr->tm_min);
            else
                snprintf(tmo, 5, "%d:", ptr->tm_min);
            strcat(tmq, tmo);

            if(ptr->tm_sec < 10)
                snprintf(tmo, 5, "0%d", ptr->tm_sec);
            else
                snprintf(tmo, 5, "%d", ptr->tm_sec);
            strcat(tmq, tmo);
            //	printf("last datetime:%s\n", tmq);
            strcpy(pstr, tmq);

            return pstr;
        }

    }

}
