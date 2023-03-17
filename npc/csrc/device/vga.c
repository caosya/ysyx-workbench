#include <common.h>
#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>


static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

void init_screen() {
  SDL_Window *window = NULL;
  char title[128];
  sprintf(title, "%s-NPC", "riscv64");
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(SCREEN_W * 2, SCREEN_H *2 , 0, &window, &renderer);
  SDL_SetWindowTitle(window, title);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STATIC, SCREEN_W, SCREEN_H);
}

void update_screen() {
//   printf("-1--%d----\n",SDL_UpdateTexture(texture, NULL, fb, SCREEN_W * 4));
//   SDL_RenderClear(renderer);
//   printf("--2-%d----\n",SDL_RenderCopy(renderer, texture, NULL, NULL));
//   SDL_RenderPresent(renderer);
  SDL_UpdateTexture(texture, NULL, fb, SCREEN_W * 4);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void vga_update_screen() {
//   static uint64_t last = 0;
//   uint64_t now = get_time();
//   if (now - last < 1000000 / TIMER_HZ) {
//     return;
//   }
//   last = now; 
  
  if (vga_update_flag == 1){
    update_screen();
    vga_update_flag = 0;
  }
}