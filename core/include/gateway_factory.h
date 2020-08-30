//
// Created by nbaiot@126.com on 2020/8/29.
//

#ifndef RTCGATEWAY_GATEWAY_FACTORY_H
#define RTCGATEWAY_GATEWAY_FACTORY_H

#include <memory>

namespace nbaiot::rtc {

class GatewayInterface;

class GatewayFactory {
public:
  std::shared_ptr<GatewayInterface> CreateRtcGateway();

};

}

#endif //RTCGATEWAY_GATEWAY_FACTORY_H
