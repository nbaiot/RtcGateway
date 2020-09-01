//
// Created by nbaiot@126.com on 2020/8/31.
//

#ifndef RTCGATEWAY_SIGNALING_RESPONSE_GENERATOR_H
#define RTCGATEWAY_SIGNALING_RESPONSE_GENERATOR_H

#include <nlohmann/json.hpp>

namespace nbaiot::rtc {

class SignalingResponseGenerator {

public:
  static nlohmann::json ResponseCreateSession(const std::string& transId, uint64_t sessionId);

  static nlohmann::json ResponseDestroySession(const std::string& transId, uint64_t sessionId, bool success,
                                               int code, const std::string& error);
};

}

#endif //RTCGATEWAY_SIGNALING_RESPONSE_GENERATOR_H
