//
// Created by nbaiot@126.com on 2020/9/1.
//

#include "signaling_process_thread.h"

#include "thread/thread_pool.h"

namespace nbaiot::rtc {

SignalingProcessThread* SignalingProcessThread::Instance() {
  static SignalingProcessThread INSTANCE;
  return &INSTANCE;
}

void SignalingProcessThread::Init(int threadCount) {
  if (init_)
    return;
  pool_ = std::make_unique<ThreadPool>(threadCount);
  pool_->Start();
  init_ = true;
}

std::shared_ptr<Worker> SignalingProcessThread::GetWorker() {
  return pool_->GetLessUsedWorker();
}

}