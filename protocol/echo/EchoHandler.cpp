//
// Created by root on 5/28/16.
//

#include "EchoHandler.h"
#include "log/ftlog.h"

namespace ft {
namespace proto {

void EchoHandler::handle(std::shared_ptr<EchoRequest>& request) {
//  if (request->data.compare("quit") == 0) {
//    request->conn.lock()->forceClose();
//  } else if (request->data.compare("exit") == 0) {
//    LogWarn << "exit(0) due to command \"exit\"";
//    ::exit(0);
//  } else
  {
    if (!request->echo_respond()) {
      LogError << "failed to echo_respond()";
    }
  }
}

}
}
