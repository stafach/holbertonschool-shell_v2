#include "hsh.h"

/**
 * builtin_setenv - Creates or updates an environment variable.
 * @sh: Shell state.
 * @argv: Arguments (argv[1] = name, argv[2] = value, no argv[3]).
 *
 * Return: 0 on success, 1 on failure.
 */
int builtin_setenv(shell_t *sh, char **argv)
{
	if (argv[1] == NULL || argv[2] == NULL || argv[3] != NULL)
	{
		fprintf(stderr, "%s: %lu: setenv: usage: setenv VARIABLE VALUE\n",
			sh->name, sh->line_no);
		return (1);
	}

	if (env_set(&sh->env, argv[1], argv[2]) == -1)
	{
		fprintf(stderr, "%s: %lu: setenv: failed to set %s\n",
			sh->name, sh->line_no, argv[1]);
		return (1);
	}

	return (0);
}

/**
 * builtin_unsetenv - Removes an environment variable.
 * @sh: Shell state.
 * @argv: Arguments (argv[1] = name, no argv[2]).
 *
 * Return: 0 on success, 1 on failure.
 */
int builtin_unsetenv(shell_t *sh, char **argv)
{
	if (argv[1] == NULL || argv[2] != NULL)
	{
		fprintf(stderr, "%s: %lu: unsetenv: usage: unsetenv VARIABLE\n",
			sh->name, sh->line_no);
		return (1);
	}

	if (env_unset(&sh->env, argv[1]) == -1)
	{
		fprintf(stderr, "%s: %lu: unsetenv: failed to unset %s\n",
			sh->name, sh->line_no, argv[1]);
		return (1);
	}

	return (0);
}
