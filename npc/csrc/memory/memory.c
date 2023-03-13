#include <cassert>
#include <common.h>
#include "verilated_dpi.h"
#include <time.h>
// #include "../device/vga.h"

u_int8_t* guest_to_host(word_t paddr) {  
  // printf("=====当前paddr========%llx\n",mem);
  return  mem + paddr - MEM_BASE; 
}

// u_int8_t* guest_to_vga(word_t paddr) {  
//   // printf("=====当前paddr========%llx\n",mem);
//   return  fb + paddr - FB_BASE; 
// }
word_t host_to_guest(u_int8_t *haddr) { return haddr - mem + MEM_BASE; }

static inline word_t host_read(void *addr, int len) {
  switch (len) {
    case 1: return *(u_int8_t  *)addr;
    case 2: return *(u_int16_t *)addr;
    case 4: return *(u_int32_t *)addr;
    case 8: 
      // printf("=====当前%lx========\n",*(u_int64_t *)addr);
      return *(u_int64_t *)addr;
    default: return 0;
  }
}

static inline void host_write(void *addr, int len, word_t data) {
  switch (len) {
    case 1: *(u_int8_t  *)addr = data; return;
    case 2: *(u_int16_t *)addr = data; return;
    case 4: *(u_int32_t *)addr = data; return;
    case 8: *(u_int64_t *)addr = data; return;
    default: return;
  }
}


word_t paddr_read(word_t addr, int len) {
  
  word_t ret = host_read(guest_to_host(addr), len);
  // printf("============read from=====%lx=========\n", addr);
  // printf("============read=====%x=========\n", ret);
  // printf("=====当前值========%llx\n",ret);
  return ret;
}

void paddr_write(word_t addr, int len, word_t data)  {
  // printf("===========write in======%lx=========\n", addr);
  // printf("===========write ======%lx=========\n", data);
  host_write(guest_to_host(addr), len, data);
  // printf("===========write ======%x=========\n", data);
}

// word_t vga_read(word_t addr, int len) {
  
//   word_t ret = host_read(guest_to_vga(addr), len);
//   return ret;
// }

// void vga_write(word_t addr, int len, word_t data)  {
//   host_write(guest_to_vga(addr), len, data);
// }


int init_time_flag = 0;
static struct timespec origin_time;

// static int vga_update_flag = 1;

// void vga_update_screen() {
//   if (vga_update_flag == 1){
//     update_screen();
//     vga_update_flag = 0;
//   }
// }

extern "C" void pmem_read(long long raddr, long long *rdata) {
  // 总是读取地址为`raddr & ~0x7ull`的8字节返回给`rdata`
  // printf("=====read========%llx\n",raddr); 
  if (raddr >= MEM_BASE && raddr < MEM_BASE+MEM_SIZE) *rdata = paddr_read(raddr, 8);
  if ((raddr == 0xa0000048)) {
    if(init_time_flag == 0) {  
      clock_gettime(CLOCK_REALTIME,&origin_time);
      init_time_flag = 1;
    }
    struct timespec now;
    clock_gettime(CLOCK_REALTIME,&now);
    // printf("=================%ld\n",now.tv_sec);
    *rdata =  (now.tv_sec-origin_time.tv_sec)*1000000;
  }
  // printf("=====当前值========%llx\n",*rdata);
//   if(raddr >= FB_BASE && raddr < FB_BASE+FB_SIZE) *rdata = vga_read(raddr, 8);
  if(raddr == 0xa0000100) *rdata = (SCREEN_W << 16 | SCREEN_H);
  // if(raddr == 0xa0000104) {
  //   if(vga_update_flag == 1)   vga_update_flag = 0;
  // }



}
extern "C" void pmem_write(long long waddr, long long wdata, char wmask) {
  // 总是往地址为`waddr & ~0x7ull`的8字节按写掩码`wmask`写入`wdata`
  // `wmask`中每比特表示`wdata`中1个字节的掩码,
  // 如`wmask = 0x3`代表只写入最低2个字节, 内存中的其它字节保持不变
  if(waddr == 0xa00003f8) {
    printf("%c", wdata);
    return;
  }
  int len;
  int bug = wmask & 0xff;
  len = bug == 0xff ? 8 :
        bug == 0xf  ? 4 :
        bug == 0x3  ? 2 :
        bug == 0x1  ? 1 : 0;
  // printf("=====len: %d=====\n", bug == 0xff); 
  // printf("=====len: %d=====\n", len); 
  // printf("=======wmask=%x=====\n",wmask & 0xff);
  // printf("=======wmask的空间分配=%ld=====\n",sizeof(wmask));
  // printf("=======addr=%llx=====\n",waddr);
  // printf("=======addr的空间分配=%lx=====\n",sizeof(waddr));
  if (waddr >= MEM_BASE && waddr < MEM_BASE+MEM_SIZE) paddr_write(waddr, len, wdata);
//   if(waddr >= FB_BASE && waddr < FB_BASE+FB_SIZE) vga_write(waddr, len, wdata);
  if(waddr == 0xa0000104) vga_update_flag = 1;
  
}


