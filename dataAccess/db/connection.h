//
// Created by root on 3/4/16.
//

#ifndef YFQSERVER_CONNECTION_H
#define YFQSERVER_CONNECTION_H

/* MySQL Connector/C++ API */
#include <mysql_connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <memory>
#include <string>

#include "../../log/ftlog.h"

namespace ft {
namespace mysql {

class Connection {
private:
  sql::Connection *conn_ = nullptr;
  sql::Statement *stmt_ = nullptr;
//            sql::PreparedStatement *prep_stmt_ = nullptr;
  sql::ResultSet *res_ = nullptr;
public:
  Connection(sql::Connection *conn)
  : conn_(conn) {
  }

  ~Connection() {
    if (conn_ != nullptr) {
      delete conn_;
      conn_ = nullptr;
    }
    if (stmt_ != nullptr) {
      delete stmt_;
      stmt_ = nullptr;
    }
  }

  bool execute(const char *sql, std::string &error) {
    if (sql == nullptr) {
      error = "sql is nullptr";
      return false;
    }

    if (!ensureConnectionIsVaild(error)) {
      return false;
    }

    try {
      return stmt_->execute(sql);
    } catch (sql::SQLException &e) {
      error = e.what();
      return false;
    }
  }

  // do SELECT
  // return nullptr on failure
  sql::ResultSet *executeQuery(const char *sql, std::string &error) {
    if (sql == nullptr) {
      error = "sql is nullptr";
      return nullptr;
    }

    if (!ensureConnectionIsVaild(error)) {
      return nullptr;
    }

    try {
      return stmt_->executeQuery(sql);
    } catch (sql::SQLException &e) {
      error = std::to_string(e.getErrorCode()) + std::string(" - ") + e.what();
      return nullptr;
    }
  }

  int executeUpdate(const char *sql, std::string &error) {
    if (sql == nullptr) {
      error = "sql is nullptr";
      return -1;
    }

    if (!ensureConnectionIsVaild(error)) {
      return -2;
    }

    try {
      return stmt_->executeUpdate(sql);
    } catch (sql::SQLException &e) {
      error = std::to_string(e.getErrorCode()) + std::string(" - ") + e.what();
      return -3;
    }
  }

private:
  bool ensureConnectionIsVaild(std::string &error) {
    assert(nullptr != conn_);
    if (!conn_->isValid()) {
      if (nullptr != stmt_) {
        delete stmt_;
        stmt_ = nullptr;
      }

      try {
        conn_->reconnect();
      } catch (sql::SQLException &e) {
        error = std::string("failed to reconnect to mysql-server") +
                std::to_string(e.getErrorCode()) + std::string(" - ") + e.what();
        return false;
      }
    }

    if (nullptr == stmt_) {
      try {
        stmt_ = conn_->createStatement();
      } catch (sql::SQLException &e) {
        error = std::string("failed to createStatement from connection") +
                std::to_string(e.getErrorCode()) + std::string(" - ") + e.what();
        return false;
      }
    }

    return true;
  }
};
}
}

#endif //YFQSERVER_CONNECTION_H
