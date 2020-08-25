/* The OpenPMD project
 * dungeon.c: the dungeon type, tiles and functions to modify dungeons
 */

#pragma once

// these will need to be modified in order to allow bigger dungeons
// or possibly making dungeon size dynamic
#define DUNGEON_WIDTH 56
#define DUNGEON_HEIGHT 32

typedef enum tile_type {
  TILE_WALL = 0,
  TILE_WATER = 1,
  TILE_GROUND = 2,
  TILE_SPECIAL = 3
} tile_type;

typedef struct dungeon_tile {
  tile_type type;
} dungeon_tile;

typedef struct dungeon {
  dungeon_tile dungeon_data[DUNGEON_WIDTH*DUNGEON_HEIGHT];
} dungeon;

dungeon* dungeon_create_empty();
void dungeon_set_tile_type(dungeon* target_dungeon,short x,short y,tile_type type);
tile_type dungeon_get_tile_type(dungeon* target_dungeon,short x,short y);
