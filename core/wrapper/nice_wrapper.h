//
// Created by nbaiot@126.com  on 2020/9/3.
//

#ifndef RTCGATEWAY_NICE_WRAPPER_H
#define RTCGATEWAY_NICE_WRAPPER_H

#include <memory>
#include <thread>
#include <boost/noncopyable.hpp>

struct _NiceAgent;
typedef struct _NiceAgent NiceAgent;

namespace nbaiot::rtc {

class NiceWrapper : public boost::noncopyable {

public:
  NiceWrapper();

  explicit NiceWrapper(bool controlMode);

  ~NiceWrapper();

  void Test();

private:
  std::unique_ptr<NiceAgent, void(*)(void*)> agent_;

};

}

#endif //RTCGATEWAY_NICE_WRAPPER_H
