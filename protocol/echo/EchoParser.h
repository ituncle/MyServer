//
// Created by root on 5/28/16.
//

#ifndef JOBSERVER_TESTPROTO_H
#define JOBSERVER_TESTPROTO_H

#include "EchoRequest.h"
#include "network/muduo/base/BlockingQueue.h"
#include "protocol/Pareser.h"

namespace ft {
namespace proto {

class EchoParser final : public Parser<EchoRequest> {
private:
  std::shared_ptr<muduo::BlockingQueue<std::shared_ptr<EchoRequest>>> requests_ =
      std::make_shared<muduo::BlockingQueue<std::shared_ptr<EchoRequest>>>();

public:
  std::shared_ptr<muduo::Queue<std::shared_ptr<EchoRequest>>> getRequestQueue() override {
    return requests_;
  }
  void parse(const std::weak_ptr<muduo::net::TcpConnection> &conn,
             muduo::net::Buffer *buf, const muduo::Timestamp &time) override ;
};

}
}

#endif //JOBSERVER_TESTPROTO_H
