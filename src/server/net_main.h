/* The OpenPMD project
 * net_main.h: the main server-side networking code
 */

#pragma once

// 256 should be more than good enough for now, might increase later though
#include "net_conn_info.h"

#define MAX_CONNECTIONS 256

extern net_connection_info* net_active_connections[MAX_CONNECTIONS];

int net_init(const char* host,int port);
void net_serve();
void net_shutdown();
