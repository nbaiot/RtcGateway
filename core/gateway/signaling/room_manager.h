//
// Created by nbaiot@126.com on 2020/8/31.
//

#ifndef RTCGATEWAY_ROOM_MANAGER_H
#define RTCGATEWAY_ROOM_MANAGER_H

#include <memory>
#include <vector>
#include <shared_mutex>
#include <unordered_map>

namespace nbaiot::rtc {

class Room;

class RoomManager {

public:
  static RoomManager* Instance();

  std::shared_ptr<Room> CreateRoom();

  std::shared_ptr<Room> CreateRoom(uint32_t roomId);

  void DestroyRoom(uint32_t roomId);

  std::shared_ptr<Room> FindRoom(uint32_t roomId);

  std::vector<std::shared_ptr<Room>> ListRoom();

private:
  RoomManager() = default;

private:
  using ReadLock = std::shared_lock<std::shared_mutex>;
  using WriteLock = std::unique_lock<std::shared_mutex>;
  std::shared_mutex room_mutex_;
  std::unordered_map<uint32_t, std::shared_ptr<Room>> rooms_;
};

}

#endif //RTCGATEWAY_ROOM_MANAGER_H
