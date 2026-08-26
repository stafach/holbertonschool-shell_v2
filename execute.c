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
 * prepare_heredocs - Reads every heredoc's content before forking.
 * @redirs: Command redirections.
 *
 * Description: getline() buffers far more of stdin than the single
 * line it hands back, and fork() would duplicate that buffer into
 * the child independently. Reading heredocs after forking would let
 * the parent's own untouched copy of those bytes get re-read (and
 * re-executed as commands) once the child is done. Doing it here, in
 * the shell's single process, before any fork(), keeps stdin's state
 * owned by one reader only.
 *
 * Return: 0 on success, -1 on failure.
 */
static int prepare_heredocs(redirect_t *redirs)
{
	int i;

	for (i = 0; i < MAX_REDIRS && redirs[i].type != REDIR_NONE; i++)
	{
		if (redirs[i].type != REDIR_HEREDOC)
			continue;

		redirs[i].fd = apply_heredoc(redirs[i].target);
		if (redirs[i].fd == -1)
			return (-1);
	}

	return (0);
}

/**
 * child_process - Applies redirections, resolves and runs the command.
 * @sh: Shell context.
 * @argv: Command arguments.
 * @redirs: Command redirections.
 * @expanded: The malloc'ed, expanded command line (owned by main's loop).
 *
 * Description: Redirections must be set up before the command is even
 * looked up, so that a target file is created/truncated even when the
 * command itself does not exist (matching sh's behavior). Every exit
 * path frees the memory this process inherited from the parent at
 * fork() time, since it will never return to main()'s own cleanup.
 */
static void child_process(shell_t *sh, char **argv, redirect_t *redirs,
			   char *expanded)
{
	char *path;

	if (apply_redirections(sh, redirs) == -1)
	{
		env_free(sh->env);
		free(expanded);
		_exit(2);
	}

	path = find_command(argv[0], sh->env);
	if (path == NULL)
	{
		print_not_found(sh, argv[0]);
		env_free(sh->env);
		free(expanded);
		_exit(127);
	}

	execve(path, argv, sh->env);
	perror(sh->name);
	free(path);
	env_free(sh->env);
	free(expanded);
	_exit(126);
}

/**
 * execute_command - Forks, then applies redirections and runs argv[0].
 * @sh: Shell context.
 * @argv: Command arguments.
 * @redirs: Command redirections.
 * @expanded: The malloc'ed, expanded command line, passed through so
 * the child can free its own copy before exiting.
 *
 * Return: Command exit status.
 */
int execute_command(shell_t *sh, char **argv, redirect_t *redirs,
		     char *expanded)
{
	pid_t pid;
	int status;
	int i;

	if (prepare_heredocs(redirs) == -1)
	{
		perror(sh->name);
		return (1);
	}

	pid = fork();
	if (pid == -1)
	{
		perror(sh->name);
		return (1);
	}

	if (pid == 0)
		child_process(sh, argv, redirs, expanded);

	for (i = 0; i < MAX_REDIRS && redirs[i].type != REDIR_NONE; i++)
		if (redirs[i].type == REDIR_HEREDOC && redirs[i].fd != -1)
			close(redirs[i].fd);

	if (waitpid(pid, &status, 0) == -1)
		return (1);

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));

	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));

	return (1);
}
