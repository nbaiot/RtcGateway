//
// Created by nbaiot@126.com on 2020/9/4.
//

#ifndef RTCGATEWAY_ICE_CONNECTION_H
#define RTCGATEWAY_ICE_CONNECTION_H

namespace nbaiot::rtc {

enum IceState {
  kIceConnectionNew, /**< 正在搜集地址或者处于"closed"状态，或当前还没有传输，等待远程候选可用， */
  kIceConnectionChecking, /**< 已收到至少一个远程候选，并进行校验，无论此时是否有可用连接。同时可能在继续收集候选 */
  kIceConnectionConnected, /**< 至少对每个候选发现了一个可用的连接，此时仍然会继续测试远程候选以便发现更优的连接。同时可能在继续收集候选 */
  kIceConnectionCompleted, /**< ICE代理已经发现了可用的连接，不再测试远程候选 */
  kIceConnectionFailed, /**< 测试了所有远程候选没有发现匹配的候选 */
  kIceConnectionDisconnected, /**< 测试不再活跃，这可能是一个暂时的状态，可以自我恢复 */
  kIceConnectionClosed, /**< 不再应答任何请求 */
  kIceConnectionMax, /**< */
};

class IceConnection {

};

}

#endif //RTCGATEWAY_ICE_CONNECTION_H
