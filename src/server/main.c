/* The OpenPMD project
 * main.c: the main file for the OpenPMD server
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "net_main.h"
/*#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>*/

#define SERVER_VERSION "Pre-alpha 0.0.1"

int main(int argc, char* argv[]) {
  net_init("0.0.0.0",10222);
  net_serve();
  net_shutdown();
  return 0;
}
