//
// Created by nbaiot@126.com on 2020/9/1.
//

#ifndef RTCGATEWAY_MEMBER_H
#define RTCGATEWAY_MEMBER_H

#include <string>
#include <memory>
#include <atomic>

namespace nbaiot::rtc {

class Room;

class Member {
public:
  Member(uint64_t sessionId, std::string uid);

  ~Member();

  std::string Uid();

  void SetRoomId(uint32_t roomId);

  uint32_t RoomId();

  void OnSessionInvalid();

private:
  void OtherMemberJoinRoom(const std::shared_ptr<Member>& member,
                           const std::shared_ptr<Room>& room);

  void OtherMemberLeaveRoom(const std::shared_ptr<Member>& member,
                            const std::shared_ptr<Room>& room);

  void RoomDestroy(const std::shared_ptr<Room>& room);

  friend class Room;

private:
  uint64_t session_id_;
  std::atomic_uint32_t room_id_;
  std::string uid_;
};

}

#endif //RTCGATEWAY_MEMBER_H
