#include "../../includes/minishell.h"

t_data	g_data;

char    **ft_envlst_to_array(t_env_lst *env_lst)
{
    char		**env_array;
	char		*temp_str;
	char		*temp_str_final;
	int			i;

	i = env_lst_size(env_lst) + 1;
	env_array = malloc(sizeof(char *) * i);
	i = 0;
	while (env_lst)
	{
		temp_str = ft_strjoin(env_lst->key, "=");
		temp_str_final = ft_strjoin(temp_str, env_lst->value);
		env_array[i] = temp_str_final;
		free(temp_str);
		i++;
		env_lst = env_lst->next;
	}
	env_array[i] = NULL;
    return (env_array);
}

void	wait_for_childprocesses(int *pids, int len)
{
	int		stat_loc;
	char	*exit_status;
	int j;

	j = 0;
	while (j < len)
	{
		waitpid(pids[j], &stat_loc, 0);
		//printf("Exitstatus:  %d      %d\n", WEXITSTATUS(stat_loc), WTERMSIG(stat_loc));
		if (WIFEXITED(stat_loc))
		{
			exit_status = ft_itoa(WEXITSTATUS(stat_loc)); // does a %256 on statloc
			export_exists("?", exit_status); // is created in ft_getenv to give it value before first command
		}
		j++;
	}
	free (pids);
}

int	is_builtin(char *cmd)
{
	static char	*str[7] = {"echo", "cd", "pwd", "export", "unset", "env", "exit"};
	int			i;
	
	i = 0;
	while (i < 7)
	{
		if (ft_streql(cmd, str[i]))
			return (i);
		i++;
	}
	return (-1);
}

pid_t	run_builtin(int in_fd, int out_fd, t_cmd_lst *cmd_lst)
{
	pid_t	pid;
	int		exit_status;
	static int (*builtin_func[7])(char **, int) = {ft_echo, ft_cd, ft_pwd, ft_export, ft_unset, ft_env, ft_exit};
	int i;
			
	if (cmd_lst->next == NULL)
		out_fd = STDOUT_FILENO;
	if (!set_redirection(&in_fd, &out_fd, cmd_lst->subcmd))
	{
		i = is_builtin(*cmd_lst->subcmd.cmd);
		exit_status = builtin_func[i](cmd_lst->subcmd.cmd, out_fd);
	}
	else
		exit_status = EXIT_FAILURE;
	pid = fork();
	if (pid == -1)
		perror("fork error"); // temporary
	if (pid == 0)
		exit(exit_status);
	return (pid);
}

pid_t	run_command_in_childprocess(int in_fd, int out_fd, t_cmd_lst *cmd_lst, int p)
{
	pid_t	pid;
	char	*path;

	pid = fork();
	if (pid == -1)
		perror("fork error"); // temporary
	if (pid == 0)
	{
		if (cmd_lst->next == NULL)
			out_fd = STDOUT_FILENO;
		if (set_redirection(&in_fd, &out_fd, cmd_lst->subcmd))
			exit (EXIT_FAILURE);
		//printf("in %d out %d, p %d\n", in_fd, out_fd, p);
		dup_fd(in_fd, out_fd);
		close (p);
		path = get_path(*cmd_lst->subcmd.cmd);
		if (execve(path, cmd_lst->subcmd.cmd, ft_envlst_to_array(g_data.env_lst)) == -1)
		{
			// free env_array
			free (path);
			exit (EXIT_FAILURE);
		}
	}
	//printf("parent closing %d and %d\n", p, out_fd);
	//close (p);
	//(void)p;
	return (pid);
}

void	exec(t_cmd_lst *cmd_lst)
{
	int		p[2];
    int     read_pipe;
	int		*pids;
	int		i;
	
	pids = malloc(lst_size(cmd_lst) * sizeof(pid_t));
    read_pipe = STDIN_FILENO;
	i = 0;
	while (cmd_lst->next)
	{
		if (pipe(p) == -1)
			perror("pipe error"); // still need to free and exit
		//printf("new pipe rd: %d  wr: %d\n", p[0], p[1]);
		if (is_builtin(*cmd_lst->subcmd.cmd) != -1)
			pids[i] = run_builtin(read_pipe, p[1], cmd_lst);
		else
			pids[i] = run_command_in_childprocess(read_pipe, p[1], cmd_lst, p[0]);
		close (p[1]);
		if (read_pipe != STDIN_FILENO) 
			close (read_pipe);
		read_pipe = p[0];
		cmd_lst = cmd_lst->next;
		i++;
	}
	if (is_builtin(*cmd_lst->subcmd.cmd) != -1)
		pids[i] = run_builtin(read_pipe, STDOUT_FILENO, cmd_lst);
	else
		pids[i] = run_command_in_childprocess(read_pipe, STDOUT_FILENO, cmd_lst, p[0]);
	if (read_pipe != STDIN_FILENO) 
		close (read_pipe);
	i++;
	// last_pipe = multiple_commands
	wait_for_childprocesses(pids, i);
	cmd_lst_clear(&cmd_lst);
}
