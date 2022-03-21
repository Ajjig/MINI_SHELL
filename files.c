#include "minishell.h"

void	gen_files(t_command *command, char *red, char *file)
{
	t_files	*new;

	new = (t_files *) malloc (sizeof(t_files));
	new -> is_append = false;
	if (!ft_strcmp(red, RED_APPEND))
		new -> is_append = true;
	new -> file = ft_strdup(file);
	new -> next = NULL;
	if (command -> files == NULL)
	{
		command -> files = new;
		return ;
	}
	while (command -> files -> next)
		command -> files = command -> files -> next;
	command -> files -> next = new;
}
