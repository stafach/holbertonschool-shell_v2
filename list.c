#include "hsh.h"

/**
 * find_connector - Finds the next top-level ';', '&&' or '||'.
 * @start: Where to start scanning.
 * @type: Set to the connector found.
 *
 * Description: A lone '|' is a pipe, not a list connector — it is
 * left untouched here for the pipeline layer to find later. Whatever
 * connector is found is cut out of the string (replaced by '\0'), so
 * each segment ends up as its own independent, null-terminated string.
 *
 * Return: pointer to the start of the next segment, or NULL if
 * @start runs to the end of the line with no connector left in it.
 */
static char *find_connector(char *start, int *type)
{
	size_t i = 0;

	while (start[i] != '\0')
	{
		if (start[i] == ';')
		{
			*type = LIST_SEMI;
			start[i] = '\0';
			return (&start[i + 1]);
		}
		if (start[i] == '&' && start[i + 1] == '&')
		{
			*type = LIST_AND;
			start[i] = '\0';
			return (&start[i + 2]);
		}
		if (start[i] == '|' && start[i + 1] == '|')
		{
			*type = LIST_OR;
			start[i] = '\0';
			return (&start[i + 2]);
		}
		i++;
	}
	return (NULL);
}

/**
 * split_list - Splits a line into ';'/'&&'/'||'-separated segments.
 * @expanded: The line to split (mutated in place).
 * @segs: Output array of segment strings.
 * @types: Output array of the connector preceding each segment
 * (types[0] is unused: the first segment always runs).
 * @max: Capacity of @segs / @types.
 *
 * Return: number of segments found.
 */
static int split_list(char *expanded, char **segs, int *types, int max)
{
	int n = 0;
	char *cur = expanded;
	int type = LIST_SEMI;

	while (cur != NULL && n < max)
	{
		segs[n] = cur;
		types[n] = type;
		n++;
		cur = find_connector(cur, &type);
	}
	return (n);
}

/**
 * run_segment - Runs one segment, dispatching to a pipeline if it
 * still contains a '|', or to the classic single-command path.
 * @sh: Shell context.
 * @segment: One ';'/'&&'/'||'-separated chunk of the line.
 * @expanded: The original malloc'ed line, for children to free.
 *
 * Return: the segment's exit status.
 */
static int run_segment(shell_t *sh, char *segment, char *expanded)
{
	char *args[MAX_ARGS];
	redirect_t redirs[MAX_REDIRS];
	int argc_cmd;
	size_t i;

	for (i = 0; segment[i] != '\0' && segment[i] != '|'; i++)
		;
	if (segment[i] == '|')
		return (execute_pipeline(sh, segment, expanded));

	init_redirections(redirs);
	argc_cmd = parse_line(segment, args, redirs);
	if (argc_cmd == -1)
		return (2);
	if (argc_cmd == 0)
		return (sh->last_status);
	if (is_builtin(args[0]))
		return (run_builtin(sh, args, &sh->should_exit));
	return (execute_command(sh, args, redirs, expanded));
}

/**
 * execute_list - Runs every segment of a line, honoring ';', '&&'
 * and '||' short-circuiting.
 * @sh: Shell context.
 * @expanded: The malloc'ed, expanded command line.
 *
 * Return: the exit status of the last segment actually run.
 */
int execute_list(shell_t *sh, char *expanded)
{
	char *segs[MAX_SEGMENTS];
	int types[MAX_SEGMENTS];
	int n = split_list(expanded, segs, types, MAX_SEGMENTS);
	int status = sh->last_status;
	int i;

	for (i = 0; i < n && !sh->should_exit; i++)
	{
		if (types[i] == LIST_AND && status != 0)
			continue;
		if (types[i] == LIST_OR && status == 0)
			continue;
		status = run_segment(sh, segs[i], expanded);
		sh->last_status = status;
	}
	return (status);
}
