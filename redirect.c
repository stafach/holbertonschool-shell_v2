#include "hsh.h"

/**
 * redirect_output - Redirects stdout to a file.
 * @sh: Shell context, for the error message.
 * @redirect: Output redirection.
 *
 * Return: 0 on success, -1 on failure.
 */
static int redirect_output(shell_t *sh, redirect_t *redirect)
{
	int fd;
	int flags;

	flags = O_WRONLY | O_CREAT | O_TRUNC;
	if (redirect->type == REDIR_APPEND)
		flags = O_WRONLY | O_CREAT | O_APPEND;

	fd = open(redirect->target, flags, 0644);
	if (fd == -1)
	{
		fprintf(stderr, "%s: %lu: cannot open %s: ",
			sh->name, sh->line_no, redirect->target);
		perror(NULL);
		return (-1);
	}

	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror(sh->name);
		close(fd);
		return (-1);
	}

	close(fd);
	return (0);
}

/**
 * redirect_input - Redirects stdin from a file.
 * @sh: Shell context, for the error message.
 * @redirect: Input redirection.
 *
 * Return: 0 on success, -1 on failure.
 */
static int redirect_input(shell_t *sh, redirect_t *redirect)
{
	int fd;

	fd = open(redirect->target, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "%s: %lu: cannot open %s: ",
			sh->name, sh->line_no, redirect->target);
		perror(NULL);
		return (-1);
	}

	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror(sh->name);
		close(fd);
		return (-1);
	}

	close(fd);
	return (0);
}

/**
 * apply_heredoc - Reads lines from stdin until the delimiter, storing
 * them in a fresh pipe.
 * @delimiter: Here-document delimiter.
 *
 * Description: Must be called in the shell's own process, before any
 * fork(), so the single buffered stdin stream is only ever consumed
 * once. A forked child later just dup2()s the returned read end onto
 * its stdin; it must never read the heredoc lines itself.
 *
 * Return: the pipe's read-end file descriptor on success, -1 on failure.
 */
int apply_heredoc(char *delimiter)
{
	int pipefd[2];
	char *line = NULL;
	size_t size = 0;
	ssize_t len;

	if (pipe(pipefd) == -1)
		return (-1);

	while (1)
	{
		len = getline(&line, &size, stdin);
		if (len == -1)
			break;

		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';

		if (sh_strcmp(line, delimiter) == 0)
			break;

		write(pipefd[1], line, sh_strlen(line));
		write(pipefd[1], "\n", 1);
	}

	free(line);
	close(pipefd[1]);
	return (pipefd[0]);
}

/**
 * apply_one_redirect - Applies one redirection.
 * @sh: Shell context, for error messages.
 * @redirect: Redirection to apply.
 *
 * Description: A heredoc's content was already read into redirect->fd
 * by prepare_heredocs(), before the fork that leads here — this just
 * plugs that already-filled pipe onto stdin.
 *
 * Return: 0 on success, -1 on failure.
 */
static int apply_one_redirect(shell_t *sh, redirect_t *redirect)
{
	if (redirect->type == REDIR_OUT ||
	    redirect->type == REDIR_APPEND)
		return (redirect_output(sh, redirect));

	if (redirect->type == REDIR_IN)
		return (redirect_input(sh, redirect));

	if (redirect->type == REDIR_HEREDOC)
	{
		if (dup2(redirect->fd, STDIN_FILENO) == -1)
			return (-1);
		close(redirect->fd);
		return (0);
	}

	return (0);
}

/**
 * apply_redirections - Applies all command redirections.
 * @sh: Shell context, for error messages.
 * @redirs: Redirection array.
 *
 * Return: 0 on success, -1 on failure.
 */
int apply_redirections(shell_t *sh, redirect_t *redirs)
{
	int i;

	for (i = 0; i < MAX_REDIRS; i++)
	{
		if (redirs[i].type == REDIR_NONE)
			break;

		if (apply_one_redirect(sh, &redirs[i]) == -1)
			return (-1);
	}

	return (0);
}
