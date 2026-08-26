#include "hsh.h"

/**
 * init_redirections - Initializes the redirection array.
 * @redirs: Redirection array.
 */
void init_redirections(redirect_t *redirs)
{
	int i;

	for (i = 0; i < MAX_REDIRS; i++)
	{
		redirs[i].type = REDIR_NONE;
		redirs[i].target = NULL;
		redirs[i].fd = -1;
	}
}

/**
 * get_redirect_type - Gets the type of a redirection token.
 * @token: Token to check.
 *
 * Return: Redirection type or REDIR_NONE.
 */
static enum redirect_type get_redirect_type(char *token)
{
	if (sh_strcmp(token, ">") == 0)
		return (REDIR_OUT);
	if (sh_strcmp(token, ">>") == 0)
		return (REDIR_APPEND);
	if (sh_strcmp(token, "<") == 0)
		return (REDIR_IN);
	if (sh_strcmp(token, "<<") == 0)
		return (REDIR_HEREDOC);

	return (REDIR_NONE);
}

/**
 * add_redirect - Adds a redirection to the redirection array.
 * @redirs: Redirection array.
 * @type: Redirection type.
 * @target: File or delimiter.
 * @count: Number of redirections.
 *
 * Return: 0 on success, -1 on failure.
 */
static int add_redirect(redirect_t *redirs, enum redirect_type type,
			char *target, int *count)
{
	if (*count >= MAX_REDIRS)
		return (-1);

	redirs[*count].type = type;
	redirs[*count].target = target;
	(*count)++;

	return (0);
}

/**
 * parse_token - Processes one token and its redirection target.
 * @token: Current token.
 * @redirs: Redirection array.
 * @count: Number of redirections.
 *
 * Return: 1 if redirection, 0 otherwise, -1 on error.
 */
static int parse_token(char *token, redirect_t *redirs, int *count)
{
	enum redirect_type type;
	char *target;

	type = get_redirect_type(token);
	if (type == REDIR_NONE)
		return (0);

	target = strtok(NULL, " \t\r\n");
	if (target == NULL)
		return (-1);

	if (add_redirect(redirs, type, target, count) == -1)
		return (-1);

	return (1);
}

/**
 * parse_line - Parses command arguments and redirections.
 * @line: Command line.
 * @argv: Command arguments.
 * @redirs: Redirection array.
 *
 * Return: Number of arguments, -1 on parsing error.
 */
int parse_line(char *line, char **argv, redirect_t *redirs)
{
	char *token;
	int argc = 0;
	int count = 0;
	int result;

	token = strtok(line, " \t\r\n");

	while (token != NULL && argc < MAX_ARGS - 1)
	{
		result = parse_token(token, redirs, &count);

		if (result == -1)
			return (-1);

		if (result == 0)
			argv[argc++] = token;

		token = strtok(NULL, " \t\r\n");
	}

	argv[argc] = NULL;

	if (token != NULL)
		return (-1);

	return (argc);
}
