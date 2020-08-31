//
// Created by nbaiot@126.com  on 2020/8/31.
//

#ifndef RTCGATEWAY_ID_GENERATOR_H
#define RTCGATEWAY_ID_GENERATOR_H

#include <string>
#include <cstdint>

namespace nbaiot::util {

uint32_t RandomUint32();

uint64_t RandomUint64();

std::string RandomString(int len = 12);

}


#endif //RTCGATEWAY_ID_GENERATOR_H
