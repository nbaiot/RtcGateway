#include <iostream>

#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <glog/logging.h>

#include "core/include/gateway_interface.h"
#include "core/include/gateway_factory.h"


using namespace nbaiot::rtc;
int main() {
  std::cout << "Hello, World!" << std::endl;

  GatewayFactory factory;
  auto gateway = factory.CreateRtcGateway();

  YAML::Node node;
  gateway->Init(node);
  gateway->Start();

  std::mutex mutex;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lk(mutex);
  cv.wait(lk);
  return 0;
}