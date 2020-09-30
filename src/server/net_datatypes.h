/* The OpenPMD project
 * net_datatypes_read.h: functions for reading and interpreting data from a socket
 */

#include "stdint.h"

uint8_t net_read_uint8(int socket_fp);
int8_t net_read_int8(int socket_fp);

uint16_t net_read_uint16(int socket_fp);
int16_t net_read_int16(int socket_fp);

uint32_t net_read_uint32(int socket_fp);
int32_t net_read_int32(int socket_fp);

uint64_t net_read_uint64(int socket_fp);
int64_t net_read_int64(int socket_fp);

float net_read_float(int socket_fp);
double net_read_double(int socket_fp);

// string and binary are both read using net_read_binary and net_read_longbinary
uint16_t net_read_binary(int socket_fp, char** input_buffer);
uint32_t net_read_longbinary(int socket_fp, char** input_buffer);

void net_write_uint8(int socket_fp, uint8_t value);
void net_write_int8(int socket_fp, int8_t value);

void net_write_uint16(int socket_fp, uint16_t value);
void net_write_int16(int socket_fp, int16_t value);

void net_write_uint32(int socket_fp, uint32_t value);
void net_write_int32(int socket_fp, int32_t value);

void net_write_uint64(int socket_fp, uint64_t value);
void net_write_int64(int socket_fp, int64_t value);

void net_write_float(int socket_fp, float value);
void net_write_double(int socket_fp, double value);

void net_write_binary(int socket_fp, char* value, uint16_t length);
void net_write_longbinary(int socket_fp, char* value, uint32_t length);
