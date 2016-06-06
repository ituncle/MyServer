//
// Created by root on 2/23/16.
//

#ifndef YFQSERVER_CRC32_H
#define YFQSERVER_CRC32_H

#include <boost/noncopyable.hpp>
#include <stdlib.h>
namespace ft {
    namespace utils {
/**
 * 该工具类用于计算一串byte的crc32校验码
 */
        class CRC32 : boost::noncopyable {
        public:
            ~CRC32() { }

            static CRC32 *instance() {
                static CRC32 instance_;  // C++11支持
                return &instance_;
            }

            /**
             * 计算buffer中指定数量的字节的crc32校验码
             */
            inline unsigned int calc(const void *buffer, size_t size) {
                unsigned int i;
                unsigned int crc = CRC;
                const char* data = reinterpret_cast<const char*>(buffer);
                for (i = 0; i < size; i++) {
                    crc = crcTable_[(crc ^ (data[i] & 0xff)) & 0xff] ^ (crc >> 8);
                }
                return crc ^ 0xffffffff;
            }

        private:
            CRC32();    // 禁止外部构造

        private:
            void create_table();

        private:
            const unsigned int CRC = 0xffffffff;
            unsigned int crcTable_[256] = {0};
        };
    }
}

#endif //YFQSERVER_CRC32_H
