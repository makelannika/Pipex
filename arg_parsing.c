/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_parsing.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakela <amakela@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 17:48:43 by amakela           #+#    #+#             */
/*   Updated: 2024/04/20 17:48:45 by amakela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static char	*quote_remover(t_pipex *data, char *arg)
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
	data->new_arg = malloc(sizeof(char) * (ft_strlen(arg) - count));
	if (!data->new_arg)
		return (NULL);
	i = 0;
	while (arg[i])
	{
		if (arg[i] == 39)
			i++;
		data->new_arg[j++] = arg[i++];
	}
	data->new_arg[i] = '\0';
	return (data->new_arg);
}

static char	*space_handler(char *arg)
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

char	*parse_arg(t_pipex *data, char *arg)
{
	int	i;
	int	is_empty;

	i = 0;
	is_empty = 1;
	if (!arg[0])
	{
		ft_printf(2, "permission denied: %s\n", arg);
		return (NULL);
	}
	space_handler(arg);
	while (arg[i])
	{
		if (ft_isprint(arg[i++]))
			is_empty = 0;
	}
	if (is_empty)
		ft_printf(2, "command not found: %s\n", arg);
	data->new_arg = quote_remover(data, arg);
	if (!data->new_arg)
		return (NULL);
	return (data->new_arg);
}
