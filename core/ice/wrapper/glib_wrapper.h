//
// Created by nbaiot@126.com on 2020/9/3.
//

#ifndef RTCGATEWAY_GLIB_WRAPPER_H
#define RTCGATEWAY_GLIB_WRAPPER_H

#include <memory>
#include <thread>


struct _GMainLoop;
typedef struct _GMainLoop GMainLoop;

struct _GMainContext;
typedef struct _GMainContext GMainContext;

namespace nbaiot::rtc {

class GlibWrapper {

public:

  static GlibWrapper* Instance();

  void StartMainLoop();

  void StopMainLoop();

  GMainContext* MainContext();

private:
  GlibWrapper();

private:
  bool main_loop_started_;
  std::unique_ptr<std::thread> loop_thread_;
  std::unique_ptr<GMainLoop, void (*)(GMainLoop*)> main_loop_;
};

}

#endif //RTCGATEWAY_GLIB_WRAPPER_H
