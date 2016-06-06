//
// Created by root on 5/28/16.
//

#ifndef JOBSERVER_TESTHANDLER_H
#define JOBSERVER_TESTHANDLER_H

#include "EchoRequest.h"
#include "protocol/Handler.h"

namespace ft {
namespace proto {

class EchoHandler final : public Handler<EchoRequest> {
public:
  EchoHandler(muduo::ThreadPool* pool, size_t workers_num) : Handler<EchoRequest>(pool, workers_num) {}
  void handle(std::shared_ptr<EchoRequest>& request) override;

//  void
};

}
}

#endif //JOBSERVER_TESTHANDLER_H
