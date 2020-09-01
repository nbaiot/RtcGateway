//
// Created by nbaiot@126.com on 2020/8/29.
//

#ifndef RTCGATEWAY_SIGNALING_SESSION_H
#define RTCGATEWAY_SIGNALING_SESSION_H

#include <atomic>
#include <memory>
#include <string>
#include <chrono>
#include <functional>

#include <nlohmann/json.hpp>

namespace nbaiot {

class Worker;

namespace rtc {

/// TODO:
struct ClientInfo {

};

class Room;

class Member;

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

  uint64_t SessionId();

  bool Invalid();

  /// Event
  void OtherMemberJoinRoom(const std::shared_ptr<Member>& member,
                           const std::shared_ptr<Room>& room);

  void OtherMemberLeaveRoom(const std::shared_ptr<Member>& member,
                            const std::shared_ptr<Room>& room);

  void RoomDestroy(const std::shared_ptr<Room>& room);


private:
  std::function<void()> SafeTask(const std::function<void(std::shared_ptr<SignalingSession>)>& func);

  void ClearSession();

  void InstallWebsocketSessionHandler();

  void OnMsgCallback(const std::string& msg);

  void UpdateReceiveTimePoint();

  /// process request
  void OnCreateSession(const std::string& transId, const nlohmann::json& body);

  void OnDestroySession(const std::string& transId, const nlohmann::json& body);

  void OnCreateRoom(const std::string& transId, const nlohmann::json& body);

  void OnDestroyRoom(const std::string& transId, const nlohmann::json& body);

  void OnExistsRoom(const std::string& transId, const nlohmann::json& body);

  void OnListRoom(const std::string& transId, const nlohmann::json& body);

  void OnJoinRoom(const std::string& transId, const nlohmann::json& body);

  void OnLeaveRoom(const std::string& transId, const nlohmann::json& body);

  friend class SignalingRequestParser;

private:
  std::atomic_uint64_t session_id_{0};
  std::atomic_uint32_t joined_room_id_{0};
  std::weak_ptr<WebsocketSession> websocket_;
  OnSessionInvalid invalid_callback_;
  std::chrono::steady_clock::time_point last_receive_point_;
  std::unique_ptr<SignalingRequestParser> request_parser_;
  std::shared_ptr<Worker> worker_;
};

}

}

#endif //RTCGATEWAY_SIGNALING_SESSION_H
