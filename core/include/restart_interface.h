//
// Created by nbaiot@126.com on 2020/8/29.
//

#ifndef RTCGATEWAY_RESTART_INTERFACE_H
#define RTCGATEWAY_RESTART_INTERFACE_H

namespace nbaiot::rtc {

class RestartInterface {

public:
  virtual ~RestartInterface() = default;

  virtual void Restart() = 0;
};

}


#endif //RTCGATEWAY_RESTART_INTERFACE_H
