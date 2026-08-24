#ifndef HSH_H
#define HSH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#define PROMPT "($) "
#define MAX_ARGS 128
#define MAX_REDIRS 16

extern char **environ;

/**
 * enum redirect_type - Types of command redirections.
 * @REDIR_NONE: No redirection.
 * @REDIR_OUT: Redirect stdout and overwrite the file.
 * @REDIR_APPEND: Redirect stdout and append to the file.
 * @REDIR_IN: Redirect stdin from a file.
 * @REDIR_HEREDOC: Redirect stdin from a here-document.
 */
enum redirect_type
{
	REDIR_NONE,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_IN,
	REDIR_HEREDOC
};

/**
 * struct redirect_s - Stores command redirection information.
 * @type: Type of redirection.
 * @target: File or delimiter used by the redirection.
 */
typedef struct redirect_s
{
	enum redirect_type type;
	char *target;
} redirect_t;

/**
 * struct shell_s - Stores the shell execution context.
 * @env: Copy of the current environment variables.
 * @name: Name used to invoke the shell.
 * @last_status: Exit status of the last command.
 * @line_no: Current command line number.
 * @should_exit: Indicates whether the shell must terminate.
 */
typedef struct shell_s
{
	char **env;
	char *name;
	int last_status;
	unsigned long line_no;
	int should_exit;
} shell_t;

/* strings.c */
size_t sh_strlen(const char *s);
int sh_strcmp(const char *s1, const char *s2);
char *sh_strdup(const char *s);

/* env.c */
char **env_copy(char **envp);
void env_free(char **envp);
char *env_get(char **envp, const char *name);
int env_set(char ***envp, const char *name, const char *value);

/* input.c */
ssize_t read_line(char **line, size_t *n);

/* parse.c */
int parse_line(char *line, char **argv, redirect_t *redirs);
void init_redirections(redirect_t *redirs);

/* redirect.c */
int apply_redirections(redirect_t *redirs);
int apply_heredoc(char *delimiter);

/* path.c */
char *find_command(char *cmd, char **envp);

/* execute.c */
int execute_command(shell_t *sh, char **argv, redirect_t *redirs);

/* builtins.c */
int is_builtin(const char *cmd);
int run_builtin(shell_t *sh, char **argv);

/* cd.c */
int builtin_cd(shell_t *sh, char **argv);

/* exit.c */
int builtin_exit(shell_t *sh, char **argv);

/* signals.c */
void sigint_handler(int sig);
void install_signals(void);

/* strings.c */
size_t sh_strlen(const char *s);
int sh_strcmp(const char *s1, const char *s2);
int sh_starts(const char *str, const char *prefix);
char *sh_strdup(const char *s);
char *sh_join3(const char *a, const char *b, const char *c);
#endif
