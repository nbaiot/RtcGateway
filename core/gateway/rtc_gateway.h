//
// Created by nbaiot@126.com on 2020/8/29.
//

#ifndef RTCGATEWAY_RTC_GATEWAY_H
#define RTCGATEWAY_RTC_GATEWAY_H

#include <memory>

#include "gateway_interface.h"

namespace nbaiot::rtc {

class RtcGateway : public GatewayInterface {

public:
  RtcGateway();

  ~RtcGateway() override;

  bool Init(const YAML::Node& config) override;

  void Start() override;

  void Stop() override;

  void SetOnReadyCallback(OnReadyCallback) override;

  void SetErrorCallback(OnErrorCallback callback) override;

  void Restart() override;

};

}

#endif //RTCGATEWAY_RTC_GATEWAY_H
