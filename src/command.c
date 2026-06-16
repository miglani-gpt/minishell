#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"

static char *duplicate_string(const char *source)
{
    size_t length;
    char *copy;

    if (source == NULL)
    {
        return NULL;
    }

    length = strlen(source);
    copy = malloc(length + 1);
    if (copy == NULL)
    {
        perror("minishell: malloc");
        return NULL;
    }

    memcpy(copy, source, length + 1);
    return copy;
}

t_command *create_command(void)
{
    t_command *command;

    command = malloc(sizeof(t_command));
    if (command == NULL)
    {
        perror("minishell: malloc");
        return NULL;
    }

    command->argv_capacity = 8;
    command->argc = 0;
    command->redirections = NULL;
    command->next = NULL;
    command->argv = malloc(sizeof(char *) * command->argv_capacity);
    if (command->argv == NULL)
    {
        perror("minishell: malloc");
        free(command);
        return NULL;
    }

    command->argv[0] = NULL;
    return command;
}

static int grow_argv_if_needed(t_command *command)
{
    int new_capacity;
    char **new_argv;

    if (command->argc + 1 < command->argv_capacity)
    {
        return 0;
    }

    new_capacity = command->argv_capacity * 2;
    new_argv = realloc(command->argv, sizeof(char *) * new_capacity);
    if (new_argv == NULL)
    {
        perror("minishell: realloc");
        return -1;
    }

    command->argv = new_argv;
    command->argv_capacity = new_capacity;
    return 0;
}

int append_argument(t_command *command, const char *argument)
{
    char *argument_copy;

    if (command == NULL || argument == NULL)
    {
        return -1;
    }

    if (grow_argv_if_needed(command) == -1)
    {
        return -1;
    }

    argument_copy = duplicate_string(argument);
    if (argument_copy == NULL)
    {
        return -1;
    }

    command->argv[command->argc] = argument_copy;
    command->argc++;
    command->argv[command->argc] = NULL;
    return 0;
}

int append_redirection(t_command *command, t_redir_type type, const char *filename)
{
    t_redirection *redirection;
    t_redirection *current;

    if (command == NULL || filename == NULL)
    {
        return -1;
    }

    redirection = malloc(sizeof(t_redirection));
    if (redirection == NULL)
    {
        perror("minishell: malloc");
        return -1;
    }

    redirection->type = type;
    redirection->filename = duplicate_string(filename);
    redirection->next = NULL;
    if (redirection->filename == NULL)
    {
        free(redirection);
        return -1;
    }

    if (command->redirections == NULL)
    {
        command->redirections = redirection;
        return 0;
    }

    current = command->redirections;
    while (current->next != NULL)
    {
        current = current->next;
    }

    current->next = redirection;
    return 0;
}

int append_command(t_command **head, t_command **tail, t_command *new_command)
{
    if (new_command == NULL)
    {
        return -1;
    }

    if (*head == NULL)
    {
        *head = new_command;
        *tail = new_command;
    }
    else
    {
        (*tail)->next = new_command;
        *tail = new_command;
    }

    return 0;
}

int command_count(t_command *commands)
{
    int count;

    count = 0;
    while (commands != NULL)
    {
        count++;
        commands = commands->next;
    }

    return count;
}

static void free_redirections(t_redirection *redirections)
{
    t_redirection *next;

    while (redirections != NULL)
    {
        next = redirections->next;
        free(redirections->filename);
        free(redirections);
        redirections = next;
    }
}

void free_commands(t_command *commands)
{
    t_command *next;
    int index;

    while (commands != NULL)
    {
        next = commands->next;

        index = 0;
        while (index < commands->argc)
        {
            free(commands->argv[index]);
            index++;
        }

        free(commands->argv);
        free_redirections(commands->redirections);
        free(commands);
        commands = next;
    }
}

const char *redir_type_to_string(t_redir_type type)
{
    if (type == REDIR_INPUT)
    {
        return "<";
    }
    if (type == REDIR_OUTPUT)
    {
        return ">";
    }
    if (type == REDIR_APPEND)
    {
        return ">>";
    }

    return "unknown";
}
