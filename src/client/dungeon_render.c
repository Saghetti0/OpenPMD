/* The OpenPMD project
 * dungeon_render.c: renders a dungeon to a SDL_Texture
 * this is only for
 */

#include "dungeon.h"
#include "dungeon_render.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Texture* dungeon_render_to_texture(dungeon* render_dungeon,SDL_Renderer* renderer,char* tileset_name) {
  // the DMA data for this tileset
  // see https://github.com/SkyTemple/skytemple-files/tree/dungeon-bin/skytemple_files/graphics/dma
  unsigned char dma[2352];
  char dungeon_tileset_filename[64];
  char dungeon_dma_filename[64];

  // get paths to the image data and dma
  snprintf(dungeon_tileset_filename,sizeof(dungeon_tileset_filename),"dungeon_gfx/%s.bmp",tileset_name);
  snprintf(dungeon_dma_filename,sizeof(dungeon_dma_filename),"dungeon_gfx/%s.dma",tileset_name);

  // load tileset into vram
  printf("Loading tileset into VRAM\n");
  SDL_Surface* dungeon_tileset_surface = SDL_LoadBMP(dungeon_tileset_filename);
  if (dungeon_tileset_surface == NULL) {
    fprintf(stderr,"Error loading tileset %s! %s",dungeon_tileset_filename,SDL_GetError());
    return NULL;
  }
  SDL_Texture* dungeon_tileset = SDL_CreateTextureFromSurface(renderer,dungeon_tileset_surface);
  SDL_FreeSurface(dungeon_tileset_surface);

  // read DMA data
  printf("Reading DMA data\n");
  FILE *fptr = fopen(dungeon_dma_filename,"rb");
  if (fptr == NULL) {
    fprintf(stderr,"Error loading DMA!");
    return NULL;
  }
  fread(&dma,1,2352,fptr);
  for (int i=0; i<2352; i++) {
    printf("%x",dma[i]);
  }
  printf("\n");
  fclose(fptr);

  // prepare renderer
  //printf("Creating texture\n");
  SDL_Texture* dungeon_rendered = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,DUNGEON_WIDTH*TILE_SIZE,DUNGEON_HEIGHT*TILE_SIZE);
  SDL_Texture* previous_render_target = SDL_GetRenderTarget(renderer);
  SDL_SetRenderTarget(renderer,dungeon_rendered);

  // render tiles to dungeon_rendered
  for (short tile_y=0; tile_y<DUNGEON_HEIGHT; tile_y++) {
    for (short tile_x=0; tile_x<DUNGEON_WIDTH; tile_x++) {
      tile_type current_tile = dungeon_get_tile_type(render_dungeon,tile_x,tile_y);
      printf("Processing tile %d, %d with type %d:\n",tile_x,tile_y,current_tile);
      /*
       * In order to render the dungeon correctly, we need to get the 8 surrounding tiles for this tile
       * and then compare if each tile is the same type as the tile we're currently processing
       * These flags look as follows:
       *   543
       *   6X2
       *   701
       * Where each number is the bit index in the lower byte index of the DMA table
       */
      // table to store the bit indexes of all surrounding tiles, from left to right and top to bottom
      short bit_index_lookup[] = {5,4,3,6,-1,2,7,0,1};
      // keep track of what index we are in the bit_index_lookup table
      short current_lookup_index = 0;
      // the final index we should check in the DMA table
      // set upper 2 bits to the current tile type
      short dma_index = (short)current_tile << 8;
      for (short tile_offset_y=-1; tile_offset_y<2; tile_offset_y++) {
        for (short tile_offset_x=-1; tile_offset_x<2; tile_offset_x++) {
          //printf("Checking %d, %d\n",tile_offset_x,tile_offset_y);
          short checking_tile_x = tile_x+tile_offset_x;
          short checking_tile_y = tile_y+tile_offset_y;
          if (!((tile_offset_x == 0) && (tile_offset_y == 0))) {
            tile_type checking_tile;
            if ((checking_tile_x<0 || checking_tile_x>=DUNGEON_WIDTH) || (checking_tile_y<0 || checking_tile_y>=DUNGEON_HEIGHT)) {
              //printf("OOB ");
              checking_tile = TILE_WALL; // set out-of-bounds tiles to wall
            } else {
              //printf("IB ");
              checking_tile = dungeon_get_tile_type(render_dungeon,checking_tile_x,checking_tile_y);
            }
            if (current_tile == checking_tile) {
              // the tile is the same type as our current tile
              //printf("MATCH!\n");
              dma_index |= 1 << bit_index_lookup[current_lookup_index];
            } else {
              // the tile is not the same type as our current tile
              //printf("NO MATCH!\n");
              dma_index &= ~(1 << bit_index_lookup[current_lookup_index]);
            }
          }
          current_lookup_index++;
        }
      }

      // multiply by 3 to get the 1st variation of the tile
      // https://github.com/SkyTemple/skytemple-files/tree/dungeon-bin/skytemple_files/graphics/dma#index-mapping
      dma_index *= 3;
      short tile_rand = rand();
      tile_rand = tile_rand & 0xF;
      if (tile_rand > 12) {
        dma_index += 1+(rand()%2);
      }
      SDL_Rect dest_rect;
      dest_rect.x = tile_x*TILE_SIZE;
      dest_rect.y = tile_y*TILE_SIZE;
      dest_rect.w = TILE_SIZE;
      dest_rect.h = TILE_SIZE;
      if (dma_index >= sizeof(dma)) {
        fprintf(stderr,"Invalid tilesheet access! Index %d outside of DMA table size %d",dma_index,(int)sizeof(dma));
      } else {
        char tilesheet_x = dma[dma_index]%16;
        char tilesheet_y = dma[dma_index]/16;
        if ((tilesheet_x > 15) || (tilesheet_y > 15) || (tilesheet_x < 0) || (tilesheet_y < 0)) {
          fprintf(stderr,"Invalid tilesheet access! %d, %d, DMA table index: %d, Tileset: %s\n",tilesheet_x,tilesheet_y,dma_index,tileset_name);
        } else {
          SDL_Rect source_rect;
          source_rect.x = tilesheet_x*TILE_SIZE;
          source_rect.y = tilesheet_y*TILE_SIZE;
          source_rect.w = TILE_SIZE;
          source_rect.h = TILE_SIZE;
          SDL_RenderCopy(renderer,dungeon_tileset,&source_rect,&dest_rect);
        }
      }
    }
  }
  // change back to previous render target
  SDL_SetRenderTarget(renderer,previous_render_target);
  return dungeon_rendered;
}
