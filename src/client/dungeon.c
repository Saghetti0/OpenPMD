/* The OpenPMD project
 * dungeon.c: the dungeon type, tiles and functions to modify dungeons
 */

#include "dungeon.h"
#include <stdlib.h>

dungeon* dungeon_create_empty() {
  // initialize an empty dungeon
  dungeon* empty_dungeon = malloc(sizeof(dungeon));
  for (int i=0; i<DUNGEON_WIDTH*DUNGEON_HEIGHT; i++) {
    dungeon_tile tile = {TILE_WALL};
    empty_dungeon->dungeon_data[i] = tile;
  }
  return empty_dungeon;
}

void free_dungeon(dungeon* freeing_dungeon) {
  free(freeing_dungeon);
  // set to null in order to prevent use after free
  dungeon** dungeon_pointer_pointer = &freeing_dungeon;
  *dungeon_pointer_pointer = NULL;
}

void dungeon_set_tile_type(dungeon* target_dungeon,short x,short y,tile_type type) {
  target_dungeon->dungeon_data[y*DUNGEON_WIDTH+x].type = type;
}

tile_type dungeon_get_tile_type(dungeon* target_dungeon,short x,short y) {
  return target_dungeon->dungeon_data[y*DUNGEON_WIDTH+x].type;
}
