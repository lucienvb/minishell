#include <lexer.h>
#include <message.h>
#include <stdio.h> // Remove
// Meta characters: SPACE, TAB, NEWLINE, ;, (, ), <, >, |, &.
// Types of tokens: words, keywords, I/O redirectors, and semicolons.

// Delimiters ";", "|", "&".

int32_t	is_quotechar(char c)
{
	if (c == '\"' || c == '\'')
	{
		return (1);
	}
	return (0);
}

int32_t	is_tokenchar(char c)
{
	if (c == '|' || c == '<' || c == '>')
	{
		return (1);
	}
	return (0);
}

char	*make_token(const char *input_line, int len)
{
	char	*symbol;

	symbol = malloc(len + 1);
	ft_strlcpy(symbol, input_line, len + 1);
	return (symbol);
}

int32_t	get_token_len(const char *input)
{
	int32_t	i;
	int32_t	quoted;

	i = 0;
	quoted = 0;
	if (is_tokenchar(input[i]))
		return (1);
	while ((!ft_iswhitespace(input[i]) || quoted) && input[i])
	{
		if (is_quotechar(input[i]) && !quoted)
			quoted = input[i];
		else if (is_tokenchar(input[i]) && !quoted)
			break ;
		else if (is_quotechar(input[i]) && quoted == input[i])
			quoted = 0;
		i++;
	}
	return (i);
}

int32_t	lexer(const char *input_line, t_list **tokens)
{
	int32_t	i;
	int32_t	token_len;
	t_list	*node;

	i = 0;
	while (input_line[i])
	{
		if (!ft_iswhitespace(input_line[i]))
		{
			token_len = get_token_len(&input_line[i]);
			node = ft_lstnew(make_token(&input_line[i], token_len));
			ft_lstadd_back(tokens, node);
			i += token_len;
		}
		else
			i++;
	}
	return (SUCCESS);
}
