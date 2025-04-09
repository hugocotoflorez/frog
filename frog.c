#define FROG_IMPLEMENTATION
#include "frog.h"

#define CC "gcc"
#define FLAGS "-Wall", "-Wextra"
#define INC "-Iinclude"
#define OUT "hsll"
#define OBJ_DIR "objects"

#define GEN_OBJ CC, FLAGS, INC, "-c"

int main(int argc, char *argv[]) {
  frog_rebuild_itself(argc, argv);

  frog_cmd_filtered_foreach("src", ".*.c", GEN_OBJ);
  frog_cmd_filtered_foreach("vshkh", ".*.c", GEN_OBJ);
  frog_cmd_filtered_foreach("vshcfp", ".*.c", GEN_OBJ);

  frog_makedir(OBJ_DIR);
  frog_shell_cmd("mv *.o " OBJ_DIR);
  frog_shell_cmd("gcc " OBJ_DIR "/*.o -o" OUT);

  return 0;
}
