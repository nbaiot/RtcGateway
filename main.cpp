#include <iostream>

#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <glog/logging.h>
#include <nice/debug.h>


#include "core/ice/wrapper/glib_wrapper.h"
#include "core/ice/wrapper/nice_wrapper.h"


#include "core/include/gateway_interface.h"
#include "core/include/gateway_factory.h"

///https://github.com/fengweiyu/webrtc_with_libnice_and_srtp/tree/master/src
///https://blog.csdn.net/chenFteng/article/details/77429453
///https://www.jianshu.com/p/ae071fc5d269


using namespace nbaiot::rtc;
int main() {
  std::cout << "Hello, World!" << std::endl;

  GlibWrapper::Instance()->StartMainLoop();


  /// 调试
  nice_debug_disable(true);

  auto niceWrapper = std::make_shared<NiceWrapper>();

  niceWrapper->Test();

//  g_object_set(G_OBJECT(agent.get()), "upnp", FALSE, NULL);
//  g_object_set(G_OBJECT(agent.get()), "controlling-mode", 0, NULL);




#if 0
  GatewayFactory factory;
  auto gateway = factory.CreateRtcGateway();

  YAML::Node node;
  gateway->Init(node);
  gateway->Start();
#endif
  std::mutex mutex;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lk(mutex);
  cv.wait(lk);
  return 0;
}