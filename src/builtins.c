#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"

int is_builtin(char *command)
{
    if (command == NULL)
    {
        return 0;
    }

    return strcmp(command, "cd") == 0 ||
           strcmp(command, "pwd") == 0 ||
           strcmp(command, "help") == 0 ||
           strcmp(command, "clear") == 0 ||
           strcmp(command, "exit") == 0;
}

int builtin_cd(char **args)
{
    if (args[1] == NULL)
    {
        char *home = getenv("HOME");

        if (home == NULL)
        {
            fprintf(stderr, "minishell: cd: HOME not set\n");
            return 1;
        }

        if (chdir(home) != 0)
        {
            perror("minishell: cd");
        }

        return 1;
    }

    if (chdir(args[1]) != 0)
    {
        perror("minishell: cd");
    }

    return 1;
}

int builtin_pwd(void)
{
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("minishell: pwd");
    }
    else
    {
        printf("%s\n", cwd);
    }

    return 1;
}

int builtin_help(void)
{
    printf("MiniShell - A simple Unix-like shell written in C\n");
    printf("\n");
    printf("Built-in commands:\n");
    printf("  cd [directory]   Change current directory\n");
    printf("  pwd              Print current working directory\n");
    printf("  help             Show this help message\n");
    printf("  clear            Clear the terminal screen\n");
    printf("  exit             Exit the shell\n");
    printf("\n");
    printf("Redirection:\n");
    printf("  command > file    Redirect output to file\n");
    printf("  command >> file   Append output to file\n");
    printf("\n");
    printf("External commands are executed using fork() and execvp().\n");

    return 1;
}

int builtin_clear(void)
{
    printf("\033[H\033[J");
    return 1;
}

int handle_builtin(char **args)
{
    if (args[0] == NULL)
    {
        return 1;
    }

    if (strcmp(args[0], "cd") == 0)
    {
        return builtin_cd(args);
    }

    if (strcmp(args[0], "pwd") == 0)
    {
        return builtin_pwd();
    }

    if (strcmp(args[0], "help") == 0)
    {
        return builtin_help();
    }

    if (strcmp(args[0], "clear") == 0)
    {
        return builtin_clear();
    }

    if (strcmp(args[0], "exit") == 0)
    {
        exit(0);
    }

    return 0;
}