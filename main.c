#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 100

void print_prompt(void)
{
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("minishell:%s> ", cwd);
    }
    else
    {
        printf("minishell> ");
    }

    fflush(stdout);
}

char *read_input(void)
{
    char *line = NULL;
    size_t buffer_size = 0;

    ssize_t characters_read = getline(&line, &buffer_size, stdin);

    if (characters_read == -1)
    {
        free(line);
        return NULL;
    }

    if (characters_read > 0 && line[characters_read - 1] == '\n')
    {
        line[characters_read - 1] = '\0';
    }

    return line;
}

char **parse_input(char *line)
{
    char **args = malloc(sizeof(char *) * MAX_ARGS);

    if (args == NULL)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    char *token = strtok(line, " ");

    while (token != NULL && index < MAX_ARGS - 1)
    {
        args[index] = token;
        index++;
        token = strtok(NULL, " ");
    }

    args[index] = NULL;

    return args;
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

void execute_external_command(char **args)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("minishell: fork failed");
        return;
    }

    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror("minishell");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

void execute_command(char **args)
{
    if (args[0] == NULL)
    {
        return;
    }

    if (handle_builtin(args))
    {
        return;
    }

    execute_external_command(args);
}

int main(void)
{
    while (1)
    {
        print_prompt();

        char *line = read_input();

        if (line == NULL)
        {
            printf("\n");
            break;
        }

        char **args = parse_input(line);

        execute_command(args);

        free(args);
        free(line);
    }

    return 0;
}