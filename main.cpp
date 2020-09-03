#include <iostream>

#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <glog/logging.h>


#include <nice/agent.h>
#include <glib-2.0/glib/gmain.h>
#include <glib-2.0/gio/gnetworking.h>


#include "core/include/gateway_interface.h"
#include "core/include/gateway_factory.h"

///https://github.com/fengweiyu/webrtc_with_libnice_and_srtp/tree/master/src



using namespace nbaiot::rtc;
int main() {
  std::cout << "Hello, World!" << std::endl;

  /// 调试
  nice_debug_disable(false);

  GMainLoop *gloop = g_main_loop_new(nullptr, false);

  if (!gloop) {
    LOG(ERROR) << ">>>>>>>>>>>>>> new loop failed";
    return 0;
  }

  LOG(INFO) << ">>>>>>>> new loop success";

  NiceAgent *agent = nice_agent_new(g_main_loop_get_context(gloop), NICE_COMPATIBILITY_RFC5245);
  if (!agent) {
    LOG(ERROR) << ">>>>>>>>>>>>>> new nice agent failed";
    return 0;
  }

  auto gThread = std::thread(g_main_loop_run, gloop);

  g_object_set(G_OBJECT(agent.get()), "upnp", FALSE, NULL);
  g_object_set(G_OBJECT(agent.get()), "controlling-mode", 0, NULL);

  gethostbyname()

//  g_main_loop_unref(gloop);


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