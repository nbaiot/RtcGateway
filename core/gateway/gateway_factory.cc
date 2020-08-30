//
// Created by nbaiot@126.com on 2020/8/29.
//

#include "gateway_factory.h"
#include "rtc_gateway.h"

namespace nbaiot::rtc{

std::shared_ptr<GatewayInterface> GatewayFactory::CreateRtcGateway() {
  return std::make_shared<RtcGateway>();
}

}