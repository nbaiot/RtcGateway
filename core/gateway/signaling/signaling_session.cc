//
// Created by nbaiot@126.com on 2020/8/29.
//

#include "signaling_session.h"

#include <glog/logging.h>

#include "gateway/messenger/websocket_session.h"
#include "signaling_request_parser.h"

namespace nbaiot::rtc {

SignalingSession::SignalingSession(std::weak_ptr<WebsocketSession> session) : websocket_(std::move(session)) {
  request_parser_ = std::make_unique<SignalingRequestParser>();
  InstallRequestParserHandler();
  InstallWebsocketSessionHandler();
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
  request_parser_->Parse(msg);
}

std::chrono::steady_clock::time_point SignalingSession::LastReceiveTimePoint() {
  return last_receive_point_;
}

void SignalingSession::UpdateReceiveTimePoint() {
  last_receive_point_ = std::chrono::steady_clock::now();
}

void SignalingSession::Close() {
  auto socket = websocket_.lock();
  if (socket) {
    socket->Close();
  }
}

void SignalingSession::InstallWebsocketSessionHandler() {
  auto ses = websocket_.lock();
  if (ses) {
    ses->SetDisconnectCallback([this]() {
        if (invalid_callback_) {
          invalid_callback_(shared_from_this());
        }
    });

    ses->SetReceiveMsgCallback([this](const std::string& msg) {
        OnMsgCallback(msg);
    });
  }
}

void SignalingSession::InstallRequestParserHandler() {

}

uint64_t SignalingSession::SessionId() {
  return session_id_;
}

void SignalingSession::OtherMemberJoinRoom(const std::shared_ptr<Member>& member, const std::shared_ptr<Room>& room) {

}

void SignalingSession::OtherMemberLeaveRoom(const std::shared_ptr<Member>& member, const std::shared_ptr<Room>& room) {

}

void SignalingSession::RoomDestroy(const std::shared_ptr<Room>& room) {

}


}