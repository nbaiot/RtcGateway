//
// Created by nbaiot@126.com on 2020/8/30.
//

#ifndef RTCGATEWAY_SIGNALING_SESSION_MANAGER_H
#define RTCGATEWAY_SIGNALING_SESSION_MANAGER_H

#include <mutex>
#include <string>
#include <memory>
#include <functional>
#include <shared_mutex>
#include <unordered_set>

#include "thread/worker.h"
#include "thread/scheduler.h"

namespace nbaiot::rtc {

class WebsocketListener;
class SignalingSession;
class WebsocketSession;

class SignalingSessionManager {

public:
  SignalingSessionManager();

  ~SignalingSessionManager();

  bool Init(const std::string& ip, uint16_t port);

private:
  void OnNewSession(std::weak_ptr<WebsocketSession> session);

  void OnSessionInvalid(const std::shared_ptr<SignalingSession>& session);

  static bool WhetherHealth(const std::shared_ptr<SignalingSession>& session);

private:
  bool init_;
  std::shared_ptr<WebsocketListener> socket_listener_;

  using ReadLock = std::shared_lock<std::shared_mutex>;
  using WriteLock = std::unique_lock<std::shared_mutex>;
  std::shared_mutex set_mutex_;
  std::unordered_set<std::shared_ptr<SignalingSession>> sessions_;

  std::shared_ptr<Scheduler> scheduler_;
  std::shared_ptr<Worker> worker_;


};

}

#endif //RTCGATEWAY_SIGNALING_SESSION_MANAGER_H
