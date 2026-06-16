#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "pipes.h"
#include "builtins.h"
#include "redirection.h"

static void close_all_pipes(int pipes[][2], int pipe_count)
{
	int index;

	index = 0;
	while (index < pipe_count)
	{
		close(pipes[index][0]);
		close(pipes[index][1]);
		index++;
	}
}

static void close_created_pipes(int pipes[][2], int created_count)
{
	int index;

	index = 0;
	while (index < created_count)
	{
		close(pipes[index][0]);
		close(pipes[index][1]);
		index++;
	}
}

static int create_all_pipes(int pipes[][2], int pipe_count)
{
	int index;

	index = 0;
	while (index < pipe_count)
	{
		if (pipe(pipes[index]) == -1)
		{
			perror("minishell: pipe");
			close_created_pipes(pipes, index);
			return -1;
		}
		index++;
	}

	return 0;
}

static int connect_pipeline_fds(int command_index, int command_count, int pipes[][2])
{
	if (command_index > 0)
	{
		if (dup2(pipes[command_index - 1][0], STDIN_FILENO) < 0)
		{
			perror("minishell: dup2");
			return -1;
		}
	}

	if (command_index < command_count - 1)
	{
		if (dup2(pipes[command_index][1], STDOUT_FILENO) < 0)
		{
			perror("minishell: dup2");
			return -1;
		}
	}

	return 0;
}

static void execute_pipeline_command_child(t_command *command,
										   int command_index,
										   int command_count,
										   int pipes[][2])
{
	if (connect_pipeline_fds(command_index, command_count, pipes) == -1)
	{
		close_all_pipes(pipes, command_count - 1);
		exit(EXIT_FAILURE);
	}

	close_all_pipes(pipes, command_count - 1);

	if (apply_redirections(command->redirections, NULL, NULL) == -1)
	{
		exit(EXIT_FAILURE);
	}

	if (is_builtin(command->argv[0]))
	{
		handle_builtin(command->argv);
		fflush(stdout);
		exit(EXIT_SUCCESS);
	}

	if (execvp(command->argv[0], command->argv) == -1)
	{
		perror("minishell");
		exit(EXIT_FAILURE);
	}
}

static int fork_pipeline_commands(t_command *commands,
								  int command_count,
								  int pipes[][2],
								  pid_t *pids)
{
	t_command *current;
	int index;

	current = commands;
	index = 0;
	while (current != NULL)
	{
		pids[index] = fork();
		if (pids[index] < 0)
		{
			perror("minishell: fork");
			return -1;
		}

		if (pids[index] == 0)
		{
			execute_pipeline_command_child(current, index, command_count, pipes);
		}

		current = current->next;
		index++;
	}

	return 0;
}

static void wait_for_pipeline(pid_t *pids, int command_count)
{
	int index;
	int status;

	index = 0;
	while (index < command_count)
	{
		if (pids[index] > 0)
		{
			waitpid(pids[index], &status, 0);
		}
		index++;
	}
}

void execute_piped_commands(t_command *commands, int command_count)
{
	int pipe_count;
	int (*pipes)[2];
	pid_t *pids;

	if (commands == NULL || command_count < 2)
	{
		return;
	}

	pipe_count = command_count - 1;
	pipes = malloc(sizeof(int[2]) * pipe_count);
	pids = malloc(sizeof(pid_t) * command_count);
	if (pipes == NULL || pids == NULL)
	{
		perror("minishell: malloc");
		free(pipes);
		free(pids);
		return;
	}

	int index;

	index = 0;
	while (index < command_count)
	{
		pids[index] = -1;
		index++;
	}

	if (create_all_pipes(pipes, pipe_count) == -1)
	{
		free(pipes);
		free(pids);
		return;
	}

	if (fork_pipeline_commands(commands, command_count, pipes, pids) == -1)
	{
		close_all_pipes(pipes, pipe_count);
		wait_for_pipeline(pids, command_count);
		free(pipes);
		free(pids);
		return;
	}

	close_all_pipes(pipes, pipe_count);
	wait_for_pipeline(pids, command_count);

	free(pipes);
	free(pids);
}
