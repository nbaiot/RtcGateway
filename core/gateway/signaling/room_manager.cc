//
// Created by nbaiot@126.com on 2020/8/31.
//

#include "room_manager.h"

#include "room.h"
#include "utils/id_generator.h"

namespace nbaiot::rtc {

RoomManager* RoomManager::Instance() {
  static RoomManager INSTANCE;
  return &INSTANCE;
}

std::shared_ptr<Room> RoomManager::CreateRoom() {
  WriteLock lk(room_mutex_);
  auto roomId = nbaiot::util::RandomUint32();
  while (rooms_.find(roomId) != rooms_.end()) {
    roomId = nbaiot::util::RandomUint32();
  }
  auto room = std::make_shared<Room>(roomId);
  rooms_.emplace(roomId, room);
  return room;
}

std::shared_ptr<Room> RoomManager::CreateRoom(uint32_t roomId) {
  WriteLock lk(room_mutex_);
  auto room = rooms_[roomId];
  if (room) {
    return room;
  }
  room = std::make_shared<Room>(roomId);
  rooms_.emplace(roomId, room);
  return room;
}

void RoomManager::DestroyRoom(uint32_t roomId) {
  WriteLock lk(room_mutex_);
  rooms_.erase(roomId);
}

std::shared_ptr<Room> RoomManager::FindRoom(uint32_t roomId) {
  ReadLock lk(room_mutex_);
  return rooms_[roomId];
}

std::vector<std::shared_ptr<Room>> RoomManager::ListRoom() {
  ReadLock lk(room_mutex_);
  std::vector<std::shared_ptr<Room>> rooms;
  rooms.reserve(rooms_.size());
  for (const auto& it: rooms_) {
    rooms.push_back(it.second);
  }
  return rooms;
}

}