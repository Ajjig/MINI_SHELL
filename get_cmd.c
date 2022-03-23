#include "minishell.h"

char	*path_joiner(char *path, char *cmd)
{
	char	*ret;
	char	*slash;

	slash = ft_strjoin(path, "/");
	ret = ft_strjoin(slash, cmd);
	free(slash);
	return (ret);
}

char	*is_in_list(char *cmd)
{
	static char	*all[] = {"echo", "cd", "pwd", "export", "unset", "env", "exit"};
	char		**others;
	char		*path;
	int			i;

	i = 0;
	while (i < 7)
		if (ft_strcmp(all[i++], cmd) == 0)
			return (ft_strdup(all[--i]));
	if (ft_strchr(cmd, '/') && access(cmd, F_OK) != -1)
		return (ft_strdup(cmd));
	path = getenv("PATH");
	if (path == NULL)
		return (printf("\x1b[31m%s: command not found\n\x1b[37m", cmd) ,NULL);
	others = ft_split(path, ':');
	i = 0;
	while (others[i])
	{
		path = path_joiner(others[i++], cmd);
		if (access(path, F_OK) != -1)
		{
			i = 0;
			while (others[i])
				free(others[i++]);
			free(others);
			return (path);
		}
		free(path);
	}
	i = 0;
	while (others[i])
		free(others[i++]);
	return (free(others), printf("%s: command not found\n", cmd), NULL);
}

char	*args_joiner(char *args, char *new)
{
	char	*joined;
	char	*space;

	if (args == NULL)
		return (ft_strdup(new));
	space = ft_strjoin(args, " ");
	joined = ft_strjoin(space, new);
	free(args);
	free(space);
	return (joined);
}


t_command	*get_cammand(char **buff, int i, t_envlist *lst)
{
	t_command	*command;
	int			ai;
	int			tmp;

	tmp = i;
	ai = 0;
	command = init_cmd(buff);
	if (command == NULL || !buff || buff[i] == NULL)
		return (NULL);
	while (buff[i])
	{
		if (buff[i] && i == 1 && !ft_strcmp(buff[i], "-n") && !ft_strcmp(_ECHO, command -> program))
			command -> options = ft_strdup(buff[i++]);
		if (i > 0)
			command -> args[ai++] = ft_strdup(buff[i]);
		if (buff[i][0] == RED_PIPE)
		{
			command -> next = get_cammand(buff, ++i, lst);
			break ;
		}
		if (ft_strchr(REDIRECTIONS, buff[i][0]))
		{
			command -> redirection = ft_strdup(buff[i++]);
			gen_files(command, buff[i - 1], buff[i]);
		}
		if (command -> program == NULL && ft_strcmp(command -> redirection, "<<"))
			command -> program = is_in_list(buff[i]);
		i++;
	}
	if (!ft_strcmp(command->redirection, "<<"))
	{
		command->heredoc = heredoc(command, lst);
	}
	command -> args[ai] = NULL;
	command -> execve = get_execve(buff, tmp);
	return command;
}
