#include <unit_test.h>

t_exitstatus zero = 0;
t_exitstatus max = 255;

 void setup_env(void)
 {
     setenv("HELLO", "Hello", 1);
     setenv("WORLD", "World", 1);
     unsetenv("UNSET");
     setenv("SPACE", "Spa ce", 1);
 }

/*******************************************************************************/
/*                           Expand_Dollar                                     */
/*******************************************************************************/

char *expand_dollar(char *arg, t_exitstatus *status);

void assert_expand_dollar(char *input, char *expected, t_exitstatus *status)
{
    char *result = expand_dollar(input, status);
    cr_expect_str_eq(result, expected);
    free(result);
}

TestSuite(expand_dollar, .init=setup_env);

Test(expand_dollar, single_dollar)
{
    assert_expand_dollar("$", "$", NULL);
}

 Test(expand_dollar, exit_status)
 {
     assert_expand_dollar("$?", "0", &zero);
 }

 Test(expand_dollar, exit_status_max)
 {
     assert_expand_dollar("$?", "255", &max);
 }

 Test(expand_dollar, single_var)
 {
     assert_expand_dollar("$HELLO", "Hello", NULL);
 }

 Test(expand_dollar, double_var)
 {
     assert_expand_dollar("$HELLO$HELLO", "Hello", NULL);
 }

 Test(expand_dollar, null)
 {
     assert_expand_dollar("$UNSET", "", NULL);
 }

/*******************************************************************************/
/*                           Combine_expanded_strings                          */
/*******************************************************************************/

char *combine_expanded_strings(t_list *stack);

void assert_combine_expanded_strings(char **input, char *expected)
{
    t_list list;
    t_list list2;
    char *output;

    list.content = *input;
    if (input[1])
        list.next = &list2;
    else
        list.next = NULL;
    list2.content = *(input + 1);
    list2.next = NULL;
    output = combine_expanded_strings(&list);
    cr_expect_str_eq(output, expected);
    free(output);
}

Test(combine_expanded_strings, basic)
{
    char *input[] = {"Hallo", NULL};
    assert_combine_expanded_strings(input, "Hallo");
}

Test(combine_expanded_strings, two)
{
    char *input[] = {"Hallo", "Wereld", NULL};
    assert_combine_expanded_strings(input, "HalloWereld");
}

/*******************************************************************************/
/*                           Copy_until_quote_or_dollar                        */
/*******************************************************************************/

t_list *copy_until_quote_or_dollar(char *arg, size_t *i);

void assert_copy_until_quote_or_dollar(char *input, char *expected)
{
    t_list *output;
    size_t i;

    output = copy_until_quote_or_dollar(input, &i);
    cr_expect_str_eq(output->content, expected);
    free(output->content);
    free(output);
}

Test(copy_until_quote_or_dollar, basic)
{
    assert_copy_until_quote_or_dollar("Hallo", "Hallo");
}

 Test(copy_until_quote_or_dollar, dollar)
 {
     assert_copy_until_quote_or_dollar("Hallo$", "Hallo");
 }

 Test(copy_until_quote_or_dollar, double_quote)
 {
     assert_copy_until_quote_or_dollar("Hallo\"", "Hallo");
 }

 Test(copy_until_quote_or_dollar, single_quote)
 {
     assert_copy_until_quote_or_dollar("Hallo\'", "Hallo");
 }

/*******************************************************************************/
/*                           Expand_token                                      */
/*******************************************************************************/

char *expand_token(char *arg, t_exitstatus *status);


 TestSuite(expand_token, .init=setup_env);

 void assert_expand_token(char *input, char *expected, t_exitstatus *status)
 {
     char *output = expand_token(input, status);
     cr_expect_str_eq(output, expected);
     free(output);
 }

 Test(expand_token, basic_arg)
 {
     assert_expand_token("Hello", "Hello", NULL);
 }

  Test(expand_token, basic_env)
  {
      assert_expand_token("$HELLO", "Hello", NULL);
  }

 Test(expand_token, exit_status)
 {
     assert_expand_token("$?", "0", &zero);
 }

Test(expand_token, exit_status_max)
{
    assert_expand_token("$?", "255", &max);
}

Test(expand_token, no_envvar)
{
    assert_expand_token("$UNSET", "", NULL);
}

Test(expand_token, envvar_single_quotes)
{
    assert_expand_token("'$VAR'", "$VAR", NULL);
}

 Test(expand_token, double_qoute_no_envvar, .timeout=1)
 {
     char input[] = "\"HELLO\"";
     assert_expand_token(input, "HELLO", NULL);
 }

 Test(expand_token, double_qoute_envvar, .timeout=1)
 {
     char input[] = "\"$HELLO\"";
     assert_expand_token(input, "Hello", NULL);
 }

 // Test(expand_token, double_qoute_two_envvar, .timeout=1)
 // {
 //     char input[] = "\"$HELLO$WORLD\"";
 //     assert_expand_token(input, "HelloWorld", NULL);
 // }

 // Test(expand_token, envvar_with_spaces)
 // {
 //     assert_expand_token("echo $SPACE", "echo Spa ce", NULL);
 // }

/*******************************************************************************/
/*                           Expand_dollar_node                                */
/*******************************************************************************/
t_list *expand_dollar_node(char *arg, size_t *i, t_exitstatus *status);

void assert_expand_dollar_node(char *input, char *expected)
{
    t_list *node;
    size_t i;

    i = 0;
    node = expand_dollar_node(input, &i, &zero);
    cr_expect_str_eq(node->content, expected);
    cr_assert(node->next == NULL);
    free(node->content);
    free(node);
}

TestSuite(expand_dollar_node, .init=setup_env);

 // Test(expand_dollar_node, double_env_var, .timeout=1)
 // {
 //     char input[] = "\"$HELLO\"";
 //     assert_expand_dollar_node(input, "Hello");
 // }

 // Test(expand_dollar_node, double_two_env_var, .timeout=1)
 // {
 //     char input[] = "\"$HELLO$WORLD\"";
 //     assert_expand_dollar_node(input, (char *[]){"Hello"});
 // }

/*******************************************************************************/
/*                           Are_quotes_closed                                 */
/*******************************************************************************/

bool are_quotes_closed(const char *str);

void assert_are_quotes_closed(char *input, bool expected)
{
    bool result = are_quotes_closed(input);
    cr_assert(result == expected);
}

Test(quotes_closed, empty_string)
{
    assert_are_quotes_closed("", true);
}

Test(quotes_closed, no_quotes)
{
    assert_are_quotes_closed("this has no quotes", true);
}

Test(quotes_are_closed, single_quotes)
{
    assert_are_quotes_closed("this 'has' single 'quotes'", true);
}

Test(quotes_are_closed, double_quotes)
{
    assert_are_quotes_closed("this \"has\" double \"quotes\"", true);
}

Test(quotes_are_closed, unmatched_single_quotes)
{
    assert_are_quotes_closed("this 'has unmatched single quotes", false);
}

Test(quotes_are_closed, unmatched_double_quotes)
{
    assert_are_quotes_closed("this \"has unmatched double quotes", false);
}

Test(quotes_are_closed, mixed_quotes)
{
    assert_are_quotes_closed("this 'has \"both\" types' of quotes", true);
}

Test(quotes_are_closed, escaped_quotes)
{
    assert_are_quotes_closed("this has escaped \\\"double\\\" quotes and \\'single\\' quotes", true);
}

Test(quotes_are_closed, multiple_lines)
{
    assert_are_quotes_closed("this has quotes\non multiple\nlines", true);
}

/*******************************************************************************/
/*                           Expand_tokens                                     */
/*******************************************************************************/

// void assert_expand_tokens(char *input, char *expected_output)
// {
//   t_list *arg_list;
//   char *result;
//
//   arg_list = ft_lstnew(ft_strdup(input));
//   expand_tokens(&arg_list, &zero);
//   result = arg_list->content;
//   cr_log_error("Input: %s", input);
//   cr_expect_str_eq(result, expected_output);
//   ft_lstclear(&arg_list, cr_free);
// }
//
// TestSuite(expand_tokens, .init=setup_env);
//
// Test(expand_tokens, empty_input)
// {
//   assert_expand_tokens("", "");
// }
//
// Test(expand_tokens, no_expansion)
// {
//     assert_expand_tokens("hello world", "hello world");
// }
//
// Test(expand_tokens, simple_expansion)
// {
//     assert_expand_tokens("echo $VAR", "echo value");
// }
//
// Test(expand_tokens, multiple_expansions)
// {
//     assert_expand_tokens("echo $ONE $TWO", "echo one two");
// }
//
// Test(expand_tokens, expansion_in_single_quotes)
// {
//     assert_expand_tokens("'echo $QSINGLE'", "echo $QSINGLE");
// }
//
// Test(expand_tokens, expansion_in_double_quotes)
// {
//     assert_expand_tokens("\"echo $QDOUBLE\"", "echo qdouble");
// }
//
// Test(expand_tokens, expansion_with_special_characters) 
// {
//     assert_expand_tokens("echo \"$SPECIEAL_CHAR\"", "echo \"special_char\"");
// }
//
// Test(expand_tokens, expansion_with_undefined_variable) 
// {
//     assert_expand_tokens("echo $UNDEFINED", "echo ");
// }
//
// Test(expand_tokens, expansion_with_exit_status) {
//     assert_expand_tokens("echo $?", "echo 123");
// }
//
// Test(expand_tokens, expansion_with_underscore) 
// {
//     assert_expand_tokens("echo $_", "echo minishell");
// }
