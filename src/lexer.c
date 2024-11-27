/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbelcher <sbelcher@student.42london.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 15:20:53 by sbelcher          #+#    #+#             */
/*   Updated: 2024/11/15 15:20:54 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

typedef struct
{
	int token_id;
	const char *str;
} lt_entry;

lt_entry lt[12] = {
	{ .token_id = TOKEN_COMMAND,		.str = "TOKEN_COMMAND" },
	{ .token_id = TOKEN_WORD,			.str = "TOKEN_WORD" },
	{ .token_id = TOKEN_PIPE,			.str = "TOKEN_PIPE" },
	{ .token_id = TOKEN_REDIRECT_OUT,	.str = "TOKEN_REDIRECT_OUT" },
	{ .token_id = TOKEN_REDIRECT_IN,	.str = "TOKEN_REDIRECT_IN" },
	{ .token_id = TOKEN_HERE_DOC,		.str = "TOKEN_HERE_DOC" },
	{ .token_id = TOKEN_APPEND,			.str = "TOKEN_APPEND" },
	{ .token_id = TOKEN_STRING,			.str = "TOKEN_STRING" },
	{ .token_id = TOKEN_BLANK,			.str = "TOKEN_BLANK" },
	{ .token_id = TOKEN_END,			.str = "TOKEN_END" }
};

t_token *create_token(t_token_type type, const char *value, t_lexer *lexer)
{
	t_token *token = malloc(sizeof(t_token));
	if (!token) return NULL;
	token->type = type;
	token->value = strdup(value);
	lexer->token_count++;
	return (token);
}

void flush_buffer(t_lexer *lexer, t_token_type type)
{
	if (lexer->buf_index > 0)
	{
		lexer->buffer[lexer->buf_index] = '\0';
		lexer->tokens[lexer->token_count] = create_token(type, lexer->buffer, lexer);
		lexer->buf_index = 0;
	}
}

void	free_tokens(t_lexer *lexer)
{
	int	i;

	i = -1;
	while (++i < lexer->token_count)
	{
		free(lexer->tokens[i]->value);
		free(lexer->tokens[i]);
		lexer->token_count = 0;
	}
}

t_state	handle_initial(char c, t_lexer *lexer)
{
	if (isspace(c))
	{
		lexer->tokens[lexer->token_count] = create_token(TOKEN_BLANK, " ", lexer);
	  return (READING_WHITESPACE);
	}
	else if (c == '\'')
	  return (IN_SINGLE_QUOTE);
	else if (c == '"')
	  return (IN_DOUBLE_QUOTE);
	else if (c == '>')
	  return (CHECK_APPEND);
	else if (c == '<')
	  return (CHECK_HERE_DOC);
	else if (c == '|')
	{
	  lexer->tokens[lexer->token_count] = create_token(TOKEN_PIPE, "|", lexer);
	  return (INITIAL);
	}
	lexer->buffer[lexer->buf_index++] = c;
	return (READING_WORD);
}

t_state	handle_reading_word(char c, t_lexer *lexer)
{
	if (isspace(c))
	{
		flush_buffer(lexer, TOKEN_WORD);
		lexer->tokens[lexer->token_count] = create_token(TOKEN_BLANK, " ", lexer);
		return (READING_WHITESPACE);
	}
	else if (c == '\'')
	{
		flush_buffer(lexer, TOKEN_WORD);
		return (IN_SINGLE_QUOTE);
	}
	else if (c == '"')
	{
		flush_buffer(lexer, TOKEN_WORD);
		return (IN_DOUBLE_QUOTE);
	}
	else if (c == '>')
	{
		flush_buffer(lexer, TOKEN_WORD);
		return (CHECK_APPEND);
	}
	else if (c == '<')
	{
		flush_buffer(lexer, TOKEN_WORD);
		return (CHECK_HERE_DOC);
	}
	else if (c == '|')
	{
		flush_buffer(lexer, TOKEN_WORD);
		lexer->tokens[lexer->token_count] = create_token(TOKEN_PIPE, "|", lexer);
		return (INITIAL);
	}
	lexer->buffer[lexer->buf_index++] = c;
	return (READING_WORD);
}

t_state	handle_in_single_quote(char c, t_lexer *lexer)
{
	if (c == '\'')
	{
		flush_buffer(lexer, TOKEN_WORD);
		return (INITIAL);
	}
	lexer->buffer[lexer->buf_index++] = c;
	return (IN_SINGLE_QUOTE);
}

t_state	handle_in_double_quote(char c, t_lexer *lexer)
{
	if (c == '"')
	{
		
		flush_buffer(lexer, TOKEN_WORD);
		return (INITIAL);
	}
	lexer->buffer[lexer->buf_index++] = c;
	return (IN_DOUBLE_QUOTE);
}

t_state	handle_check_append(char c, t_lexer *lexer)
{
	if (c == '>')
	{
		lexer->tokens[lexer->token_count] = create_token(TOKEN_APPEND, ">>", lexer);
		return (INITIAL);
	}
	lexer->tokens[lexer->token_count] = create_token(TOKEN_REDIRECT_OUT, ">", lexer);
	return (handle_initial(c, lexer));
}

t_state	handle_check_here_doc(char c, t_lexer *lexer)
{
	if (c == '<')
	{
		lexer->tokens[lexer->token_count] = create_token(TOKEN_HERE_DOC, "<<", lexer);
		return (INITIAL);
	}
	lexer->tokens[lexer->token_count] = create_token(TOKEN_REDIRECT_IN, "<", lexer);
	return (handle_initial(c, lexer));
}

t_state	handle_reading_whitespace(char c, t_lexer *lexer)
{
	if (!isspace(c))
		return (handle_initial(c, lexer));
	else
		return READING_WHITESPACE;
}

const char *get_idstring(int token)
{
	int i;
	const char *str = NULL;

	i = -1;
	while (++i < 11)
	{
		if (lt[i].token_id == token)
		{
			str = lt[i].str;
			break ;
		}
	}
	return str;
}

void	preprocess_tokens(t_token **tokens, int *token_count)
{
	int		i;
	int		j;
	int		k;
	int		start;
	int		total_length;
	char	*concatenated_value;
	
	i = 0;
	j = 0;
	while (i < *token_count)
	{
		if (tokens[i]->type == TOKEN_WORD)
		{
			start = i;
			total_length = 0;
			while (i < *token_count && tokens[i]->type == TOKEN_WORD)
				total_length += ft_strlen(tokens[i++]->value);
			concatenated_value = malloc(total_length + 1);
			if (!concatenated_value)
			{
				ft_printf("Error: Memory allocation failed.\n");
				exit(EXIT_FAILURE);
			}
			concatenated_value[0] = '\0';
			k = start;
			while (k < i)
			{
				strcat(concatenated_value, tokens[k]->value);
				free(tokens[k]->value);
				free(tokens[k++]);
			}
			tokens[j] = malloc(sizeof(t_token));
			if (!tokens[j])
			{
				ft_printf("Error: Memory allocation failed.\n");
				free(concatenated_value);
				exit(EXIT_FAILURE);
			}
			tokens[j]->type = TOKEN_WORD;
			tokens[j]->value = concatenated_value;
			j++;
		}
		else if (tokens[i]->type != TOKEN_BLANK)
		{
			if (i != j)
				tokens[j] = tokens[i];
			j++;
			i++;
		}
		else
		{
			free(tokens[i]->value);
			free(tokens[i]);
			i++;
		}
	}
	*token_count = j;
}

int main(void)
{
	t_state current_state;
	int i;
	t_lexer lexer = { .buf_index = 0, .token_count = 0 };
	char ps[1024];
	int	token_pos;
	char *line = NULL;

	sprintf(ps, "> ");
	line = readline(ps);
	while (line)
	{
		i = -1;
		current_state = INITIAL;
		while (line[++i] != '\0')
		{
			if (current_state == INITIAL)
				current_state = handle_initial(line[i], &lexer);
			else if (current_state == READING_WORD)
				current_state = handle_reading_word(line[i], &lexer);
			else if (current_state == IN_SINGLE_QUOTE)
				current_state = handle_in_single_quote(line[i], &lexer);
			else if (current_state == IN_DOUBLE_QUOTE)
				current_state = handle_in_double_quote(line[i], &lexer);
			else if (current_state == CHECK_APPEND)
				current_state = handle_check_append(line[i], &lexer);
			else if (current_state == CHECK_HERE_DOC)
				current_state = handle_check_here_doc(line[i], &lexer);
			else if (current_state == READING_WHITESPACE)
				current_state = handle_reading_whitespace(line[i], &lexer);
		}
		flush_buffer(&lexer, TOKEN_WORD);
		ft_printf("Tokens before preprocess:\n");
		for (i = 0; i < lexer.token_count; i++)
			printf("Token Type: \"%s\", Value: %s\n",
				   get_idstring(lexer.tokens[i]->type), lexer.tokens[i]->value);
		preprocess_tokens(lexer.tokens, &lexer.token_count);
		printf("\n\nTokens after preprocess:\n");
		for (i = 0; i < lexer.token_count; i++)
			printf("Token Type: \"%s\", Value: %s\n",
				   get_idstring(lexer.tokens[i]->type), lexer.tokens[i]->value);
		token_pos = 0;
		t_ast_node *ast = parse_pipeline(lexer.tokens, &token_pos);
		if (!ast)
		{
			ft_printf("Error: Failed to parse the command.\n");
			free_tokens(&lexer);
			free(line);
			line = readline(ps);
		}
		ft_printf("\n\nAbstract Syntax Tree:\n");
		print_ast(ast, 0);
		free_ast(ast);
		free_tokens(&lexer);
		free(line);
		line = readline(ps);
	}
	free(line);
	return (0);
}
