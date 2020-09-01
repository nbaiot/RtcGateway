//
// Created by nbaiot@126.com on 2020/9/1.
//

#ifndef RTCGATEWAY_SIGNALING_PROCESS_THREAD_H
#define RTCGATEWAY_SIGNALING_PROCESS_THREAD_H

#include <memory>

namespace nbaiot {

class Worker;
class ThreadPool;

namespace rtc {

class SignalingProcessThread {

public:

  static SignalingProcessThread* Instance();

  void Init(int threadCount);

  std::shared_ptr<Worker> GetWorker();

private:
  SignalingProcessThread() = default;

private:
  bool init_{false};
  std::unique_ptr<ThreadPool> pool_;
};

}

}

#endif //RTCGATEWAY_SIGNALING_PROCESS_THREAD_H
