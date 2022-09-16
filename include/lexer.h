/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   lexer.h                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: safoh <safoh@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/09/13 16:02:56 by safoh         #+#    #+#                 */
/*   Updated: 2022/09/16 09:38:56 by dritsema      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include <libft.h>
# include <stdint.h>

typedef s_token
{
	int		type;
	char	*symbol;
}	t_token;

int32_t	lexer(char *input_line, char **envp, t_list **tokens);

#endif
