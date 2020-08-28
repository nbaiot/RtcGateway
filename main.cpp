#include <iostream>

#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <glog/logging.h>

#include "core/thread/thread_pool.h"
#include "core/thread/worker.h"

using namespace nbaiot;
int main() {
  std::cout << "Hello, World!" << std::endl;

  auto pool = std::make_shared<ThreadPool>(5);

  pool->Start();

  auto worker = pool->GetLessUsedWorker();

  LOG(INFO) << ">>>>>>>>>>>>> start...::::" << std::this_thread::get_id();
  worker->Start();
  LOG(INFO) << ">>>>>>>>>>>>> worker thread id:" << worker->ThreadId();

  worker->PostTask([](){
    LOG(INFO) << ">>>>>>>>>>>>>>>>>>>>>la la la...:" << std::this_thread::get_id();
  });

  worker->ScheduleFromNow([](){
    LOG(INFO) << ">>>>>>> timer task....";
  }, std::chrono::duration<int>(3));


  worker->ScheduleEvery([](){
      LOG(INFO) << ">>>>>>> period task....";
      std::this_thread::sleep_for(std::chrono::seconds(1));
      return true;
  }, std::chrono::duration<int>(3));

  std::this_thread::sleep_for(std::chrono::seconds(9));
  pool->Stop();
  std::mutex mutex;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lk(mutex);
  cv.wait(lk);
  return 0;
}