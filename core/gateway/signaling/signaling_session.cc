//
// Created by nbaiot@126.com on 2020/8/29.
//

#include "signaling_session.h"

#include <glog/logging.h>

#include "gateway/messenger/websocket_session.h"
#include "signaling_request_parser.h"
#include "signaling_process_thread.h"
#include "signaling_response_generator.h"
#include "signaling_packet_defines.h"
#include "room_manager.h"
#include "room.h"
#include "thread/worker.h"
#include "utils/id_generator.h"

namespace nbaiot::rtc {

SignalingSession::SignalingSession(std::weak_ptr<WebsocketSession> session) : websocket_(std::move(session)) {
  request_parser_ = std::make_unique<SignalingRequestParser>(this);
  InstallWebsocketSessionHandler();
  UpdateReceiveTimePoint();
  worker_ = SignalingProcessThread::Instance()->GetWorker();
}

SignalingSession::~SignalingSession() {
  auto ses = websocket_.lock();
  if (ses) {
    ses->SetDisconnectCallback(nullptr);
    ses->SetReceiveMsgCallback(nullptr);
  }
  /// 清除 session 相关信息
  if (!Invalid()) {
    session_id_ = 0;
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
  ClearSession();
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

uint64_t SignalingSession::SessionId() {
  return session_id_;
}

bool SignalingSession::Invalid() {
  return session_id_ == 0;
}

void SignalingSession::OtherMemberJoinRoom(const std::shared_ptr<Member>& member, const std::shared_ptr<Room>& room) {

}

void SignalingSession::OtherMemberLeaveRoom(const std::shared_ptr<Member>& member, const std::shared_ptr<Room>& room) {

}

void SignalingSession::RoomDestroy(const std::shared_ptr<Room>& room) {

}

void SignalingSession::OnCreateSession(const std::string& transId, const nlohmann::json& body) {
  /// 如果当前已有 sessionId 则无需创建
  worker_->PostTask(SafeTask([transId, body](const std::shared_ptr<SignalingSession>& session) {
      if (session->SessionId() == 0) {
        session->session_id_ = nbaiot::util::RandomUint64();
      }
      auto response = SignalingResponseGenerator::ResponseCreateSession(transId, session->session_id_);
      auto socket = session->websocket_.lock();
      if (socket) {
        socket->Send(std::make_shared<std::string>(response.dump()));
      }
  }));
}

void SignalingSession::OnDestroySession(const std::string& transId, const nlohmann::json& body) {
  auto sid = body.value(SESSION_ID, uint64_t(0));
  bool success = false;
  int code = SUCCESS_CODE;
  std::string error;
  if (sid == session_id_) {
    ClearSession();
    success = true;
  } else {
    code = INVALID_SESSION_CODE;
    error = "invalid session";
  }
  auto response = SignalingResponseGenerator::ResponseDestroySession(transId, session_id_, success, code, error);
  auto socket = websocket_.lock();
  if (socket) {
    socket->Send(std::make_shared<std::string>(response.dump()));
  }
}

void SignalingSession::ClearSession() {
   /// leave room
  auto room = RoomManager::Instance()->FindRoom(joined_room_id_);
  if (room) {
    room->RemoveMember(SessionId());
  }
  session_id_ = 0;
  joined_room_id_ = 0;
}

void SignalingSession::OnCreateRoom(const std::string& transId, const nlohmann::json& body) {

}

void SignalingSession::OnDestroyRoom(const std::string& transId, const nlohmann::json& body) {

}

void SignalingSession::OnExistsRoom(const std::string& transId, const nlohmann::json& body) {

}

void SignalingSession::OnListRoom(const std::string& transId, const nlohmann::json& body) {

}


void SignalingSession::OnJoinRoom(const std::string& transId, const nlohmann::json& body) {

}

void SignalingSession::OnLeaveRoom(const std::string& transId, const nlohmann::json& body) {

}

std::function<void()>
SignalingSession::SafeTask(const std::function<void(std::shared_ptr<SignalingSession>)>& func) {
  auto weakSelf = weak_from_this();
  return [func, weakSelf]() {
      if (auto self = weakSelf.lock()) {
        func(self);
      }
  };
}

}