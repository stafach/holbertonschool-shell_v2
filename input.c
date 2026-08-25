#include "hsh.h"

/**
 * read_line - reads one command line.
 * @line: input buffer.
 * @n: input buffer size.
 * Return: getline result, or -1 on EOF.
 */
ssize_t read_line(char **line, size_t *n)
{
	ssize_t len;

	while (1)
	{
		if (isatty(STDIN_FILENO))
		{
			printf("%s", PROMPT);
			fflush(stdout);
		}
		len = getline(line, n, stdin);
		if (len != -1 || errno != EINTR)
			break;
	}
	if (len > 0 && (*line)[len - 1] == '\n')
		(*line)[len - 1] = '\0';
	return (len);
}

/**
 * expand_redirs - inserts spaces around redirection operators so that
 * they are recognized even when glued to adjacent words (e.g. "ls>out").
 * @line: the raw line read from the user
 *
 * Return: a newly malloc'ed, expanded copy of line, or NULL on failure
 */
char *expand_redirs(const char *line)
{
	char *out;
	size_t i = 0, j = 0, len = sh_strlen(line);

	out = malloc(len * 3 + 1);
	if (out == NULL)
		return (NULL);

	while (line[i] != '\0')
	{
		if (line[i] == '<' || line[i] == '>')
		{
			out[j++] = ' ';
			out[j++] = line[i++];
			if (line[i] == out[j - 1])
				out[j++] = line[i++];
			out[j++] = ' ';
		}
		else
			out[j++] = line[i++];
	}
	out[j] = '\0';

	return (out);
}
