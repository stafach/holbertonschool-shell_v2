#include "hsh.h"

/**
 * update_cd_env - updates PWD and OLDPWD after cd.
 * @sh: shell state.
 * @old: previous working directory.
 * @print_dir: whether cd - was used.
 * Return: 0 on success, -1 on failure.
 */
int update_cd_env(shell_t *sh, char *old, int print_dir)
{
	char cwd[READ_BUF];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("getcwd");
		return (-1);
	}
	if (env_set(&sh->env, "OLDPWD", old) == -1)
		return (-1);
	if (env_set(&sh->env, "PWD", cwd) == -1)
		return (-1);
	if (print_dir)
		printf("%s\n", cwd);
	return (0);
}

/**
 * sh_is_number - checks whether a string is an integer.
 * @s: string.
 * Return: 1 if numeric, otherwise 0.
 */
int sh_is_number(const char *s)
{
	size_t i = 0;

	if (s == NULL || s[0] == '\0')
		return (0);
	if (s[0] == '+')
		i++;
	if (s[i] == '\0')
		return (0);
	while (s[i] != '\0')
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

/**
 * sh_atoi_status - converts an integer string.
 * @s: string.
 * @value: output value.
 * Return: 0 on success, -1 on overflow.
 */
int sh_atoi_status(const char *s, int *value)
{
	unsigned long long n = 0;
	unsigned long long limit;
	size_t i = 0;
	int sign = 1;
	unsigned int digit;

	if (s[0] == '+' || s[0] == '-')
	{
		if (s[i++] == '-')
			sign = -1;
	}
	limit = sign < 0 ? 2147483648ULL : 2147483647ULL;
	while (s[i] != '\0')
	{
		digit = (unsigned int)(s[i] - '0');
		if (n > (limit - digit) / 10)
			return (-1);
		n = n * 10 + digit;
		i++;
	}
	if (sign < 0)
	{
		*value = (n == 2147483648ULL) ? (-2147483647 - 1) : -(int)n;
		return (0);
	}
	*value = (int)n;
	return (0);
}
