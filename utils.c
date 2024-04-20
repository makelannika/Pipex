/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakela <amakela@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 17:47:36 by amakela           #+#    #+#             */
/*   Updated: 2024/04/20 17:47:39 by amakela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

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

int	close_and_free(t_pipex *data)
{
	int	i;

	i = 0;
	close(data->ends[0]);
	close(data->ends[1]);
	close(data->read_end);
	if (data->paths)
	{
		while (data->paths[i])
			free(data->paths[i++]);
	}
	if (data->new_arg)
		free(data->new_arg);
	if (data->cmd)
	{
		i = 0;
		while (data->cmd[i])
			free(data->cmd[i++]);
	}
	if (data->path)
		free(data->path);
	if (data->pids)
		free(data->pids);
	return (-1);
}
