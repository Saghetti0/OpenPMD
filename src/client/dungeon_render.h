/* The OpenPMD project
 * dungeon_render.h: renders a dungeon to a SDL_Texture
 */

#pragma once

#include <SDL2/SDL.h>
#include "dungeon.h"

#define TILE_SIZE 24

SDL_Texture* dungeon_render_to_texture(dungeon* render_dungeon,SDL_Renderer* renderer,char* tileset_name);
