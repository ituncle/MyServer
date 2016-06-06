//
// Created by root on 3/4/16.
//

#ifndef YFQSERVER_CONNECTIONPOOL_H
#define YFQSERVER_CONNECTIONPOOL_H


#include <boost/noncopyable.hpp>

/* Standard C/C++ Includes */
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cassert>

#include <pthread.h>

/* MySQL Connector/C++ API */
#include <cppconn/driver.h>

#include <cstdio>

#include "connection.h"
#include "../../log/ftlog.h"
#include "../../network/muduo/base/Mutex.h"
#include "../../network/muduo/base/Condition.h"

namespace ft {
namespace mysql {

class ConnectionPool final : boost::noncopyable {
private:
  ConnectionPool() : not_empty_(lock_) {
    try {
      driver_ = get_driver_instance();
    } catch (sql::SQLException &e) {
      LogFatal << "failed to create one instance of sql::Driver: " << e.getErrorCode() << ": " << e.what();
    }
  }

  sql::Connection *createConnection(std::string &error) {
    sql::Connection *conn = nullptr;
    try {
      // connect("tcp://host:port/schema", user, pwd);
      sql::Connection *conn = driver_->connect(url_, user_, pwd_);
//                    conn->setSchema(db_);
      return conn;
    } catch (sql::SQLException &e) {
      if (conn != nullptr) delete conn;
      char buf[300] = {0};
      snprintf(buf, sizeof(buf), "failed to connect to mysql-server(\"%s\") with user(\"%s\") and password(\"%s\"): ",
               url_.c_str(), user_.c_str(), pwd_.c_str());
      error = std::string(buf) +
              std::to_string(e.getErrorCode()) + std::string(" - ") + e.what();
      return nullptr;
    }
  }

public:
  ~ConnectionPool() {
    muduo::MutexLockGuard guard(lock_);
  }

  /**
   * get the unique instance of this class ConnectionPool
   */
  static ConnectionPool *instance() {
    static ConnectionPool pool;       // it's ok on centos7 with c++11
    return &pool;
  }

  bool init(const std::string &host,
            const std::string &port,
            const std::string &schema,
            const std::string &user,
            const std::string &pwd,
            size_t maxsize = 1,
            size_t initsize = 1) {
    muduo::MutexLockGuard guard(lock_);

    host_ = host;
    port_ = port;
    schema_ = schema;
    user_ = user;
    pwd_ = pwd;
//                db_ = db;
    maxsize_ = maxsize;
    initsize_ = initsize;
    currsize_ = 0;
    // "tcp://host:port/schema"
    const int len = 9 + host_.length() + port_.length() + schema_.length();
    std::unique_ptr<char[]> buf(new char[len]);
    snprintf(buf.get(), len, "tcp://%s:%s/%s", host_.c_str(), port_.c_str(), schema_.c_str());
    url_.assign(buf.get(), len);

    connections_.reserve(maxsize);

    std::string error;
    for (size_t i = 0; i < initsize_ && i < maxsize_; ++i) {
      sql::Connection *sqlconn = createConnection(error);
      if (sqlconn == nullptr) {
        LogError << error;
        return false;
      }
      connections_.emplace_back(new Connection(sqlconn));
      ++currsize_;
    }

    return true;
  }

  std::shared_ptr<Connection> getConnection() {
    std::shared_ptr<Connection> conn;

    muduo::MutexLockGuard guard(lock_);

    if (connections_.size() == 0 && currsize_ < maxsize_) {
      std::string error;
      sql::Connection *sqlconn = createConnection(error);
      if (sqlconn != nullptr) {
        conn.reset(new Connection(sqlconn));
        ++currsize_;
        return conn;
      } else {
        LogError << error;
      }
    }

    while (connections_.size() == 0) {
      not_empty_.wait();
    }
    assert(connections_.size() > 0);

    conn = connections_.back();
    connections_.pop_back();

    return conn;
  }

  void releaseConnection(std::shared_ptr<Connection> &conn) {
    muduo::MutexLockGuard guard(lock_);
    connections_.push_back(conn);
  }

  void destoryConnection(std::shared_ptr<Connection> &conn) {
    // need to do nothing
  }

  size_t size() const {
    muduo::MutexLockGuard guard(lock_);
    return currsize_;
  }

  size_t idleSize() const {
    muduo::MutexLockGuard guard(lock_);
    return connections_.size();
  }

  size_t maxSize() const {
    return maxsize_;
  }

  const std::string &host() const {
    return host_;
  }

  const std::string &port() const {
    return port_;
  }

  const std::string &schema() const {
    return schema_;
  }

  const std::string &user() const {
    return user_;
  }

  const std::string &password() const {
    return pwd_;
  }

private:
  std::string host_;
  std::string port_;
  std::string schema_;
  std::string url_;
  std::string user_;
  std::string pwd_;
  size_t maxsize_ = 1;    // 连接池中最大连接数
  size_t initsize_ = 1;   // the init count of connections in the pool
  size_t currsize_ = 0;    // the current count of connections in the pool
  sql::Driver *driver_;

  mutable muduo::MutexLock lock_;
  mutable muduo::Condition not_empty_;

  std::vector<std::shared_ptr<Connection>> connections_;    //连接池容器
};

class ConnectionGuard final {
private:
  std::shared_ptr<Connection> connection_;
  ConnectionPool* pool_;
public:
  ConnectionGuard(ConnectionPool* pool) : pool_(pool) {
    assert(nullptr != pool);
    connection_ = pool->getConnection();
  }

  std::shared_ptr<Connection>& connection() {
    return connection_;
  }

  ~ConnectionGuard() {
    if (connection_) {
      pool_->releaseConnection(connection_);
    }
  }
};
}
}

#endif //YFQSERVER_CONNECTIONPOOL_H
