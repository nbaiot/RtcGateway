//
// Created by nbaiot@126.com on 2020/8/29.
//

#include "signaling_session.h"

#include <glog/logging.h>

#include "messenger/websocket_session.h"

namespace nbaiot::rtc {

SignalingSession::SignalingSession(std::weak_ptr<WebsocketSession> session) : websocket_(std::move(session)) {
  auto ses = websocket_.lock();
  if (ses) {
    ses->SetDisconnectCallback([this](){
      if (invalid_callback_) {
        invalid_callback_(shared_from_this());
      }
    });

    ses->SetReceiveMsgCallback([this](const std::string& msg){
      OnMsgCallback(msg);
    });
  }

  UpdateReceiveTimePoint();
}

SignalingSession::~SignalingSession() {
  auto ses = websocket_.lock();
  if (ses) {
    ses->SetDisconnectCallback(nullptr);
    ses->SetReceiveMsgCallback(nullptr);
  }
#if DEBUG_SIGNALING
  LOG(INFO) << "~~~~~~~~~~SignalingSession";
#endif
}

void SignalingSession::SetInvalidCallback(SignalingSession::OnSessionInvalid callback) {
  invalid_callback_ = std::move(callback);
}

void SignalingSession::OnMsgCallback(const std::string& msg) {
  UpdateReceiveTimePoint();
}

std::chrono::steady_clock::time_point SignalingSession::LastReceiveTimePoint() {
  return last_receive_point_;
}

void SignalingSession::UpdateReceiveTimePoint() {
  last_receive_point_ =  std::chrono::steady_clock::now();
}

void SignalingSession::Close() {
  auto socket = websocket_.lock();
  if (socket) {
    socket->Close();
  }
}


}