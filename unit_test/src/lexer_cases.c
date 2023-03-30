#include <criterion/assert.h>
#include <criterion/internal/assert.h>
#include <unit_test.h>
#include "libft.h"

extern t_status zero;
extern t_status max;

void redirect_all_std(void)
{
    cr_redirect_stdout();
    cr_redirect_stderr();
}
TestSuite(lexer, .init=redirect_all_std);
t_list	*lexer(const char *command_line, t_status *exit_status);

/*******************************************************************************/
/*                           Lexer_one                                         */
/*******************************************************************************/

void assert_lexer_one(char *command_line, char **expected)
{
    t_list *tokens;

    tokens = lexer(command_line, &zero);
    cr_expect_str_eq(tokens->content, *expected);
    cr_expect(tokens->next == NULL);
    ft_lstclear(&tokens, free);
}

Test(lexer, one_ls)
{
    char *expected[] = {"ls", NULL};
    assert_lexer_one("ls", expected);
}

// THIS ONE DOESN'T MAKE SENSE
//Test(lexer, one_dollar)
//{
//    char *expected[] = {"$", NULL};
//    assert_lexer_one("$", expected);
//}

/*******************************************************************************/
/*                           Lexer_two                                         */
/*******************************************************************************/

void assert_lexer_two(char *command_line, char **expected)
{
    t_list *tokens;

    tokens = lexer(command_line, &zero);
    cr_expect_str_eq(tokens->content, *expected++);
    cr_expect_str_eq(tokens->next->content, *expected);
    cr_expect(tokens->next->next == NULL);
    ft_lstclear(&tokens, free);
}

Test(lexer, two_ls_l)
{
    char *expected[] = {"ls", "-l", NULL};
    assert_lexer_two("ls -l", expected);
}

Test(lexer, echo_dollar)
{
    char *expected[] = {"echo", "$", NULL};
    assert_lexer_two("echo $", expected);
}

Test(lexer, echo_hello)
{
	char *expected[] = {"echo", "hello", NULL};
	assert_lexer_two("echo hello", expected);
}

Test(lexer, dollar_dollar)
{
	char *expected[] = {"$", "$", NULL};
	assert_lexer_two("$ $", expected);
}

Test(lexer, echo_hello_with_quotes_one)
{
	char *expected[] = {"echo", "'he\"llo'", NULL};
	assert_lexer_two("echo 'he\"llo'", expected);
}

Test(lexer, echo_hello_with_quotes_two)
{
	char *expected[] = {"echo", "'hello'", NULL};
	assert_lexer_two("echo 'hello'", expected);
}

Test(lexer, echo_hello_with_quotes_three)
{
	char *expected[] = {"echo", "\"hel\'lo\"", NULL};
	assert_lexer_two("echo \"hel\'lo\"", expected);
}

//Test(lexer, echo_hello_in_quotes)
//{
//	char *expected[] = {"echo", "hello", NULL};
//	assert_lexer_two("echo 'hel'lo'", expected);
//}

/*******************************************************************************/
/*                           null_Lexer                                        */
/*******************************************************************************/

// possible test case -- multiple commands: "ls -l ; pwd | grep foo"

void assert_lexer_null(char *command_line, char *message)
{
    t_list *tokens;

    tokens = lexer(command_line, &zero);
    fflush(stderr);
    cr_expect(tokens==NULL);
    ft_lstclear(&tokens, free);
    cr_assert_stderr_eq_str(message);
}

Test(lexer, null_pipe)
{
    assert_lexer_null("|", "sheldon: syntax error near unexpected token `|'\n");
}

Test(lexer, null_input)
{
    assert_lexer_null("<", "sheldon: syntax error near unexpected token `newline'\n");
}

Test(lexer, null_ouput)
{
    assert_lexer_null(">", "sheldon: syntax error near unexpected token `newline'\n");
}

Test(lexer, null_append)
{
    assert_lexer_null(">>", "sheldon: syntax error near unexpected token `newline'\n");
}

Test(lexer, null_heredoc)
{
    assert_lexer_null("<<", "sheldon: syntax error near unexpected token `newline'\n");
}

Test(lexer, null_pipe_double)
{
    assert_lexer_null("||", "sheldon: syntax error near unexpected token `||'\n");
}

Test(lexer, null_append_output)
{
    assert_lexer_null(">> >", "sheldon: syntax error near unexpected token `>'\n");
}

Test(lexer, null_heredoc_input)
{
    assert_lexer_null("<< <", "sheldon: syntax error near unexpected token `<'\n");
}

Test(lexer, null_heredoc_input_two)
{
	assert_lexer_null("<< >", "sheldon: syntax error near unexpected token `>'\n");
}

Test(lexer, null_append_output_two)
{
	assert_lexer_null(">> <", "sheldon: syntax error near unexpected token `<'\n");
}

Test(lexer, null_append_output_nospace)
{
	assert_lexer_null(">><", "sheldon: syntax error near unexpected token `<'\n");
}

Test(lexer, pre_space_null_append_output_nospace)
{
	assert_lexer_null(" >><", "sheldon: syntax error near unexpected token `<'\n");
}

Test(lexer, null_quote_count_is_odd_one)
{
	assert_lexer_null("echo \'hel\'\'lo\'\'", "sheldon: quote is unclosed\n");
}

Test(lexer, null_quote_count_is_odd_two)
{
	assert_lexer_null("echo \'hello", "sheldon: quote is unclosed\n");
}

Test(lexer, null_quote_count_is_odd_three)
{
	assert_lexer_null("echo \"hel\"lo\"", "sheldon: quote is unclosed\n");
}

Test(lexer, null_quote_count_is_odd_four)
{
	assert_lexer_null("echo hello\"", "sheldon: quote is unclosed\n");
}

Test(lexer, null_exclamation_mark)
{
	assert_lexer_null("!", "sheldon: syntax error near unexpected token `newline'\n");
}

//Test(lexer, null_command)
//{
//    assert_lexer_null("echo");
//}
