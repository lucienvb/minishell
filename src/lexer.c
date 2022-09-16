/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   lexer.c                                         |o_o || |                */
/*                                                     +:+                    */
/*   By: safoh <safoh@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/09/13 16:20:44 by safoh         #+#    #+#                 */
/*   Updated: 2022/09/16 17:42:27 by safoh        \___)=(___/                 */
/*                                                                            */
/* ************************************************************************** */

#include <lexer.h>
#include <message.h>
#include <stdio.h> // Remove
// Meta characters: SPACE, TAB, NEWLINE, ;, (, ), <, >, |, &.
// Types of tokens: words, keywords, I/O redirectors, and semicolons.

// int32_t	is_metachar(char c)
// {
// 	if ()
// 	{

// 	}
// }

t_token	*make_token(const char *command_line, int len)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	token->symbol = malloc(len + 1);
	ft_strlcpy(token->symbol, command_line, len + 1);
	return (token);
}

int32_t	get_token_len(const char *input)
{
	int32_t	i;

	i = 0;
	while (!ft_iswhitespace(input[i]) && input[i])
	{
		i++;
	}
	return (i);
}

int32_t	lexer(const char *command_line, t_list **tokens)
{
	int32_t	i;
	int32_t	token_len;
	t_list	*node;

	i = 0;
	(void)tokens;
	while (command_line[i])
	{
		if (!ft_iswhitespace(command_line[i]))
		{
			token_len = get_token_len(&command_line[i]);
			node = ft_lstnew(make_token(&command_line[i], token_len));
			ft_lstadd_back(tokens, node);
			i += token_len;
		}
		else
			i++;
	}
	return (SUCCESS);
}
