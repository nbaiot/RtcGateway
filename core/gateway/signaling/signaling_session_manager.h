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

namespace nbaiot {

class Worker;

class Scheduler;

class ThreadPool;

namespace rtc {

class WebsocketListener;

class SignalingSession;

class WebsocketSession;

class SignalingSessionManager {

public:
  static SignalingSessionManager* Instance();

  bool Init(const std::string& ip, uint16_t port);

  std::shared_ptr<SignalingSession> FindSession(uint64_t sessionId);


private:
  SignalingSessionManager();

  ~SignalingSessionManager();

  void OnNewSession(std::weak_ptr<WebsocketSession> session);

  void OnSessionDisconnect(const std::shared_ptr<SignalingSession>& session);

  static bool WhetherHealth(const std::shared_ptr<SignalingSession>& session);

private:
  bool init_;
  std::shared_ptr<WebsocketListener> socket_listener_;

  using ReadLock = std::shared_lock<std::shared_mutex>;
  using WriteLock = std::unique_lock<std::shared_mutex>;
  std::shared_mutex set_mutex_;
  std::unordered_set<std::shared_ptr<SignalingSession>> sessions_;
  std::shared_ptr<Scheduler> scheduler_;
  std::shared_ptr<Worker> health_worker_;


};

}

}

#endif //RTCGATEWAY_SIGNALING_SESSION_MANAGER_H
