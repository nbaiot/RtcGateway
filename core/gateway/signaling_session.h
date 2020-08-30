//
// Created by nbaiot@126.com on 2020/8/29.
//

#ifndef RTCGATEWAY_SIGNALING_SESSION_H
#define RTCGATEWAY_SIGNALING_SESSION_H

#include <memory>
#include <string>
#include <chrono>

#include <functional>

namespace nbaiot::rtc {

/// TODO:
struct ClientInfo {

};

class WebsocketSession;

class SignalingSession : public std::enable_shared_from_this<SignalingSession> {

public:
  using OnSessionInvalid = std::function<void(const std::shared_ptr<SignalingSession>& session)>;

  explicit SignalingSession(std::weak_ptr<WebsocketSession> session);

  ~SignalingSession();

  void SetInvalidCallback(OnSessionInvalid callback);

  std::chrono::steady_clock::time_point LastReceiveTimePoint();

  void Close();

private:
  void OnMsgCallback(const std::string& msg);

  void UpdateReceiveTimePoint();

private:
  std::weak_ptr<WebsocketSession> websocket_;
  OnSessionInvalid invalid_callback_;
  std::chrono::steady_clock::time_point last_receive_point_;
};

}


#endif //RTCGATEWAY_SIGNALING_SESSION_H
