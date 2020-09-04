//
// Created by nbaiot@126.com on 2020/9/4.
//

#include "sdp_info.h"

#include <sstream>
#include <algorithm>
#include <glog/logging.h>

namespace nbaiot::rtc {

std::string CandidateInfo::TypeName() const{
  switch (candidateType) {
    case kHost: return "host";
    case kSrflx: return "srflx";
    case kPrflx: return "prflx";
    case kRelay: return "relay";
    default: return "host";
  }
}

std::string CandidateInfo::ToString() const {
  std::ostringstream sdpStream;
  sdpStream << "a=candidate:" << foundation << " " << componentId << " ";
  sdpStream << netProtocol << " " << priority << " " << hostAddress << " ";
  sdpStream << hostPort << " typ " << TypeName();
  if (!remoteAddress.empty()) {
    sdpStream << " raddr " << remoteAddress << " rport " << remotePort;
  }
  return sdpStream.str();
}

SdpInfo::SdpInfo(std::vector<RtpMap> rtp_mappings) : internalPayloadVector_(std::move(rtp_mappings)){
  isBundle = false;
  isRtcpMux = false;
  isFingerprint = false;
  dtlsRole = kACTPASS;
  internal_dtls_role = kACTPASS;
  hasAudio = false;
  hasVideo = false;
  profile = kSAVPF;
  videoCodecs = 0;
  audioCodecs = 0;
  videoSdpMLine = -1;
  audioSdpMLine = -1;
  videoBandwidth = 0;
  google_conference_flag_set = "";
  videoDirection = audioDirection = kSendRecv;
}

std::string SdpInfo::AddCandidate(const CandidateInfo& info) {
  candidateVector_.push_back(info);
  return info.ToString();
}

void SdpInfo::AddCrypto(const CryptoInfo& info) {
  cryptoVector_.push_back(info);
}

std::vector<CandidateInfo>& SdpInfo::CandidateInfos() {
  return candidateVector_;
}

std::vector<CryptoInfo>& SdpInfo::CryptoInfos() {
  return cryptoVector_;
}

std::vector<RtpMap>& SdpInfo::PayloadInfos() {
  return payloadVector;
}

std::vector<RtpExtMap> SdpInfo::ExtensionMap(MediaType media) {
  std::vector<RtpExtMap> valid_extensions;
  for (const RtpExtMap& extension : extMapVector) {
    if (IsValidExtension(extension.uri) && extension.mediaType == media) {
      valid_extensions.push_back(extension);
    }
  }
  return valid_extensions;
}

bool SdpInfo::IsValidExtension(const std::string& uri) {
  auto value = std::find_if(supported_ext_map_.begin(), supported_ext_map_.end(), [uri](const RtpExtMap &extension) {
      return extension.uri == uri;
  });
  return value != supported_ext_map_.end();
}

uint32_t SdpInfo::AudioInternalPT(uint32_t externalPT) {
  /// should use separate mapping for video and audio at the very least
  /// standard requires separate mappings for each media, even!
  auto found = outInPTMap.find(externalPT);
  if (found != outInPTMap.end()) {
    return found->second;
  }
  return externalPT;
}

uint32_t SdpInfo::VideoInternalPT(uint32_t externalPT) {
  /// WARNING
  /// should use separate mapping for video and audio at the very least
  /// standard requires separate mappings for each media, even!
  return AudioInternalPT(externalPT);
}

uint32_t SdpInfo::AudioExternalPT(uint32_t internalPT) {
  /// should use separate mapping for video and audio at the very least
  /// standard requires separate mappings for each media, even!
  auto found = inOutPTMap.find(internalPT);
  if (found != inOutPTMap.end()) {
    return found->second;
  }
  return internalPT;
}

uint32_t SdpInfo::VideoExternalPT(uint32_t internalPT) {
  /// WARNING
  /// should use separate mapping for video and audio at the very least
  /// standard requires separate mappings for each media, even!
  return AudioExternalPT(internalPT);
}

RtpMap* SdpInfo::CodecByExternalPayloadType(const uint32_t payloadType) {
  for (auto & rtp : payloadVector) {
    if (rtp.payloadType == payloadType) {
      return &rtp;
    }
  }
  return nullptr;
}

void SdpInfo::SetCredentials(const std::string& username, const std::string& password, MediaType media) {
  switch (media) {
    case(kVideo):
      iceVideoUsername_ = std::string(username);
      iceVideoPassword_ = std::string(password);
      break;
    case(kAudio):
      iceAudioUsername_ = std::string(username);
      iceAudioPassword_ = std::string(password);
      break;
    default:
      iceVideoUsername_ = std::string(username);
      iceVideoPassword_ = std::string(password);
      iceAudioUsername_ = std::string(username);
      iceAudioPassword_ = std::string(password);
      break;
  }
}

std::string SdpInfo::Username(MediaType media) const {
  if (media == kAudio) {
    return iceAudioUsername_;
  }
  return iceVideoUsername_;
}

std::string SdpInfo::Password(MediaType media) const {
  if (media == kAudio) {
    return iceAudioPassword_;
  }
  return iceVideoPassword_;
}

RtpMap* SdpInfo::GetCodecByName(const std::string& codecName, uint32_t clockRate) {
  for (auto & rtp : internalPayloadVector_) {
    if (rtp.encodingName == codecName && rtp.clockRate == clockRate) {
      return &rtp;
    }
  }
  return nullptr;
}

bool SdpInfo::SupportCodecByName(const std::string& codecName, uint32_t clockRate) {
  RtpMap *rtp = GetCodecByName(codecName, clockRate);
  if (rtp) {
    return SupportPayloadType(rtp->payloadType);
  }
  return false;
}

bool SdpInfo::SupportPayloadType(uint32_t payloadType) {
  if (inOutPTMap.count(payloadType) > 0) {
    for (const auto & rtp : payloadVector) {
      if (inOutPTMap[rtp.payloadType] == payloadType) {
        return true;
      }
    }
  }
  return false;
}

void SdpInfo::CreateOfferSdp(bool videoEnabled, bool audioEnabled, bool bundle) {
  LOG(INFO) << "Creating offerSDP: video " << videoEnabled
            << ", audio " << audioEnabled
            << ", bundle " << bundle
            << ", payloadVector: " << payloadVector.size()
            << ", extSize: " << extMapVector.size();

  if (payloadVector.empty()) {
    payloadVector = internalPayloadVector_;
  }

  this->isBundle = bundle;
  this->profile = kSAVPF;
  this->isRtcpMux = true;
  if (videoEnabled)
    this->videoSdpMLine = 0;
  if (audioEnabled)
    this->audioSdpMLine = 0;

  for (auto & rtp : payloadVector) {
    if (rtp.mediaType == kVideo) {
      videoCodecs++;
    } else if (rtp.mediaType == kAudio) {
      audioCodecs++;
    }
  }

  this->hasVideo = videoEnabled;
  this->hasAudio = audioEnabled;
  this->videoDirection = kSendRecv;
  this->audioDirection = kSendRecv;
}

void SdpInfo::CopyInfoFromSdp(const std::shared_ptr<SdpInfo>& offerSdp) {
  payloadVector = offerSdp->payloadVector;
  videoCodecs = offerSdp->videoCodecs;
  audioCodecs = offerSdp->audioCodecs;
  inOutPTMap = offerSdp->inOutPTMap;
  outInPTMap = offerSdp->outInPTMap;
  extMapVector = offerSdp->extMapVector;
  LOG(INFO) << "Offer SDP successfully copied, extSize: " << extMapVector.size()
            << ", payloadSize: " << payloadVector.size()
            << ", videoCodecs: " << videoCodecs
            << ", audioCodecs: " << audioCodecs;
}

void SdpInfo::SetOfferSdp(std::shared_ptr<SdpInfo> offerSdp) {
  this->videoCodecs = offerSdp->videoCodecs;
  this->audioCodecs = offerSdp->audioCodecs;
  this->payloadVector = offerSdp->payloadVector;
  this->isBundle = offerSdp->isBundle;
  this->profile = offerSdp->profile;
  this->isRtcpMux = offerSdp->isRtcpMux;
  this->videoSdpMLine = offerSdp->videoSdpMLine;
  this->audioSdpMLine = offerSdp->audioSdpMLine;
  this->inOutPTMap = offerSdp->inOutPTMap;
  this->outInPTMap = offerSdp->outInPTMap;
  this->hasVideo = offerSdp->hasVideo;
  this->hasAudio = offerSdp->hasAudio;
  this->bundleTags = offerSdp->bundleTags;
  this->extMapVector = offerSdp->extMapVector;
  this->google_conference_flag_set = offerSdp->google_conference_flag_set;

  switch (offerSdp->videoDirection) {
    case kSendOnly:
      this->videoDirection = kRecvOnly;
      break;
    case kRecvOnly:
      this->videoDirection = kSendOnly;
      break;
    case kSendRecv:
      this->videoDirection = kSendRecv;
      break;
    case kInactive:
      this->videoDirection = kInactive;
      break;
    default:
      this->videoDirection = kSendRecv;
      break;
  }
  switch (offerSdp->audioDirection) {
    case kSendOnly:
      this->audioDirection = kRecvOnly;
      break;
    case kRecvOnly:
      this->audioDirection = kSendOnly;
      break;
    case kSendRecv:
      this->audioDirection = kSendRecv;
      break;
    case kInactive:
      this->audioDirection = kInactive;
      break;
    default:
      this->audioDirection = kSendRecv;
      break;
  }
}

void SdpInfo::UpdateSupportedExtensionMap(const std::vector<RtpExtMap>& ext_map) {
  supported_ext_map_ = ext_map;
}

bool SdpInfo::PostProcessInfo() {
  /// If there is no video or audio credentials we use the ones we have
  if (iceVideoUsername_.empty() && iceAudioUsername_.empty()) {
    LOG(ERROR) << "No valid credentials for ICE";
    return false;
  } else if (iceVideoUsername_.empty()) {
    LOG(INFO) << "Video credentials empty, setting the audio ones";
    iceVideoUsername_ = iceAudioUsername_;
    iceVideoPassword_ = iceAudioPassword_;
  } else if (iceAudioUsername_.empty()) {
    LOG(INFO) << "Audio credentials empty, setting the video ones";
    iceAudioUsername_ = iceVideoUsername_;
    iceAudioPassword_ = iceVideoPassword_;
  }

  for (auto & c : candidateVector_) {
    c.isBundle = isBundle;
    if (c.mediaType == kVideo) {
      c.username = iceVideoUsername_;
      c.password = iceVideoPassword_;
    } else {
      c.username = iceAudioUsername_;
      c.password = iceAudioPassword_;
    }
  }

  ///  go through the payload_map_ and match it with internalPayloadVector_
  ///  generate rtpMaps and payloadVector
  std::vector<RtpMap> rtx_maps;
  for (const RtpMap& internal_map : internalPayloadVector_) {
    for (const std::pair<const unsigned int, RtpMap>& parsed_pair : payload_parsed_map_) {
      const RtpMap& parsed_map = parsed_pair.second;
      if (internal_map.encodingName != parsed_map.encodingName ||
          internal_map.clockRate != parsed_map.clockRate) {
        continue;
      }
      if (parsed_map.encodingName == "rtx") {  /// we'll process those later when we have the pt maps
        rtx_maps.push_back(parsed_map);
        continue;
      }
      RtpMap negotiated_map(parsed_map);
      negotiated_map.channels = internal_map.channels;

      LOG(INFO) << "message: Checking feedback types, parsed:" << parsed_map.feedbackTypes.size()
                << ", internal:" << internal_map.feedbackTypes.size();

      negotiated_map.feedbackTypes = negotiateFeedback(parsed_map, internal_map);
      negotiated_map.formatParameters = maybeCopyFormatParameters(parsed_map, internal_map);

      if (internal_map.formatParameters.empty() ||
          parsed_map.formatParameters.size() == negotiated_map.formatParameters.size()) {
        if (negotiated_map.mediaType == kVideo) {
          videoCodecs++;
        } else {
          audioCodecs++;
        }

        LOG(INFO) << "Mapping " << parsed_map.encodingName << "/" << parsed_map.clockRate << ":" << parsed_map.payloadType
                  << "to " <<  internal_map.encodingName << "/" << internal_map.clockRate << "/" << internal_map.payloadType;
        payloadVector.push_back(negotiated_map);
        outInPTMap[parsed_map.payloadType] = internal_map.payloadType;
        inOutPTMap[internal_map.payloadType] = parsed_map.payloadType;
      }
    }
  }

  //  Check atp rtx
  for (RtpMap& rtx_map : rtx_maps) {
    for (const RtpMap& internal_map : internalPayloadVector_) {
      if (internal_map.encoding_name == "rtx") {
        auto parsed_apt = rtx_map.format_parameters.find(kAssociatedPt);
        auto internal_apt = internal_map.format_parameters.find(kAssociatedPt);
        if (parsed_apt == rtx_map.format_parameters.end() &&
            internal_apt == internal_map.format_parameters.end()) {
          continue;
        }
        unsigned int internal_apt_pt = std::stoul(internal_apt->second);
        unsigned int parsed_apt_pt = std::stoul(parsed_apt->second);
        ELOG_DEBUG("message: looking for apt correspondence, internal_apt_pt: %u, parsed_apt_pt: %u",
                   internal_apt_pt, parsed_apt_pt);
        if (outInPTMap[parsed_apt_pt] == internal_apt_pt) {
          ELOG_DEBUG("message: matched atp for rtx, internal_apt_pt: %u, parsed_apt_pt: %u",
                     internal_apt_pt, parsed_apt_pt);
          if (rtx_map.media_type == VIDEO_TYPE) {
            videoCodecs++;
          } else {
            audioCodecs++;
          }
          outInPTMap[rtx_map.payload_type] = internal_map.payload_type;
          inOutPTMap[internal_map.payload_type] = rtx_map.payload_type;
          payloadVector.push_back(rtx_map);
        }
      }
    }
  }
  return true;
}

std::vector<std::string> SdpInfo::NegotiateFeedback(const RtpMap& parsed_map, const RtpMap& internal_map) {
  return std::vector<std::string>();
}

std::map<std::string, std::string> SdpInfo::MaybeCopyFormatParameters(const RtpMap& parsed_map) {
  return std::map<std::string, std::string>();
}

}