/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvera-f <alvera-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 14:04:32 by alvera-f          #+#    #+#             */
/*   Updated: 2025/01/21 14:30:30 by alvera-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	open_outfile(char *outfile)
{
	int	fd;

	fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		handle_error("Error opening outfile");
	}
	return (fd);
}

void	execute_first_command(char *cmd, char *infile,
										int pipe_fd[], char **envp)
{
	int	fd_infile;

	fd_infile = open_infile(infile);
	if (fork() == 0)
	{
		close(pipe_fd[0]);
		redirect_fds(fd_infile, pipe_fd[1]);
		execute_child_command(cmd, envp);
	}
	close(fd_infile);
	close(pipe_fd[1]);
}

void	execute_second_command(char *cmd, char *outfile,
										int pipe_fd[], char **envp)
{
	int	fd_outfile;

	close(pipe_fd[1]);
	fd_outfile = open_outfile(outfile);
	if (fork() == 0)
	{
		redirect_fds(pipe_fd[0], fd_outfile);
		execute_child_command(cmd, envp);
	}
	close(pipe_fd[0]);
	close(fd_outfile);
}

void	execute_pipeline(t_pipeline *pipeline)
{
	int	pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		handle_error("Error creating pipe");
	execute_first_command(pipeline->cmd1, pipeline->infile,
		pipe_fd, pipeline->envp);
	execute_second_command(pipeline->cmd2, pipeline->outfile,
		pipe_fd, pipeline->envp);
	wait(NULL);
	wait(NULL);
}

int	main(int argc, char **argv, char **envp)
{
	const char	*msg;
	t_pipeline	pipeline;

	if (argc != 5)
	{
		msg = "Usage: <program> infile cmd1 cmd2 outfile\n";
		write(2, msg, ft_strlen(msg));
		return (EXIT_FAILURE);
	}
	pipeline.infile = argv[1];
	pipeline.cmd1 = argv[2];
	pipeline.cmd2 = argv[3];
	pipeline.outfile = argv[4];
	pipeline.envp = envp;
	execute_pipeline(&pipeline);
	return (EXIT_SUCCESS);
}
