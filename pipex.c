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

char	*quote_remover(char *arg, char *new_arg)
{
	int	i;
	int	j;
	int	count;

	i = 0;
	j = 0;
	count = 0;
	while (arg[i++])
	{
		if (arg[i] == 39)
			count++;
	}
	new_arg = malloc(sizeof(char) * (ft_strlen(arg) - count));
	if (!new_arg)
		return (NULL);
	i = 0;
	while (arg[i])
	{
		if (arg[i] == 39)
			i++;
		new_arg[j++] = arg[i++];
	}
	new_arg[i] = '\0';
	return (new_arg);
}

char	*space_handler(char *arg)
{
	int		i;
	int		in_quotes;
	bool	in_brackets;

	i = 0;
	in_quotes = -1;
	in_brackets = false;
	while (arg[i])
	{
		if (arg[i] == 39)
			in_quotes *= -1;
		else if (arg[i] == '{')
			in_brackets = true;
		else if (arg[i] == '}')
			in_brackets = false;
		if (arg[i] == ' ' && (in_quotes == -1 && in_brackets == false))
			arg[i] = 7;
		else
			i++;
	}
	return (arg);
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
	if (!data->paths)
		return (0);
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
	ft_printf(2, "command not found: %s\n", data->cmd[0]);
	exit(127);
}

int	path_check(t_pipex *data)
{
	if (!data->paths)
		return (0);
	if (access(data->cmd[0], F_OK) == 0)
	{
		data->path = data->cmd[0];
		return (0);
	}
	ft_printf(2, "No such file or directory: %s\n", data->cmd[0]);
	return (-1);
}

int	get_cmd(char *arg, t_pipex *data)
{
	data->cmd = ft_split(arg, 7);
	if (!data->cmd)
	{
		ft_printf(2, "Error\nSplit failed when getting a command\n");
		return (close_and_free(data));
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

char	*parse_arg(char *arg, char *new_arg)
{
	if (!arg[0] || arg[0] == '.')
	{
		ft_printf(2, "permission denied: %s\n", arg);
		return (NULL);
	}
	space_handler(arg);
	new_arg = quote_remover(arg, new_arg);
	if (!new_arg)
		return (NULL);
	return (new_arg);
}

int	do_cmd(t_pipex *data, char **argv, char **envp)
{
	char	*new_arg;

	new_arg = NULL;
	data->pids[data->count] = fork();
	if (data->pids[data->count] < 0)
	{
		ft_printf(2, "Error\nFork failed\n");
		return (-1);
	}
	if (data->pids[data->count] == 0)
	{
		close(data->read_end);
		new_arg = parse_arg(argv[data->count + 2], new_arg);
		if (!new_arg)
			return (-1);
		if (get_cmd(new_arg, data) == -1)
			return (-1);
		if (!data->path)
		{
			ft_printf(2, "command not found: %s\n", data->cmd[0]);
			return (-1);
		}
		execve(data->path, data->cmd, envp);
		ft_printf(2, "Error\nCould not execute execve\n");
		return (-1);
	}
	return (0);
}

int	last_child(t_pipex *data, char **argv, int argc)
{
	data->ends[0] = dup(data->read_end);
	close(data->read_end);
	data->ends[1] = open(argv[data->cmds + 2],
			O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (data->ends[1] < 0)
	{
		ft_printf(2, "permission denied: %s\n", argv[argc - 1]);
		data->error = true;
		return (-1);
	}
	dup2(data->ends[0], STDIN_FILENO);
	close(data->ends[0]);
	dup2(data->ends[1], STDOUT_FILENO);
	close(data->ends[1]);
	return (0);
}

int	middle_child(t_pipex *data)
{
	if (pipe(data->ends) == -1)
	{
		ft_printf(2, "Error opening a pipe\n");
		return (-1);
	}
	dup2(data->read_end, STDIN_FILENO);
	close(data->read_end);
	dup2(data->ends[1], STDOUT_FILENO);
	// close(data->ends[1]);
	data->read_end = dup(data->ends[0]);
	close(data->ends[0]);
	return (0);
}

int	first_child(t_pipex *data, char **argv)
{
	data->read_end = dup(data->ends[0]);
	close(data->ends[0]);
	data->ends[0] = open(argv[1], O_RDONLY);
	if (data->ends[0] < 0)
	{
		ft_printf(2, "permission denied: %s\n", argv[1]);
		data->error = true;
	}
	dup2(data->ends[0], STDIN_FILENO);
	close(data->ends[0]);
	dup2(data->ends[1], STDOUT_FILENO);
	close(data->ends[1]);
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
	data->paths = NULL;
	return (data->paths);
}

int	init_data(t_pipex *data, int argc, char **envp)
{
	data->cmds = argc - 3;
	data->count = 0;
	if (pipe(data->ends) == -1)
		return (-1);
	data->read_end = 0;
	data->paths = get_paths(envp, data);
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
			{
				close_and_free(&data);
				exit(EXIT_FAILURE);
			}
		}
		close(data.ends[0]);
		close(data.ends[1]);
		data.count++;
	}
	wait_children(data.pids, data.cmds);
}
