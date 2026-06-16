#ifndef LEXER_H
#define LEXER_H

typedef enum e_token_type
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIR_IN,
    TOKEN_REDIR_OUT,
    TOKEN_REDIR_APPEND,
    TOKEN_BACKGROUND
} t_token_type;

typedef struct s_token
{
    t_token_type type;
    char *value;
    struct s_token *next;
} t_token;

t_token *lexer_tokenize(const char *line);
void free_tokens(t_token *tokens);
const char *token_type_to_string(t_token_type type);

#endif
