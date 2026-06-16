#ifndef PARSER_H
#define PARSER_H

#include "command.h"

#define MAX_ARGS 100

typedef struct s_parsed_input
{
    t_command *commands;
    int command_count;
    int is_background;
} t_parsed_input;

t_parsed_input parse_input(const char *line);
void free_parsed_input(t_parsed_input *parsed);

#endif
