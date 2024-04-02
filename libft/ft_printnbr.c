/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printnbr.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakela <amakela@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 12:30:43 by amakela           #+#    #+#             */
/*   Updated: 2024/01/01 18:29:48 by amakela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_printnbr(long nbr, int *count)
{
	if (nbr < 0)
	{
		if (ft_printchar('-') == -1)
			return (-1);
		(*count)++;
		ft_printnbr(-nbr, count);
	}
	else if (nbr > 9)
	{
		if ((ft_printnbr(nbr / 10, count) == -1)
			|| ft_printnbr(nbr % 10, count) == -1)
			return (-1);
	}
	else
	{
		if (ft_printchar(nbr + 48) == -1)
			return (-1);
		(*count)++;
	}
	return (*count);
}
