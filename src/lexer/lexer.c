#include <minishell.h>

// returns true if the count of quotes (' or ") is even
// applied on the first found quote in the string
static bool	are_quotes_closed(const char *str)
{
	size_t	count;
	size_t	i;
	int		quote;
	char	c;

	i = 0;
	count = 0;
	quote = 0;
	while (str[i])
	{
		if (is_quote(str[i]) && !quote)
		{
			c = str[i];
			quote = 1;
		}
		if (str[i] == c && quote)
			count++;
		i++;
	}
	if (count % 2 == 0)
		return (true);
	return (false);
}

static size_t	get_token_len(const char *command_line)
{
	size_t	next_quote;
	size_t	i;

	i = 0;
	if (is_tokenchar(&command_line[i]))
		return (is_tokenchar(&command_line[i]));
	while (command_line[i] && !ft_iswhitespace(command_line[i])
		&& !is_tokenchar(&command_line[i]))
	{
		if (is_quotechar(command_line[i]))
		{
			next_quote = i + 1;
			while (command_line[next_quote] != command_line[i]
				&& command_line[next_quote])
				next_quote++;
			i = next_quote;
		}
		i++;
	}
	return (i);
}

static char	*make_token(const char **command_line, t_status *exit)
{
	char	*symbol;
	size_t	token_len;

	token_len = get_token_len(*command_line);
	symbol = ft_substr(*command_line, 0, token_len);
	if (!symbol)
	{
		*exit = message_system_call_error("make_token");
		return (NULL);
	}
	*command_line += token_len;
	return (symbol);
}

// Helper function to create a token and handle errors
static t_list	*create_token_node(const char **command_line, t_status *exit)
{
	t_list	*node;
	char	*token;

	token = make_token(command_line, exit);
	if (!token)
	{
		*exit = message_system_call_error("lexer");
		return (NULL);
	}
	node = ft_lstnew(token);
	if (!node)
	{
		*exit = message_system_call_error("lexer");
		free(token);
		return (NULL);
	}
	return (node);
}

t_list	*lexer(const char *command_line, t_status *exit)
{
	t_list	*node;
	t_list	*tokens;

	if (check_lexical_conventions(command_line, exit) == false)
		return (NULL);
	if (are_quotes_closed(command_line) == false)
	{
		*exit = message_general_error(E_QUOTES, command_line);
		return (NULL);
	}
	tokens = NULL;
	while (*command_line)
	{
		ft_skip_whitespaces(&command_line);
		if (*command_line)
		{
			node = create_token_node(&command_line, exit);
			if (!node)
				return (NULL);
			ft_lstadd_back(&tokens, node);
		}
	}
	return (tokens);
}
