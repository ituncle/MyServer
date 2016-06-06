//
// Created by root on 2/23/16.
//
#include "crc32.h"

//CRC32* CRC32::m_instance = new CRC32;
//unsigned int CRC32::crcTable_[256] = {0};


ft::utils::CRC32::CRC32()
{
    create_table();
}


void ft::utils::CRC32::create_table()
{
    unsigned int c;
    unsigned int i, j;

    for (i = 0; i < 256; i++) {
        c = i;
        for (j = 0; j < 8; j++) {
            if (c & 1) {
                c = static_cast<decltype(c)>(0xedb88320L ^ (c >> 1));
            } else {
                c = c >> 1;
            }
        }
        crcTable_[i] = c;
    }
}

