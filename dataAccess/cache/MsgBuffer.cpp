/*
 * MsgBuffer.cpp
 *
 *  Created on: Feb 29, 2016
 *      Author: kingTang
 */

#include "MsgBuffer.h"


namespace ft{
	namespace buffer{

		LoginedUser::LoginedUser(){
			userMap = std::make_shared<std::map<user_type /*userId*/, info_type /*userId*/> >();
		}
		LoginedUser * LoginedUser::instance(){
			static LoginedUser * qLifeInstance = new LoginedUser();
			return qLifeInstance;
		}
		bool LoginedUser::isOnLine(user_type userId){
			if(!userMap){
				LogError << "userMap is null";
				return Error::Failed_To_Judge_IsOnLine;
			}
			userMapIter = userMap->find(userId);
			if(userMapIter == userMap->end()){
				return false;
			}
			return true;
		}

		Error::ErrorCode LoginedUser::insert(user_type userId, info_type userInfo){
			MutexLockGuard guard(mutex_);
			if(!userMap){
				LogError << "userMap is null";
				return Error::Failed_To_Insert_UserInfo;
			}
			userMapIter = userMap->find(userId);
			if(userMapIter != userMap->end()){
				userMap->erase(userMapIter->first);
			}

			std::pair<user_type, info_type> pair(userId, userInfo);
			userMap->insert(pair);
			return Error::None;
		}
		LoginedUser::info_type LoginedUser::getInfo(user_type userId){
			if(!userMap){
				LogError << "userMap is null";
				return std::weak_ptr<muduo::net::TcpConnection>();
			}
			userMapIter = userMap->find(userId);
			if(userMapIter == userMap->end()){
				return std::weak_ptr<muduo::net::TcpConnection>();
			}
			return userMapIter->second;
		}
		Error::ErrorCode LoginedUser::remove(info_type conn){
			if(!conn.lock()){
				LogError << "conn.lock() 返回空";
				return Error::Failed_To_Remove_UserInfo;
			}
			MutexLockGuard guard(mutex_);
			if(!userMap){
				LogError << "userMap is null";
				return Error::Failed_To_Remove_UserInfo;
			}
			std::shared_ptr<std::list<std::string> >  removeList = std::make_shared<std::list<std::string> >();
			userMapIter = userMap->begin();
			for(;userMapIter != userMap->end();userMapIter++){
				if(!userMapIter->second.lock()){
					removeList->push_back(userMapIter->first);
					continue;
				}
				if(userMapIter->second.lock().get() == conn.lock().get()){
					removeList->push_back(userMapIter->first);
					break;
				}
			}
			if(userMapIter != userMap->end()){
				LogInfo << "删除用户,userId = " << userMapIter->first;
			}else{
				LogInfo << "用户信息缓存中未找到符合条件的用户, 未执行删除,";
			}
			for(std::string userId : *removeList){
				userMap->erase(userId);
			}

			return Error::None;
		}
		QLifeUserMsgBuffer * QLifeUserMsgBuffer::instance(){
			static QLifeUserMsgBuffer * qLifeInstance = new QLifeUserMsgBuffer();
			return qLifeInstance;
		}

		QLifeUserMsgBuffer::QLifeUserMsgBuffer(){
			_map = std::make_shared<std::map<std::string /*userId*/, std::shared_ptr<QLifeTypeMsgBuffer>>>();
		}
		QLifeTypeMsgBuffer::QLifeTypeMsgBuffer(std::string userIdP){
			userId = userIdP;
			_map = std::make_shared<std::map<int /*massage Type*/, std::shared_ptr<QLifeTimeMsgBuffer>>>();
		}


		QLifeTimeMsgBuffer::QLifeTimeMsgBuffer(std::string userIdP, int type){
			userId = userIdP;
			msgType = type;
//			_map = std::make_shared<std::map<std::string /*dateTime*/, std::shared_ptr<QLifeMsg> /*massage*/>>();
		}

		QLifeMsg::QLifeMsg(){
			msgType = 0;
		}

		QLifeMsg::QLifeMsg(std::string userIdP, int type, std::string dateTimeP){
			userId = userIdP;
			msgType = type;
			dateTime = dateTimeP;
		}
		int QLifeTimeMsgBuffer::checkMsgBuffer(const std::string &userId, int msgType, std::string dateTime /*DateTime*/){
			LogInfo << "QLifeTimeMsgBuffer::checkMsgBuffer";
			std::map<std::string , std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>>>::iterator mapTimeIter;
			if(!_map) _map = std::make_shared<std::map<std::string , std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>> >>();

			mapTimeIter = _map->find(dateTime);
			if(mapTimeIter == _map->end()){
				std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>>  typeBuf = std::make_shared<std::list<std::shared_ptr<QLifeMsg> /*massage*/>>();
				std::pair<std::string ,std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>>> pair(dateTime, typeBuf);
				_map->insert(pair);
				mapTimeIter = _map->find(dateTime);
			}
			LogInfo << "QLifeTimeMsgBuffer::checkMsgBuffer 2 ";
			return 0;
		}
		int QLifeTypeMsgBuffer::checkMsgBuffer(const std::string &userId, int msgType, std::string dateTime /*DateTime*/){
			LogInfo << "QLifeTypeMsgBuffer::checkMsgBuffer";
			std::map<int /*massage Type*/, std::shared_ptr<QLifeTimeMsgBuffer>>::iterator mapTypeIter;
			if(!_map) _map = std::make_shared<std::map<int ,std::shared_ptr<QLifeTimeMsgBuffer> >>();

			mapTypeIter = _map->find(msgType);
			if(mapTypeIter == _map->end()){
				std::shared_ptr<QLifeTimeMsgBuffer>  typeBuf = std::make_shared<QLifeTimeMsgBuffer>(userId, msgType);
				std::pair<int ,std::shared_ptr<QLifeTimeMsgBuffer>> pair(msgType, typeBuf);
				_map->insert(pair);
				mapTypeIter = _map->find(msgType);
			}

			return mapTypeIter->second->checkMsgBuffer(userId, msgType, dateTime);
		}

		int QLifeUserMsgBuffer::checkMsgBuffer(const std::string &userId, int msgType, std::string dateTime /*DateTime*/){
			LogInfo << "QLifeUserMsgBuffer::checkMsgBuffer";
			MutexLockGuard guard(mutex_);
			std::map<std::string /*userId*/, std::shared_ptr<QLifeTypeMsgBuffer>>::iterator mapUserIter;

			if(!_map) _map = std::make_shared<std::map<std::string ,std::shared_ptr<QLifeTypeMsgBuffer> >>();

			mapUserIter = _map->find(userId);
			if(mapUserIter == _map->end()){
				std::shared_ptr<QLifeTypeMsgBuffer>  typeBuf = std::make_shared<QLifeTypeMsgBuffer>(userId);
				std::pair<std::string ,std::shared_ptr<QLifeTypeMsgBuffer>> pair(userId, typeBuf);
				_map->insert(pair);
				mapUserIter = _map->find(userId);
			}
			return mapUserIter->second->checkMsgBuffer(userId, msgType, dateTime);
		}
		int QLifeTimeMsgBuffer::insertQLifeMsg(const std::string &userId, int msgType, std::string dateTime /*DateTime*/, std::shared_ptr<QLifeMsg> &msg){
			LogInfo << "QLifeTimeMsgBuffer::insertQLifeMsg";
//			checkMsgBuffer(userId, msgType, dateTime);
			std::map<std::string , std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>>>::iterator mapTimeIter;
			mapTimeIter = _map->find(dateTime);
			if(mapTimeIter == _map->end()){
				LogError << "insertQLifeMsg failed error url = -3";
				return -3;
			}

			if(!mapTimeIter->second){
				LogError << "insertQLifeMsg failed error url = -2";
				return -2;
			}
			if(!msg){
				LogError << "insertQLifeMsg failed error url = -1";
				return -1;
			}
			mapTimeIter->second->push_back(msg);
			return 0;
		}
		int QLifeTypeMsgBuffer::insertQLifeMsg(const std::string &userId, int msgType, std::string dateTime /*DateTime*/, std::shared_ptr<QLifeMsg> &msg){
			LogInfo << "QLifeTypeMsgBuffer::insertQLifeMsg";
//			checkMsgBuffer(userId, msgType, dateTime);
			std::map<int /*massage Type*/, std::shared_ptr<QLifeTimeMsgBuffer>>::iterator mapTypeIter;
			mapTypeIter = _map->find(msgType);
			if(mapTypeIter == _map->end()){
				LogError << "insertQLifeMsg failed error url = -4";
				return -4;
			}
			int ret = mapTypeIter->second->insertQLifeMsg(userId, msgType,dateTime, msg);
			if(mapTypeIter->second->_map){
				LogInfo << "QLifeTypeMsgBuffer::insertQLifeMsg mapTypeIter->second->_map not nullptr";
			}
			LogInfo << "QLifeTypeMsgBuffer::insertQLifeMsg ret = " << ret;
			return ret;
		}
		/**
		 *
		 */
		int QLifeUserMsgBuffer::insertQLifeMsg(const std::string &userId, int msgType, std::string date /*DateTime*/, const std::string &msg){
			LogInfo << "QLifeUserMsgBuffer::insertQLifeMsg";
			checkMsgBuffer(userId, msgType, date);
			std::map<std::string /*userId*/, std::shared_ptr<QLifeTypeMsgBuffer>>::iterator mapUserIter;
			mapUserIter = _map->find(userId);
			if(mapUserIter == _map->end()){
				LogError << "insertQLifeMsg failed error url = -5";
				return -5;
			}
			char dateTime[20];
			ft::utils::Time::getLocalFormatTime(dateTime);
			std::shared_ptr<QLifeMsg> qMsg = std::make_shared<QLifeMsg>();
			qMsg->dateTime = dateTime;
			qMsg->date = date;
			qMsg->msg = msg;
			qMsg->msgType = msgType;
			qMsg->userId = userId;
			int ret = mapUserIter->second->insertQLifeMsg(userId, msgType, date, qMsg);

			return ret ;
		}
		/**
		 * 由指定的userId 提醒类型 日期得到对应消息列表
		 * 如果当前notifyType是日提醒，dateTime是该日日期
		 * 如果当前notifyType是周提醒，dateTime是该星期日日期
		 * 如果当前notifyType是月提醒，dateTime是该月最后一天日期
		 * 如果当前notifyType是季提醒，dateTime是该季最后一天日期
		 * 如果当前notifyType是半年提醒，dateTime是该半年最后一天日期
		 * 如果当前notifyType是年提醒，dateTime是该年最后一天日期
		 */
		  int QLifeUserMsgBuffer::getQLifeMsg(const std::string &userId, int msgType,std::string dateTime, int reqType,
				  std::shared_ptr<std::map<std::string /*date*/,
				  std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>> >> & loadMap){
			MutexLockGuard guard(mutex_);
			std::map<std::string /*userId*/, std::shared_ptr<QLifeTypeMsgBuffer>>::iterator mapUserIter;
			std::map<int /*massage Type*/, std::shared_ptr<QLifeTimeMsgBuffer>>::iterator mapTypeIter;
			if(_map){
				mapUserIter = _map->find(userId);
			}
			else {
				LogError << "getQLifeMsg failed error url = -1";
				return -1;
			}
			if(mapUserIter == _map->end()){
				LogInfo << "QLifeUserMsgBuffer::getQlifeMsg mapUserIter == _map->end()";
				return -2;
			}
			if(!mapUserIter->second){
				LogError << "QLifeUserMsgBuffer::getQlifeMsg mapUserIter->second is nullptr";
				return -3;
			}
			if(mapUserIter->second->_map){
				mapTypeIter = mapUserIter->second->_map->find(msgType);
			}
			else {
				LogError << "getQLifeMsg failed error url = -2";
				return -4;
			}
			if(mapTypeIter == mapUserIter->second->_map->end()){
				LogInfo << "QLifeUserMsgBuffer::getQlifeMsg mapTypeIter == mapUserIter->second->_map->end()";
				return -5;
			}
			if(!mapTypeIter->second){
				LogError << "QLifeUserMsgBuffer::getQlifeMsg mapUserIter->second is nullptr";
				return -6;
			}
			if(!mapTypeIter->second->_map){
				LogError << "QLifeUserMsgBuffer::getQlifeMsg mapTypeIter->second->_map is nullptr";
				return -7;
			}
			return getQlifeMsg(mapTypeIter->second->_map, reqType, dateTime, loadMap);

		}


		int QLifeUserMsgBuffer::getQlifeMsg(std::shared_ptr<std::map<std::string /*date*/,
				std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>> >> & mapp,
				int reqType, std::string &dateTime,
				std::shared_ptr<std::map<std::string /*date*/,
				std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>> >> & loadMap)
		{
			LogInfo << "QLifeUserMsgBuffer::getQlifeMsg 2";
			if(!mapp || !loadMap){
				LogError << "QLifeUserMsgBuffer::getQlifeMsg !mapp || !loadMap";
				return -8;
			}
			std::map<std::string /*dateTime*/, std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>> >::iterator mapDateIter;
			int dayNum = 0;

			switch(reqType){
			case ft::knowledgeBase::HealthSugData::DAY:
				dayNum = 1;
				break;
			case ft::knowledgeBase::HealthSugData::WEEK:
				dayNum = 7;
				break;
			case ft::knowledgeBase::HealthSugData::MONTH:
				/**
				 * 得到该月多少天
				 */
				dayNum = 30;
				break;
			case ft::knowledgeBase::HealthSugData::SEASON:
				/**
				 * 得到该月多少天
				 */
				dayNum = 90;
				break;
			case ft::knowledgeBase::HealthSugData::HALFY:
				/**
				 * 得到该月多少天
				 */
				dayNum = 180;
				break;
			case ft::knowledgeBase::HealthSugData::YEAR:
				/**
				 * 得到该月多少天
				 */
				dayNum = 365;
				break;
			}
			date today = day_clock::local_day();
			std::string todayStr = to_iso_extended_string(today);
			date yestoday = today - days(1);
			std::string yestodayStr = to_iso_extended_string(yestoday);

			date nextDate(from_string(dateTime));
			LogInfo << "QLifeUserMsgBuffer::getQlifeMsg dayNum = " << dayNum;
			mapDateIter = mapp->end();
			if(mapp){
				for(int i = 0; i < dayNum; i++){
					std::string nextStr = to_iso_extended_string(nextDate);

					mapDateIter = mapp->find(nextStr);
					if(mapDateIter != mapp->end()){
						std::pair<std::string /*date*/,std::shared_ptr<std::list<std::shared_ptr<QLifeMsg> /*massage*/>>> pair(mapDateIter->first, mapDateIter->second);
						loadMap->insert(pair);
					}


					nextDate = nextDate - days(1);
				}
			}

			return 0;
		}

		int QLifeUserMsgBuffer::getQLifeMsgPush(const std::string &userId, std::shared_ptr<std::map<int /*massage Type*/, std::shared_ptr<QLifeTimeMsgBuffer>>> & loadMap){
			if(!loadMap){
				LogError << "QLifeUserMsgBuffer::getQlifeMsgPush  !loadMap";
				return -8;
			}
			MutexLockGuard guard(mutex_);
			std::map<std::string /*userId*/, std::shared_ptr<QLifeTypeMsgBuffer>>::iterator mapUserIter;
			std::map<int /*massage Type*/, std::shared_ptr<QLifeTimeMsgBuffer>>::iterator mapTypeIter;
			if(_map){
				mapUserIter = _map->find(userId);
			}
			else {
				LogError << "getQLifeMsg failed error url = -1";
				return -1;
			}
			if(mapUserIter == _map->end()){
				LogInfo << "QLifeUserMsgBuffer::getQlifeMsg mapUserIter == _map->end()";
				return -2;
			}
			if(!mapUserIter->second){
				LogError << "QLifeUserMsgBuffer::getQlifeMsg mapUserIter->second is nullptr";
				return -3;
			}
			if(!mapUserIter->second->_map){
				LogError << "QLifeUserMsgBuffer::getQlifeMsg mapUserIter->second->_map is nullptr";
				return -4;
			}
			LogInfo << "开始遍历查找";
			mapTypeIter = mapUserIter->second->_map->begin();
			std::shared_ptr<std::list<int> > listPtr = std::make_shared<std::list<int> >();
			for(;mapTypeIter != mapUserIter->second->_map->end(); mapTypeIter++){
				if(mapTypeIter->first == MessageType::QualityLifeDayNotifyPush
						|| mapTypeIter->first == MessageType::QualityLifeWeekNotifyPush
						|| mapTypeIter->first == MessageType::QualityLifeMonthNotifyPush
						|| mapTypeIter->first == MessageType::QualityLifeSeasonNotifyPush
						|| mapTypeIter->first == MessageType::QualityLifeHalfYearNotifyPush
						|| mapTypeIter->first == MessageType::QualityLifeYearNotifyPush
						){
					LogInfo << "找到推送消息了，pushType = " << mapTypeIter->first;
					std::pair<int /*massage Type*/, std::shared_ptr<QLifeTimeMsgBuffer>> pair(mapTypeIter->first, mapTypeIter->second);
					loadMap->insert(pair);
					listPtr->push_back(mapTypeIter->first);
				}
			}
			for(auto type : *listPtr){
				mapUserIter->second->_map->erase(type);
			}
			return 0;
		}
	}

}

