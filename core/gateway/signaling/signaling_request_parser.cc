//
// Created by nbaiot@126.com on 2020/8/31.
//

#include "signaling_request_parser.h"

#include <glog/logging.h>
#include <nlohmann/json.hpp>

#include "signaling_packet_defines.h"

using namespace nlohmann;

namespace nbaiot::rtc {

bool SignalingRequestParser::Parse(const std::string& msg) {
  try {
    auto js = json::parse(msg);

  } catch (std::exception& e) {
    LOG(ERROR) << "parse msg:" << msg << "\n"
               << "error:" << e.what();
    return false;
  }
  return true;
}

}