#include "hsh.h"

/**
 * is_builtin - checks for a builtin command.
 * @cmd: command name.
 * Return: 1 for builtin, otherwise 0.
 */
int is_builtin(const char *cmd)
{
	return (sh_strcmp(cmd, "exit") == 0 ||
		sh_strcmp(cmd, "cd") == 0 ||
		sh_strcmp(cmd, "env") == 0);
}

/**
 * run_builtin - runs a builtin command.
 * @sh: shell state.
 * @argv: arguments.
 * @should_exit: exit flag.
 * Return: status, or -1 if not a builtin.
 */
int run_builtin(shell_t *sh, char **argv, int *should_exit)
{
	int status;

	if (!is_builtin(argv[0]))
		return (-1);
	if (sh_strcmp(argv[0], "exit") == 0)
	{
		status = builtin_exit(sh, argv, should_exit);
		return (status);
	}
	if (sh_strcmp(argv[0], "cd") == 0)
		return (builtin_cd(sh, argv));
	return (builtin_env(sh, argv));
}

/**
 * builtin_env - prints the shell environment.
 * @sh: shell state.
 * @argv: arguments.
 * Return: status.
 */
int builtin_env(shell_t *sh, char **argv)
{
	size_t i;

	(void)argv;
	for (i = 0; sh->env[i] != NULL; i++)
		printf("%s\n", sh->env[i]);
	return (0);
}

/**
 * builtin_exit - handles exit and its arguments.
 * @sh: shell state.
 * @argv: arguments.
 * @should_exit: exit flag.
 * Return: exit status or current status.
 */
int builtin_exit(shell_t *sh, char **argv, int *should_exit)
{
	int value;

	if (argv[1] == NULL)
	{
		*should_exit = 1;
		return (sh->last_status);
	}
	if (!sh_is_number(argv[1]) ||
	    sh_atoi_status(argv[1], &value) == -1)
	{
		fprintf(stderr, "%s: %lu: exit: Illegal number: %s\n",
			sh->name, sh->line_no, argv[1]);
		return (2);
	}
	if (argv[2] != NULL)
	{
		fprintf(stderr, "%s: %lu: exit: too many arguments\n",
			sh->name, sh->line_no);
		return (1);
	}
	*should_exit = 1;
	return (value & 255);
}

/**
 * builtin_cd - changes the shell current directory.
 * @sh: shell state.
 * @argv: arguments.
 * Return: status.
 */
int builtin_cd(shell_t *sh, char **argv)
{
	char *target;
	char *old;
	int print_dir = 0;
	char cwd[READ_BUF];

	target = argv[1];
	if (target == NULL)
		target = env_get(sh->env, "HOME");
	else if (sh_strcmp(target, "-") == 0)
	{
		target = env_get(sh->env, "OLDPWD");
		print_dir = 1;
	}
	if (target == NULL)
	{
		fprintf(stderr, "%s: %lu: cd: %s not set\n", sh->name, sh->line_no,
			argv[1] == NULL ? "HOME" : "OLDPWD");
		return (1);
	}
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (1);
	if (chdir(target) == -1)
	{
		fprintf(stderr, "%s: %lu: cd: can't cd to %s\n",
			sh->name, sh->line_no, target);
		return (2);
	}
	old = sh_strdup(cwd);
	if (old == NULL)
		return (1);
	if (update_cd_env(sh, old, print_dir) == -1)
	{
		free(old);
		return (1);
	}
	free(old);
	return (0);
}
