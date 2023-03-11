#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t keyboard = inl(KBD_ADDR);
  if(keyboard == 0) {
    kbd->keydown = 0;
    kbd->keycode = AM_KEY_NONE;
  }else {
    kbd->keycode = keyboard &(~KEYDOWN_MASK);
    kbd->keydown = kbd->keycode != keyboard; 
  } 
  
}
