#include "hsh.h"

/**
 * parse_line - splits a command line into arguments.
 * @line: input line.
 * @argv: output argument vector.
 * Return: number of arguments.
 */
int parse_line(char *line, char **argv)
{
	char *token;
	int argc = 0;

	token = strtok(line, " \t\r\n");
	while (token != NULL && argc < MAX_ARGS - 1)
	{
		argv[argc++] = token;
		token = strtok(NULL, " \t\r\n");
	}
	argv[argc] = NULL;
	return (argc);
}
