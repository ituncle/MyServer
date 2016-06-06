//
// Created by root on 5/29/16.
//

#ifndef JOBSERVER_ECHOREQUEST_H
#define JOBSERVER_ECHOREQUEST_H

#include "network/muduo/net/TcpConnection.h"

#include <string>

namespace ft {
namespace proto {

struct EchoRequest {
  std::weak_ptr<muduo::net::TcpConnection> conn;
  std::string data;
  muduo::Timestamp time;

  EchoRequest(const std::weak_ptr<muduo::net::TcpConnection> &connection,
              const char* buf,
              size_t len,
              const muduo::Timestamp &recvTime)
  : conn(connection),
    data(buf, len),
    time(recvTime) {
  }

  bool echo_respond();
};

}
}

#endif //JOBSERVER_ECHOREQUEST_H
