#include <getopt.h>
#include <common.h>
#include "monitor.h"
#include "../difftest/dut.h"
#include "sdb/sdb.h"

static char *diff_so_file = NULL;
static int difftest_port = 1234;

char *img_file = NULL;
static long load_img() {
  if (img_file == NULL) {
    printf("No image is given. Use the default build-in image.\n");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  assert(fp);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  printf("The image is %s, size = %ld\n", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(mem, size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"elf"      , required_argument, NULL, 'e'},
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  // printf("++++++++++++++++++++%d++++++++\n", argc);
  // printf("++++++++++++++++++++%s++++++++\n", *argv);
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:e:", table, NULL)) != -1) {
    printf("++++++++++++++++++++++++++++\n");
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      // case 'p': sscanf(optarg, "%d", &difftest_port); break;
      // case 'e': elf_file = optarg; break;
      // case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg;printf("=====%s\n",optarg); break;
      case 1: img_file = optarg; printf("=====%s\n",optarg);return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\n");
        exit(0);
    }
  }
  printf("++++++++++++++++++++000++++++++\n");
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  // printf("%ld\n",load_img());
  long img_size = load_img();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);


}