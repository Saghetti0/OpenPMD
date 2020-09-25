/* The OpenPMD project
 * net_datatypes_read.c: functions for reading and interpreting data from a socket
 */

#include "net_datatypes.h"

#ifdef __unix__
#include <sys/socket.h>
#include <sys/types.h>
#elif defined(_WIN32) || defined(WIN32)
#include <winsock2.h>
#endif

uint8_t net_read_uint8(int socket_fp) {
  char buf[1];
  recv(socket_fp,buf,1,0);
  return (uint8_t) buf[0];
}

int8_t net_read_int8(int socket_fp) {
  char buf[1];
  recv(socket_fp,buf,1,0);
  return (int8_t) buf[0];
}

uint16_t net_read_uint16(int socket_fp) {
  char buf[2];
  recv(socket_fp,buf,2,0);
  uint16_t output = 0;
  output |= buf[1];
  output <<= 8;
  output |= buf[0];
  return output;
}

int16_t net_read_int16(int socket_fp) {
  char buf[2];
  recv(socket_fp,buf,2,0);
  int16_t output = 0;
  output |= buf[1];
  output <<= 8;
  output |= buf[0];
  return output;
}

uint32_t net_read_uint32(int socket_fp) {
  char buf[4];
  recv(socket_fp,buf,4,0);
  uint32_t output = 0;
  output |= buf[3];
  output <<= 8;
  output |= buf[2];
  output <<= 8;
  output |= buf[1];
  output <<= 8;
  output |= buf[0];
  return output;
}

int32_t net_read_int32(int socket_fp) {
  char buf[4];
  recv(socket_fp,buf,4,0);
  int32_t output = 0;
  output |= buf[3];
  output <<= 8;
  output |= buf[2];
  output <<= 8;
  output |= buf[1];
  output <<= 8;
  output |= buf[0];
  return output;
}

uint64_t net_read_uint64(int socket_fp) {
  char buf[8];
  recv(socket_fp,buf,8,0);
  uint64_t output = 0;
  // this pains me to look at
  output |= buf[7];
  output <<= 8;
  output |= buf[6];
  output <<= 8;
  output |= buf[5];
  output <<= 8;
  output |= buf[4];
  output <<= 8;
  output |= buf[3];
  output <<= 8;
  output |= buf[2];
  output <<= 8;
  output |= buf[1];
  output <<= 8;
  output |= buf[0];
  return output;
}

int64_t net_read_int64(int socket_fp) {
  char buf[8];
  recv(socket_fp,buf,8,0);
  int64_t output = 0;
  output |= buf[7];
  output <<= 8;
  output |= buf[6];
  output <<= 8;
  output |= buf[5];
  output <<= 8;
  output |= buf[4];
  output <<= 8;
  output |= buf[3];
  output <<= 8;
  output |= buf[2];
  output <<= 8;
  output |= buf[1];
  output <<= 8;
  output |= buf[0];
  return output;
}
