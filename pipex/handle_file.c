#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024 // Tamaño del buffer

void handle_error(const char *message);

int open_infile(char *infile)
{
    int fd;

    // Intentar abrir el archivo en modo de solo lectura
    fd = open(infile, O_RDONLY);

    // Si ocurre un error al abrir el archivo
    if (fd < 0)
        handle_error("Error opening file");

    // Retornar el descriptor de archivo
    return fd;
}

ssize_t read_from_file(int fd, char *buffer, size_t size)
{
    ssize_t bytes_read;

    // Leer datos del archivo
    bytes_read = read(fd, buffer, size);

    // Verificar si ocurre un error
    if (bytes_read < 0)
        handle_error("Error reading from file");

    // Retornar la cantidad de bytes leídos
    return bytes_read;
}

void write_to_pipe(int pipe_fd, char *buffer, size_t size)
{
    ssize_t bytes_written;

    // Escribir datos en el pipe
    bytes_written = write(pipe_fd, buffer, size);

    // Verificar si ocurre un error
    if (bytes_written < 0)
        handle_error("Error writing to pipe");
}

void redirect_to_pipe(int fd_infile, int pipe_fd)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Leer del archivo y escribir en el pipe en un bucle
    while ((bytes_read = read_from_file(fd_infile, buffer, BUFFER_SIZE)) > 0)
    {
        write_to_pipe(pipe_fd, buffer, bytes_read);
    }

    // Cerrar el extremo de escritura del pipe
    close(pipe_fd);
}

void redirect_fds(int input_fd, int output_fd)
{
    if (input_fd != STDIN_FILENO)
    {
        if (dup2(input_fd, STDIN_FILENO) < 0)
            handle_error("Error redirecting input");
        close(input_fd);
    }

    if (output_fd != STDOUT_FILENO)
    {
        if (dup2(output_fd, STDOUT_FILENO) < 0)
            handle_error("Error redirecting output");
        close(output_fd);
    }
}

// Ejecutar un comando en el proceso hijo
void execute_child_command(char *cmd, char **envp)
{
    char *args[] = {"/bin/sh", "-c", cmd, NULL};

    // Ejecutar el comando usando execve
    if (execve(args[0], args, envp) == -1)
        handle_error("Error executing command");
}

// Función principal para ejecutar un comando
void execute_command(char *cmd, char **envp, int input_fd, int output_fd)
{
    pid_t pid = fork(); // Crear un proceso hijo

    if (pid < 0)
    {
        handle_error("Error creating child process");
    }

    if (pid == 0)
    {
        // Proceso hijo
        redirect_fds(input_fd, output_fd);        // Redirigir ficheros
        execute_child_command(cmd, envp);        // Ejecutar el comando
    }
    else
    {
        // Proceso padre espera al hijo
        if (waitpid(pid, NULL, 0) < 0)
        {
            handle_error("Error waiting for child process");
        }
    }
}

int main(int argc, char **argv, char **envp)
{
    int fd_infile;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s infile command\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Abrir el archivo de entrada
    fd_infile = open_infile(argv[1]);

    // Ejecutar el comando, redirigiendo la entrada desde el archivo
    execute_command(argv[2], envp, fd_infile, STDOUT_FILENO);

    // Cerrar el archivo de entrada
    close(fd_infile);

    return EXIT_SUCCESS;
}
