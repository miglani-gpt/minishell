#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"

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

static int is_redirection(t_token_type type)
{
    return type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT || type == TOKEN_REDIR_APPEND;
}

static int count_tokens(t_token *tokens)
{
    int count;

    count = 0;
    while (tokens != NULL)
    {
        count++;
        tokens = tokens->next;
    }

    return count;
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
            if (current->next == NULL || current->next->type == TOKEN_PIPE)
            {
                fprintf(stderr, "minishell: syntax error near unexpected token '|'\n");
                return -1;
            }
        }

        if (is_redirection(current->type))
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

static t_parsed_input empty_parsed_input(void)
{
    t_parsed_input parsed;

    parsed.args = NULL;
    parsed.owned_values = NULL;
    parsed.owned_count = 0;

    return parsed;
}

t_parsed_input parse_input(const char *line)
{
    t_parsed_input parsed;
    t_token *tokens;
    t_token *current;
    int token_count;
    int index;

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

    token_count = count_tokens(tokens);
    if (token_count >= MAX_ARGS)
    {
        fprintf(stderr, "minishell: too many arguments; maximum is %d\n", MAX_ARGS - 1);
        free_tokens(tokens);
        return parsed;
    }

    parsed.args = malloc(sizeof(char *) * (token_count + 1));
    parsed.owned_values = malloc(sizeof(char *) * token_count);
    if (parsed.args == NULL || parsed.owned_values == NULL)
    {
        perror("minishell: malloc");
        free(parsed.args);
        free(parsed.owned_values);
        free_tokens(tokens);
        return empty_parsed_input();
    }

    current = tokens;
    index = 0;
    while (current != NULL)
    {
        parsed.owned_values[index] = duplicate_string(current->value);
        if (parsed.owned_values[index] == NULL)
        {
            parsed.owned_count = index;
            free_parsed_input(&parsed);
            free_tokens(tokens);
            return empty_parsed_input();
        }

        parsed.args[index] = parsed.owned_values[index];
        index++;
        current = current->next;
    }

    parsed.args[index] = NULL;
    parsed.owned_count = index;

    free_tokens(tokens);
    return parsed;
}

void free_parsed_input(t_parsed_input *parsed)
{
    int index;

    if (parsed == NULL)
    {
        return;
    }

    if (parsed->owned_values != NULL)
    {
        index = 0;
        while (index < parsed->owned_count)
        {
            free(parsed->owned_values[index]);
            index++;
        }
    }

    free(parsed->owned_values);
    free(parsed->args);

    parsed->args = NULL;
    parsed->owned_values = NULL;
    parsed->owned_count = 0;
}
