//
// Created by nbaiot@126.com on 2020/8/29.
//

#ifndef RTCGATEWAY_GATEWAY_INTERFACE_H
#define RTCGATEWAY_GATEWAY_INTERFACE_H

#include <string>
#include <functional>
#include <yaml-cpp/yaml.h>

#include "restart_interface.h"

namespace nbaiot::rtc {

class GatewayInterface : public RestartInterface {

public:
  using OnReadyCallback = std::function<void()>;
  using OnErrorCallback = std::function<void(int code, const std::string& des)>;

  virtual bool Init(const YAML::Node& config) = 0;

  virtual void Start() = 0;

  virtual void Stop() = 0;

  virtual void SetOnReadyCallback(OnReadyCallback callback) = 0;

  virtual void SetErrorCallback(OnErrorCallback callback) = 0;

};


}


#endif //RTCGATEWAY_GATEWAY_INTERFACE_H
