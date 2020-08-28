/* The OpenPMD project
 * net_conn_info.h: file containing connection the net_connection_info struct
 */

#pragma once

#include <pthread.h>
#ifdef __unix__
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#elif defined(_WIN32) || defined(WIN32)
#include <winsock2.h>
#include <Ws2tcpip.h>
#endif

typedef struct net_connection_info {
  int socket_fp;
  pthread_t connection_thread;
  int connection_id;
  struct sockaddr_in client_addr;
  int connection_slot_id;
} net_connection_info;
