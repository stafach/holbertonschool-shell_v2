#include "hsh.h"

/**
 * print_not_found - Prints a command-not-found error.
 * @sh: Shell context.
 * @cmd: Command that was not found.
 */
static void print_not_found(shell_t *sh, const char *cmd)
{
	fprintf(stderr, "%s: %lu: %s: not found\n",
		sh->name, sh->line_no, cmd);
}

/**
 * child_process - Executes a command in the child process.
 * @sh: Shell context.
 * @argv: Command arguments.
 * @path: Executable path.
 * @redirs: Command redirections.
 */
static void child_process(shell_t *sh, char **argv, char *path,
			  redirect_t *redirs)
{
	if (apply_redirections(redirs) == -1)
		_exit(1);

	execve(path, argv, sh->env);
	perror(sh->name);
	_exit(126);
}

/**
 * execute_external - Forks and executes an external command.
 * @sh: Shell context.
 * @argv: Command arguments.
 * @path: Executable path.
 * @redirs: Command redirections.
 *
 * Return: Command exit status.
 */
static int execute_external(shell_t *sh, char **argv, char *path,
			     redirect_t *redirs)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror(sh->name);
		return (1);
	}

	if (pid == 0)
		child_process(sh, argv, path, redirs);

	if (waitpid(pid, &status, 0) == -1)
		return (1);

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));

	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));

	return (1);
}

/**
 * execute_command - Finds and executes an external command.
 * @sh: Shell context.
 * @argv: Command arguments.
 * @redirs: Command redirections.
 *
 * Return: Command exit status.
 */
int execute_command(shell_t *sh, char **argv, redirect_t *redirs)
{
	char *path;
	int status;

	path = find_command(argv[0], sh->env);
	if (path == NULL)
	{
		print_not_found(sh, argv[0]);
		return (127);
	}

	status = execute_external(sh, argv, path, redirs);
	free(path);

	return (status);
}
