/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forking.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakela <amakela@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 17:48:25 by amakela           #+#    #+#             */
/*   Updated: 2024/04/20 17:48:26 by amakela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static int	find_path(t_pipex *data)
{
	int		i;
	char	*temp;

	i = 0;
	if (data->paths)
	{
		while (data->paths[i])
		{
			temp = ft_strjoin(data->paths[i], data->cmd[0]);
			if (!temp)
				return (-1);
			if (access(temp, F_OK) == 0)
			{
				data->path = temp;
				return (0);
			}
			free(temp);
			i++;
		}
	}
	ft_printf(2, "command not found: %s\n", data->cmd[0]);
	close_and_free(data);
	exit(127);
}

static int	path_check(t_pipex *data)
{
	if (access(data->cmd[0], F_OK) == 0)
	{
		data->path = ft_substr(data->cmd[0], 0, ft_strlen(data->cmd[0]));
		return (0);
	}
	ft_printf(2, "no such file or directory: %s\n", data->cmd[0]);
	return (-1);
}

static int	get_cmd(char *arg, t_pipex *data)
{
	data->cmd = ft_split(arg, 7);
	if (!data->cmd)
	{
		ft_printf(2, "Error\nSplit failed when getting a command\n");
		return (-1);
	}
	if (data->cmd[0] == '\0')
	{
		ft_printf(2, "command not found: %s\n", arg);
		close_and_free(data);
		exit(127);
	}
	if (ft_strchr(data->cmd[0], '/'))
	{
		if (path_check(data) == -1)
			return (-1);
	}
	else
	{
		if (find_path(data) == -1)
			return (-1);
	}
	return (0);
}

static int	do_cmd(t_pipex *data, char **argv, char **envp)
{
	if (data->read_end != -1)
		close(data->read_end);
	dup2(data->ends[0], STDIN_FILENO);
	dup2(data->ends[1], STDOUT_FILENO);
	if (data->ends[0] != -1)
		close(data->ends[0]);
	if (data->ends[1] != -1)
		close(data->ends[1]);
	data->new_arg = parse_arg(data, argv[data->count + 2]);
	if (!data->new_arg)
		return (-1);
	if (get_cmd(data->new_arg, data) == -1)
		return (-1);
	if (!data->path)
	{
		ft_printf(2, "command not found: %s\n", data->cmd[0]);
		return (-1);
	}
	execve(data->path, data->cmd, envp);
	ft_printf(2, "permission denied: %s\n", data->cmd[0]);
	return (-1);
}

int	forking(t_pipex *data, char **argv, char **envp)
{
	data->pids[data->count] = fork();
	if (data->pids[data->count] < 0)
	{
		ft_printf(2, "Error\nFork failed\n");
		return (-1);
	}
	if (data->pids[data->count] == 0)
	{
		if (do_cmd(data, argv, envp) == -1)
			return (-1);
	}
	return (0);
}
