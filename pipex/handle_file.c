#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 1024 // Tamaño del buffer

void handle_error(const char *message);

// Prototipo de ft_split de tu libft
char **ft_split(char const *s, char c);

// Buscar variable PATH en envp
char *get_path_variable(char **envp)
{
    size_t i = 0;

    while (envp[i])
    {
        if (strncmp(envp[i], "PATH=", 5) == 0)
        {
            return envp[i] + 5;
        }
        i++;
    }
    return NULL;
}

// Construir la ruta completa del comando
char *construct_path(char *path, char *cmd)
{
    char *full_path = malloc(strlen(path) + strlen(cmd) + 2);
    if (!full_path)
        handle_error("Memory allocation error");

    sprintf(full_path, "%s/%s", path, cmd);
    return full_path;
}

// Validar y encontrar la ruta ejecutable
char *validate_path(char **paths, char *cmd)
{
    size_t i = 0;
    char *full_path;

    // Recorrer las posibles rutas
    while (paths[i])
    {
        full_path = construct_path(paths[i], cmd); // Construir ruta completa
        if (access(full_path, X_OK) == 0) // Verificar si es ejecutable
        {
            free_paths(paths); // Liberar memoria de paths
            return full_path;  // Retornar ruta válida
        }
        free(full_path); // Liberar la ruta si no es válida
        i++;
    }

    // Liberar memoria de paths y retornar NULL si no se encontró una ruta válida
    free_paths(paths);
    return NULL;
}



// Buscar ejecutable en el PATH
char *find_executable(char *cmd, char **envp)
{
    char *path_var = get_path_variable(envp);
    char **paths;

    if (!path_var)
        return NULL;

    paths = ft_split(path_var, ':');
    if (!paths)
        return NULL;

    return validate_path(paths, cmd); // Validar y devolver la ruta ejecutable
}

void redirect_fds(int input_fd, int output_fd)
{
    if (input_fd != STDIN_FILENO)
    {
        if (dup2(input_fd, STDIN_FILENO) < 0)
            handle_error("Error redirecting input");
    }

    if (output_fd != STDOUT_FILENO)
    {
        if (dup2(output_fd, STDOUT_FILENO) < 0)
            handle_error("Error redirecting output");
    }
}

char **split_command(char *cmd)
{
    char **args = ft_split(cmd, ' ');

    if (!args || !args[0])
    {
        fprintf(stderr, "Error: invalid command\n");
        exit(EXIT_FAILURE);
    }

    return args;
}

// Ejecutar el comando
void execute_child_command(char *cmd, char **envp)
{
    char **args = split_command(cmd);
    char *path = find_executable(args[0], envp);

    if (!path)
    {
        fprintf(stderr, "Command not found: %s\n", args[0]);
        size_t i = 0;
        while (args[i])
        {
            free(args[i]);
            i++;
        }
        free(args);
        exit(EXIT_FAILURE);
    }

    execve(path, args, envp);

    perror("execve");
    free(path);
    size_t i = 0;
    while (args[i])
    {
        free(args[i]);
        i++;
    }
    free(args);
    exit(EXIT_FAILURE);
}

int open_infile(char *infile)
{
    int fd = open(infile, O_RDONLY);
    if (fd < 0)
        handle_error("Error opening infile");
    return fd;
}

int open_outfile(char *outfile)
{
    int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        handle_error("Error opening outfile");
    return fd;
}

void execute_first_command(char *cmd, char *infile, int pipe_fd[], char **envp)
{
    int fd_infile = open_infile(infile);

    if (fork() == 0)
    {
        close(pipe_fd[0]);                 // Cerrar extremo de lectura del pipe
        redirect_fds(fd_infile, pipe_fd[1]); // Redirigir entrada y salida
        execute_child_command(cmd, envp);   // Ejecutar el comando
    }

    close(pipe_fd[1]); // Cerrar el extremo de escritura del pipe en el padre
    close(fd_infile);
}

void execute_second_command(char *cmd, char *outfile, int pipe_fd[], char **envp)
{
    int fd_outfile = open_outfile(outfile);

    if (fork() == 0)
    {
        close(pipe_fd[1]);                 // Cerrar extremo de escritura del pipe
        redirect_fds(pipe_fd[0], fd_outfile); // Redirigir entrada y salida
        execute_child_command(cmd, envp);   // Ejecutar el comando
    }

    close(pipe_fd[0]); // Cerrar el extremo de lectura del pipe en el padre
    close(fd_outfile);
}

void execute_pipeline(char *cmd1, char *cmd2, char *infile, char *outfile, char **envp)
{
    int pipe_fd[2];

    // Crear una tubería
    if (pipe(pipe_fd) == -1)
        handle_error("Error creating pipe");

    // Ejecutar el primer comando
    execute_first_command(cmd1, infile, pipe_fd, envp);

    // Ejecutar el segundo comando
    execute_second_command(cmd2, outfile, pipe_fd, envp);

    // Esperar a ambos hijos
    wait(NULL);
    wait(NULL);
}

int main(int argc, char **argv, char **envp)
{
    if (argc != 5)
    {
        const char *msg = "Usage: <program> infile cmd1 cmd2 outfile\n";
        write(2, msg, strlen(msg));  // 2 es el descriptor de stderr
        return EXIT_FAILURE;
    }

    // Ejecutar la tubería entre los dos comandos
    execute_pipeline(argv[2], argv[3], argv[1], argv[4], envp);

    return EXIT_SUCCESS;
}
