//
// Created by nbaiot@126.com on 2020/9/4.
//

#ifndef RTCGATEWAY_SDP_INFO_H
#define RTCGATEWAY_SDP_INFO_H

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <cstdint>

namespace nbaiot::rtc {

enum CandidateType {
  kHost,  /**< 本机地址 */
  kSrflx, /**< stun 获取的外部地址*/
  kPrflx, /**< */
  kRelay, /**< turn 分配的中继地址*/
};

enum MediaType {
  kVideo, /**< */
  kAudio, /**< */
  kOther, /**< */
};

enum StreamDirection {
  kSendRecv,  /**< */
  kSendOnly,  /**< */
  kRecvOnly,  /**< */
  kInactive,  /**< */
};

enum Profile {
  kAVPF,  /**< 启用RTCP反馈，也会根据RTCP反馈动态调整码率 */
  kSAVPF, /**< 加密的 AVPF*/
};

enum DtlsRole {
  kACTPASS, /**< 当前客户端在DTLS协商过程中，既可以作为客户端，也可以作为服务端 */
  kACTIVE,
  kPASSIVE,
};

struct CryptoInfo {
  int tag{0};
  /**
   * The cipher suite. Only AES_CM_128_HMAC_SHA1_80 is supported as of now.
   */
  std::string cipherSuite;

  std::string keyParams;

  MediaType mediaType{kOther};
};


struct CandidateInfo {

  std::string TypeName() const;

  std::string ToString() const;

  bool isBundle{false};
  int tag{0};
  uint32_t priority;
  uint32_t componentId;
  std::string foundation;
  std::string hostAddress;
  uint16_t hostPort{0};
  std::string remoteAddress;
  uint16_t remotePort{0};
  std::string netProtocol;
  CandidateType candidateType;
  std::string transProtocol;
  std::string username;
  std::string password;
  MediaType mediaType;
  std::string sdp;
};

struct BundleTag {
  BundleTag(std::string id, MediaType type) : id(std::move(id)), mediaType(type) {}

  std::string id;
  MediaType mediaType{kOther};
};

/**
 * Codec map
 */
struct RtpMap {
  uint32_t payloadType{0};
  std::string encodingName;
  uint32_t clockRate;
  MediaType mediaType;
  uint32_t channels;
  std::vector<std::string> feedbackTypes;
  std::map<std::string, std::string> formatParameters;
};

/**
 * A RTP extmap description
 */

struct RtpExtMap {
  RtpExtMap(uint32_t value, std::string uri) : value(value), uri(std::move(uri)) {
  }

  uint32_t value{0};
  std::string uri;
  StreamDirection direction{kSendRecv};
  std::string parameters;
  MediaType mediaType{kOther};
};

struct SdpInfo {

  explicit SdpInfo(std::vector<RtpMap> rtp_mappings);

  std::string AddCandidate(const CandidateInfo& info);

  void AddCrypto(const CryptoInfo& info);

  std::vector<CandidateInfo>& CandidateInfos();

  std::vector<CryptoInfo>& CryptoInfos();

  std::vector<RtpMap>& PayloadInfos();

  std::vector<RtpExtMap> ExtensionMap(MediaType media);

  /**
  * @brief map external payload type to an internal id
  * @param externalPT The audio payload type as coming from this source
  * @return The internal payload id
  */
  uint32_t AudioInternalPT(uint32_t externalPT);

  /**
   * @brief map external payload type to an internal id
   * @param externalPT The video payload type as coming from this source
   * @return The internal payload id
   */
  uint32_t VideoInternalPT(uint32_t externalPT);

  /**
   * @brief map internal payload id to an external payload type
   * @param internalPT The payload type id used internally
   * @return The external payload type as provided to this source
   */
  uint32_t AudioExternalPT(uint32_t internalPT);
  /**
   * @brief map internal payload it to an external payload type
   * @param internalPT The payload id as used internally
   * @return The external video payload type
   */
  uint32_t VideoExternalPT(uint32_t internalPT);

  RtpMap* CodecByExternalPayloadType(uint32_t payloadType);

  void SetCredentials(const std::string& username, const std::string& password, MediaType media);

  std::string Username(MediaType media) const;

  std::string Password(MediaType media) const;

  RtpMap* GetCodecByName(const std::string& codecName, uint32_t clockRate);

  bool SupportCodecByName(const std::string& codecName, uint32_t clockRate);

  bool SupportPayloadType(uint32_t payloadType);

  void CreateOfferSdp(bool videoEnabled, bool audioEnabled, bool bundle);

  void CopyInfoFromSdp(const std::shared_ptr<SdpInfo>& offerSdp);

  /**
   * @brief copies relevant information from the offer sdp for which this will be an answer sdp
   * @param offerSdp The offer SDP as received via signaling and parsed
   */
  void SetOfferSdp(std::shared_ptr<SdpInfo> offerSdp);

  void UpdateSupportedExtensionMap(const std::vector<RtpExtMap>& ext_map);

  bool IsValidExtension(const std::string& uri);

  bool PostProcessInfo();

  /**
  * The audio and video SSRCs for this particular SDP.
  */
  std::map<std::string, uint32_t> audio_ssrc_map;
  std::map<std::string, std::vector<uint32_t>> video_ssrc_map;
  std::map<std::string, std::map<uint32_t, uint32_t>> video_rtx_ssrc_map;

  /**
 * Is it Bundle
 */
  bool isBundle;
  /**
  * Has audio
  */
  bool hasAudio;
  /**
  * Has video
  */
  bool hasVideo;

  /**
  * Is there rtcp muxing
  */
  bool isRtcpMux;

  StreamDirection videoDirection, audioDirection;

  /**
  * RTP Profile type
  */
  Profile profile;

  /**
 * Is there DTLS fingerprint
 */
  bool isFingerprint;

  /**
   * DTLS Fingerprint
   */
  std::string fingerprint;

  /**
  * DTLS Role
  */
  DtlsRole dtlsRole;

  /**
   * Internal DTLS Role
   */
  DtlsRole internal_dtls_role;

  /**
  * Mapping from internal PT (key) to external PT (value)
  */
  std::map<uint32_t, uint32_t> inOutPTMap;

  /**
   * Mapping from external PT (key) to intermal PT (value)
   */
  std::map<uint32_t, uint32_t> outInPTMap;

  /**
   * The negotiated payload list
   */
  std::vector<RtpMap> payloadVector;

  std::vector<BundleTag> bundleTags;
  std::vector<RtpExtMap> extMapVector;

  /*
  * MLines for video and audio
  */
  int videoSdpMLine;
  int audioSdpMLine;
  int videoCodecs, audioCodecs;
  unsigned int videoBandwidth;
  std::vector<CandidateInfo> candidateVector_;
  std::vector<CryptoInfo> cryptoVector_;
  std::vector<RtpMap> internalPayloadVector_;
  std::string iceVideoUsername_, iceAudioUsername_;
  std::string iceVideoPassword_, iceAudioPassword_;
  std::map<unsigned int, RtpMap> payload_parsed_map_;
  std::vector<RtpExtMap> supported_ext_map_;
  std::string google_conference_flag_set;


private:
  std::vector<std::string> NegotiateFeedback(const RtpMap& parsed_map,
                                             const RtpMap& internal_map);
  std::map<std::string, std::string> MaybeCopyFormatParameters(const RtpMap& parsed_map,
};

}

#endif //RTCGATEWAY_SDP_INFO_H
