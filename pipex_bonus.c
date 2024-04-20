/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakela <amakela@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 18:27:50 by amakela           #+#    #+#             */
/*   Updated: 2024/04/20 18:27:52 by amakela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	arg_check(int argc)
{
	if (argc < 5)
	{
		ft_printf(2, "Error\nToo few arguments\n");
		return (-1);
	}
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	data;

	if (arg_check(argc) == -1)
		exit(EXIT_FAILURE);
	if (init_data(&data, argc, envp) == -1)
		exit(EXIT_FAILURE);
	while (data.count < data.cmds)
	{
		data.error = false;
		if (get_fds(&data, argv, argc) == -1)
			exit(EXIT_FAILURE);
		if (data.error == false)
		{
			if (forking(&data, argv, envp) == -1)
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
