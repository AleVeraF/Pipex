/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvera-f <alvera-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 16:33:23 by alvera-f          #+#    #+#             */
/*   Updated: 2025/01/21 18:22:08 by alvera-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*find_executable(char *cmd, char **envp)
{
	char	*path_var;
	char	**paths;
	char	*full_path;

	if (!cmd || !*cmd)
		handle_error("Invalid command");
	path_var = get_path_variable(envp);
	paths = ft_split(path_var, ':');
	if (!paths)
		handle_error("Error splitting PATH variable");
	full_path = validate_path(paths, cmd);
	if (!full_path)
	{
		write(2, "Command not found: ", 19);
		write(2, cmd, ft_strlen(cmd));
		write(2, "\n", 1);
	}
	return (full_path);
}

void	redirect_fds(int input_fd, int output_fd)
{
	if (input_fd >= 0)
	{
		if (dup2(input_fd, STDIN_FILENO) < 0)
		{
			handle_error("Error redirecting input");
		}
		close(input_fd);
	}
	if (output_fd >= 0)
	{
		if (dup2(output_fd, STDOUT_FILENO) < 0)
		{
			handle_error("Error redirecting output");
		}
		close(output_fd);
	}
}

void	free_arguments(char **args)
{
	size_t	i;

	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	execute_child_command(char *cmd, char **envp)
{
	char	**args;
	char	*path;

	args = split_command(cmd);
	if (!args || !args[0])
		handle_error("Invalid command");
	path = find_executable(args[0], envp);
	if (!path)
	{
		free_arguments(args);
		_exit(127);
	}
	execve(path, args, envp);
	perror("execve failed");
	free(path);
	free_arguments(args);
	_exit(EXIT_FAILURE);
}

int	open_infile(char *infile)
{
	int	fd;

	fd = open(infile, O_RDONLY);
	if (fd < 0)
	{
		handle_error("Error opening infile");
	}
	return (fd);
}
