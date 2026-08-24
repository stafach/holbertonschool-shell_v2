#include "hsh.h"

/**
 * env_count - counts environment entries.
 * @envp: environment array.
 * Return: number of entries.
 */
static size_t env_count(char **envp)
{
	size_t n = 0;

	while (envp != NULL && envp[n] != NULL)
		n++;
	return (n);
}

/**
 * env_copy - copies an environment array.
 * @envp: source environment.
 * Return: allocated copy, or NULL.
 */
char **env_copy(char **envp)
{
	size_t n = env_count(envp);
	size_t i;
	char **copy;

	copy = malloc((n + 1) * sizeof(char *));
	if (copy == NULL)
		return (NULL);
	for (i = 0; i < n; i++)
	{
		copy[i] = sh_strdup(envp[i]);
		if (copy[i] == NULL)
		{
			while (i > 0)
				free(copy[--i]);
			free(copy);
			return (NULL);
		}
	}
	copy[n] = NULL;
	return (copy);
}

/**
 * env_free - frees an environment array.
 * @envp: environment array.
 */
void env_free(char **envp)
{
	size_t i;

	if (envp == NULL)
		return;
	for (i = 0; envp[i] != NULL; i++)
		free(envp[i]);
	free(envp);
}

/**
 * env_get - gets a variable from an environment.
 * @envp: environment array.
 * @name: variable name.
 * Return: value, or NULL.
 */
char *env_get(char **envp, const char *name)
{
	size_t i;
	size_t len;

	if (envp == NULL || name == NULL)
		return (NULL);
	len = sh_strlen(name);
	for (i = 0; envp[i] != NULL; i++)
		if (sh_starts(envp[i], name) && envp[i][len] == '=')
			return (envp[i] + len + 1);
	return (NULL);
}

/**
 * env_set - sets or adds an environment variable.
 * @envp: environment pointer.
 * @name: variable name.
 * @value: variable value.
 * Return: 0 on success, -1 on failure.
 */
int env_set(char ***envp, const char *name, const char *value)
{
	size_t i;
	size_t n = 0;
	char *entry;
	char **tmp;

	if (envp == NULL || *envp == NULL || name == NULL || value == NULL)
		return (-1);
	entry = sh_join3(name, "=", value);
	if (entry == NULL)
		return (-1);
	for (i = 0; (*envp)[i] != NULL; i++)
	{
		n++;
		if (sh_starts((*envp)[i], name) &&
		    (*envp)[i][sh_strlen(name)] == '=')
		{
			free((*envp)[i]);
			(*envp)[i] = entry;
			return (0);
		}
	}
	tmp = malloc((n + 2) * sizeof(char *));
	if (tmp == NULL)
	{
		free(entry);
		return (-1);
	}
	for (i = 0; i < n; i++)
		tmp[i] = (*envp)[i];
	tmp[n] = entry;
	tmp[n + 1] = NULL;
	free(*envp);
	*envp = tmp;
	return (0);
}

/**
 * env_unset - removes an environment variable.
 * @envp: environment pointer.
 * @name: variable name.
 * Return: 0 on success, -1 on failure.
 */
int env_unset(char ***envp, const char *name)
{
	size_t i;
	size_t j;
	size_t n = 0;
	char **tmp;

	if (envp == NULL || *envp == NULL || name == NULL)
		return (-1);
	for (i = 0; (*envp)[i] != NULL; i++)
		n++;
	tmp = malloc((n + 1) * sizeof(char *));
	if (tmp == NULL)
		return (-1);
	j = 0;
	for (i = 0; (*envp)[i] != NULL; i++)
	{
		if (sh_starts((*envp)[i], name) &&
		    (*envp)[i][sh_strlen(name)] == '=')
			free((*envp)[i]);
		else
			tmp[j++] = (*envp)[i];
	}
	tmp[j] = NULL;
	free(*envp);
	*envp = tmp;
	return (0);
}
