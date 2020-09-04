//
// Created by nbaiot@126.com on 2020/9/3.
//

#include "glib_wrapper.h"

#include <glib-2.0/glib.h>

namespace nbaiot::rtc {

GlibWrapper::GlibWrapper() : main_loop_started_(false), main_loop_(g_main_loop_new(nullptr, false),
                                                                   [](GMainLoop* loop) {
                                                                     if (loop) {
                                                                       g_main_loop_unref(loop);
                                                                     }
                                                                   }) {
}

GlibWrapper* GlibWrapper::Instance() {
  static GlibWrapper INSTANCE;
  return &INSTANCE;
}


void GlibWrapper::StartMainLoop() {
  if (main_loop_started_)
    return;
  loop_thread_ = std::make_unique<std::thread>(g_main_loop_run, main_loop_.get());
  main_loop_started_ = true;
}

void GlibWrapper::StopMainLoop() {
  if (!main_loop_started_)
    return;
  main_loop_started_ = false;
  g_main_loop_quit(main_loop_.get());
  if (loop_thread_ && loop_thread_->joinable()) {
    loop_thread_->join();
  }
}

GMainContext* GlibWrapper::MainContext() {
  if (main_loop_started_)
    return g_main_loop_get_context(main_loop_.get());
  return nullptr;
}


}