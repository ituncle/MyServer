//
// Created by root on 5/29/16.
//

#include "EchoRequest.h"

namespace ft {
namespace proto {
  bool EchoRequest::echo_respond() {
    auto connection = conn.lock();
    if (!connection) return false;

    connection->send(data.data(), data.length());
    return true;
  }
}
}