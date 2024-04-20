/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakela <amakela@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 17:47:58 by amakela           #+#    #+#             */
/*   Updated: 2024/04/20 17:47:59 by amakela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static int	last_child(t_pipex *data, char **argv, int argc)
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
	return (0);
}

static int	middle_child(t_pipex *data)
{
	int	tmp;

	if (pipe(data->ends) == -1)
	{
		ft_printf(2, "Error opening a pipe\n");
		return (-1);
	}
	tmp = dup(data->read_end);
	data->read_end = dup2(data->ends[0], data->read_end);
	data->ends[0] = dup2(tmp, data->ends[0]);
	close(tmp);
	return (0);
}

static int	first_child(t_pipex *data, char **argv)
{
	data->read_end = dup(data->ends[0]);
	close(data->ends[0]);
	data->ends[0] = open(argv[1], O_RDONLY);
	if (data->ends[0] < 0)
	{
		ft_printf(2, "permission denied: %s\n", argv[1]);
		data->error = true;
	}
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
