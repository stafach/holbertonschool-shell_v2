#include "hsh.h"

/**
 * sh_strlen - returns string length.
 * @s: string.
 * Return: length.
 */
size_t sh_strlen(const char *s)
{
	size_t n = 0;

	if (s == NULL)
		return (0);
	while (s[n] != '\0')
		n++;
	return (n);
}

/**
 * sh_strcmp - compares two strings.
 * @a: first string.
 * @b: second string.
 * Return: difference between first different characters.
 */
int sh_strcmp(const char *a, const char *b)
{
	size_t i = 0;

	while (a[i] != '\0' && b[i] != '\0' && a[i] == b[i])
		i++;
	return ((unsigned char)a[i] - (unsigned char)b[i]);
}

/**
 * sh_starts - checks whether a string starts with a prefix.
 * @s: string.
 * @prefix: prefix.
 * Return: 1 if it starts with prefix, otherwise 0.
 */
int sh_starts(const char *s, const char *prefix)
{
	size_t i = 0;

	while (prefix[i] != '\0')
	{
		if (s[i] != prefix[i])
			return (0);
		i++;
	}
	return (1);
}

/**
 * sh_strdup - duplicates a string.
 * @s: string.
 * Return: duplicate, or NULL.
 */
char *sh_strdup(const char *s)
{
	size_t i;
	size_t n = sh_strlen(s);
	char *copy;

	copy = malloc(n + 1);
	if (copy == NULL)
		return (NULL);
	for (i = 0; i <= n; i++)
		copy[i] = s[i];
	return (copy);
}

/**
 * sh_join3 - joins three strings.
 * @a: first string.
 * @b: second string.
 * @c: third string.
 * Return: new string, or NULL.
 */
char *sh_join3(const char *a, const char *b, const char *c)
{
	size_t i = 0;
	size_t j = 0;
	size_t n = sh_strlen(a) + sh_strlen(b) + sh_strlen(c);
	char *s = malloc(n + 1);

	if (s == NULL)
		return (NULL);
	while (a[i] != '\0')
		s[j++] = a[i++];
	i = 0;
	while (b[i] != '\0')
		s[j++] = b[i++];
	i = 0;
	while (c[i] != '\0')
		s[j++] = c[i++];
	s[j] = '\0';
	return (s);
}
