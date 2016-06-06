//
// Created by root on 5/27/16.
//

#ifndef JOBSERVER_HANDLER_H
#define JOBSERVER_HANDLER_H

#include "log/ftlog.h"
#include "network/muduo/base/Queue.h"
#include "network/muduo/base/ThreadPool.h"

#include <boost/noncopyable.hpp>

namespace ft {
namespace proto {

template <typename T>
class Handler : boost::noncopyable {
  using Request_t = T;
private:
  enum State {
    kRuning,
    kStoping,
    kStoped
  } state = kStoped;

  muduo::ThreadPool* pool_;
  size_t threads_num_;
  std::shared_ptr<muduo::Queue<std::shared_ptr<Request_t>>> requets_;
  std::string name_;

public:
  Handler(muduo::ThreadPool *pool, size_t workers_num) : pool_(pool), threads_num_(workers_num) {}
  virtual ~Handler() {stop();}

  virtual void handle(std::shared_ptr<Request_t>& request) = 0;
  void setName(std::string name) {
    swap(name_, name);
  }
  const std::string& name() const {return name_;}

  void start(std::shared_ptr<muduo::Queue<std::shared_ptr<Request_t>>> requets) {
    assert(requets != nullptr);
    if (state != kStoped) {  // FIXME: unsafe
      LogWarn << "The handler \"" << name() << "\" had been started";
      return;
    }
    state = kRuning;
    requets_ = requets;
    for (int i = 0; i < threads_num_; ++i) {
      pool_->run([this]() {
          LogInfo << "handler \"" << name() << "\" start...";
          while (this->state == kRuning) {  // FIXME: unsafe
            auto request = this->requets_->take();
            if (this->state != kRuning || !request) break;
            this->handle(request);
          }
          this->state = kStoped;  // FIXME: unsafe, use atomic instead
          LogInfo << "handler \"" << this->name() << "\" is stopped";
      });
    }
  }

private:
  void stop() {
    LogInfo << "to stop handler \"" << name() << "\"";
    if (state == kRuning) {
      state = kStoping;  // FIXME: unsafe, use atomic instead
      requets_->put(std::shared_ptr<Request_t>());
      while (state != kStoped); // FIXME: unsafe, use atomic instead
    }
  }
};

}
}
#endif //JOBSERVER_HANDLER_H
