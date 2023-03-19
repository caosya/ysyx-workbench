#include <am.h>
#include <klib-macros.h>
#include "npc.h"
void __am_timer_init();

void __am_timer_rtc(AM_TIMER_RTC_T *);
void __am_timer_uptime(AM_TIMER_UPTIME_T *);
void __am_input_keybrd(AM_INPUT_KEYBRD_T *);
// void __am_timer_rtc(AM_TIMER_RTC_T *);

void __am_gpu_init();
void __am_gpu_config(AM_GPU_CONFIG_T *);
void __am_gpu_status(AM_GPU_STATUS_T *);
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *);

static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) { cfg->present = true; cfg->has_rtc = true; }
static void __am_input_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = true;  }

typedef void (*handler_t)(void *buf);
static void *lut[128] = {
  [AM_TIMER_CONFIG] = __am_timer_config,
  [AM_TIMER_RTC   ] = __am_timer_rtc,
  [AM_TIMER_UPTIME] = __am_timer_uptime,
  [AM_INPUT_CONFIG] = __am_input_config,
  [AM_INPUT_KEYBRD] = __am_input_keybrd,
  [AM_GPU_CONFIG  ] = __am_gpu_config,
  [AM_GPU_FBDRAW  ] = __am_gpu_fbdraw,
  [AM_GPU_STATUS  ] = __am_gpu_status,
};

static void fail(void *buf) { panic("access nonexist register"); }

bool ioe_init() {
  for (int i = 0; i < LENGTH(lut); i++)
    if (!lut[i]) lut[i] = fail;
  __am_timer_init();
  __am_gpu_init();
  return true;
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  
  uint32_t data = *(volatile uint32_t *)(VGACTL_ADDR);
  uint16_t w = (uint16_t)(data >> 16);
  uint16_t h = (uint16_t)(data & 0xFFFF);

  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {

  int win_weight = io_read(AM_GPU_CONFIG).width;  // TODO: get the correct width

  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t *pi = (uint32_t *)(uintptr_t)ctl->pixels;
  // printf("width:%d (x:%d, y:%d) w=%d, h=%d\n", win_weight, ctl->x, ctl->y, ctl->w, ctl->h);
  for (int i = 0; i < ctl->h; ++i){
    for (int j = 0; j < ctl->w; ++j){
      // printf("====fb=%d====\n",(ctl->y) * win_weight + i * win_weight + ctl->x + j);
      // printf("====pi==%x===\n",pi[i * (ctl->w) + j]);
      fb[(ctl->y) * win_weight + i * win_weight + ctl->x + j] = pi[i * (ctl->w) + j];
    }
  }
 if (ctl->sync) {
    *(volatile uint32_t *)(SYNC_ADDR) = 1;
  }
}
void __am_gpu_init(){

}
void __am_gpu_status(AM_GPU_STATUS_T *){
  
}




void ioe_read (int reg, void *buf) { ((handler_t)lut[reg])(buf); }
void ioe_write(int reg, void *buf) { ((handler_t)lut[reg])(buf); }
