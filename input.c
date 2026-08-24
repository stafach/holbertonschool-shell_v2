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
