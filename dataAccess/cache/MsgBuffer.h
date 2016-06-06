#ifndef QUALITY_LIFE_MSG_BUFFER_h
#define QUALITY_LIFE_MSG_BUFFER_h

#include "../../network/muduo/base/Mutex.h"
#include "../../protocol/yfq/yfqprotoserver3.pb.h"
#include "../../network/muduo/net/TcpConnection.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include "../../protocol/messageType.h"
#include "../../protocol/yfq/errcode.h"
#include "../../log/ftlog.h"
#include "../../utils/Time.h"
#include <iostream>
#include <memory>
#include <map>
#include <list>

using namespace ft::utils;
using namespace boost::gregorian;
using namespace std;
using namespace ft::net::Protocol;
namespace ft{
using muduo::MutexLockGuard;
namespace buffer {

		class LoginedUser{
		private:
		using user_type = std::string;
		using info_type = const std::weak_ptr<muduo::net::TcpConnection>;
		public:
			std::shared_ptr<std::map<user_type /*userId*/, info_type /*userId*/> > userMap;
			std::map<user_type /*userId*/, info_type /*userId*/>::iterator userMapIter;
		public:
			LoginedUser();
			bool isOnLine(user_type userId);
			Error::ErrorCode insert(user_type, info_type);
			info_type getInfo(user_type userId);
			Error::ErrorCode remove(info_type userId);
		public:
			static LoginedUser * instance();
		private:
			mutable muduo::MutexLock mutex_;
		};

		/**
		 * 指定用户/指定消息类型/指定时间的消息缓存
		 *用户ID-->消息类型-->时间-->消息
		 */
		class QLifeMsg{
		public:
			std::string userId;
			int msgType;
			std::string dateTime;
			std::string date;
			std::string msg;
		public:
			QLifeMsg();
			QLifeMsg(std::string userIdP, int type, std::string dateTimeP);
		};
		/**
		 * 指定用户/指定消息类型的消息缓存
		 * 用户ID-->消息类型-->时间
		 */
		class QLifeTimeMsgBuffer {
		public:
			std::string userId;
			int msgType;
			std::shared_ptr<std::map<std::string /*date*/,std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>> >> _map;
		public:
			QLifeTimeMsgBuffer(std::string userIdP, int type);
			int checkMsgBuffer(const std::string &userId, int msgType, std::string dateTime /*DateTime*/);
			int insertQLifeMsg(const std::string &userId, int msgType, std::string dateTime /*DateTime*/, std::shared_ptr<QLifeMsg> &msg);
		};
		/**
				 * 指定用户的消息缓存
				 * 用户ID-->消息类型
				 */
		class QLifeTypeMsgBuffer {
		public:
			std::string userId;
			std::shared_ptr<std::map<int /*massage Type*/, std::shared_ptr<QLifeTimeMsgBuffer>>> _map;
		public:
			QLifeTypeMsgBuffer(std::string userIdP);
			int checkMsgBuffer(const std::string &userId, int msgType, std::string dateTime /*DateTime*/);
			int insertQLifeMsg(const std::string &userId, int msgType, std::string dateTime /*DateTime*/, std::shared_ptr<QLifeMsg> &msg);
		};
		/**
		 * 品质生活用户消息缓存
		 */
		class QLifeUserMsgBuffer{
		public:
			QLifeUserMsgBuffer();
		public:
			static QLifeUserMsgBuffer * instance();
			int insertQLifeMsg(const std::string &userId, int msgType, std::string dateTime, const std::string &msg);
			int getQLifeMsg(const std::string &userId, int msgType/*health type*/, std::string dateTime, int notifyType/*notifyType*/
					,std::shared_ptr<std::map<std::string /*date*/,std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>> >> & );
			int getQLifeMsgPush(const std::string &userId, std::shared_ptr<std::map<int /*massage Type*/, std::shared_ptr<QLifeTimeMsgBuffer>>> & loadMap);

		private:
			 mutable muduo::MutexLock mutex_;
			std::shared_ptr<std::map<std::string ,std::shared_ptr<QLifeTypeMsgBuffer> > > _map;
		private:
			int getQlifeMsg(std::shared_ptr<std::map<std::string /*date*/,std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>> >> &map,
					int notifyType, std::string &dateTime,
					std::shared_ptr<std::map<std::string /*date*/,std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>> >> &);
			int checkMsgBuffer(const std::string &userId, int msgType, std::string dateTime /*DateTime*/);
		};

	}

}


#endif
