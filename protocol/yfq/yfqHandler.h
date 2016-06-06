//
// Created by root on 5/27/16.
//

#ifndef JOBSERVER_YFQHANDLER_H
#define JOBSERVER_YFQHANDLER_H

#include "protocol/Handler.h"
#include "yfqParser.h"
#include "errcode.h"

#include "log/ftlog.h"

namespace ft {
namespace proto {

class YfqHandler : public Handler<Request> {
public:
  YfqHandler(muduo::ThreadPool* pool, size_t workers_num) : Handler<Request>(pool, workers_num) {}

  void handle(std::shared_ptr<Request>& request) {
    yfqError::ErrorCode error = yfqError::Invalid;
    LogInfo << "reqtype_ = " << request->reqtype_;
    switch (request->reqtype_) {
      case MessageType::HeartBeat:
        error = heartBeat(request);
        break;
      default:
        LogError << "unkown message, type = " << request->reqtype_ << ", bodylen = " << request->rquest_.length();
        break;
    }

    // 我们认为，Error::Invalid表示不需要向对方回复“通用应答包”
    if (yfqError::Invalid == error) {
      return;
    }

    // 需要回复“通用应答包”的，统一在这里回复
    YfqProtocol::CommonAns ans;
    ans.set_request_id(request->reqtype_);
    ans.set_error_code(error);
    ans.set_progress(request->progress);
    request->respond(ans.SerializeAsString(), MessageType::CommonAns);
  }

  yfqError::ErrorCode heartBeat(const std::shared_ptr<Request> &request) {
    assert(request->reqtype_ == MessageType::HeartBeat);
    auto hb = std::make_shared<YfqProtocol::HeartBeat>();
    if (!hb->ParseFromString(request->rquest_)) {
      LogError << "收到心跳包数据 , 但protobuf解析失败";
      return yfqError::Protocol_Fatal;
    }

    LogTrace << "受到心跳包数据" << ", unix 时间戳 为 " << hb->date_time();
    // TODO

    // 1, respond
    // 2, set connection last active time

    request->respond(request->rquest_, request->reqtype_);

    return yfqError::Invalid;
  }
};
}
}

#endif //JOBSERVER_YFQHANDLER_H
