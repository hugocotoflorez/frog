#ifndef FROG_H_
#define FROG_H_

/* FROG: Fast Recompilation and Object Generation
 * Author: Hugo Coto Florez
 * Repo: github.com/hugocotoflorez/frog
 * License: licenseless
 *
 * Inspired by https://github.com/tsoding/nob.h */

/* ----------| Quickstart |----------
 * [frog.c]
 * #define FROG_IMPLEMENTATION
 * #include "frog.h"
 *
 * int
 * main(int argc, char *argv[])
 * {
 *         frog_da_str src = { 0 };
 *         frog_rebuild_itself(argc, argv);
 *         frog_filter_files(&src, "./src", ".*.c");
 *         frog_cmd_foreach(src, "gcc", "-c", NULL);
 *         frog_cmd_wait("gcc", "main.o", "-o", "executable", NULL);
 *         frog_delete_filter(&src);
 *         return 0;
 * }
 * [shell]
 * $ gcc frog.c -o frog
 * $ ./frog
 *
 * Note that frog should be builded manually once. The name of
 * this executable would be used for futures self recompilations.
 * It recompiles itself if source is newer than executable.
 */

#include <dirent.h>
#include <errno.h>
#include <regex.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "da.h"

#define OLD_NAME "%s.old"
#define LOG_PRINT 0

#if defined(LOG_PRINT) && LOG_PRINT
#define LOG(format, ...) fprintf(stderr, "[LOG] " format, ##__VA_ARGS__)
#else
#define LOG(...)
#endif

typedef DA(char *) frog_da_str;

// #define FROG_IMPLEMENTATION
#ifdef FROG_IMPLEMENTATION

#define frog_path_join(path, file)                                 \
        ({                                                         \
                char *s = malloc(strlen(path) + strlen(file) + 2); \
                strcpy(s, path);                                   \
                strcat(s, "/");                                    \
                strcat(s, file);                                   \
                s;                                                 \
        })

#define frog_cmd_filtered_foreach(path, filter, ...)              \
        do {                                                      \
                frog_da_str __filter = { 0 };                     \
                frog_filter_files(&__filter, "./vshcfp", ".*.c"); \
                frog_cmd_foreach(__filter, ##__VA_ARGS__, NULL);  \
                frog_delete_filter(&__filter);                    \
        } while (0)

void
frog_makedir(const char *dir)
{
        errno = 0;
        if (mkdir(dir, 0700) != 0) {
                if (errno != EEXIST) {
                        perror(dir);
                        exit(1);
                }
        }
}


int
frog_cmd_asyncl(const char *command, char *args[])
{
        /* Todo: Recompile files only it they where modidied */
        int pid;
        char **arg;
        printf("[CMD] ");
        for (arg = args; *arg; ++arg) {
                printf("%s ", *arg);
        }
        printf("\n");

        switch (pid = fork()) {
        case 0:
                execvp(command, args);
                perror("execvp");
                exit(1);
        case -1:
                perror("fork");
                exit(1);
        default:
                return pid;
        }
}

int
frog_cmd_asyncv(const char *command, va_list fargs)
{
        frog_da_str cmdargs = { 0 };
        char *sarg;
        int pid;

        da_append(&cmdargs, (char *) command);

        do {
                sarg = va_arg(fargs, char *);
                da_append(&cmdargs, sarg);
                LOG("Adding argument: %s\n", sarg);
        } while (sarg && *sarg);


        pid = frog_cmd_asyncl(command, cmdargs.data);
        va_end(fargs);
        da_destroy(&cmdargs);
        return pid;
}

/* Execute program with variadict arguments (null terminated argument list)
 * and return the pid of the new process that is executing the command.
 * It dont handle exit status neither wait for termination. */
int
frog_cmd_async(const char *command, ...)
{
        va_list fargs;
        va_start(fargs, command);
        return frog_cmd_asyncv(command, fargs);
}

#define frog_shell_cmd(cmd) frog_cmd_wait("sh", "-c", cmd, NULL);

/* Execute command with variadict arguments (null terminated)
 * wait for command termination and return the exit code */
int
frog_cmd_wait(const char *command, ...)
{
        int pid;
        va_list fargs;
        int status;
        va_start(fargs, command);
        pid = frog_cmd_asyncv(command, fargs);
        waitpid(pid, &status, 0);
        return status;
}

/* return 1 if file1 is newer than file2, or 0 if its older */
int
frog_is_newer(const char *file1, const char *file2)
{
        struct stat f1s;
        struct stat f2s;
        stat(file1, &f1s);
        stat(file2, &f2s);
        return 0 < f1s.st_mtim.tv_sec - f2s.st_mtim.tv_sec +
                   (f1s.st_mtim.tv_nsec - f2s.st_mtim.tv_nsec) * 1e-9;
}

#define frog_rebuild_itself(argc, argv)                                                  \
        do {                                                                             \
                if (frog_is_newer(__FILE__, argv[0])) {                                  \
                        char new_name[32];                                               \
                        snprintf(new_name, sizeof new_name, OLD_NAME, argv[0]);          \
                        rename(argv[0], new_name);                                       \
                        LOG("Rebuilding " __FILE__ "\n");                                \
                        frog_cmd_wait("gcc", __FILE__, "-o", argv[0], "-std=c11", NULL); \
                        waitpid(frog_cmd_asyncl(argv[0], argv), NULL, 0);                \
                        exit(0);                                                         \
                }                                                                        \
        } while (0)

void
frog_cmd_foreach(frog_da_str files, const char *command, ...)
{
        int *pids = malloc(sizeof(int) * files.size);
        frog_da_str cmdargs = { 0 };
        int i;
        char *sarg;
        va_list fargs;

        va_start(fargs, command);
        da_append(&cmdargs, (char *) command);

        do {
                sarg = va_arg(fargs, char *);
                da_append(&cmdargs, sarg);
                LOG("Adding argument: %s\n", sarg);
        } while (sarg && *sarg);

        va_end(fargs);

        /* Prev null is goint go be replaced by filename */
        da_append(&cmdargs, NULL);

        for (i = 0; i < files.size; i++) {
                cmdargs.data[cmdargs.size - 2] = files.data[i];
                pids[i] = frog_cmd_asyncl(command, cmdargs.data);
        }

        for (i = 0; i < files.size; i++) {
                waitpid(pids[i], NULL, 0);
        }

        free(pids);
        da_destroy(&cmdargs);
}

void
frog_delete_filter(frog_da_str *da)
{
        for_da_each(elem, *da)
        {
                free(*elem);
        }
        da_destroy(da);
}

int
frog_filter_files(frog_da_str *out, const char *path, const char *pattern)
{
#define NMATCH 32
        regex_t regex;
        frog_da_str files = { 0 };
        regmatch_t m[NMATCH];

        DIR *dir;
        struct dirent *dir_entry;

        if ((dir = opendir(path)) == NULL) {
                perror(path);
                exit(1);
        }

        while ((dir_entry = readdir(dir))) {
                if (dir_entry->d_type != DT_REG)
                        continue;
                LOG("Adding %s/%s\n", path, dir_entry->d_name);
                da_append(&files, frog_path_join(path, dir_entry->d_name));
        }

        closedir(dir);

        if (regcomp(&regex, pattern, REG_ICASE)) {
                perror("regcomp");
                exit(1);
        }

        for_da_each(entry, files)
        {
                switch (regexec(&regex, *entry, NMATCH, m, REG_NOTBOL)) {
                case 0:
                        da_append(out, *entry);
                        break;

                case REG_NOMATCH:
                        break;
                default:
                        perror("regexec");
                        break;
                }
        }

        da_destroy(&files);
        regfree(&regex);
        return 0;
}

#endif // FROG_IMPLEMENTATION
#endif // FROG_H_
