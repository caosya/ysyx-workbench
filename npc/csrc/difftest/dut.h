#ifndef __DIFF_H__
#define __DIFF_H__

void difftest_step(u_int64_t pc, u_int64_t npc);

void init_difftest(char *ref_so_file, long img_size, int port);
#endif