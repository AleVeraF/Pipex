/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvera-f <alvera-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 14:04:22 by alvera-f          #+#    #+#             */
/*   Updated: 2025/01/21 18:21:33 by alvera-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	handle_error(const char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}

char	*get_path_variable(char **envp)
{
	size_t	i;

	i = 0;
	while (envp[i])
	{
		if (strncmp(envp[i], "PATH=", 5) == 0)
		{
			return (envp[i] + 5);
		}
		i++;
	}
	handle_error("PATH variable not found");
	return (NULL);
}

void	free_paths(char **paths)
{
	size_t	i;

	i = 0;
	if (paths)
	{
		while (paths[i])
		{
			free (paths[i]);
			i++;
		}
		free(paths);
	}
}

char	*construct_path(char *path, char *cmd)
{
	char	*full_path;
	size_t	len_path;
	size_t	len_cmd;
	size_t	total_len;

	len_path = ft_strlen(path);
	len_cmd = ft_strlen(cmd);
	total_len = len_path + len_cmd + 2;
	full_path = malloc(total_len);
	if (!full_path)
		handle_error("Memory allocation error");
	ft_strlcpy(full_path, path, len_path + 1);
	if (path[len_path - 1] != '/')
		ft_strlcpy(full_path + len_path, "/", 2);
	ft_strlcpy(full_path + len_path + (path[len_path - 1] != '/'),
		cmd, len_cmd + 1);
	return (full_path);
}

char	*validate_path(char **paths, char *cmd)
{
	size_t	i;
	char	*full_path;

	i = 0;
	while (paths[i])
	{
		full_path = construct_path(paths[i], cmd);
		if (access(full_path, X_OK) == 0)
		{
			free_paths(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_paths(paths);
	return (NULL);
}
