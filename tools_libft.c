/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools_libft.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvera-f <alvera-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 14:04:59 by alvera-f          #+#    #+#             */
/*   Updated: 2025/01/21 14:31:14 by alvera-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	ft_strlen(const char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		i++;
	}
	return (i);
}

void	ft_bzero(void *s, size_t n)
{
	unsigned char	*ptr;
	size_t			i;

	ptr = (unsigned char *)s;
	i = 0;
	while (i < n)
	{
		ptr[i] = 0;
		i++;
	}
}

char	**split_command(char *cmd)
{
	char	**args;

	args = ft_split(cmd, ' ');
	if (!args || !args[0])
	{
		handle_error("Error: invalid command\n");
	}
	return (args);
}

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	r;

	i = 0;
	r = 0;
	while (src[r] != '\0')
	{
		r++;
	}
	if (size == 0)
		return (r);
	if (size > 0)
	{
		while (i < size - 1 && src[i] != '\0')
		{
			dst[i] = src[i];
			i++;
		}
		dst[i] = '\0';
	}
	return (r);
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*ptr;

	ptr = malloc(nmemb * size);
	if (ptr != NULL)
	{
		ft_bzero(ptr, nmemb * size);
		return (ptr);
	}
	return (NULL);
}
