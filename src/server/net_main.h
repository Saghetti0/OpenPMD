/* The OpenPMD project
 * net_main.h: the main server-side networking code
 */

int net_init(const char* host,int port);
void net_serve();
void net_shutdown();
