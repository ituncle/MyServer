//
// Created by root on 5/28/16.
//

#include "EchoParser.h"
#include "log/ftlog.h"

namespace ft {
namespace proto {

void EchoParser::parse(const std::weak_ptr<muduo::net::TcpConnection> &conn,
           muduo::net::Buffer *buf, const muduo::Timestamp &time) {
//  const char* end = nullptr;
//  size_t len = 0;
//  while ((end = buf->findCRLF()) != nullptr){
//    len = end - buf->peek();
//    std::shared_ptr<EchoRequest> request =
//        std::make_shared<EchoRequest>(conn, buf->peek(), len, time);
//    requests_->put(request);
//    buf->retrieve(len + 2);
//  }
  size_t len = buf->readableBytes();
  std::shared_ptr<EchoRequest> request =
        std::make_shared<EchoRequest>(conn, buf->peek(), len, time);
  requests_->put(request);
  buf->retrieve(len);

}

}
}
