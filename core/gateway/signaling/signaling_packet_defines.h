//
// Created by nbaiot@126.com on 2020/8/31.
//

#ifndef RTCGATEWAY_SIGNALING_PACKET_DEFINES_H
#define RTCGATEWAY_SIGNALING_PACKET_DEFINES_H

namespace nbaiot::rtc {

#define MESSAGE "message"

#define REQUEST "request"

#define ACTION "action"

#define TRANSACTION "transaction"

#define BODY "body"

#define CREATE_SESSION "create_session"

#define DESTROY_SESSION "destroy_session"

#define CREATE_ROOM "create_room"

#define DESTROY_ROOM "destroy_room"

#define EXISTS_ROOM "exists_room"

#define LIST_ROOM "list_room"

#define JOIN_ROOM "join_room"

#define LEAVE_ROOM "leave_room"

#define SESSION_ID "session_id"

#define SUCCESS "success"

#define ERROR "error"

#define ERROR_CODE "error_code"

#define CODE "code"


#define SUCCESS_CODE 800
#define INVALID_SESSION_CODE 801

}

#endif //RTCGATEWAY_SIGNALING_PACKET_DEFINES_H
