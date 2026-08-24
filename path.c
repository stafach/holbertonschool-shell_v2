#include "hsh.h"

/**
 * has_slash - checks for a slash in a command.
 * @s: command string.
 * Return: 1 if a slash exists, otherwise 0.
 */
static int has_slash(const char *s)
{
	while (*s != '\0')
	{
		if (*s == '/')
			return (1);
		s++;
	}
	return (0);
}

/**
 * build_candidate - builds a PATH candidate.
 * @dir: PATH directory.
 * @cmd: command name.
 * Return: allocated candidate, or NULL.
 */
static char *build_candidate(char *dir, char *cmd)
{
	size_t a = sh_strlen(dir);
	size_t b = sh_strlen(cmd);
	char *path;
	size_t i;

	path = malloc(a + b + 2);
	if (path == NULL)
		return (NULL);
	for (i = 0; i < a; i++)
		path[i] = dir[i];
	if (a > 0 && dir[a - 1] != '/')
		path[i++] = '/';
	for (a = 0; a < b; a++)
		path[i++] = cmd[a];
	path[i] = '\0';
	return (path);
}

/**
 * find_command - searches a command in PATH.
 * @cmd: command name.
 * @envp: environment.
 * Return: executable path, or NULL.
 */
char *find_command(char *cmd, char **envp)
{
	char *path;
	char *copy;
	char *dir;
	char *end;
	char *candidate;
	int has_next;

	if (cmd == NULL || cmd[0] == '\0')
		return (NULL);
	if (has_slash(cmd))
		return (sh_strdup(cmd));
	path = env_get(envp, "PATH");
	if (path == NULL)
		return (NULL);
	copy = sh_strdup(path);
	if (copy == NULL)
		return (NULL);
	dir = copy;
	while (1)
	{
		end = dir;
		while (*end != '\0' && *end != ':')
			end++;
		has_next = (*end == ':');
		if (has_next)
			*end = '\0';
		candidate = build_candidate(dir, cmd);
		if (candidate == NULL)
			break;
		if (access(candidate, X_OK) == 0)
		{
			free(copy);
			return (candidate);
		}
		free(candidate);
		if (!has_next)
			break;
		dir = end + 1;
	}
	free(copy);
	return (NULL);
}
