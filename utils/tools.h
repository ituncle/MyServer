/*
 * tools.h
 *
 *  Created on: Mar 8, 2016
 *      Author: kingTang
 */

#ifndef UTILS_TOOLS_H_
#define UTILS_TOOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <uuid/uuid.h>

namespace ft
{
	namespace utils{
		class Tools{
		public:
			/**
			 * 产生GUID
			 */
			static std::string getGuid();
		};
	}
}



#endif /* UTILS_TOOLS_H_ */
