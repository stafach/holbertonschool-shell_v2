#include "hsh.h"

/**
 * sigint_handler - handles Ctrl-C.
 * @sig: signal number.
 */
void sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
}

/**
 * install_signals - installs the interactive SIGINT handler.
 */
void install_signals(void)
{
	signal(SIGINT, sigint_handler);
}

/**
 * main - entry point for the shell.
 * @argc: argument count.
 * @argv: argument vector.
 * Return: shell exit status.
 */
int main(int argc, char **argv)
{
	shell_t sh;
	char *line = NULL;
	size_t n = 0;
	char *args[MAX_ARGS];
	int status = 0;
	int should_exit = 0;

	(void)argc;
	sh.name = argv[0];
	sh.env = env_copy(environ);
	sh.last_status = 0;
	sh.line_no = 0;
	if (sh.env == NULL)
		return (1);
	install_signals();
	while (!should_exit)
	{
		status = read_line(&line, &n);
		if (status == -1)
			break;
		sh.line_no++;
		if (parse_line(line, args) == 0)
			continue;
		status = run_builtin(&sh, args, &should_exit);
		if (status == -1)
			status = execute_command(&sh, args);
		sh.last_status = status;
	}
	free(line);
	env_free(sh.env);
	return (sh.last_status);
}
