#include "minishell.h"

t_command	*get_next_cmd(void)
{
	char		*read;
	t_command	*command;
	char		**buff;
	int			i;

	i = 0;
	read = readline(MINISHELL);
	if (!read)
	{
		puts("exit");
		exit(1);
	}
	if (ft_strlen(read) > 0)
		add_history(read);
	buff = args_splitter(read);
	command = get_cammand(buff, 0);
	free(read);
	return command;
}

void	free_cmd(t_command *command)
{
	int	i;

	i = 0;
	free(command -> program);
	while (command -> args && command -> args[i])
		free(command -> args[i++]);
	i = 0;
	while (command -> execve[i])
		free(command -> execve[i++]);
	free(command -> execve);
	free(command -> args);
	free(command -> options);
	free(command -> redirection);
	free(command);
}
void redirection_handler(t_command *command)
{
	int file_out;

	file_out = 0;
	if (command->files != NULL)
	{

		if (!ft_strcmp(command->redirection, ">"))
		{
			file_out = open(command->files->file, O_CREAT | O_RDWR | O_TRUNC, 0666);
			dup2(file_out, 1);
			close(file_out);
		}
		else if (!ft_strcmp(command->redirection, ">>"))
		{
			file_out = open(command->files->file, O_CREAT | O_RDWR | O_APPEND, 0666);
			dup2(file_out, 1);
			close(file_out);
		}
		else if (!ft_strcmp(command->redirection, "<")) // heredoc
		{
			file_out = open(command->files->file, O_RDONLY);
			dup2(file_out, 0);
			close(file_out);
		}
		else if (!ft_strcmp(command->redirection, "<<")) // heredoc
		{
			dup2(file_out, 0);
			close(file_out);
		}
	}
}

void pipe_handler(t_command *command, t_envlist *lst)
{
	int fd[2];
	int save_stdout = dup(0);
	t_command *tmp;
	int pid;

	tmp = command;

	while (tmp)
	{
		if (tmp->next)
			pipe(fd);
		pid = fork();
		if (pid < 0)
			perror("error\n");
		if (pid == 0)
		{
			if (tmp->next)
			{
				close(1);
				dup2(fd[1], 1);
				close(fd[1]);
				close(fd[0]);
			}
			redirection_handler(tmp);
			__exec__(tmp, lst);

			exit(0);
		}
		close(0);
		if (tmp->next)
		{
			dup2(fd[0], 0);
			close(fd[0]);
			close(fd[1]);
		}
		tmp = tmp->next;
	}
	tmp = command;
	while (tmp)
	{
		wait(NULL);
		tmp = tmp->next;
	}
	close(0);
	dup2(save_stdout, 0);
	close(save_stdout);
}

int	main(int ac, char **av, char **envp)
{
	t_command	*command;
	t_envlist	*lst;

	av = NULL;
	lst = __env__init(envp);
	signal_handler();
	while (true)
	{
		ac = 0;
		command = get_next_cmd();
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		pipe_handler(command, lst);
		if (command -> program != NULL && ft_strcmp(command -> program, "exit") == 0)
		{
			free_cmd(command);
			exit(0);
		}
		free_cmd(command);
		command = NULL;
	}
}
