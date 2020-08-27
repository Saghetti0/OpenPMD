/* The OpenPMD project
 * main.c: the main file for the OpenPMD client
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include "dungeon.h"
#include "dungeon_render.h"

#define SCREEN_TILE_WIDTH 11
#define SCREEN_TILE_HEIGHT 8

int main(int argc, char const *argv[]) {
  srand(time(NULL));
  // create window
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

  double screen_scale = 3;

  window = SDL_CreateWindow("OpenPMD really buggy pre-alpha",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,TILE_SIZE*SCREEN_TILE_WIDTH*screen_scale,TILE_SIZE*SCREEN_TILE_HEIGHT*screen_scale,0);
  if (window == NULL) {
    fprintf(stderr,"Error creating window! %s",SDL_GetError());
    exit(1);
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
  if (renderer == NULL) {
    fprintf(stderr,"Error creating renderer! %s",SDL_GetError());
    exit(1);
  }

  // set icon
  SDL_Surface* icon_surface = SDL_LoadBMP("icon.bmp");
  if (icon_surface == NULL) {
    fprintf(stderr,"Error creating loading icon! %s",SDL_GetError());
    exit(1);
  }
  SDL_SetWindowIcon(window,icon_surface);
  SDL_FreeSurface(icon_surface);

  printf("Creating dungeon...\n");
  dungeon* test_dungeon = dungeon_create_empty();
  for (short i=1; i<6; i++) {
    for (short j=1; j<6; j++) {
      dungeon_set_tile_type(test_dungeon,i,j,TILE_GROUND);
    }
  }
  for (short i=3; i<9; i++) {
    for (short j=3; j<5; j++) {
      dungeon_set_tile_type(test_dungeon,i,j,TILE_WATER);
    }
  }

  // render dungeon
  SDL_Texture* dungeon_rendered = dungeon_render_to_texture(test_dungeon,renderer,"dungeon1");
  bool run = true;
  int frame_ctr = 0;
  while (run) {
    SDL_Rect src_rect;
    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.w = TILE_SIZE*SCREEN_TILE_WIDTH;
    src_rect.h = TILE_SIZE*SCREEN_TILE_HEIGHT;
    SDL_RenderCopy(renderer,dungeon_rendered,&src_rect,NULL);
    SDL_RenderPresent(renderer);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        run = false;
      }
    }
    SDL_Delay(33);
    frame_ctr++;
  }
  return 0;
}
