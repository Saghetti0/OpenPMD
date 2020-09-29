/* The OpenPMD project
 * net_datatypes_read.c: functions for reading and interpreting data from a socket
 */

#include "net_datatypes.h"
#include <stdlib.h>
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

float net_read_float(int socket_fp) {
  float output;
  // cast to char* to prevent mingw gcc from complaining
  recv(socket_fp,(char*)&output,4,0);
  return output;
}

double net_read_double(int socket_fp) {
  double output;
  recv(socket_fp,(char*)&output,4,0);
  return output;
}

uint16_t net_read_binary(int socket_fp, char** input_buffer) {
  /* We're taking a pointer to a pointer as an argument because the caller doesn't know what size the data will be
   * (and by extension, can't allocate a buffer and pass in a pointer to this function)
   * Example code for how this function should be called
   *
   * char* data;
   * uint16_t data_size = net_read_binary(socket_fp,&data);
   * // data now contains a pointer to the buffer
   *
   * In this function, we change data to point to a buffer that we malloc() (because we know what size it will be) and return that size
   * Because of this, make sure to always free() data to prevent memory leaks!
   */

   uint16_t data_size = net_read_uint16(socket_fp);
   char* buffer = malloc(data_size);
   recv(socket_fp,buffer,data_size,0);
   *input_buffer = buffer;
   return data_size;
}

uint32_t net_read_longbinary(int socket_fp, char** input_buffer) {
  // see note from net_read_binary

  uint32_t data_size = net_read_uint32(socket_fp);
  char* buffer = malloc(data_size);
  recv(socket_fp,buffer,data_size,0);
  *input_buffer = buffer;
  return data_size;
}
