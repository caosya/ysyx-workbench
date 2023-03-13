// #include <common.h>
// #include "../../memory/memory.h"
// #include "../../reg/reg.h"
// #include <readline/readline.h>
// #include <readline/history.h>

// // #include "verilated_dpi.h"
// // uint64_t *cpu_gpr = NULL;
// // extern "C" void set_gpr_ptr(const svOpenArrayHandle r) {
// //   cpu_gpr = (uint64_t *)(((VerilatedDpiOpenVar*)r)->datap());
// // }

// // 一个输出RTL中通用寄存器的值的示例
// // void dump_gpr() {
// //   int i;
// //   for (i = 0; i < 32; i++) {
// //     printf("gpr[%d] = 0x%016lx\n", i, cpu_gpr[i]);
// //   }
// // }

// #define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))
// #define NR_CMD ARRLEN(cmd_table)

// static int is_batch_mode = false;

// /* We use the `readline' library to provide more flexibility to read from stdin. */
// static char* rl_gets() {
//   static char *line_read = NULL;

//   if (line_read) {
//     free(line_read);
//     line_read = NULL;
//   }

//   line_read = readline("(npc) ");

//   if (line_read && *line_read) {
//     add_history(line_read);
//   }

//   return line_read;
// }

// // static int cmd_help(char *args);

// static int cmd_c(char *args) {
//   cpu_exec(-1);
//   return 0;
// }


// static int cmd_q(char *args) {
//   npc_state.state = NPC_QUIT;
//   return -1;
// }

// static int cmd_si(char *args) {
//   int step;
//   if(args == NULL) {
// 	  step = 1;
//   }else {
// 	  sscanf(args, "%d", &step);
//   }
//   cpu_exec(step);
//   return 0;
// }
// static int cmd_info (char *args) {
//   if(strcmp(args,"r") == 0) {
//     isa_reg_display();
//   }else{
//     printf("Wrong Command!\n");
//   }
//   return 0;
// }

// static int cmd_x(char *args) {
//   if(args == NULL) {
//     printf("Wrong Command!\n");
//     return 0;
//   }
// 	int num,i;
//   u_int64_t expr;
// 	sscanf(args,"%d%lx",&num,&expr);
//   for(i=0; i<num; i++) {
//     printf("0x%016lx 0x%08lx\n",expr+4*i, paddr_read(expr+4*i,4));
//   }
//   return 0;
// }

// static struct {
//   const char *name;
//   const char *description;
//   int (*handler) (char *);
// } cmd_table [] = {
//   // { "help", "Display informations about all supported commands", cmd_help },
//   { "c", "Continue the execution of the program", cmd_c },
//   { "q", "Exit NEMU", cmd_q },
//   { "si", "SINGLE STEP EXECUTION", cmd_si},
//   {"info", "PRINT REGISTER",cmd_info},
//   {"x", "SCAN MEMORY", cmd_x},
// //   {"p", "EXPRESSION EVALUATION", cmd_p},
// //   {"w", "NEW WATCHPOINT", cmd_w},
// //   {"d", "DELETE WATCHPOINT", cmd_d},
// //   {"s", "print stack", cmd_s},

//   /* TODO: Add more commands */

// };

// void test() {
//   printf("=====test=====%d\n",npc_state.state);
// }

// void sdb_set_batch_mode() {
//   is_batch_mode = true;
// }

// void sdb_mainloop() {
//   if (is_batch_mode ) {
//     cmd_c(NULL);
//     return;
//   }
//   for (char *str; (str = rl_gets()) != NULL; ) {
//     char *str_end = str + strlen(str);

//     /* extract the first token as the command */
//     char *cmd = strtok(str, " ");
//     if (cmd == NULL) { continue; }

//     /* treat the remaining string as the arguments,
//      * which may need further parsing
//      */
//     char *args = cmd + strlen(cmd) + 1;
//     if (args >= str_end) {
//       args = NULL;
//     }

// // #ifdef CONFIG_DEVICE
// //     extern void sdl_clear_event_queue();
// //     sdl_clear_event_queue();
// // #endif

//     int i;
//     for (i = 0; i < NR_CMD; i ++) {
//       if (strcmp(cmd, cmd_table[i].name) == 0) {
//         if (cmd_table[i].handler(args) < 0) { return; }
//         break;
//       }
//     }

//     if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
//   }
//   printf("Unknown command \n");
// }