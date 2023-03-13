/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "../../../include/isa.h"
#include <cpu/cpu.h>
#include <difftest-def.h>
#include <memory/paddr.h>

// CPU_state *state = NULL;

// 在DUT host memory的`buf`和REF guest memory的`dest`之间拷贝`n`字节,
// `direction`指定拷贝的方向, `DIFFTEST_TO_DUT`表示往DUT拷贝, `DIFFTEST_TO_REF`表示往REF拷贝
void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
  if(direction == DIFFTEST_TO_REF) {
    for(size_t i = 0; i < n; i++) {
      paddr_write(addr+i, 1, *((uint8_t*)buf+i));
    }
  }else{
    assert(0);
  }  
}

// `direction`为`DIFFTEST_TO_DUT`时, 获取REF的寄存器状态到`dut`;
// `direction`为`DIFFTEST_TO_REF`时, 设置REF的寄存器状态为`dut`;
void difftest_regcpy(void *dut, bool direction) {
  // assert(0);
  CPU_state* dummy = (CPU_state *)dut;
  if(direction == DIFFTEST_TO_DUT) {
    for( int i = 0; i < 32; i++) {
      dummy->gpr[i] = cpu.gpr[i];
    }
    dummy->pc = cpu.pc;
  }else {
    for( int i = 0; i < 32; i++) {
      cpu.gpr[i] = dummy->gpr[i];
      // printf("====%d=====%016lx\n",i,cpu.gpr[i]);
    }
    cpu.pc = dummy->pc ;
    // printf("======pc:%016lx \n",cpu.pc);
  }

}

// 让REF执行`n`条指令
void difftest_exec(uint64_t n) {
  // assert(0);
  cpu_exec(n);
}

void difftest_raise_intr(word_t NO) {
  assert(0);
}
// 初始化REF的DiffTest功能
void difftest_init(int port) {
  /* Perform ISA dependent initialization. */
  init_isa();
}
