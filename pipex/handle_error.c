#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void handle_error(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}