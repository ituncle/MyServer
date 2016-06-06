//
// Created by root on 5/27/16.
//

#ifndef JOBSERVER_QUEUE_H
#define JOBSERVER_QUEUE_H

namespace muduo
{

template<typename T>
class Queue {
public:
  virtual ~Queue() {}
  virtual  void put(const T& x) = 0;

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  virtual void put(T&& x) = 0;
#endif

  virtual T take() = 0;
  virtual bool empty() const = 0;
  virtual size_t size() const = 0;

  virtual bool full() const {return false;} // = 0;//
  virtual size_t capacity() const {return 0;} // = 0;//
};
}

#endif //JOBSERVER_QUEUE_H
