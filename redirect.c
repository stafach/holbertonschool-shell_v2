#include "hsh.h"

/**
 * redirect_output - Redirects stdout to a file.
 * @redirect: Output redirection.
 *
 * Return: 0 on success, -1 on failure.
 */
static int redirect_output(redirect_t *redirect)
{
	int fd;
	int flags;

	flags = O_WRONLY | O_CREAT | O_TRUNC;
	if (redirect->type == REDIR_APPEND)
		flags = O_WRONLY | O_CREAT | O_APPEND;

	fd = open(redirect->target, flags, 0644);
	if (fd == -1)
	{
		perror("hsh");
		return (-1);
	}

	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("hsh");
		close(fd);
		return (-1);
	}

	close(fd);
	return (0);
}

/**
 * redirect_input - Redirects stdin from a file.
 * @redirect: Input redirection.
 *
 * Return: 0 on success, -1 on failure.
 */
static int redirect_input(redirect_t *redirect)
{
	int fd;

	fd = open(redirect->target, O_RDONLY);
	if (fd == -1)
	{
		perror("hsh");
		return (-1);
	}

	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("hsh");
		close(fd);
		return (-1);
	}

	close(fd);
	return (0);
}

/**
 * apply_heredoc - Reads input until the delimiter is reached.
 * @delimiter: Here-document delimiter.
 *
 * Return: 0 on success, -1 on failure.
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

	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		close(pipefd[0]);
		return (-1);
	}

	close(pipefd[0]);
	return (0);
}

/**
 * apply_one_redirect - Applies one redirection.
 * @redirect: Redirection to apply.
 *
 * Return: 0 on success, -1 on failure.
 */
static int apply_one_redirect(redirect_t *redirect)
{
	if (redirect->type == REDIR_OUT ||
	    redirect->type == REDIR_APPEND)
		return (redirect_output(redirect));

	if (redirect->type == REDIR_IN)
		return (redirect_input(redirect));

	if (redirect->type == REDIR_HEREDOC)
		return (apply_heredoc(redirect->target));

	return (0);
}

/**
 * apply_redirections - Applies all command redirections.
 * @redirs: Redirection array.
 *
 * Return: 0 on success, -1 on failure.
 */
int apply_redirections(redirect_t *redirs)
{
	int i;

	for (i = 0; i < MAX_REDIRS; i++)
	{
		if (redirs[i].type == REDIR_NONE)
			break;

		if (apply_one_redirect(&redirs[i]) == -1)
			return (-1);
	}

	return (0);
}
