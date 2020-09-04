//
// Created by nbaiot@126.com on 2020/9/4.
//

#ifndef RTCGATEWAY_RTC_PEER_CONNECTION_H
#define RTCGATEWAY_RTC_PEER_CONNECTION_H

namespace nbaiot::rtc {

enum RTCPeerConnectionState {
  kNew,           /**< */
  kConnecting,    /**< */
  kConnected,     /**< */
  kDisconnected,  /**< */
  kFailed,        /**< */
  kClosed,        /**< */
};

class RtcPeerConnection {

};

}

#endif //RTCGATEWAY_RTC_PEER_CONNECTION_H
