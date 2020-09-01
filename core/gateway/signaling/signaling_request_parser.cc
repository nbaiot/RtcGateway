//
// Created by nbaiot@126.com on 2020/8/31.
//

#include "signaling_request_parser.h"

#include <glog/logging.h>
#include <nlohmann/json.hpp>

#include "signaling_session.h"
#include "signaling_packet_defines.h"

using namespace nlohmann;

namespace nbaiot::rtc {

SignalingRequestParser::SignalingRequestParser(SignalingSession* session) : session_(session) {

}

bool SignalingRequestParser::Parse(const std::string& msg) {
  try {
    auto js = json::parse(msg);
    auto type = js.value(MESSAGE, "");
    auto transId = js.value(TRANSACTION, "");
    if (type == REQUEST) {
      auto action = js.value(ACTION, "");
      auto body = js.value(BODY, json());
      if (action == CREATE_SESSION) {
        session_->OnCreateSession(transId, body);
      } else if (action == DESTROY_SESSION) {
        session_->OnDestroySession(transId, body);
      } else if (action == CREATE_ROOM) {
        session_->OnCreateRoom(transId, body);
      } else if (action == DESTROY_ROOM) {
        session_->OnDestroyRoom(transId, body);
      } else if (action == EXISTS_ROOM) {
        session_->OnExistsRoom(transId, body);
      } else if (action == LIST_ROOM) {
        session_->OnListRoom(transId, body);
      } else if (action == JOIN_ROOM) {
        session_->OnJoinRoom(transId, body);
      } else if (action == LEAVE_ROOM) {
        session_->OnLeaveRoom(transId, body);
      }
    }
  } catch (std::exception& e) {
    LOG(ERROR) << "parse msg:" << msg << "\n"
               << "error:" << e.what();
    return false;
  }
  return true;
}

}