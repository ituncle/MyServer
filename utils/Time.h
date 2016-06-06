//
// Created by root on 3/2/16.
//

#ifndef YFQSERVER_TIMER_H
#define YFQSERVER_TIMER_H

#include <ctime>
#include <iostream>

namespace ft {
    namespace utils {
        class Time {
        public:
            /**
             * 得到当前时间，填充在pstr
             * pstr 要填充的地址
             * 返回当前时间
             */
            static std::string getLocalFormatTime(char *pstr);
            /**
             * 得到当前日期，填充在pstr
             * pstr 要填充的地址
             * 返回当前时间
             */
            static std::string getCurrentDate(char *pstr);
            /**
             * 得到date日期的下一日日期
             * pstr 传进去的字符串地址 format YYYY-MM-DD
             * 返回下一日日期
             */
            static std::string nextDate(char * &date);
        };
    }
}

#endif //YFQSERVER_TIMER_H
