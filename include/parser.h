#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 100

typedef struct s_parsed_input
{
    char **args;
    char **owned_values;
    int owned_count;
} t_parsed_input;

t_parsed_input parse_input(const char *line);
void free_parsed_input(t_parsed_input *parsed);

#endif
