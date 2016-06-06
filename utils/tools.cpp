/*
 * tools.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: kingTang
 */

#include "tools.h"

namespace ft {
    namespace utils {

        typedef struct _uuid_t {
            unsigned int data1;
            unsigned short data2;
            unsigned short data3;
            unsigned char data4[8];
        } uuid_t;

        std::string Tools::getGuid() {
            uuid_t guid;
            uuid_generate(reinterpret_cast<unsigned char *>(&guid));
            char buf[36] = {0};
            snprintf(buf,
                     sizeof(buf),
                     "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                     guid.data1, guid.data2, guid.data3,
                     guid.data4[0], guid.data4[1],
                     guid.data4[2], guid.data4[3],
                     guid.data4[4], guid.data4[5],
                     guid.data4[6], guid.data4[7]);
            return std::string(buf);
        }

    }
}
