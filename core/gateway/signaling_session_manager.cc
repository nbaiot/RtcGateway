//
// Created by nbaiot@126.com on 2020/8/30.
//

#include "signaling_session_manager.h"

#include <glog/logging.h>

#include "messenger/websocket_listener.h"
#include "messenger/websocket_session.h"

#include "signaling_session.h"

namespace nbaiot::rtc {

SignalingSessionManager::SignalingSessionManager() : init_{false} {
  socket_listener_ = std::make_shared<WebsocketListener>(2);
  scheduler_ = std::make_shared<Scheduler>(2);
  worker_ = std::make_shared<Worker>(scheduler_);
  worker_->Start();
}

SignalingSessionManager::~SignalingSessionManager() {
  worker_->Stop();
  scheduler_->Stop();

  if (socket_listener_) {
    socket_listener_->SetOnNewConnectionCallback(nullptr);
    socket_listener_->SetErrorCallback(nullptr);
  }

}


bool SignalingSessionManager::Init(const std::string& ip, uint16_t port) {
  if (init_)
    return true;

  if (!socket_listener_->Init(ip, port)) {
    LOG(ERROR) << "RtcGateway socket listener failed";
    return false;
  }

  socket_listener_->SetOnNewConnectionCallback([this](std::weak_ptr<WebsocketSession> session) {
    OnNewSession(std::move(session));
  });

  socket_listener_->Run();

  init_ = true;

  return init_;
}

void SignalingSessionManager::OnNewSession(std::weak_ptr<WebsocketSession> session) {
#if DEBUG_SIGNALING
  LOG(INFO) << ">>>>>>>>>>>>new session";
#endif
  auto signalingSession = std::make_shared<SignalingSession>(std::move(session));
  signalingSession->SetInvalidCallback([this](const std::shared_ptr<SignalingSession>& session) {
    OnSessionInvalid(session);
  });

  auto weakSession = signalingSession->weak_from_this();

  /// period check session health
  worker_->ScheduleEvery([weakSession]() {
    auto session = weakSession.lock();
    if (session) {
      if (!WhetherHealth(session)) {
        session->Close();
        return false;
      } else {
        return true;
      }
    }
#if DEBUG_SIGNALING
    LOG(INFO) << ">>>>>>>>>>>>>>>>>> remove check session health";
#endif
    return false;
  }, std::chrono::seconds(5));

  WriteLock lk(set_mutex_);
  sessions_.insert(signalingSession);
}

void SignalingSessionManager::OnSessionInvalid(const std::shared_ptr<SignalingSession>& session) {
  WriteLock lk(set_mutex_);
  auto it = sessions_.find(session);
  if (it != sessions_.end()) {
    /// TODO: 添加其他处理
#if DEBUG_SIGNALING
    LOG(INFO) << ">>>>>>>>>> session invalid!!!";
#endif
    sessions_.erase(it);
  }
}


bool SignalingSessionManager::WhetherHealth(const std::shared_ptr<SignalingSession>& session) {
#if DEBUG_SIGNALING
  LOG(INFO) << ">>>>>>>>>> check session health...";
#endif
  auto now = std::chrono::steady_clock::now();
  auto delay = now - session->LastReceiveTimePoint();
  return !(delay >= std::chrono::seconds(5));
}

}