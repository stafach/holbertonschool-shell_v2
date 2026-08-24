#ifndef HSH_H
#define HSH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define PROMPT "($) "
#define READ_BUF 4096
#define MAX_ARGS 128

extern char **environ;

/**
 * struct shell_s - Stores the shell execution context.
 * @env: Copy of the current environment variables.
 * @name: Name of the shell executable (argv[0]).
 * @last_status: Exit status of the last executed command.
 * @line_no: Current command line number.
 */
typedef struct shell_s
{
	char **env;
	char *name;
	int last_status;
	unsigned long line_no;
} shell_t;

char **env_copy(char **envp);
void env_free(char **envp);
char *env_get(char **envp, const char *name);
int env_set(char ***envp, const char *name, const char *value);
int env_unset(char ***envp, const char *name);

size_t sh_strlen(const char *s);
int sh_strcmp(const char *a, const char *b);
int sh_starts(const char *s, const char *prefix);
char *sh_strdup(const char *s);
char *sh_join3(const char *a, const char *b, const char *c);
int sh_is_number(const char *s);
int sh_atoi_status(const char *s, int *value);

ssize_t read_line(char **line, size_t *n);
int parse_line(char *line, char **argv);
int builtin_exit(shell_t *sh, char **argv, int *should_exit);
int builtin_cd(shell_t *sh, char **argv);
int builtin_env(shell_t *sh, char **argv);
int update_cd_env(shell_t *sh, char *old, int print_dir);
int is_builtin(const char *cmd);
int run_builtin(shell_t *sh, char **argv, int *should_exit);

char *find_command(char *cmd, char **envp);
int execute_command(shell_t *sh, char **argv);
int execute_external(shell_t *sh, char **argv, char *path);
void print_not_found(shell_t *sh, char *cmd);
void print_exec_error(shell_t *sh, char *cmd, int code);

void sigint_handler(int sig);
void install_signals(void);

#endif
