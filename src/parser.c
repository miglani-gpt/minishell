#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "lexer.h"

static int is_redirection_token(t_token_type type)
{
    return type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT || type == TOKEN_REDIR_APPEND;
}

static t_redir_type token_to_redir_type(t_token_type type)
{
    if (type == TOKEN_REDIR_IN)
    {
        return REDIR_INPUT;
    }
    if (type == TOKEN_REDIR_APPEND)
    {
        return REDIR_APPEND;
    }

    return REDIR_OUTPUT;
}

static t_parsed_input empty_parsed_input(void)
{
    t_parsed_input parsed;

    parsed.commands = NULL;
    parsed.command_count = 0;
    parsed.is_background = 0;

    return parsed;
}

static int validate_tokens(t_token *tokens)
{
    t_token *current;

    if (tokens == NULL)
    {
        return -1;
    }

    if (tokens->type == TOKEN_PIPE)
    {
        fprintf(stderr, "minishell: syntax error near unexpected token '|'\n");
        return -1;
    }

    current = tokens;
    while (current != NULL)
    {
        if (current->type == TOKEN_PIPE)
        {
            if (current->next == NULL || current->next->type == TOKEN_PIPE ||
                current->next->type == TOKEN_BACKGROUND)
            {
                fprintf(stderr, "minishell: syntax error near unexpected token '|'\n");
                return -1;
            }
        }

        if (is_redirection_token(current->type))
        {
            if (current->next == NULL || current->next->type != TOKEN_WORD)
            {
                fprintf(stderr, "minishell: syntax error: expected file after '%s'\n",
                        token_type_to_string(current->type));
                return -1;
            }
        }

        if (current->type == TOKEN_BACKGROUND && current->next != NULL)
        {
            fprintf(stderr, "minishell: syntax error near unexpected token '&'\n");
            return -1;
        }

        current = current->next;
    }

    return 0;
}

static int validate_commands_have_programs(t_command *commands)
{
    t_command *current;

    current = commands;
    while (current != NULL)
    {
        if (current->argc == 0)
        {
            fprintf(stderr, "minishell: syntax error: missing command\n");
            return -1;
        }

        current = current->next;
    }

    return 0;
}

static int append_token_to_command(t_command *command, t_token **current)
{
    if ((*current)->type == TOKEN_WORD)
    {
        if (append_argument(command, (*current)->value) == -1)
        {
            return -1;
        }
        return 0;
    }

    if (is_redirection_token((*current)->type))
    {
        t_token *filename_token;

        filename_token = (*current)->next;
        if (append_redirection(command, token_to_redir_type((*current)->type),
                               filename_token->value) == -1)
        {
            return -1;
        }

        *current = filename_token;
        return 0;
    }

    return 0;
}

static t_parsed_input build_commands_from_tokens(t_token *tokens)
{
    t_parsed_input parsed;
    t_command *head;
    t_command *tail;
    t_command *current_command;
    t_token *current;

    parsed = empty_parsed_input();
    head = NULL;
    tail = NULL;
    current_command = create_command();
    if (current_command == NULL)
    {
        return parsed;
    }

    if (append_command(&head, &tail, current_command) == -1)
    {
        free_commands(current_command);
        return parsed;
    }

    current = tokens;
    while (current != NULL)
    {
        if (current->type == TOKEN_PIPE)
        {
            current_command = create_command();
            if (append_command(&head, &tail, current_command) == -1)
            {
                free_commands(head);
                return empty_parsed_input();
            }
        }
        else if (current->type == TOKEN_BACKGROUND)
        {
            parsed.is_background = 1;
        }
        else
        {
            if (append_token_to_command(current_command, &current) == -1)
            {
                free_commands(head);
                return empty_parsed_input();
            }
        }

        current = current->next;
    }

    if (validate_commands_have_programs(head) == -1)
    {
        free_commands(head);
        return empty_parsed_input();
    }

    parsed.commands = head;
    parsed.command_count = command_count(head);
    return parsed;
}

t_parsed_input parse_input(const char *line)
{
    t_parsed_input parsed;
    t_token *tokens;

    parsed = empty_parsed_input();
    tokens = lexer_tokenize(line);

    if (tokens == NULL)
    {
        return parsed;
    }

    if (validate_tokens(tokens) == -1)
    {
        free_tokens(tokens);
        return parsed;
    }

    parsed = build_commands_from_tokens(tokens);
    free_tokens(tokens);

    return parsed;
}

void free_parsed_input(t_parsed_input *parsed)
{
    if (parsed == NULL)
    {
        return;
    }

    free_commands(parsed->commands);
    parsed->commands = NULL;
    parsed->command_count = 0;
    parsed->is_background = 0;
}
