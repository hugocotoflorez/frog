#define FROG_IMPLEMENTATION
#include "frog.h"

int
main(int argc, char *argv[])
{
        frog_da_str src = { 0 };
        frog_rebuild_itself(argc, argv);

        frog_filter_files(&src, "./src", ".*.c");
        frog_cmd_foreach(src, "gcc", "-c", NULL);
        frog_delete_filter(&src);

        frog_makedir("objs");
        frog_shell_cmd("mv *.o objs");
        frog_shell_cmd("gcc objs/*.o -o executable");

        return 0;
}
