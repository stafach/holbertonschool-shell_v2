#include "hsh.h"

/**
 * print_not_found - prints a command-not-found error.
 * @sh: shell state.
 * @cmd: command.
 */
void print_not_found(shell_t *sh, char *cmd)
{
	fprintf(stderr, "%s: %lu: %s: not found\n", sh->name,
		sh->line_no, cmd);
}

/**
 * print_exec_error - prints an execution error.
 * @sh: shell state.
 * @cmd: command.
 * @code: exit code.
 */
void print_exec_error(shell_t *sh, char *cmd, int code)
{
	fprintf(stderr, "%s: %lu: %s: %s\n", sh->name, sh->line_no,
		cmd, code == 126 ? "Permission denied" : "not found");
}

/**
 * execute_external - forks and executes a command.
 * @sh: shell state.
 * @argv: arguments.
 * @path: executable path.
 * Return: child status.
 */
int execute_external(shell_t *sh, char **argv, char *path)
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
	{
		execve(path, argv, sh->env);
		if (errno == EACCES)
			_exit(126);
		_exit(127);
	}
	if (waitpid(pid, &status, 0) == -1)
		return (1);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

/**
 * execute_command - resolves and executes a command.
 * @sh: shell state.
 * @argv: arguments.
 * Return: command status.
 */
int execute_command(shell_t *sh, char **argv)
{
	char *path;
	int status;

	path = find_command(argv[0], sh->env);
	if (path == NULL)
	{
		print_not_found(sh, argv[0]);
		return (127);
	}
	status = execute_external(sh, argv, path);
	free(path);
	if (status == 126)
		print_exec_error(sh, argv[0], status);
	return (status);
}
