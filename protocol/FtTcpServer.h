//
// Created by root on 3/1/16.
//

#ifndef YFQSERVER_YFQTCPSERVER_H
#define YFQSERVER_YFQTCPSERVER_H

#include "protocol/Pareser.h"
#include "protocol/Handler.h"
#include "network/muduo/net/TcpServer.h"
#include "network/muduo/net/EventLoop.h"
#include "network/muduo/net/InetAddress.h"
#include "network/muduo/net/TcpConnection.h"
#include "network/muduo/net/Buffer.h"
#include "network/muduo/base/Timestamp.h"
#include "network/muduo/base/Logging.h"

#include <boost/noncopyable.hpp>

#include <functional>

#include <cassert>

namespace ft {

using muduo::net::EventLoop;
using muduo::net::InetAddress;
using muduo::net::TcpConnectionPtr;
using muduo::net::Buffer;
using muduo::Timestamp;


template <typename T>
class TcpServer final : boost::noncopyable {
  using ProtoPtr = std::shared_ptr<ft::proto::Parser<T>>;
  using HandlerPtr = std::shared_ptr<ft::proto::Handler<T>>;
private:
  EventLoop *loop_;
  muduo::net::TcpServer server_;
  std::string name_;
  ProtoPtr proto_;
  HandlerPtr handler_;

public:
  TcpServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const char* name,
               const ProtoPtr& proto,
               const HandlerPtr& handler)
  : loop_(loop),
    server_(loop_, listenAddr, name),
    name_(name),
    proto_(proto),
    handler_(handler) {
    assert(loop_ && proto_ && handler_);

    server_.setThreadNum(0);  // how many threads for socket events
    server_.setConnectionCallback(std::bind(&TcpServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&TcpServer::onMessage, this,
                                         std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    server_.setWriteCompleteCallback(std::bind(&TcpServer::onWriteComplete, this, std::placeholders::_1));

    proto_->setName(name_);
    handler_->setName(name_);
  }

  void setThreadNum(int threadnum) {
    server_.setThreadNum(threadnum);
  }

  void start() {
    handler_->start(proto_->getRequestQueue());
    server_.start();
  }

private:
  void onConnection(const TcpConnectionPtr &conn) {
    if (conn->connected()) {
      LogTrace << "accept a new connection from " << conn->peerAddress().toIpPort();
    } else {
      LogTrace << "the connection from " << conn->peerAddress().toIpPort() << " disconnect";
    }
  }

  void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time) {
    assert(nullptr != buf);
    auto msg = muduo::printBytes(buf->peek(), 0, buf->readableBytes(), 10);
    LogInfo << " recv " << buf->readableBytes() << " bytes from " << conn->peerAddress().toIpPort() << ": " << msg;

    proto_->parse(conn, buf, time);
  }

  void onWriteComplete(const TcpConnectionPtr &conn) {
    LogInfo << "send data completely to " << conn->peerAddress().toIpPort();
  }
};
}

#endif //YFQSERVER_YFQTCPSERVER_H
