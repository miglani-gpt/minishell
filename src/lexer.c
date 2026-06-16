#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static int is_shell_whitespace(char character)
{
    return character == ' ' || character == '\t' || character == '\n' ||
           character == '\r' || character == '\v' || character == '\f';
}

static int is_operator_character(char character)
{
    return character == '|' || character == '<' || character == '>' || character == '&';
}

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

static t_token *create_token(t_token_type type, const char *value)
{
    t_token *token;

    token = malloc(sizeof(t_token));
    if (token == NULL)
    {
        perror("minishell: malloc");
        return NULL;
    }

    token->type = type;
    token->value = duplicate_string(value);
    token->next = NULL;

    if (token->value == NULL)
    {
        free(token);
        return NULL;
    }

    return token;
}

static int append_token(t_token **head, t_token **tail, t_token *new_token)
{
    if (new_token == NULL)
    {
        return -1;
    }

    if (*head == NULL)
    {
        *head = new_token;
        *tail = new_token;
    }
    else
    {
        (*tail)->next = new_token;
        *tail = new_token;
    }

    return 0;
}

static int append_character(char **buffer, size_t *length, size_t *capacity, char character)
{
    char *new_buffer;
    size_t new_capacity;

    if (*length + 1 >= *capacity)
    {
        new_capacity = (*capacity) * 2;
        new_buffer = realloc(*buffer, new_capacity);
        if (new_buffer == NULL)
        {
            perror("minishell: realloc");
            free(*buffer);
            *buffer = NULL;
            return -1;
        }

        *buffer = new_buffer;
        *capacity = new_capacity;
    }

    (*buffer)[*length] = character;
    (*length)++;
    (*buffer)[*length] = '\0';

    return 0;
}

static char *read_word(const char *line, size_t *index)
{
    size_t capacity;
    size_t length;
    char *word;
    int saw_quote_or_character;

    capacity = 32;
    length = 0;
    saw_quote_or_character = 0;
    word = malloc(capacity);
    if (word == NULL)
    {
        perror("minishell: malloc");
        return NULL;
    }
    word[0] = '\0';

    while (line[*index] != '\0' && !is_shell_whitespace(line[*index]) &&
           !is_operator_character(line[*index]))
    {
        if (line[*index] == '\'' || line[*index] == '"')
        {
            char quote;

            quote = line[*index];
            saw_quote_or_character = 1;
            (*index)++;

            while (line[*index] != '\0' && line[*index] != quote)
            {
                if (append_character(&word, &length, &capacity, line[*index]) == -1)
                {
                    return NULL;
                }
                (*index)++;
            }

            if (line[*index] != quote)
            {
                fprintf(stderr, "minishell: syntax error: unclosed %c quote\n", quote);
                free(word);
                return NULL;
            }

            (*index)++;
        }
        else
        {
            saw_quote_or_character = 1;
            if (append_character(&word, &length, &capacity, line[*index]) == -1)
            {
                return NULL;
            }
            (*index)++;
        }
    }

    if (!saw_quote_or_character)
    {
        free(word);
        return NULL;
    }

    return word;
}

static t_token *read_operator(const char *line, size_t *index)
{
    if (line[*index] == '|')
    {
        (*index)++;
        return create_token(TOKEN_PIPE, "|");
    }

    if (line[*index] == '<')
    {
        (*index)++;
        return create_token(TOKEN_REDIR_IN, "<");
    }

    if (line[*index] == '>')
    {
        if (line[*index + 1] == '>')
        {
            *index += 2;
            return create_token(TOKEN_REDIR_APPEND, ">>");
        }

        (*index)++;
        return create_token(TOKEN_REDIR_OUT, ">");
    }

    if (line[*index] == '&')
    {
        (*index)++;
        return create_token(TOKEN_BACKGROUND, "&");
    }

    return NULL;
}

t_token *lexer_tokenize(const char *line)
{
    t_token *head;
    t_token *tail;
    size_t index;

    head = NULL;
    tail = NULL;
    index = 0;

    if (line == NULL)
    {
        return NULL;
    }

    while (line[index] != '\0')
    {
        if (is_shell_whitespace(line[index]))
        {
            index++;
            continue;
        }

        if (is_operator_character(line[index]))
        {
            if (append_token(&head, &tail, read_operator(line, &index)) == -1)
            {
                free_tokens(head);
                return NULL;
            }
        }
        else
        {
            char *word;
            t_token *token;

            word = read_word(line, &index);
            if (word == NULL)
            {
                free_tokens(head);
                return NULL;
            }

            token = create_token(TOKEN_WORD, word);
            free(word);

            if (append_token(&head, &tail, token) == -1)
            {
                free_tokens(head);
                return NULL;
            }
        }
    }

    return head;
}

void free_tokens(t_token *tokens)
{
    t_token *next;

    while (tokens != NULL)
    {
        next = tokens->next;
        free(tokens->value);
        free(tokens);
        tokens = next;
    }
}

const char *token_type_to_string(t_token_type type)
{
    if (type == TOKEN_WORD)
    {
        return "word";
    }
    if (type == TOKEN_PIPE)
    {
        return "|";
    }
    if (type == TOKEN_REDIR_IN)
    {
        return "<";
    }
    if (type == TOKEN_REDIR_OUT)
    {
        return ">";
    }
    if (type == TOKEN_REDIR_APPEND)
    {
        return ">>";
    }
    if (type == TOKEN_BACKGROUND)
    {
        return "&";
    }

    return "unknown";
}
