//
// Created by nbaiot@126.com on 2020/8/31.
//

#ifndef RTCGATEWAY_SIGNALING_REQUEST_PARSER_H
#define RTCGATEWAY_SIGNALING_REQUEST_PARSER_H

#include <memory>

namespace nbaiot::rtc {

class SignalingSession;

class SignalingRequestParser {

public:
  explicit SignalingRequestParser(SignalingSession* session);

  bool Parse(const std::string& msg);

private:
  SignalingSession* session_;
};

}

#endif //RTCGATEWAY_SIGNALING_REQUEST_PARSER_H
