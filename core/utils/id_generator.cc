//
// Created by nbaiot@126.com  on 2020/8/31.
//

#include "id_generator.h"

#include <random>

namespace nbaiot::util {

uint32_t RandomUint32() {
  std::random_device rd;
  return std::default_random_engine(rd())();
}

uint64_t RandomUint64() {
  std::random_device rd;
  auto h = std::default_random_engine(rd())() & 0x1FFFFF;
  auto l = std::default_random_engine(rd())();
  return (h << 32) | l;
}

std::string RandomString(int len) {
  std::string str;
  std::random_device rd;
  std::default_random_engine random(rd());
  for (int i = 0; i < len; ++i) {
    switch (random() % 3) {
      case 1:
        str += 'A' + random() % 26;
        break;
      case 2:
        str += 'a' + random() % 26;
        break;
      default:
        str += '0' + random() % 10;
        break;
    }
  }
  return str;
}

}