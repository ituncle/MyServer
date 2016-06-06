//
// Created by root on 2/25/16.
//

#ifndef YFQSERVER_PROTO_H
#define YFQSERVER_PROTO_H

#include "network/muduo/net/TcpConnection.h"
#include "protocol/yfq/yfqprotoserver3.pb.h"
#include "network/muduo/net/Buffer.h"
#include "network/muduo/base/Timestamp.h"
#include "network/muduo/base/Queue.h"

#include <vector>

#include <cassert>

namespace ft {
namespace proto {
template <typename Request_t>
class Parser : boost::noncopyable {
private:
  std::string name_;
public:
  Parser() {}
  virtual ~Parser() {}

  void setName(std::string name) {
    swap(name_, name);
  }
  const std::string& name() const {return name_;}
  virtual std::shared_ptr<muduo::Queue<std::shared_ptr<Request_t>>> getRequestQueue() = 0;
  virtual void parse(const std::weak_ptr<muduo::net::TcpConnection> &conn,
             muduo::net::Buffer *buf, const muduo::Timestamp &time) = 0;
};
}
}

#endif //YFQSERVER_PROTO_H
