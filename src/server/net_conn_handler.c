/* The OpenPMD project
 * net_conn_handler.c: code responsible for handling a connected client
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

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

#include "net_conn_handler.h"
#include "net_conn_info.h"
#include "net_main.h"

void* net_conn_main(void* current_conn_info_voidptr) {
  net_connection_info* current_conn_info = (net_connection_info*) current_conn_info_voidptr;
  int socket_fp = current_conn_info->socket_fp;
  int connection_id = current_conn_info->connection_id;
  /*
  // TODO: set up inet_ntop to work with winsock
  // going to use inet_ntoa for now
  // hopefully ipv6 isn't that big of an issue
  char ip_address_str[INET_ADDRSTRLEN];
  printf("Connection established with %s\n",inet_ntop(AF_INET,&current_conn_info->client_addr,ip_address_str,INET_ADDRSTRLEN));
  */
  char* ip_address_str = inet_ntoa(current_conn_info->client_addr.sin_addr);
  char conn_identifier[32];
  sprintf(conn_identifier,"%s$%d",ip_address_str,connection_id);
  printf("Connection established with %s\n",conn_identifier);
  while (1) {
    unsigned char buffer[256];
    memset(buffer,0,256);
    int status = recv(socket_fp,(char*)&buffer,255,0);
    if (status < 0) {
       printf("Connection lost with %s due to a network error\n",conn_identifier);
       perror(conn_identifier);
       break;
    }
    if (status == 0) {
      printf("Connection lost with %s\n",conn_identifier);
      break;
    }
    int i = 0;
    while(buffer[i]) {
      buffer[i] = toupper(buffer[i]);
      i++;
    }
    printf("Recieved message from %s: %s\n",conn_identifier,buffer);
    status = send(socket_fp,(const char*)buffer,status,0); // status = length of buffer
    if (status < 0) {
       printf("Error writing to socket\n");
       perror(conn_identifier);
       break;
    }
  }
  // clean up
  //printf("Closing connection %s\n",conn_identifier);
  close(socket_fp);
  free(current_conn_info_voidptr);
  // remove entry in table
  // this is probably a really stupid way of doing things
  // actually, not probably, this *is* a really stupid way of doing things
  net_active_connections[current_conn_info->connection_slot_id] = NULL;
  return NULL;
}
