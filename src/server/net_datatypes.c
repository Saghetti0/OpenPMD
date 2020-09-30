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

// need net_write work on a big endian processor? uncomment the #define
// note: will hurt performace a bit because the byte order needs to be swapped every time

#ifndef NET_REVERSE_BYTE_ORDER
//#define NET_REVERSE_BYTE_ORDER
#endif

// for internal use
void _net_reverse_buffer(char* buffer, int size) {
  char tmp[size];
  // make sure the counter is signed so it goes to -1 upon completion (less than 0)
  signed int ctr = size-1;
  int ctr2 = 0;
  // count down from size-1 to 0
  while (ctr >= 0) {
    tmp[ctr2] = buffer[ctr];
    ctr--;
    ctr2++;
  }
  // copy temp back to original buffer
  memcpy(buffer,tmp,size);
}

// net_read calls

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

// net_write calls

void net_write_uint8(int socket_fp, uint8_t value) {
  send(socket_fp,(char*)&value,1,0);
}

void net_write_int8(int socket_fp, int8_t value) {
  send(socket_fp,(char*)&value,1,0);
}

void net_write_uint16(int socket_fp, uint16_t value) {
  // begin the big endian shenanigains
  #ifdef NET_REVERSE_BYTE_ORDER
  _net_reverse_buffer((char*)&value,2);
  #endif
  send(socket_fp,(char*)&value,2,0);
}

void net_write_int16(int socket_fp, int16_t value) {
  #ifdef NET_REVERSE_BYTE_ORDER
  _net_reverse_buffer((char*)&value,2);
  #endif
  send(socket_fp,(char*)&value,2,0);
}

void net_write_uint32(int socket_fp, uint32_t value) {
  #ifdef NET_REVERSE_BYTE_ORDER
  _net_reverse_buffer((char*)&value,4);
  #endif
  send(socket_fp,(char*)&value,4,0);
}

void net_write_int32(int socket_fp, int32_t value) {
  #ifdef NET_REVERSE_BYTE_ORDER
  _net_reverse_buffer((char*)&value,4);
  #endif
  send(socket_fp,(char*)&value,4,0);
}

void net_write_uint64(int socket_fp, uint64_t value) {
  #ifdef NET_REVERSE_BYTE_ORDER
  _net_reverse_buffer((char*)&value,8);
  #endif
  send(socket_fp,(char*)&value,8,0);
}

void net_write_int64(int socket_fp, int64_t value) {
  #ifdef NET_REVERSE_BYTE_ORDER
  _net_reverse_buffer((char*)&value,8);
  #endif
  send(socket_fp,(char*)&value,8,0);
}

void net_write_float(int socket_fp, float value) {
  send(socket_fp,(char*)&value,4,0);
}

void net_write_double(int socket_fp, double value) {
  send(socket_fp,(char*)&value,8,0);
}

void net_write_binary(int socket_fp, char* value, uint16_t length) {
  net_write_uint16(socket_fp,length);
  send(socket_fp,value,length,0);
}

void net_write_longbinary(int socket_fp, char* value, uint32_t length) {
  net_write_uint32(socket_fp,length);
  send(socket_fp,value,length,0);
}
