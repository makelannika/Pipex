/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakela <amakela@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/24 18:50:56 by amakela           #+#    #+#             */
/*   Updated: 2024/03/24 18:50:58 by amakela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	awk_handler(t_pipex *data, char *arg)
{
	data->cmd = malloc(sizeof(char *) * 3);
	if (!data->cmd)
		return (-1);
	data->cmd[0] = ft_substr(arg, 0, 3);
	if (!data->cmd[0])
		return (-1);
	data->cmd[1] = ft_substr(arg, 4, ft_strlen(arg) - 4);
	if (!data->cmd[1])
		return (-1);
	data->cmd[2] = NULL;
	return (1);
}

int	space_checker(char *arg)
{
	int i = 0;
	int flag = 1;

	if (arg[i] == 32)
		flag = -1;
	while (arg[i])
	{
		if (arg[i] == 32 && arg[i + 1] == 32)
			flag = -1;
		i++;
	}
	if (arg[i - 1] == 32)
		flag = -1;
	if (flag == -1)
		ft_printf(2, "Command not found: %s\n", arg);
	return (flag);
}

int	wait_children(int *pids, int count)
{
	int	status;
	int	exitcode;
	int	i;

	i = 0;
	while (i < count)
	{
		waitpid(pids[i], &status, 0);
		if (WIFEXITED(status) && i == count - 1)
			exitcode = WEXITSTATUS(status);
		else if (WIFSIGNALED(status) && i == count - 1)
			exitcode = WTERMSIG(status);
		i++;
	}
	exit(exitcode);
}

void	free_str_arr(char **array)
{
	int	i;

	i = 0;
	while (array[i])
		free(array[i++]);
}

int	close_and_free(t_pipex *data)
{
	close(data->ends[0]);
	close(data->ends[1]);
	close(data->read_end);
	if (data->paths)
		free_str_arr(data->paths);
	if (data->cmd)
		free_str_arr(data->cmd);
	if (data->path)
		free(data->path);
	if (data->pids)
		free(data->pids);
	return (-1);
}

int	find_path(t_pipex *data)
{
	int		i;
	char	*temp;

	i = 0;
	while (data->paths[i])
	{
		temp = ft_strjoin(data->paths[i], data->cmd[0]);
		if (!temp)
		{
			ft_printf(2, "Error\nStrjoin failed while finding path\n");
			return (close_and_free(data));
		}
		if (access(temp, F_OK) == 0)
		{
			data->path = temp;
			return (0);
		}
		free(temp);
		i++;
	}
	ft_printf(2, "Command not found: %s\n", data->cmd[0]);
	// data->error = true;
	exit(127);
	// return (close_and_free(data));
}

int	get_cmd(char *arg, t_pipex *data)
{
	if (ft_strchr(arg, 34))
	{
		data->cmd = ft_split(arg, 34);
		data->cmd[0][ft_strlen(data->cmd[0]) - 1] = '\0';
	}
	else if (ft_strchr(arg, 39))
	{
		data->cmd = ft_split(arg, 39);
		data->cmd[0][ft_strlen(data->cmd[0]) - 1] = '\0';
	}
	else
	{
		if (ft_strncmp(arg, "awk", 3) == 0)
			awk_handler(data, arg);
		else
			data->cmd = ft_split(arg, 32);
	}
	if (!data->cmd)
	{
		ft_printf(2, "Error\nSplit failed when getting a command\n");
		return (close_and_free(data));
	}
	if (data->cmd[0] == NULL)
	{
		ft_printf(2, "Command not found: %s\n", "");
		return (-1);
	}
	return (0);
}

int	path_check(t_pipex *data)
{
	if (access(data->cmd[0], F_OK) == 0)
	{
		data->path = data->cmd[0];
		return (0);
	}
	ft_printf(2, "No such file or directory: %s\n", data->cmd[0]);
	return (-1);
}

int	do_cmd(t_pipex *data, char **argv, char **envp)
{
	data->pids[data->count] = fork();
	if (data->pids[data->count] < 0)
	{
		ft_printf(2, "Error\nFork failed\n");
		return (close_and_free(data));
	}
	if (data->pids[data->count] == 0)
	{
		close(data->read_end);
		if (!space_checker(argv[data->count + 2]))
			return (-1);
		if (get_cmd(argv[data->count + 2], data) == -1)
			return (-1);
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
		execve(data->path, data->cmd, envp);
		ft_printf(2, "Error\nCould not execute execve\n");
		return (close_and_free(data));
	}
	return (0);
}

int	last_child(t_pipex *data, char **argv, int argc)
{
	data->ends[0] = dup(data->read_end);
	data->ends[1] = open(argv[data->cmds + 2],
			O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (data->ends[1] < 0)
	{
		perror(argv[argc - 1]);
		data->error = true;
		return (close_and_free(data));
	}
	dup2(data->ends[0], STDIN_FILENO);
	dup2(data->ends[1], STDOUT_FILENO);
	return (0);
}

int	middle_child(t_pipex *data)
{
	if (pipe(data->ends) == -1)
	{
		ft_printf(2, "Error opening a pipe\n");
		return (close_and_free(data));
	}
	dup2(data->read_end, STDIN_FILENO);
	dup2(data->ends[1], STDOUT_FILENO);
	data->read_end = dup(data->ends[0]);
	data->ends[0] = dup(STDIN_FILENO);
	return (0);
}

int	first_child(t_pipex *data, char **argv)
{
	data->read_end = data->ends[0];
	data->ends[0] = open(argv[1], O_RDONLY);
	if (data->ends[0] < 0)
	{
		perror(argv[1]);
		data->error = true;
		// return (close_and_free(data));
	}
	dup2(data->ends[0], STDIN_FILENO);
	dup2(data->ends[1], STDOUT_FILENO);
	return (0);
}

int	get_fds(t_pipex *data, char **argv, int argc)
{
	if (data->count == 0)
		return (first_child(data, argv));
	if (data->count == data->cmds - 1)
		return (last_child(data, argv, argc));
	else
		return (middle_child(data));
}

void	add_slash(t_pipex *data)
{
	int		i;
	char	*old;
	char	*new;

	i = 0;
	while (data->paths[i])
	{
		old = data->paths[i];
		new = ft_strjoin(old, "/");
		if (new == NULL)
		{
			close_and_free(data);
			return ;
		}
		free(old);
		data->paths[i] = new;
		i++;
	}
}

char	**get_paths(char **envp, t_pipex *data)
{
	int		i;
	char	*envpaths;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH", 4) == 0)
		{
			envpaths = ft_substr(envp[i], 5, ft_strlen(envp[i]) - 5);
			if (!envpaths)
				return (NULL);
			data->paths = ft_split(envpaths, ':');
			free(envpaths);
			if (data->paths)
				add_slash(data);
			return (data->paths);
		}
		i++;
	}
	return (NULL);
}

int	init_data(t_pipex *data, int argc, char **envp)
{
	data->cmds = argc - 3;
	data->count = 0;
	if (pipe(data->ends) == -1)
		return (-1);
	data->read_end = 0;
	if (!get_paths(envp, data))
		return (close_and_free(data));
	data->cmd = NULL;
	data->path = NULL;
	data->pids = malloc(data->cmds * sizeof(int));
	if (!data->pids)
		return (close_and_free(data));
	data->error = false;
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	data;

	if (argc < 5)
	{
		ft_printf(2, "Error\nToo few arguments\n");
		exit(EXIT_FAILURE);
	}
	if (init_data(&data, argc, envp) == -1)
		exit(EXIT_FAILURE);
	while (data.count < data.cmds)
	{
		data.error = false;
		if (get_fds(&data, argv, argc) == -1)
			exit(EXIT_FAILURE);
		if (data.error == false)
		{
			if (do_cmd(&data, argv, envp) == -1)
				exit(EXIT_FAILURE);
		}
		close(data.ends[0]);
		close(data.ends[1]);
		data.count++;
	}
	wait_children(data.pids, data.cmds);
}
