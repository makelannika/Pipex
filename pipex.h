/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakela <amakela@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/24 16:28:34 by amakela           #+#    #+#             */
/*   Updated: 2024/03/24 16:29:12 by amakela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <stdbool.h>
# include "libft/libft.h"

typedef struct s_pipex
{
	int		cmds;
	int		count;
	int		ends[2];
	int		read_end;
	char	**paths;
	char	*new_arg;
	char	**cmd;
	char	*path;
	int		*pids;
	bool	error;
}	t_pipex;

int		init_data(t_pipex *data, int argc, char **envp);
int		get_fds(t_pipex *data, char **argv, int argc);
int		forking(t_pipex *data, char **argv, char **envp);
char	*parse_arg(t_pipex *data, char *arg);
int		wait_children(int *pids, int count);
int		close_and_free(t_pipex *data);

#endif
