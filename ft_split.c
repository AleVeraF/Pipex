/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvera-f <alvera-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 14:04:00 by alvera-f          #+#    #+#             */
/*   Updated: 2025/01/20 14:04:07 by alvera-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "pipex.h"

int	ft_counter(const char *str, char delimiter)
{
	int	count;
	int	i;
	int	is_token;

	is_token = 0;
	count = 0;
	i = 0;
	if (str == NULL || *str == '\0')
		return (0);
	while (str[i] != '\0')
	{
		if (str[i] != delimiter && is_token == 0)
		{
			count++;
			is_token = 1;
		}
		else if (str[i] == delimiter)
			is_token = 0;
		i++;
	}
	return (count);
}

char	**free_matrix(int position, char **matrix)
{
	while (position >= 0)
	{
		free(matrix[position--]);
	}
	free(matrix);
	return (NULL);
}

char	*allocate_and_copy(const char *str, int start, size_t len)
{
	char	*substring;

	substring = (char *)malloc((len + 1) * sizeof(char));
	if (!substring)
		return (0);
	ft_strlcpy(substring, &str[start], len + 1);
	return (substring);
}

char	**process_word(const char *s, char c, char **t)
{
	int	i;
	int	start;
	int	position;

	i = 0;
	position = 0;
	while (s[i] != '\0')
	{
		while (s[i] == c)
			i++;
		start = i;
		while (s[i] != c && s[i] != '\0')
		{
			i++;
			if (s[i] == c || s[i] == '\0')
			{
				t[position] = allocate_and_copy(s, start, i - start);
				if (!t[position])
					return (free_matrix(position, t));
				position++;
			}
		}
	}
	return (t);
}

char	**ft_split(char const *s, char c)
{
	size_t	words;
	char	**t;

	words = ft_counter(s, c);
	t = (char **)ft_calloc((words + 1), sizeof(char *));
	if (!t)
		return (NULL);
	return (process_word(s, c, t));
}
