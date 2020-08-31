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

class SignalingRequestParser;

class SignalingSession : public std::enable_shared_from_this<SignalingSession> {

public:
  using OnSessionInvalid = std::function<void(const std::shared_ptr<SignalingSession>& session)>;

  explicit SignalingSession(std::weak_ptr<WebsocketSession> session);

  ~SignalingSession();

  /// 链接断开或者失效
  void SetInvalidCallback(OnSessionInvalid callback);

  std::chrono::steady_clock::time_point LastReceiveTimePoint();

  void Close();

private:
  void InstallWebsocketSessionHandler();

  void InstallRequestParserHandler();

  void OnMsgCallback(const std::string& msg);

  void UpdateReceiveTimePoint();

private:
  std::weak_ptr<WebsocketSession> websocket_;
  OnSessionInvalid invalid_callback_;
  std::chrono::steady_clock::time_point last_receive_point_;
  std::unique_ptr<SignalingRequestParser> request_parser_;
};

}


#endif //RTCGATEWAY_SIGNALING_SESSION_H
