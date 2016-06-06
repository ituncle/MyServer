// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_BLOCKINGQUEUE_H
#define MUDUO_BASE_BLOCKINGQUEUE_H

#include "Condition.h"
#include "Mutex.h"
#include "Queue.h"

#include <boost/noncopyable.hpp>
#include <deque>
#include <assert.h>

namespace muduo
{

template<typename T>
class BlockingQueue : public Queue<T>, boost::noncopyable
{
 public:
  BlockingQueue()
    : mutex_(),
      notEmpty_(mutex_),
      queue_()
  {
  }

  void put(const T& x) override
  {
    MutexLockGuard lock(mutex_);
    queue_.push_back(x);
    notEmpty_.notify(); // wait morphing saves us
    // http://www.domaigne.com/blog/computing/condvars-signal-with-mutex-locked-or-not/
  }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  void put(T&& x) override
  {
    MutexLockGuard lock(mutex_);
    queue_.push_back(std::move(x));
    notEmpty_.notify();
  }
  // FIXME: emplace()
#endif

  T take() override
  {
    MutexLockGuard lock(mutex_);
    // always use a while-loop, due to spurious wakeup
    while (queue_.empty())
    {
      notEmpty_.wait();
    }
    assert(!queue_.empty());
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    T front(std::move(queue_.front()));
#else
    T front(queue_.front());
#endif
    queue_.pop_front();
    return front;
  }

  bool empty() const override  {
    MutexLockGuard lock(mutex_);
    return queue_.empty();
  }

  size_t size() const override
  {
    MutexLockGuard lock(mutex_);
    return queue_.size();
  }

 private:
  mutable MutexLock mutex_;
  Condition         notEmpty_;
  std::deque<T>     queue_;
};

}

#endif  // MUDUO_BASE_BLOCKINGQUEUE_H
