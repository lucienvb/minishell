#ifndef MINISHELL_H
# define MINISHELL_H

# include "ast.h"
# include <stdio.h>
# include <readline/readline.h>
# include <sys/types.h>
# include <sys/wait.h>

typedef struct s_minishell
{
	char	**env;
	int32_t	exit_code;
	t_list	*ast;
	t_list	*tokens;

}	t_minishell;

char	**dup_envp(char **envp);
int32_t	init_handlers(void);
int32_t	minishell(t_minishell *shell, char **envp);

#endif
