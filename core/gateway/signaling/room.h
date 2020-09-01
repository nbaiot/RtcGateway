//
// Created by nbaiot@126.com on 2020/8/31.
//

#ifndef RTCGATEWAY_ROOM_H
#define RTCGATEWAY_ROOM_H

#include <string>
#include <memory>
#include <functional>
#include <shared_mutex>
#include <unordered_map>
#include "member.h"


namespace nbaiot::rtc {

class Member;

class Room : public std::enable_shared_from_this<Room> {
public:
  explicit Room(uint32_t id);

  ~Room();

  uint32_t Id();

  void AddMember(const std::shared_ptr<Member>& member);

  void RemoveMember(const std::string& uid);

  void RemoveMember(uint64_t sessionId);

  std::shared_ptr<Member> FindMember(const std::string& uid);

  void EachMember(const std::function<bool(std::shared_ptr<Member>& member)>& func);

  bool NoMember();

private:
  uint32_t id_;
  using ReadLock = std::shared_lock<std::shared_mutex>;
  using WriteLock = std::unique_lock<std::shared_mutex>;
  std::shared_mutex member_mutex_;
  std::unordered_map<std::string, std::shared_ptr<Member>> members_;
};


}

#endif //RTCGATEWAY_ROOM_H
