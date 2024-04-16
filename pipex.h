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
	char	**cmd;
	char	*path;
	int		*pids;
	bool	error;
}	t_pipex;

#endif
