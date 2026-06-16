#ifndef COMMAND_H
#define COMMAND_H

typedef enum e_redir_type
{
    REDIR_INPUT,
    REDIR_OUTPUT,
    REDIR_APPEND
} t_redir_type;

typedef struct s_redirection
{
    t_redir_type type;
    char *filename;
    struct s_redirection *next;
} t_redirection;

typedef struct s_command
{
    char **argv;
    int argc;
    int argv_capacity;
    t_redirection *redirections;
    struct s_command *next;
} t_command;

t_command *create_command(void);
int append_argument(t_command *command, const char *argument);
int append_redirection(t_command *command, t_redir_type type, const char *filename);
int append_command(t_command **head, t_command **tail, t_command *new_command);
int command_count(t_command *commands);
void free_commands(t_command *commands);
const char *redir_type_to_string(t_redir_type type);

#endif
