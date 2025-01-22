/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvera-f <alvera-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 14:04:50 by alvera-f          #+#    #+#             */
/*   Updated: 2025/01/22 14:48:37 by alvera-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10000000
# endif
# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/wait.h>
# include <string.h>

typedef struct s_pipeline
{
	char	*cmd1;
	char	*cmd2;
	char	*infile;
	char	*outfile;
	char	**envp;
}	t_pipeline;

size_t	ft_strlcpy(char *dst, const char *src, size_t size);
void	*ft_calloc(size_t nmemb, size_t size);
void	handle_error(const char *message);
char	**ft_split(char const *s, char c);
char	*get_path_variable(char **envp);
char	*construct_path(char *path, char *cmd);
char	*validate_path(char **paths, char *cmd);
char	*find_executable(char *cmd, char **envp);
void	redirect_fds(int input_fd, int output_fd);
char	**split_command(char *cmd);
void	execute_child_command(char *cmd, char **envp);
int		open_infile(char *infile);
int		open_outfile(char *outfile);
void	execute_first_command(char *cmd, char *infile,
			int pipe_fd[], char **envp);
void	execute_second_command(char *cmd, char *outfile,
			int pipe_fd[], char **envp);
void	execute_pipeline(t_pipeline *pipeline);
int		main(int argc, char **argv, char **envp);
void	handle_error(const char *message);
int		ft_counter(const char *str, char delimiter);
char	**free_matrix(int position, char **matrix);
char	*allocate_and_copy(const char *str, int start, size_t len);
char	**process_word(const char *s, char c, char **t);
char	**ft_split(char const *s, char c);
void	free_paths(char **paths);
int		ft_strlen(const char *str);
void	free_arguments(char **args);
int		ft_strncmp(const char *s1, const char *s2, size_t n);

#endif
