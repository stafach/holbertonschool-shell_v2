#include "hsh.h"

/**
 * split_pipe - Splits a segment into '|'-separated command strings.
 * @segment: The segment to split (mutated in place).
 * @cmds: Output array of command strings.
 * @max: Capacity of @cmds.
 *
 * Return: number of commands found.
 */
static int split_pipe(char *segment, char **cmds, int max)
{
	int n = 0;
	char *cur = segment;
	size_t i;

	while (cur != NULL && n < max)
	{
		cmds[n++] = cur;
		i = 0;
		while (cur[i] != '\0' && cur[i] != '|')
			i++;
		if (cur[i] == '\0')
			cur = NULL;
		else
		{
			cur[i] = '\0';
			cur = &cur[i + 1];
		}
	}
	return (n);
}

/**
 * run_stage - Wires one pipeline stage's stdin/stdout, then runs it.
 * @sh: Shell context.
 * @cmd_str: This stage's own command text.
 * @in_fd: Where this stage reads from (STDIN_FILENO for the first
 * stage, otherwise the previous stage's pipe read end).
 * @out_fd: Where this stage writes to (STDOUT_FILENO for the last
 * stage, otherwise this stage's pipe write end).
 * @expanded: The original malloc'ed line, freed here before exiting.
 *
 * Description: Never returns — a builtin exits right after running,
 * and an external command falls through to child_process(), which
 * always calls _exit() itself (execve() on success never returns
 * either). Every command in a pipeline runs in its own process, even
 * a builtin, exactly like a real shell running it in a subshell.
 */
static void run_stage(shell_t *sh, char *cmd_str, int in_fd, int out_fd,
		       char *expanded)
{
	char *args[MAX_ARGS];
	redirect_t redirs[MAX_REDIRS];
	int status;

	if (in_fd != STDIN_FILENO)
	{
		dup2(in_fd, STDIN_FILENO);
		close(in_fd);
	}
	if (out_fd != STDOUT_FILENO)
	{
		dup2(out_fd, STDOUT_FILENO);
		close(out_fd);
	}

	init_redirections(redirs);
	if (parse_line(cmd_str, args, redirs) <= 0)
	{
		env_free(sh->env);
		free(expanded);
		_exit(2);
	}
	if (is_builtin(args[0]))
	{
		status = run_builtin(sh, args, &sh->should_exit);
		fflush(stdout);
		env_free(sh->env);
		free(expanded);
		_exit(status);
	}
	child_process(sh, args, redirs, expanded);
}

/**
 * spawn_stage - Forks one pipeline stage.
 * @sh: Shell context.
 * @cmd_str: This stage's own command text.
 * @in_fd: Read end this stage should use as stdin.
 * @out_fd: Write end this stage should use as stdout.
 * @expanded: The original malloc'ed line.
 *
 * Return: the child's pid, or -1 on a failed fork().
 */
static pid_t spawn_stage(shell_t *sh, char *cmd_str, int in_fd, int out_fd,
			  char *expanded)
{
	pid_t pid;

	pid = fork();
	if (pid == -1)
	{
		perror(sh->name);
		return (-1);
	}
	if (pid == 0)
		run_stage(sh, cmd_str, in_fd, out_fd, expanded);
	return (pid);
}

/**
 * wait_all - Waits for every stage, keeping the last one's status.
 * @pids: The pipeline's child pids.
 * @n: Number of children.
 *
 * Return: the last stage's exit status (a pipeline's own status,
 * same convention as sh: no pipefail).
 */
static int wait_all(pid_t *pids, int n)
{
	int status = 0;
	int st;
	int i;

	for (i = 0; i < n; i++)
	{
		if (waitpid(pids[i], &st, 0) == -1 || i != n - 1)
			continue;
		if (WIFEXITED(st))
			status = WEXITSTATUS(st);
		else if (WIFSIGNALED(st))
			status = 128 + WTERMSIG(st);
	}
	return (status);
}

/**
 * execute_pipeline - Runs a '|'-separated chain of commands.
 * @sh: Shell context.
 * @segment: The segment to run (mutated in place by split_pipe()).
 * @expanded: The original malloc'ed line, for stages to free.
 *
 * Return: the pipeline's exit status.
 */
int execute_pipeline(shell_t *sh, char *segment, char *expanded)
{
	char *cmds[MAX_PIPES];
	pid_t pids[MAX_PIPES];
	int n = split_pipe(segment, cmds, MAX_PIPES);
	int in_fd = STDIN_FILENO;
	int pfd[2];
	int i;

	for (i = 0; i < n; i++)
	{
		if (i < n - 1 && pipe(pfd) == -1)
		{
			perror(sh->name);
			return (1);
		}
		pids[i] = spawn_stage(sh, cmds[i], in_fd,
				       i < n - 1 ? pfd[1] : STDOUT_FILENO,
				       expanded);
		if (in_fd != STDIN_FILENO)
			close(in_fd);
		if (i < n - 1)
		{
			close(pfd[1]);
			in_fd = pfd[0];
		}
	}
	return (wait_all(pids, n));
}
