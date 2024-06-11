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
	status = 0;
	exitcode = 0;
	while (i < count)
	{
		waitpid(pids[i], &status, 0);
		if (WIFEXITED(status) && i == count - 1)
			exitcode = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exitcode = WTERMSIG(status);
		i++;
	}
	exit(exitcode);
}

static void	free_str_array(char **array)
{
	int	i;

	i = 0;
	while (array[i])
		free(array[i++]);
}

int	close_and_free(t_pipex *data)
{
	int	i;

	i = 0;
	if (data->ends[0] != -1)
		close(data->ends[0]);
	if (data->ends[1])
		close(data->ends[1] != -1);
	if (data->read_end != -1)
		close(data->read_end);
	if (data->paths)
		free_str_array(data->paths);
	if (data->new_arg)
		free(data->new_arg);
	if (data->cmd)
		free_str_array(data->cmd);
	if (data->path)
		free(data->path);
	if (data->pids)
		free(data->pids);
	return (-1);
}
