#ifndef __COMMON_H__
#define __COMMON_H__

#include "stdio.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cassert>

typedef u_int64_t word_t;
typedef int64_t  sword_t;

enum { NPC_RUNNING, NPC_STOP, NPC_END, NPC_ABORT, NPC_QUIT };

typedef struct {
  int state;
  word_t halt_pc;
  u_int32_t halt_ret;
} NPCState;

typedef struct {
  word_t gpr[32];
  word_t pc;
} CPU_state;

extern NPCState npc_state;
extern CPU_state cpu;

extern word_t *cpu_gpr;

#define SCREEN_W  400
#define SCREEN_H  300

#define MEM_SIZE 0x8000000  
#define MEM_BASE 0x80000000

#define FB_SIZE (SCREEN_W * SCREEN_H * 4)
#define FB_BASE 0xa1000000


#define IRINGBUF_LINES 64
#define IRINGBUF_LENGTH 128


#define pc_offset(pc) pc-MEM_BASE

extern u_int8_t mem[];
extern u_int8_t fb[];
extern char *img_file;

extern int vga_update_flag;

void cpu_exec(word_t n);

// ----------- log -----------

#define ANSI_FG_BLACK   "\33[1;30m"
#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_FG_YELLOW  "\33[1;33m"
#define ANSI_FG_BLUE    "\33[1;34m"
#define ANSI_FG_MAGENTA "\33[1;35m"
#define ANSI_FG_CYAN    "\33[1;36m"
#define ANSI_FG_WHITE   "\33[1;37m"
#define ANSI_BG_BLACK   "\33[1;40m"
#define ANSI_BG_RED     "\33[1;41m"
#define ANSI_BG_GREEN   "\33[1;42m"
#define ANSI_BG_YELLOW  "\33[1;43m"
#define ANSI_BG_BLUE    "\33[1;44m"
#define ANSI_BG_MAGENTA "\33[1;35m"
#define ANSI_BG_CYAN    "\33[1;46m"
#define ANSI_BG_WHITE   "\33[1;47m"
#define ANSI_NONE       "\33[0m"

#define ANSI_FMT(str, fmt) fmt str ANSI_NONE


#define FMT_WORD "0x%016lx"

#endif