//
// Created by nbaiot@126.com on 2020/8/31.
//

#include "room.h"
#include "room_manager.h"

namespace nbaiot::rtc {

Room::Room(uint32_t id) : id_(id) {

}

Room::~Room() {

}

uint32_t Room::Id() {
  return id_;
}

void Room::AddMember(const std::shared_ptr<Member>& member) {
  if (FindMember(member->Uid()))
    return;
  WriteLock lk(member_mutex_);
  for (const auto& it: members_) {
    if (it.second) {
      it.second->OtherMemberJoinRoom(member, shared_from_this());
    }
  }
  members_.emplace(member->Uid(), member);
}

void Room::RemoveMember(const std::string& uid) {
  auto member = FindMember(uid);
  if (member) {
    {
      WriteLock lk(member_mutex_);
      members_.erase(uid);
    }
    for (const auto& it: members_) {
      if (it.second) {
        it.second->OtherMemberLeaveRoom(member, shared_from_this());
      }
    }

    if (NoMember()) {
      RoomManager::Instance()->DestroyRoom(id_);
    }
  }
}

std::shared_ptr<Member> Room::FindMember(const std::string& uid) {
  ReadLock lk(member_mutex_);
  return members_[uid];
}

bool Room::NoMember() {
  ReadLock lk(member_mutex_);
  return members_.empty();
}


}