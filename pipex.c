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

char	**get_paths(char **envp, char ***paths);
char	**add_slash(char **paths);
char	**get_cmd(char *arg);

int	main(int argc, char **argv, char **envp)
{
	int		ends[2];
	int		pid;
	char	**paths;
	char	**cmd1;
	char	**cmd2;

	if (argc != 5)
	{
		perror("Error\nInvalid amount of arguments\n");
		// return (-1);
	}
	if (!get_paths(envp, &paths))
		return (-1);
	pipe(ends);
	pid = fork();
	if (pid < 0)
	{
		perror("Error\nFork failed\n");
		return (-1);
	}
	if (pid == 0) // first child process / cmd1
	{
		ft_printf("In child process\n");
		close(ends[0]);
		int	infile = open(argv[1], O_RDONLY);
		if (infile < 0)
			return (-1);
		dup2(infile, 0);
		dup2(ends[1], 1);
		cmd1 = get_cmd(argv[2]);
		int i = 0;
		while (paths[i])
		{
			ft_printf("%s\n", paths[i]);
			if (execve(paths[i], cmd1, envp) == -1)
			{
				perror("Could not execute execve\n");
				// return (-1);
			}
			i++;
		}
		close(infile);
	}
	else // parent process / cmd2
	{
		wait(NULL);
		ft_printf("\n");
		close(ends[1]);
		ft_printf("In parent process\n");
		int outfile = open(argv[4], O_RDWR);
		if (outfile < 0)
		//	return (-1);
		dup2(ends[0], 0);
		dup2(outfile, 1);
		cmd2 = get_cmd(argv[3]);
		int i = 0;
		while (paths[i])
		{
			if (execve(paths[i], cmd2, envp) == -1)
			{
				perror("Could not execute execve\n");
				// return (-1);
			}
			i++;
		}
		close(outfile);
	}
}

char	**get_paths(char **envp, char ***paths)
{
	int		i;
	char	*envpath;

	i = 0;
	while (envp[i])
		{
			if (ft_strncmp(envp[i], "PATH", 4) == 0)
			{
				envpath = ft_substr(envp[i], 5, ft_strlen(envp[i]) - 5);
				if (!envpath)
					return (NULL);
				*paths = ft_split(envpath, ':');
				free(envpath);
				if (*paths)
					*paths = add_slash(*paths);
				return (*paths);
			}
			i++;
		}
		return (NULL);
}
char	**get_cmd(char *arg)
{
	char	**cmd;

	cmd = ft_split(arg, 32);
	return (cmd);
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
