
#include <dlfcn.h>

#include <common.h>


enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };


void (*ref_difftest_memcpy)(word_t addr, void *buf, word_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(word_t n) = NULL;
// void (*ref_difftest_raise_intr)(word_t NO) = NULL;


static bool is_skip_ref = false;
static int skip_dut_nr_inst = 0;

// this is used to let ref skip instructions which
// can not produce consistent behavior with NEMU
void difftest_skip_ref() {
  is_skip_ref = true;
  // If such an instruction is one of the instruction packing in QEMU
  // (see below), we end the process of catching up with QEMU's pc to
  // keep the consistent behavior in our best.
  // Note that this is still not perfect: if the packed instructions
  // already write some memory, and the incoming instruction in NEMU
  // will load that memory, we will encounter false negative. But such
  // situation is infrequent.
  skip_dut_nr_inst = 0;
}

// this is used to deal with instruction packing in QEMU.
// Sometimes letting QEMU step once will execute multiple instructions.
// We should skip checking until NEMU's pc catches up with QEMU's pc.
// The semantic is
//   Let REF run `nr_ref` instructions first.
//   We expect that DUT will catch up with REF within `nr_dut` instructions.
void difftest_skip_dut(int nr_ref, int nr_dut) {
  skip_dut_nr_inst += nr_dut;

  while (nr_ref -- > 0) {
    ref_difftest_exec(1);
  }
}

void init_difftest(char *ref_so_file, long img_size, int port) {
  assert(ref_so_file != NULL);



  void * handle;
  handle = dlopen(ref_so_file, RTLD_LAZY);
  assert(handle);


  ref_difftest_memcpy = (void (*)(word_t, void*, word_t, bool))dlsym(handle, "difftest_memcpy");
  assert(ref_difftest_memcpy);

  ref_difftest_regcpy = (void (*)(void*, bool))dlsym(handle, "difftest_regcpy");
  assert(ref_difftest_regcpy);

  ref_difftest_exec = (void (*)(word_t))dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

//   ref_difftest_raise_intr = dlsym(handle, "difftest_raise_intr");
//   assert(ref_difftest_raise_intr);

  void (*ref_difftest_init)(int) = (void (*)(int))dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  printf("Differential testing: %s", ANSI_FMT("ON\n", ANSI_FG_GREEN));
  printf("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in menuconfig.\n", ref_so_file);

  ref_difftest_init(port);

  ref_difftest_memcpy(0x80000000, mem, img_size, DIFFTEST_TO_REF);
  ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
}

bool isa_difftest_checkregs(CPU_state *ref_r, word_t npc) {
  for(int i = 0; i < 32; i++) {  
    //Todo cpu的状态得在运行时处理
    if(ref_r->gpr[i] != cpu.gpr[i]) {
      printf("对于[%s]寄存器, expceted %lx, but got %lx.\n", regs[i], ref_r->gpr[i], cpu.gpr[i]);
      return false;
    }
  }
  if(ref_r->pc != npc) {
    printf("PC expected %lx but got %lx\n", ref_r->pc, npc);
    return false;
  }
  return true;
}
void write_cpu_regs(){
  for(int i = 0; i < 32; i++) {
    cpu.gpr[i] = cpu_gpr[i];
  }
}

static void checkregs(CPU_state *ref, word_t pc, word_t npc) {
  if (!isa_difftest_checkregs(ref, npc)) {
    npc_state.state = NPC_ABORT;
    npc_state.halt_pc = pc;
    npc_state.halt_ret = -1;
    isa_reg_display();
  }
}

void difftest_step(word_t pc, word_t npc) {
  CPU_state ref_r;
  write_cpu_regs();

    //NEMU的简化会导致某些指令的行为与REF有所差异, 因而无法进行对比.
    // 为了解决这个问题, 框架中准备了difftest_skip_ref()和difftest_skip_dut()这两个函数:比如nemu_trap,无法与spike...一致
  if (skip_dut_nr_inst > 0) {
    ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);
    if (ref_r.pc == npc) {
      skip_dut_nr_inst = 0;
      checkregs(&ref_r, pc, npc);
      return;
    }
    skip_dut_nr_inst --;
    if (skip_dut_nr_inst == 0)
      printf("can not catch up with ref.pc = " FMT_WORD " at pc = " FMT_WORD, ref_r.pc, pc);
    return;
  }

  if (is_skip_ref) {
    // to skip the checking of an instruction, just copy the reg state to reference design
    ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
    is_skip_ref = false;
    return;
  }

  ref_difftest_exec(1);

  ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);

  checkregs(&ref_r, pc, npc);
}


