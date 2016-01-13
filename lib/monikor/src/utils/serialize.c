#include <stdint.h>
#include <arpa/inet.h>

#ifndef htonll
static inline int is_big_endian(void) {
  uint16_t endianess = 1;

  if (*((uint8_t *)(&endianess)))
    return 0;
  else
    return 1;
}

uint64_t htonll(uint64_t hostlong) {
  if (is_big_endian())
    return hostlong;
  else
    return ((uint64_t)htonl(hostlong & 0xFFFFFFFF) << 32 | htonl(hostlong >> 32));
}

uint64_t ntohll(uint64_t netlong) {
  if (is_big_endian())
    return netlong;
  else
    return ((uint64_t)ntohl(netlong & 0xFFFFFFFF) << 32 | ntohl(netlong >> 32));
}
#endif

float ntohf(uint32_t netfloat) {
  union {
    float _float;
    uint32_t _int;
  } val;

  val._int = ntohl(netfloat);
  return val._float;
}

uint32_t htonf(float hostfloat) {
  union {
    float _float;
    uint32_t _int;
  } val;

  val._float = hostfloat;
  return htonl(val._int);
}
