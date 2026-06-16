#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

char **parse_input(char *line)
{
    char **args = malloc(sizeof(char *) * MAX_ARGS);

    if (args == NULL)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    char *token = strtok(line, " \t\r\v\f");

    while (token != NULL && index < MAX_ARGS - 1)
    {
        args[index] = token;
        index++;
        token = strtok(NULL, " \t\r\v\f");
    }

    args[index] = NULL;

    return args;
}