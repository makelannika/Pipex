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

char	**get_paths(char **envp, t_pipex *data);
void	add_slash(t_pipex *data);
int		get_cmd(char *arg, t_pipex *data);
int		find_path(t_pipex *data);
int		get_fds(t_pipex *data, int i, char **argv);
void	do_cmd(t_pipex *data, char **envp, int i);
void	first_child(t_pipex *data, char **argv);
void	middle_child(t_pipex *data);
void	last_child(t_pipex *data, char **argv);

int	main(int argc, char **argv, char **envp)
{
	t_pipex	data;
	int		i;

	if (argc < 5)
		return (-1);
	i = 0;
	data.cmds = argc - 3;
	data.pids = malloc(data.cmds * sizeof(int));
	if (!data.pids)
		return (-1);
	if (!get_paths(envp, &data)) // free pids
		return (-1);
	while (i < data.cmds)
	{
		get_fds(&data, i, argv);
		if (!get_cmd(argv[i + 2], &data))
			return (-1); // error close & free
		if (!find_path(&data))
			return (-1); // error close & free
		do_cmd(&data, envp, i);
		close(data.ends[0]);
		close(data.ends[1]);
		i++;
	}
	return (0);
}

void	do_cmd(t_pipex *data, char **envp, int i)
{
	data->pids[i] = fork();
	if (data->pids[i] < 0)
	{
		// error
		return ;
	}
	if (data->pids[i] == 0)
	{
		close(data->read_end);
		if (execve(data->path, data->cmd, envp) == -1)
		{
			perror("Could not execute execve\n");
			return ;
		}
	}
	else
		wait(NULL); // waitpid
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

int	get_cmd(char *arg, t_pipex *data)
{
	data->cmd = ft_split(arg, 32);
	if (!data->cmd)
	{
		// free paths & pids
		return (-1);
	}
	return (1);
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

int	find_path(t_pipex *data)
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

int	get_fds(t_pipex *data, int i, char **argv)
{
	if (pipe(data->ends))
		return (-1); // free pids & paths
	if (i == 0)
		first_child(data, argv);
	else if (i == data->cmds - 1)
		last_child(data, argv);
	else
		middle_child(data);
	return (0);
}

void	first_child(t_pipex *data, char **argv)
{
	data->read_end = data->ends[0];
	data->ends[0] = open(argv[1], O_RDONLY);
	if (data->ends[0] < 0)
	{
		perror("Error opening a file\n");
		return ; // free pids & paths
	}
	dup2(data->ends[0], STDIN_FILENO);
	dup2(data->ends[1], STDOUT_FILENO);
}

void middle_child(t_pipex *data)
{
	dup2(data->read_end, STDIN_FILENO);
	dup2(data->ends[1], STDOUT_FILENO);
	data->read_end = data->ends[0];
}

void	last_child(t_pipex *data, char **argv)
{
	close(data->ends[0]);
	close(data->ends[1]);
	data->ends[0] = data->read_end;
	data->ends[1] = open(argv[data->cmds + 2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (data->ends[1] < 0)
	{
		perror("Error opening a file\n");
		return ; // free pids, paths, cmd & path
	}
	dup2(data->ends[0], STDIN_FILENO);
	dup2(data->ends[1], STDOUT_FILENO);
}
