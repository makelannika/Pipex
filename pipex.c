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

char	**get_paths(char **envp, t_data *data);
char	**add_slash(char **paths);
int		open_files(t_data *data, char **argv, int argc);
int		get_cmd(char *arg, t_data *data);
int		find_path(t_data *data);
int		child_process(t_data *data, int i, char **argv, char **envp);

int	main(int argc, char **argv, char **envp)
{
	t_data	data;
	int		pid;
	int		i;

	i = 0;
	data.pipes = argc - 4;
	if (data.pipes < 1)
	{
		perror("Error\nInvalid amount of arguments\n");
		return (-1);
	}
	if (!get_paths(envp, &data) || !open_files(&data, argv, argc))
		return (-1);
	pipe(data.ends);
	while (i <= data.pipes)
	{
		pid = fork();
		if (pid < 0)
		{
			perror("Error\nFork failed\n");
			return (-1);
		}
		if (pid > 0) // parent process
		{
			close(data.ends[1]);
			wait(NULL);
		}
		if (pid == 0) // child process
			child_process(&data, i, argv, envp);
		i++;
	}
	return (0);
}

char	**get_paths(char **envp, t_data *data)
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
					data->paths = add_slash(data->paths);
				return (data->paths);
			}
			i++;
		}
		return (NULL);
}

int	open_files(t_data *data, char **argv, int argc)
{
	data->infile = open(argv[1], O_RDONLY);
	data->outfile = open(argv[argc - 1], O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (data->infile < 0 || data->outfile < 0)
	{
		perror("Error opening a file\n");
		return (-1);
	}
	return (1);
}

int	get_cmd(char *arg, t_data *data)
{
	data->cmd = ft_split(arg, 32);
	if (!data->cmd)
		return (-1);
	return (1);
}

char **add_slash(char **paths)
{
	int		i = 0;
	char	*temp;

	while (paths[i])
	{
		temp = paths[i];
		paths[i] = ft_strjoin(temp, "/");
		free(temp);
		if (paths[i] == NULL)
		{
			// free array
			return (NULL);
		}
		i++;
	}
	return (paths);
}

int	find_path(t_data *data)
{
	int		i;
	char	*temp;

	i = 0;
	while (data->paths[i])
	{
		temp = ft_strjoin(data->paths[i], data->cmd[0]);
		if (access(temp, F_OK) == 0)
		{
			data->path = temp;
			return (1);
		}
		free(temp);
		i++;
	}
	return (-1);
}

int	child_process(t_data *data, int i, char **argv, char **envp)
{
	if (i == 0) // first child
	{
		close(data->ends[0]);
		close(data->outfile);
		dup2(data->infile, 0);
		dup2(data->ends[1], 1);
	}
	else
	{
		close(data->ends[1]);
		close(data->infile);
		dup2(data->ends[0], 0);
		dup2(data->outfile, 1);
	}
	if (!get_cmd(argv[i + 2], data))
		return (-1); // error close & free
	if (!find_path(data))
		return (-1); // error close & free
	if (execve(data->path, data->cmd, envp) == -1)
	{
		perror("Could not execute execve\n");
		// return (-1);
	}
	return (0);
}
