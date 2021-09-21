#include "../../includes/utils.h"

int	lst_size(t_cmd_lst *cmd_lst)
{
	int	count;

	if (cmd_lst == NULL)
		return (0);
	count = 1;
	while ((cmd_lst->next) != NULL)
	{
		cmd_lst = (cmd_lst->next);
		count++;
	}
	return (count);
}

int	env_lst_size(t_env_lst *env_lst)
{
	int	count;

	if (env_lst == NULL)
		return (0);
	count = 1;
	while ((env_lst->next) != NULL)
	{
		env_lst = (env_lst->next);
		count++;
	}
	return (count);
}