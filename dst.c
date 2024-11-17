#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 1024

typedef enum e_token_type {
	TOKEN_COMMAND,
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_OUT,
	TOKEN_REDIRECT_IN,
	TOKEN_HERE_DOC,
	TOKEN_APPEND,
	TOKEN_STRING,
	TOKEN_BLANK,
	TOKEN_END
} t_token_type;

typedef struct s_token {
	t_token_type type;
	char *value;
} t_token;

typedef enum e_state {
	INITIAL,
	READING_WORD,
	IN_SINGLE_QUOTE,
	IN_DOUBLE_QUOTE,
	CHECK_APPEND,
	CHECK_HERE_DOC,
	READING_WHITESPACE,
	END
} t_state;

typedef struct s_lexer {
	char buffer[1024];
	int buf_index;
	t_token *tokens[MAX_TOKENS];
	int token_count;
} t_lexer;

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
		return (INITIAL);
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

int	main()
{
	t_state	current_state;
	char 	buffer[1024];
	int		i;
	t_lexer lexer = { .buf_index = 0, .token_count = -1 };
	char input[] = "echo < 'Hello | World' | grep > txt >> output.txt";

	i = -1;
	current_state = INITIAL;
	while (input[++i] != '\0')
	{
		if (current_state == INITIAL)
			current_state = handle_initial(input[i], &lexer);
		else if (current_state == READING_WORD)
			current_state = handle_reading_word(input[i], &lexer);
		else if (current_state == IN_SINGLE_QUOTE)
			current_state = handle_in_single_quote(input[i], &lexer);
		else if (current_state == IN_DOUBLE_QUOTE)
			current_state = handle_in_double_quote(input[i], &lexer);
		else if (current_state == CHECK_APPEND)
			current_state = handle_check_append(input[i], &lexer);
		else if (current_state == CHECK_HERE_DOC)
			current_state = handle_check_here_doc(input[i], &lexer);
		else if (current_state == READING_WHITESPACE)
			current_state = handle_reading_whitespace(input[i], &lexer);
	}
	flush_buffer(&lexer, TOKEN_WORD);
	i = -1;
	while (++i < lexer.token_count)
        printf("Token Type: %d, Value: %s\n", lexer.tokens[i]->type, lexer.tokens[i]->value);

	free_tokens(&lexer);
    return (0);
}
