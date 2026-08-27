#include "hsh.h"

/**
 * sigint_handler - Handles Ctrl-C.
 * @sig: Signal number.
 */
void sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
}

/**
 * install_signals - Installs the interactive SIGINT handler.
 */
void install_signals(void)
{
	signal(SIGINT, sigint_handler);
}

/**
 * main - Entry point of the shell.
 * @argc: Number of arguments.
 * @argv: Program arguments.
 *
 * Return: Last command exit status.
 */
int main(int argc, char **argv)
{
	shell_t sh;
	char *expanded, *line = NULL;
	size_t size = 0;
	int status;

	(void)argc;

	sh.name = argv[0];
	sh.env = env_copy(environ);
	sh.last_status = 0;
	sh.line_no = 0;
	sh.should_exit = 0;

	install_signals();

	while (!sh.should_exit)
	{
		status = read_line(&line, &size);
		if (status == -1)
			break;

		sh.line_no++;
		expanded = expand_operators(line);
		free(line);
		line = NULL;
		size = 0;
		if (expanded == NULL)
		{
			sh.last_status = 1;
			continue;
		}
		sh.last_status = execute_list(&sh, expanded);
		free(expanded);
	}

	free(line);
	env_free(sh.env);
	return (sh.last_status);
}
