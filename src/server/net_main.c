/* The OpenPMD project
 * net_main.c: the main server-side networking code
 */

#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include "net_conn_info.h"
#include "net_conn_handler.h"

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

#ifndef MAKEWORD
#define MAKEWORD(a,b)   ((WORD)(((BYTE)(a))|(((WORD)((BYTE)(b)))<<8)))
#endif

// 256 should be more than good enough for now, might increase later though
#define MAX_CONNECTIONS 256

static net_connection_info* net_active_connections[MAX_CONNECTIONS];
// needed more efficiently find a free slot
// TODO: implement
//static int net_next_slot_id = 0;
static int net_server_socket_fd = 0;
static unsigned int net_next_conn_id = 0;

int net_init(const char* host,int port) {
  #if defined(_WIN32) || defined(WIN32)
  // call winsock's stupid init function
  // TODO: maybe add winsock verison error handling?
  // i doubt this is going to be run on windows 95 though...
  WSADATA wsa_data;
  WSAStartup(MAKEWORD(2, 2),&wsa_data);
  #endif

  // initialize net_active_connections
  for (int slot_id=0; slot_id<MAX_CONNECTIONS-1; slot_id++) {
    net_active_connections[slot_id] = NULL;
  }

  net_server_socket_fd = socket(AF_INET,SOCK_STREAM,0);

  if (net_server_socket_fd < 0) {
     perror("Error while creating socket");
     return 1;
  }

  struct sockaddr_in server_addr;
  memset((char *) &server_addr,0,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY; // TODO: add support for specifying host in the config
  server_addr.sin_port = htons(port);

  /*int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    return 1;
  }*/

  if (bind(net_server_socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
     perror("Error while binding socket");
    return 1;
  }

  listen(net_server_socket_fd,5);

  return 0;
}

void net_serve() {
  while (1) {
    struct sockaddr_in client_addr;
    memset((char *) &client_addr,0,sizeof(client_addr));
    int client_addr_len = sizeof(client_addr);
    int client_socket_fp = accept(net_server_socket_fd,(struct sockaddr *)&client_addr,&client_addr_len);

    if (client_socket_fp < 0) {
       perror("Error while accepting connection");
       continue;
    }

    // find free slot in net_sockets
    short found_slot = 0;
    for (int checking_slot_id = 0; checking_slot_id<MAX_CONNECTIONS-1; checking_slot_id++) {
      if (net_active_connections[checking_slot_id] == NULL) {
        found_slot = 1;
        break;
      }
    }

    if (found_slot == 0) {
      printf("Error: No connection slots available! (at maximum of %d)\n",MAX_CONNECTIONS);
      printf("Disconnecting client\n");
      close(client_socket_fp);
      continue;
    }

    net_connection_info* conn_info = (net_connection_info*) malloc(sizeof(net_connection_info));
    memset(conn_info,0,sizeof(net_connection_info));
    conn_info->socket_fp = client_socket_fp;
    conn_info->connection_id = net_next_conn_id;
    conn_info->client_addr = client_addr;
    net_next_conn_id++;

    pthread_t thread_id;
    int status = pthread_create(&thread_id,NULL,net_conn_main,(void*)conn_info);
    // NOTE: this causes the connection_thread field to be NULL for some time after the thread is created
    // this may cause issues down the line
    conn_info->connection_thread = thread_id;

    if (status) {
      perror("Unable to create thread");
      continue;
    }
  }
}

void net_shutdown() {
  #if defined(_WIN32) || defined(WIN32)
  // ugh, winsock again
  WSACleanup();
  #endif
}
