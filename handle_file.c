#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024 // Tamaño del buffer

void handle_error(const char *message);

int open_infile(char *infile)
{
    int fd;

    // Intentar abrir el archivo en modo de solo lectura
    fd = open(infile, O_RDONLY);

    // Si ocurre un error al abrir el archivo
    if (fd < 0)
    {
        perror("Error opening file");  // Esto imprimirá el error específico
        handle_error("Error opening file");
    }

    // Retornar el descriptor de archivo
    //printf("Archivo de entrada abierto: %s\n", infile);  // Verificación de archivo abierto
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
        // No cerrar input_fd aquí, ya que dup2 lo toma como nuevo STDIN
    }

    if (output_fd != STDOUT_FILENO)
    {
        if (dup2(output_fd, STDOUT_FILENO) < 0)
            handle_error("Error redirecting output");
        // No cerrar output_fd aquí, ya que dup2 lo toma como nuevo STDOUT
    }
}

// Ejecutar un comando en el proceso hijo
void execute_child_command(char *cmd, char **envp)
{
    char *args[] = {"/bin/sh", "-c", cmd, NULL};

    execve(args[0], args, envp);

    // Si execve falla, imprime un mensaje y finaliza
    perror(cmd);
    exit(EXIT_FAILURE);
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
        // Proceso hijo para el primer comando
        close(pipe_fd[0]);                 // Cerrar extremo de lectura del pipe
        redirect_fds(fd_infile, pipe_fd[1]); // Redirigir entrada y salida
        execute_child_command(cmd, envp); // Ejecutar el comando
    }

    close(pipe_fd[1]); // Cerrar el extremo de escritura del pipe en el padre
    close(fd_infile);
}

void execute_second_command(char *cmd, char *outfile, int pipe_fd[], char **envp)
{
    int fd_outfile = open_outfile(outfile);

    if (fork() == 0)
    {
        // Proceso hijo para el segundo comando
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
