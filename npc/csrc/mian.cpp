#include <verilated.h>  
#include "verilated_vcd_c.h"
#include "Vtop.h"
#include "svdpi.h"
#include "Vtop__Dpi.h"
#include "verilated_dpi.h"


#include <common.h>
#include "monitor/monitor.h"

// #include "difftest/dut.h"

// #include "device/vga.h"


// #include "memory/memory.h"

  static const uint32_t img [] = {
  0x00000297,  // auipc t0,0
  0x0002b823,  // sd  zero,16(t0)
  0x0102b503,  // ld  a0,16(t0)
  0x00100073,  // ebreak (used as nemu_trap)
  0xdeadbeef,  // some data
};

void init_disasm(const char *triple);

word_t *cpu_gpr = NULL;

// extern "C" void set_gpr_ptr(const svOpenArrayHandle r) {
//   cpu_gpr = (uint64_t *)(((VerilatedDpiOpenVar*)r)->datap());
// }

Vtop* dut = new Vtop("top");
NPCState npc_state;
CPU_state cpu;
u_int8_t mem[MEM_SIZE] = {};
u_int8_t fb[FB_SIZE] = {};

VerilatedVcdC* tfp = new VerilatedVcdC; //导出vcd波形需要加此语句 


/*********************************************/
// axi4_mem<32,64,4> sim_mem(4096l*1024*1024);
// axi4_ptr<32, 64, 4> mem_ptr;
// axi4<32, 64, 4> mem_sigs;
// axi4_ref<32, 64, 4> mem_sigs_ref(mem_sigs);


// void init_sim_mem() {
//   sim_mem.load_binary(img_file, 0x80000000);

//   mem_ptr.araddr = &(dut->axi_ar_addr_o);
//   mem_ptr.arburst = &(dut->axi_ar_burst_o);
//   mem_ptr.arid = &(dut->axi_ar_id_o);
//   mem_ptr.arlen = &(dut->axi_ar_len_o);
//   mem_ptr.arready = &(dut->axi_ar_ready_i);
//   mem_ptr.arsize = &(dut->axi_ar_size_o);
//   mem_ptr.arvalid = &(dut->axi_ar_valid_o);


//   mem_ptr.awaddr = &(dut->axi_aw_addr_o);
//   mem_ptr.awburst = &(dut->axi_aw_burst_o);
//   mem_ptr.awid = &(dut->axi_aw_id_o);
//   mem_ptr.awlen = &(dut->axi_aw_len_o);
//   mem_ptr.awready = &(dut->axi_aw_ready_i);
//   mem_ptr.awvalid = &(dut->axi_aw_valid_o);
//   mem_ptr.awsize = &(dut->axi_aw_size_o);



//   mem_ptr.rdata = &(dut->axi_r_data_i);
//   mem_ptr.rid = &(dut->axi_r_id_i);
//   mem_ptr.rlast = &(dut->axi_r_last_i);
//   mem_ptr.rready = &(dut->axi_r_ready_o);
//   mem_ptr.rresp = &(dut->axi_r_resp_i);
//   mem_ptr.rvalid = &(dut->axi_r_valid_i);


//   mem_ptr.wdata = &(dut->axi_w_data_o);
//   mem_ptr.wlast = &(dut->axi_w_last_o);
//   mem_ptr.wready = &(dut->axi_w_ready_i);
//   mem_ptr.wstrb = &(dut->axi_w_strb_o);
//   mem_ptr.wvalid = &(dut->axi_w_valid_o);


//   mem_ptr.bid = &(dut->axi_b_id_i);
//   mem_ptr.bready = &(dut->axi_b_ready_o);
//   mem_ptr.bresp = &(dut->axi_b_resp_i);
//   mem_ptr.bvalid = &(dut->axi_b_valid_i);

//   assert(mem_ptr.check());
//   // axi4_ref<32, 64, 4> mem_ref(mem_ptr);
//   printf("--check complete--\n");  

// }

/*********************************************/

  
  

int times = 0;
void single_cycle() {

  // contextp->timeInc(1);
  dut->clk = 1; 
  // dut->rst = 0;
  dut->eval();
  tfp->dump(times++);
  dut->clk = 0; 
  dut->eval();
  tfp->dump(times++); 
}

static void reset(int n) {
  // dut->pc = 0x7ffffffc;
  // dut->pc = 0x80000000;
  // dut->rst_n = 1;
  // dut->eval();
  // tfp->dump(times++);

  dut->rst_n = 0;
  // dut->eval();
  // tfp->dump(times++);
  while (n-- > 0) {
    single_cycle();
  }
  dut->rst_n = 1;
}

int vga_update_flag = 1;

char iringbuf[IRINGBUF_LINES][IRINGBUF_LENGTH];
long iringbuf_end = 0;

void set_iring_buf(word_t pc, uint32_t* instr) {

  char logbuf[128];
  char *itrace = logbuf;

  itrace += snprintf(itrace, 128, FMT_WORD ":", pc);

  uint8_t *inst = (uint8_t *)instr;
  for (int i = 3; i >= 0; i--) {
    itrace += snprintf(itrace, 4, " %02x", inst[i]);
  }
  memset(itrace, ' ', 1);
  itrace += 1;
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  // disassemble(itrace, logbuf + sizeof(logbuf) - itrace-1, (uint64_t)pc, (uint8_t *)instr, 4);

  strncpy(iringbuf[iringbuf_end % IRINGBUF_LINES], logbuf, IRINGBUF_LENGTH);
  iringbuf_end++;
  for(int i = 0; i < 64 && iringbuf[i][0] != '\0'; i++) {
    printf("=======%s\n", iringbuf[i]);
  }
}

#define NPCTRAP(thispc, code) set_NPC_state(NPC_END, thispc, code)

void set_NPC_state(int state, word_t pc, int halt_ret) {
  // difftest_skip_ref();
  npc_state.state = state;
  npc_state.halt_pc = pc;
  npc_state.halt_ret = halt_ret;
}

word_t instr_count = 0;

void break_exec(int code){
  // printf("=======%x=======\n",code);
  NPCTRAP(dut->pc, code);
}

void execute(word_t n) {
  // int dif_time = 0;
  int instr_count_origin = 0;
  for(int i = 0; i < n; i++) {
    // printf("======test======\n");
    instr_count++; //统计已经执行的指令条数

    // word_t pc_old = dut->pc;
    // bool flag = dut->pc1 == dut->pc2;
    uint32_t instr;
    instr = img[(dut->pc-MEM_BASE)/4];
    dut->instr = instr;
    single_cycle(); 
    // cpu.pc = dut->pc;

    //Itrace  
    // set_iring_buf(pc_old, &instr);
    // printf("pc:%x\n",dut->pc);

    // Difftest
    // if(instr_count > 2 && flag) {
    //   difftest_step(pc_old , cpu.pc);
    //   instr_count_origin++;
    // }   

    if(npc_state.state != NPC_RUNNING) break;


    // vga_update_screen();

  }
  printf("======npc已经执行======%ld条指令当前PC=%lx\n",instr_count, dut->pc);
  printf("======nemu已经执行======%ld条指令当前PC=%lx\n",instr_count_origin, dut->pc);
}

void cpu_exec(word_t n) {

  //初始化npc_state
  switch(npc_state.state) {
    case  NPC_END : case NPC_ABORT :
      printf("Program execution has ended. To restart the program, exit NPC and run again.\n");
      return;
    default : npc_state.state = NPC_RUNNING;
  }

  execute(n);

  switch (npc_state.state){
    case NPC_RUNNING: npc_state.state = NPC_STOP; break;

    case NPC_END: case NPC_ABORT:
      printf("NPC: %s at pc = " FMT_WORD"\n",
          (npc_state.state == NPC_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (npc_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          npc_state.halt_pc);
    //   if(npc_state.halt_ret != 0) {
    //     int error_locate = (iringbuf_end - 1) % IRINGBUF_LINES;
    //     for(int i = 0; i < 64 && iringbuf[i][0] != '\0'; i++) {
    //       if(error_locate == i) {
    //         printf("--> %s\n", iringbuf[i]);
    //       }else printf("    %s\n", iringbuf[i]);
    //     }
    //   }
      break;
    case NPC_QUIT:break;

  }
}

int main(int argc, char *argv[]) {


  Verilated::commandArgs(argc, argv); 

//波形
  Verilated::traceEverOn(true); //导出vcd波形需要加此语句 
  dut->trace(tfp, 0);   
  tfp->open("wave.vcd"); //打开vcd
  
  // init_screen();

  //初始化
//   for( int i = 0; i < 32; i++) {
//       cpu.gpr[i] = 0;
//   }
//   cpu.pc = MEM_BASE;

  init_disasm("riscv64-pc-linux-gnu");


  init_monitor(argc, argv);


  // init_sim_mem();

  //打印当前用户程序的指令
  // for(int i = 0; i < 20; i++){
  //   printf("=====%02x====\n",mem[i]);
  // }
  reset(2);
  // printf("=======\n");
//   sdb_mainloop();
  cpu_exec(10);

  dut->final();
  tfp->close();
  delete dut;
  return 0;
}
