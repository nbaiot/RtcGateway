//
// Created by nbaiot@126.com on 2020/8/31.
//

#include "signaling_response_generator.h"
#include "signaling_packet_defines.h"

namespace nbaiot::rtc {

nlohmann::json SignalingResponseGenerator::ResponseCreateSession(const std::string& transId, uint64_t sessionId) {
  nlohmann::json res;
  res[MESSAGE] = SUCCESS;
  res[TRANSACTION] = transId;
  nlohmann::json body;
  body[ACTION] = CREATE_ROOM;
  body[SESSION_ID] = sessionId;
  res[BODY] = body;
  return res;
}

nlohmann::json
SignalingResponseGenerator::ResponseDestroySession(const std::string& transId, uint64_t sessionId, bool success,
                                                   int code, const std::string& error) {
  nlohmann::json res;
  res[MESSAGE] = success ? SUCCESS : ERROR;
  res[TRANSACTION] = transId;
  nlohmann::json body;
  body[ACTION] = DESTROY_ROOM;
  body[SESSION_ID] = sessionId;
  if (!success) {
    body[CODE] = code;
    body[ERROR] = error;
  }
  res[BODY] = body;
  return res;
}

}