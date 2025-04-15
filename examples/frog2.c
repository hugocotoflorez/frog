#include "frog.h"

int
main (int argc, char *argv[])
{
        frog_rebuild_itself (argc, argv);
        frog_cmd_filtered_foreach ("src", ".*.c", "gcc", "-Wall", "-c");
        frog_makedir ("objs");
        frog_shell_cmd ("mv *.o objs");
        frog_shell_cmd ("gcc objs/*.o -o hello");
        return 0;
}
