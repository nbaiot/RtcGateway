//
// Created by nbaiot@126.com on 2020/8/29.
//

#include "rtc_gateway.h"

#include <glog/logging.h>

#include "signaling_session_manager.h"

namespace nbaiot::rtc {

RtcGateway::RtcGateway() {
  signaling_manager_ = std::make_shared<SignalingSessionManager>();
}

RtcGateway::~RtcGateway() {

}

bool RtcGateway::Init(const YAML::Node& config) {
  signaling_manager_->Init("0.0.0.0", 9080);
  return true;
}

void RtcGateway::Start() {

}

void RtcGateway::Stop() {

}

void RtcGateway::SetOnReadyCallback(OnReadyCallback) {

}

void RtcGateway::SetErrorCallback(GatewayInterface::OnErrorCallback callback) {

}

void RtcGateway::Restart() {

}

}