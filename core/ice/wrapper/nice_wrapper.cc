//
// Created by nbaiot@126.com  on 2020/9/3.
//

#include "nice_wrapper.h"

#include <string>
#include <nice/agent.h>
#include <glog/logging.h>

#include "glib_wrapper.h"

namespace nbaiot::rtc {

/// 参考 janus
/// 参考 Android webrtc api


void cb_new_local_candidate(NiceAgent* agent,
                            NiceCandidate* candidate,
                            gpointer user_data) {

  /// 对应 SdpInfo 中的 candidateInfo toString
  gchar *cand = nice_agent_generate_local_candidate_sdp(agent, candidate);
//  std::string cand_str(cand);
//  nice->OnCandidate(cand_str);
  LOG(INFO) << "cb_new_local_candidate, type:" << candidate->type
            << ", stream_id:" << candidate->stream_id
            << ", component_id:" << candidate->component_id
            << ", foundation:" << candidate->foundation
            << ", priority:" << candidate->priority
            << ", transport:" << candidate->transport
            << ", cand:" << cand;
}

static void cb_new_remote_candidate(NiceAgent* agent,
                                    NiceCandidate *candidate,
                                    gpointer user_data) {
  LOG(INFO) << ">>>>>>>>>>>>>>>>cb_new_remote_candidate";
}

static void cb_candidate_gathering_done(NiceAgent* agent,
                                        guint stream_id,
                                        gpointer user_data) {
  LOG(INFO) << ">>>>>>>>>>>>>>>>cb_candidate_gathering_done:" << stream_id;
}

static void cb_new_selected_pair(NiceAgent* agent,
                                 guint stream_id,
                                 guint component_id,
                                 NiceCandidate* lcandidate,
                                 NiceCandidate* rcandidate,
                                 gpointer user_data) {

  LOG(INFO) << ">>>>>>>>>>>>>>>>cb_new_selected_pair";
}

static void cb_component_state_changed(NiceAgent* agent,
                                       guint _stream_id,
                                       guint component_id,
                                       guint state,
                                       gpointer user_data) {
  LOG(INFO) << ">>>>>>>>>>>>>>>>cb_component_state_changed";
}

static void cb_nice_recv(NiceAgent* agent,
                         guint stream_id,
                         guint component_id,
                         guint len,
                         gchar* buf,
                         gpointer user_data) {
  LOG(INFO) << ">>>>>>>>>>>>>>>>>>>>>cb_nice_recv len:" << len;
}

NiceWrapper::NiceWrapper(bool controlMode) : agent_(nice_agent_new(GlibWrapper::Instance()->MainContext(), NICE_COMPATIBILITY_RFC5245),
                                    [](void* agent) {
                                        if (agent) {
                                          g_object_unref(agent);
                                        }
                                    }) {

  /// 当本地发现新的候选地址
  g_signal_connect(agent_.get(), "new-candidate-full",
                   G_CALLBACK(cb_new_local_candidate), this);

  /// 当发现新的远端候选地址
  g_signal_connect(agent_.get(), "new-remote-candidate-full",
                   G_CALLBACK(cb_new_remote_candidate), this);

  /// 端候选地址全部收集完成时回调执行
  g_signal_connect(agent_.get(), "candidate-gathering-done",
                   G_CALLBACK(cb_candidate_gathering_done), this);

  /// 本端在接收到对端的连接候选地址并尝试连接后，发现一个新的可通信的连接地址对时
  /// 一个可选地址对由本端一个候选地址和对端的一个候选地址组成
  g_signal_connect(agent_.get(), "new-selected-pair-full",
                   G_CALLBACK(cb_new_selected_pair), this);

  /// 连接状态发生改变时
  g_signal_connect(agent_.get(), "component-state-changed",
                   G_CALLBACK(cb_component_state_changed), this);

  /// 禁用 upnp
  g_object_set(agent_.get(), "upnp", FALSE, NULL);
  /// controlling or controlled
  g_object_set(agent_.get(), "controlling-mode", controlMode, NULL);

}

NiceWrapper::NiceWrapper() : NiceWrapper(true) {

}

NiceWrapper::~NiceWrapper() {

}

void NiceWrapper::Test() {
  /// https://github.com/libnice/libnice/blob/master/examples/sdp-example.c
  /// Create a new stream with one component
  /// 添加流的时候，需要指定这条流的组件数componetnum，
  /// 组件也就是这条流要使用的udp端口的个数，
  /// 以视频流为例，如果rtp和rtcp的端口是分开的，那么创建的组件数就是2，rtp的组件id是1，rtcp的组件id是2（参考rtc5245）
  /// 如果rtp和rtcp mux，那么只需要创建一个组件就可以了，组件id就是1
  /// 视频和音频group bundle，并且rtcp-mux时，只需要一个stream，一个组件就可以了
  /// nice_agent_add_stream的返回值是从1开始的
  auto stream_id = nice_agent_add_stream(agent_.get(), 1);
  LOG(INFO) << ">>>>>>>stream id:" << stream_id;
  nice_agent_set_stream_name(agent_.get(), stream_id, "video");

  /// Attach to the component to receive the data
  /// Without this call, candidates cannot be gathered
  /// attach之后才会把流和组件对象和glib的网络层绑定起来，attach的回调函数用来回调收到的网络数据
  /// 当组件的状态变为ready的时候，ice的连接状态完成
  /// 接着客户端主动发起dtls连接，dtls的数据包和码流包一样都是通过ice建立的udp通道来完成的。
  /// 要区分数据包是dtls包，还是rtp，rtcp的包，区分完成之后，将不同的数据包分流到对应的业务处理中
  nice_agent_attach_recv(agent_.get(), stream_id, 1,
                         GlibWrapper::Instance()->MainContext(), cb_nice_recv, this);

  /// Start gathering local candidates
  /// 开启收集流的候选项，收集完成之后，会通过 candidate-gathering-done 返回
  /// 根据回调中的流id，获取ice协商时的用户名和密码；
  /// 从NiceCandidate中可以得到候选项的foundation，优先级，ip地址，端口，候选项类别等。
  if (!nice_agent_gather_candidates(agent_.get(), stream_id)) {
    LOG(ERROR) << "Failed to start candidate gathering";
  }

  /// 在ice的连通性检查时，需要指明这次连通性测试的用户名和密码
  /// ice-ufrag, ice-pwd
  std::stringstream nice_sdp;
  std::stringstream result;
  std::string line;
  gchar* raw_sdp = nice_agent_generate_local_sdp(agent_.get());
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
  LOG(INFO) << ">>>>>>>>>>>>>.result:\n" << result.str();
}

}