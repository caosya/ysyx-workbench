#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  // int i;
  // int w = io_read(AM_GPU_CONFIG).width;  // TODO: get io_read(the correct width
  // int h = io_read(AM_GPU_CONFIG).height;  // TODO: get the correct height
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * h; i ++) fb[i] = i;
  // outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t w_h = inl(VGACTL_ADDR);
  uint16_t w = (uint16_t) (w_h >> 16);
  uint16_t h = (uint16_t)(w_h & 0xffff);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x;
  int y = ctl->y;
  uint32_t* pixels = (uint32_t *)(ctl->pixels);
  int w = ctl->w;
  int h = ctl->h;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;

  int width = io_read(AM_GPU_CONFIG).width;  
  // int height = io_read(AM_GPU_CONFIG).height;  
  for(int i=0; i<h; i++) {
    for (int j=0; j<w; j++) {
      fb[x+y*width+i*width+j] = pixels[i*w+j];
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
