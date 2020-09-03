//
// Created by nbaiot@126.com  on 2020/9/3.
//

#include "nice_wrapper.h"

#include <nice/agent.h>
#include <glog/logging.h>

#include "glib_wrapper.h"

namespace nbaiot::rtc {

/// 参考 janus
/// 参考 Android webrtc api


void cb_new_local_candidate(NiceAgent *agent, guint stream_id, guint component_id, gchar *foundation, gpointer user_data) {

//  gchar *cand = nice_agent_generate_local_candidate_sdp(agent, candidate);
//  std::string cand_str(cand);
//  nice->OnCandidate(cand_str);
//  g_free(cand);
  LOG(INFO) << ">>>>>>>>>>>>>>>>cb_new_local_candidate";
}

static void cb_new_remote_candidate(NiceAgent* agent, guint stream_id, guint component_id,
                                              gchar* foundation, gpointer user_data) {
  LOG(INFO) << ">>>>>>>>>>>>>>>>cb_new_remote_candidate";
}

static void cb_candidate_gathering_done(NiceAgent* agent, guint stream_id, gpointer user_data) {
  LOG(INFO) << ">>>>>>>>>>>>>>>>cb_candidate_gathering_done";
}

static void cb_new_selected_pair(NiceAgent* agent, guint _stream_id,
                                 guint component_id, gchar* lfoundation,
                                 gchar* rfoundation, gpointer data) {

  LOG(INFO) << ">>>>>>>>>>>>>>>>cb_new_selected_pair";
}

static void cb_component_state_changed(NiceAgent* agent, guint _stream_id,
                                       guint component_id, guint state,
                                       gpointer data) {
  LOG(INFO) << ">>>>>>>>>>>>>>>>cb_component_state_changed";
}

static void cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
                         guint len, gchar *buf, gpointer data) {
  LOG(INFO) << ">>>>>>>>>>>>>>>>>>>>>cb_nice_recv len:" << len;
}

NiceWrapper::NiceWrapper() : agent_(nice_agent_new(GlibWrapper::Instance()->MainContext(), NICE_COMPATIBILITY_RFC5245),
                                    [](void* agent) {
                                      if (agent) {
                                        g_object_unref(agent);
                                      }
                                    }) {

  g_signal_connect(agent_.get(), "new-candidate",
                   G_CALLBACK(cb_new_local_candidate), this);

  g_signal_connect(agent_.get(), "component-state-changed",
                   G_CALLBACK(cb_new_remote_candidate), this);

  /// 端候选地址全部收集完成时回调执行
  g_signal_connect(agent_.get(), "candidate-gathering-done",
                   G_CALLBACK(cb_candidate_gathering_done), this);

  /// 本端在接收到对端的连接候选地址，在尝试连接后，
  /// 发现一个新的可通信的连接地址对时，调用方法cb_new_selected_pair
  /// 一个可选地址对由本端一个候选地址和对端的一个候选地址组成
  g_signal_connect(agent_.get(), "new-selected-pair",
                   G_CALLBACK(cb_new_selected_pair), this);

  /// 当通道的连接状态发生变化时回调方法
  /// S一个agent可包含多个component，可视为多路复用一个链路
  g_signal_connect(agent_.get(), "new-remote-candidate",
                   G_CALLBACK(cb_component_state_changed), this);

  g_object_set(G_OBJECT(agent_.get()), "upnp", FALSE, NULL);
  /// 按照ice的规范，主动发起offer的一端，应设置为控制一端
  g_object_set(G_OBJECT(agent_.get()), "controlling-mode", 0, NULL);

}

NiceWrapper::NiceWrapper(bool controlMode) : NiceWrapper() {

}

NiceWrapper::~NiceWrapper() {

}

void NiceWrapper::Test() {
  /// https://github.com/libnice/libnice/blob/master/examples/sdp-example.c
  /// Create a new stream with one component
  auto stream_id = nice_agent_add_stream(agent_.get(), 1);
  LOG(INFO) << ">>>>>>>stream id:" << stream_id;
  nice_agent_set_stream_name (agent_.get(), stream_id, "video");

  /// Attach to the component to receive the data
  /// Without this call, candidates cannot be gathered
  nice_agent_attach_recv(agent_.get(), stream_id, 1,
                         GlibWrapper::Instance()->MainContext(), cb_nice_recv, this);

  // Start gathering local candidates
  if (!nice_agent_gather_candidates(agent_.get(), stream_id)) {
    LOG(ERROR) << "Failed to start candidate gathering";
  }

  std::stringstream nice_sdp;
  std::stringstream result;
  std::string line;
  gchar *raw_sdp = nice_agent_generate_local_sdp(agent_.get());
  LOG(ERROR) << ">>>>>>>>>>>>>>>>>>local sdp:\n" << std::string(raw_sdp);
  nice_sdp << raw_sdp;
  if (raw_sdp) {
    while (std::getline(nice_sdp, line)) {
      if (g_str_has_prefix(line.c_str(), "a=ice-ufrag:") || g_str_has_prefix(line.c_str(), "a=ice-pwd:")) {
        result << line << "\r\n";
      }
    }
    g_free(raw_sdp);
  } else {
    LOG(ERROR) << ">>>>>>>>>>> error";
  }
  LOG(INFO) << ">>>>>>>>>>>>>.result:\n" <<  result.str();
}

}