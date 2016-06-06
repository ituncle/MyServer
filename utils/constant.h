/*
 * Constant.h
 *
 *  Created on: Mar 28, 2016
 *      Author: kingTang
 */

#ifndef UTILS_CONSTANT_H_
#define UTILS_CONSTANT_H_

namespace ft {
	namespace utils {
		class MAX_BITS {
		public:
			/**
			 * 整形/短整形以字符串出现时其字符数
			 * 注：不同进制，其字符数不一样
			 */
	    	enum {
				INT_HEX = 8,				//十六进制
				INT_DECIMAL = 11,			//十进制
				INT_BINARY = 32,			//二进制
				SHORT_HEX = 4,				//十六进制
				SHORT_DECIMAL = 6,			//十进制
				SHORT_BINARY = 16,			//二进制
	    	} ;
		};
	}
}



#endif /* UTILS_CONSTANT_H_ */
