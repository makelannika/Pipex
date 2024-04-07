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

void	free_str_arr(char **array)
{
	int	i;

	i = 0;
	while(array[i])
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

void	do_cmd(t_pipex *data, char **envp)
{
	data->pids[data->count] = fork();
	if (data->pids[data->count] < 0)
	{
		// error, close fds, free paths, pids, cmd & path
		return ;
	}
	if (data->pids[data->count] == 0)
	{
		close(data->read_end);
		if (execve(data->path, data->cmd, envp) == -1)
		{
			perror("Could not execute execve\n");
			return ;
		}
	}
	else
		wait(NULL); // waitpid loop
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
			return (-1); // error, close fds, free paths, pids & cmd
		if (access(temp, F_OK) == 0)
		{
			data->path = temp;
			return (1);
		}
		free(temp);
		i++;
	}
	return (-1); // error, close fds, free paths, pids & cmd
}

int	get_cmd(char *arg, t_pipex *data)
{
	data->cmd = ft_split(arg, 32);
	if (!data->cmd)
	{
		// error, close fds, free paths & pids
		return (-1);
	}
	return (1);
}

void	last_child(t_pipex *data, char **argv)
{
	data->ends[0] = data->read_end;
	data->ends[1] = open(argv[data->cmds + 2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (data->ends[1] < 0)
	{
		perror("Error opening the output file\n");
		return ; // free pids, paths, cmd & path
	}
	dup2(data->ends[0], STDIN_FILENO);
	dup2(data->ends[1], STDOUT_FILENO);
}

int	middle_child(t_pipex *data)
{
	if (pipe(data->ends) == -1)
		return (-1);
	dup2(data->read_end, STDIN_FILENO);
	dup2(data->ends[1], STDOUT_FILENO);
	data->read_end = data->ends[0];
	return (1);
}

void	first_child(t_pipex *data, char **argv)
{
	data->read_end = data->ends[0];
	data->ends[0] = open(argv[1], O_RDONLY);
	if (data->ends[0] < 0)
	{
		perror("Error opening the input file\n");
		return ; // free pids & paths
	}
	dup2(data->ends[0], STDIN_FILENO);
	dup2(data->ends[1], STDOUT_FILENO);
}

int	get_fds(t_pipex *data, char **argv)
{
	if (data->count == 0)
		first_child(data, argv);
	else if (data->count == data->cmds - 1)
		last_child(data, argv);
	else
		middle_child(data);
	return (0);
}

void	add_slash(t_pipex *data)
{
	int		i = 0;
	char	*temp;

	while (data->paths[i])
	{
		temp = data->paths[i];
		data->paths[i] = ft_strjoin(temp, "/");
		free(temp);
		if (data->paths[i] == NULL)
		{
			// free paths
			return ;
		}
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
		return (close_and_free(data));
	data->read_end = 0;
	if (!get_paths(envp, data))
		return (close_and_free(data));
	data->cmd = NULL;
	data->path = NULL;
	data->pids = malloc(data->cmds * sizeof(int));
	if (!data->pids)
		return (close_and_free(data));
	return (1);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	data;

	if (argc < 5)
	{
		ft_printf(2, "Error\nToo few arguments\n");
		exit(EXIT_FAILURE);
	}
	if (!init_data(&data, argc, envp))
		exit(EXIT_FAILURE);
	while (data.count < data.cmds)
	{
		get_fds(&data, argv);
		if (!get_cmd(argv[data.count + 2], &data))
			exit(EXIT_FAILURE);
		if (!find_path(&data))
			exit(EXIT_FAILURE);
		do_cmd(&data, envp);
		close(data.ends[0]);
		close(data.ends[1]);
		data.count++;
	}
	exit(EXIT_SUCCESS);
}
