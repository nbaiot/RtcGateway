//
// Created by nbaiot@126.com on 2020/9/1.
//

#include "member.h"
#include "room.h"
#include "room_manager.h"
#include "signaling_session_manager.h"
#include "signaling_session.h"

namespace nbaiot::rtc {


Member::Member(uint64_t sessionId, std::string uid) : session_id_(sessionId), room_id_(0), uid_(std::move(uid)) {

}

Member::~Member() {

}

std::string Member::Uid() {
  return uid_;
}

void Member::OtherMemberJoinRoom(const std::shared_ptr<Member>& member, const std::shared_ptr<Room>& room) {
  auto session = SignalingSessionManager::Instance()->FindSession(session_id_);
  if (session) {
    session->OtherMemberJoinRoom(member, room);
  }
}

void Member::OtherMemberLeaveRoom(const std::shared_ptr<Member>& member, const std::shared_ptr<Room>& room) {
  auto session = SignalingSessionManager::Instance()->FindSession(session_id_);
  if (session) {
    session->OtherMemberLeaveRoom(member, room);
  }
}

void Member::RoomDestroy(const std::shared_ptr<Room>& room) {
  auto session = SignalingSessionManager::Instance()->FindSession(session_id_);
  if (session) {
    session->RoomDestroy(room);
  }
}

void Member::SetRoomId(uint32_t roomId) {
  room_id_ = roomId;
}

uint32_t Member::RoomId() {
  return room_id_;
}

void Member::OnSessionInvalid() {
  auto room = RoomManager::Instance()->FindRoom(room_id_);
  if (room) {
    room->RemoveMember(uid_);
  }
}

}