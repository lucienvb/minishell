#include <minishell.h>
#include "libft.h"

static int32_t	open_heredoc(char *path)
{
	return (here_doc(path));
}

static int32_t	open_input(char *path)
{
	return (open(path, O_RDONLY));
}

static int32_t	open_output(char *path)
{
	return (open(path, O_WRONLY | O_CREAT | O_TRUNC, 0664));
}

static int32_t	open_append(char *path)
{
	return (open(path, O_WRONLY | O_CREAT | O_APPEND, 0664));
}

static int32_t	open_fd_type(char *path, t_type type, t_status *status)
{
	if (type == HEREDOC)
		return (open_heredoc(path));
	else if (type == INPUT)
		return (open_input(path));
	else if (type == OUTPUT)
		return (open_output(path));
	else if (type == APPEND)
		return (open_append(path));
	else
	{
		*status = message_general_error(E_GENERAL, "open_fd_type: invalid type");
		return (false);
	}
}

static int32_t redirect_file_descriptor(int source_fd, t_type redirection)
{
    if (redirection == INPUT || redirection == HEREDOC)
    	return (dup2(source_fd, STDIN_FILENO));
    else
    	return (dup2(source_fd, STDOUT_FILENO));
}

int32_t	redirect(t_redir *redir, t_type type, t_status *status)
{
	int32_t	fd;

	fd = open_fd_type(redir->filename, type, status);
	if (fd == ERROR)
		return (ERROR);
	if (redirect_file_descriptor(fd, type) == ERROR)
	{
		close(fd);
		return (ERROR);
	}
	close(fd);
	return (SUCCESS);
}

static void close_fd_if_open(int32_t *fd)
{
  if (*fd != -1)
  {
    close(*fd);
    *fd = -1;
  }
}

static bool handle_redirection(t_redir *redir, int32_t *input_fd, int32_t *output_fd, t_status *status)
{
  int32_t ret;

  ret = redirect(redir, redir->type, status);
  if (ret == ERROR)
    return (false);
  if (redir->type == INPUT || redir->type == HEREDOC)
  {
    close_fd_if_open(input_fd);
    *input_fd = ret;
  }
  else
  {
    close_fd_if_open(output_fd);
    *output_fd = ret;
  }
  return (true);
}

int32_t setup_redirects(t_command *command, t_status *status)
{
  t_redir *redir;
  int32_t i;
  int32_t input_fd;
  int32_t output_fd;

  i = 0;
  input_fd = -1;
  output_fd = -1;
  while (i < command->n_redirs)
  {
    get_next_redir(command, &redir);
    if (handle_redirection(redir, &input_fd, &output_fd, status) == false)
    {
      close_fd_if_open(&input_fd);
      close_fd_if_open(&output_fd);
      return (message_system_call_error("setup_redirects: "));
    }
    i++;
  }
  return (SUCCESS);
}

t_builtin	builtin_lookup(char *cmd)
{
	int32_t	i;

	static const t_builtin lookup[] = {
		{.name = "echo", .func = ft_echo},
		{.name = "cd", .func = ft_cd},
		{.name = "pwd", .func = ft_pwd},
		{.name = "export", .func = ft_export},
		{.name = "unset", .func = ft_unset},
		{.name = "env", .func = ft_env},
		{.name = "exit", .func = ft_exit},
		{NULL, NULL}};
	i = 0;
	while (lookup[i].name != NULL &&
			ft_strncmp(lookup[i].name, cmd, ft_strlen(cmd) + 1))
		i++;
	return (lookup[i]);
}

int32_t	execute_builtin(char **arguments, t_minishell *shell)
{
	t_builtin	builtin_function;

	builtin_function = builtin_lookup(arguments[0]);
	if (builtin_function.name == NULL)
		return (-1);
	builtin_function.func(arguments, shell);
	return (shell->status);
}

int32_t wait_for_child_processes(pid_t *pid_array, size_t array_length)
{
	int32_t status;
	size_t i;

	for (i = 0; i < array_length; ++i)
	{
		waitpid(pid_array[i], &status, WUNTRACED);
	}
	return (WEXITSTATUS(status));
}

static bool	file_exits(char *path)
{
	return (access(path, F_OK) == SUCCESS);
}

static char	*search_cmd_in_path(const char *path, const char *cmd)
{
	char		**path_dirs;
	char		*cmd_path;
	size_t		i;

	path_dirs = ft_split(path, ':');
	if (!path_dirs)
		return (NULL);
	i = 0;
	while (path_dirs[i])
	{
		cmd_path = ft_strjoin(path_dirs[i], cmd);
		if (!cmd_path)
			break ;
		if (file_exits(cmd_path))
			break;
		free(cmd_path);
		cmd_path = NULL;
		i++;
	}
	ft_matrixfree(&path_dirs);
	return (cmd_path);
}

char		*get_cmd_path(char *path, char *cmd)
{
	char	*cmd_path;
	char	*slash_cmd;

	if (!path || !cmd)
		return (NULL);
	if (cmd[0] == '/' || cmd[0] == '.')
	{
		if (file_exits(cmd))
			return (ft_strdup(cmd));
		return (NULL);
	}
	slash_cmd = ft_strjoin("/", cmd);
	if (!slash_cmd)
		return (NULL);
	cmd_path = search_cmd_in_path(path, slash_cmd);
	free(slash_cmd);
	return (cmd_path);
}

void	execute_child_command(t_minishell *shell, char **arguments)
{
	char	*command_path;
	char	*path;
	char **envp;

	path = dict_get(&shell->env, "PATH");
	command_path = get_cmd_path(path, arguments[0]);
	if (!command_path)
	{
		message_general_error(E_COMMAND_NOT_FOUND, NULL);
		_exit(E_COMMAND_NOT_FOUND);
	}
	envp = dict_to_envp(&shell->env);
	execve(command_path, arguments, envp);
	ft_matrixfree(&envp);
	free(command_path);
	message_system_call_error("execve");
	_exit(E_COMMAND_NOT_FOUND);
}

int32_t	execute_simple_command(t_command *cmd, t_minishell *shell)
{
	pid_t	pid;
	int32_t	status;
	char	**arguments;

	status = setup_redirects(cmd, &shell->status);
	if (status)
		return (status);
	arguments = get_arguments(cmd);
	if (!arguments || *arguments == NULL)
		return (E_GENERAL);
	if (*arguments == NULL)
	{
		free(arguments);
		return (SUCCESS);
	}
	status = execute_builtin(arguments, shell);
	if (status >= 0)
	{
		free(arguments);
		return (status);
	}
	pid = fork();
	if (pid == -1)
		return (E_GENERAL);
	if (pid == 0)
	{
		execute_child_command(shell, arguments);
		_exit(E_COMMAND_NOT_FOUND);
	}
	free(arguments);
	return (wait_for_child_processes(&pid, 1));
}

int32_t	execute_pipe_command(t_command *cmd, t_minishell *shell)
{
	char	**arguments;
	int32_t	status;

	status = setup_redirects(cmd, &shell->status);
	if (status)
		_exit(status);
	arguments = get_arguments(cmd);
	if (!arguments)
		_exit(E_COMMAND_NOT_FOUND);
	status = execute_builtin(arguments, shell);
	if (status >= 0)
		_exit(E_COMMAND_NOT_FOUND);
	execute_child_command(shell, arguments);
	_exit(E_COMMAND_NOT_FOUND);
	return (status);
}

int32_t handle_first_pipe(int32_t *pipe_fds)
{
    if (pipe(pipe_fds) == ERROR)
        return (ERROR);
    if (dup2(pipe_fds[1], STDOUT_FILENO) == ERROR)
        return (ERROR);
    return (close(pipe_fds[1]));
}

int32_t handle_middle_pipes(int32_t *pipe_fds)
{
    if (dup2(pipe_fds[STDIN_FILENO], STDIN_FILENO) == ERROR)
        return (ERROR);
    if (close(pipe_fds[STDIN_FILENO]) == ERROR)
        return (ERROR);
    if (pipe(pipe_fds) == ERROR)
        return (ERROR);
    if (dup2(pipe_fds[STDOUT_FILENO], STDOUT_FILENO) == ERROR)
        return (ERROR);
    return (close(pipe_fds[STDOUT_FILENO]));
}

int32_t handle_last_pipe(int32_t *pipe_fds, int32_t *std_fds)
{
    if (dup2(pipe_fds[STDIN_FILENO], STDIN_FILENO) == ERROR)
        return (ERROR);
    if (close(pipe_fds[STDIN_FILENO]) == ERROR)
        return (ERROR);
    return (dup2(std_fds[STDOUT_FILENO], STDOUT_FILENO));
}

int32_t handle_pipes(int32_t *pipe_fds, int32_t *std_fds, int32_t n_commands, int32_t i)
{
		if (i == 0)
			return (handle_first_pipe(pipe_fds));
		else if (i == n_commands - 1)
			return (handle_last_pipe(pipe_fds, std_fds));
		return (handle_middle_pipes(pipe_fds));
}

int32_t process_command(t_command_table *ct, t_minishell *shell)
{
  t_command *cmd;
  pid_t pid;

  get_next_command(ct, &cmd);
  pid = fork();
  if (pid == -1)
		return (ERROR);
	if (pid == 0)
    execute_pipe_command(cmd, shell);
  return (pid);
}

int32_t process_command_and_handle_error(t_command_table *ct, t_minishell *shell)
{
    int32_t pid = process_command(ct, shell);
    if (pid == ERROR)
    {
        shell->status = message_general_error(E_GENERAL, "Executor: ");
        return ERROR;
    }
    return pid;
}

void execute_pipeline(t_command_table *ct, t_minishell *shell)
{
  int32_t pipe_fds[2];
  int32_t i;

  i = 0;
  shell->is_pipeline = true;
  while (i < ct->n_commands)
  {
    if (handle_pipes(pipe_fds, shell->std_fds, ct->n_commands, i) == ERROR)
    {
      shell->status = message_general_error(E_GENERAL, "Executor: ");
      break;
    }
    ct->pids[i] = process_command_and_handle_error(ct, shell);
    if (ct->pids[i] == ERROR)
      break;
    i++;
  }
  shell->status = wait_for_child_processes(ct->pids, ct->n_commands);
  free(ct->pids);
}

void		execute_command_table(t_command_table *ct, t_minishell *shell)
{
	t_command	*cmd;

	if (ct->n_commands == 1)
	{
		get_next_command(ct, &cmd);
		execute_simple_command(cmd, shell);
	}
	else
		execute_pipeline(ct, shell);
}

void reset_std_fds(int32_t *std_fds, t_status *status)
{
  if (dup2(std_fds[STDIN_FILENO], STDIN_FILENO) == ERROR) 
    *status = message_system_call_error("Reset_std_fds: ");
  if (dup2(std_fds[STDOUT_FILENO], STDOUT_FILENO) == ERROR)
    *status = message_system_call_error("Reset_std_fds: ");
}

void	executor(t_minishell *shell)
{
	t_command_table	*ct;

	get_one_command_table(&shell->ast, &ct);
	execute_command_table(ct, shell);
	reset_std_fds(shell->std_fds, &shell->status);
}
