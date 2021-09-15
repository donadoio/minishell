#include "../../includes/minishell.h"

t_data  g_data;

static void ft_unset_del(char *cmd)
{
    t_env_lst   *temp;
    t_env_lst   *temp2;

    temp = g_data.env_lst;
    while (temp)
    {
        if (ft_streql(cmd, temp->key) == 1)
        {
            free(temp->key);
            free(temp->value);
            temp2 = temp;
            temp2 = temp2->next;
            temp->previous->next = temp2;
            temp2->previous = temp->previous;
            free(temp);
            return ;
        }
        temp = temp->next;
    }
    return ;
}

int ft_unset(char **cmd, int fd_out)
{
    int i;

    i = 0;
    while (cmd[i] != NULL)
        i++;
    i--;
    while (i >= 0)
    {
        if (ft_isdigit(cmd[i][0]) == 1 || ft_isalnum_str(cmd[i]) == 0)
        {
            ft_dprintf(fd_out, "unset: %s: invalid parameter name\n", cmd[i]);
            return (2);
        }
        else
            ft_unset_del(cmd[i]);
        i--;
    }
    return (0);
}