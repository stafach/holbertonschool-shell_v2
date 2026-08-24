#include "hsh.h"


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
