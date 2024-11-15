#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "libft/libft.h"

#define MAX_TOKENS 1024


typedef enum e_char_type {
	CHAR_WHITESPACE,
	CHAR_ALPHA,
	CHAR_DIGIT,
	CHAR_SPECIAL,
	CHAR_QUOTE,
	CHAR_OTHER
} t_char_type;

typedef enum e_token_type {
	TOKEN_COMMAND,
	TOKEN_ARGUMENT,
	TOKEN_PIPE,
	TOKEN_REDIRECT_OUT,
	TOKEN_REDIRECT_IN,
	TOKEN_STRING,
	TOKEN_END
} t_token_type;

typedef struct s_token {
	t_token_type type;
	char *value;
} t_token;

t_char_type get_char_type(char c)
{
	if (isspace(c))
		return (CHAR_WHITESPACE);
	if (ft_isalpha(c))
		return (CHAR_ALPHA);
	if (ft_isdigit(c))
		return (CHAR_DIGIT);
	if (c == '|' || c == '<' || c == '>')
		return (CHAR_SPECIAL);
	if (c == '"' || c == '\'')
		return (CHAR_QUOTE);
	return (CHAR_OTHER);
}

t_token *create_token(t_token_type type, const char *value)
{
	t_token *token = malloc(sizeof(t_token));
	if (!token) return NULL;
	token->type = type;
	token->value = ft_strdup(value);
	return (token);
}

void free_tokens(t_token **tokens)
{
	int i = 0;
	while (tokens[i] != NULL)
	{
		free(tokens[i]->value);
		free(tokens[i]);
		i++;
	}
	free(tokens);
}

t_token **tokenize_input(const char *input)
{
	t_token **tokens = malloc(sizeof(t_token *) * MAX_TOKENS);
	int token_count = 0;
	char buffer[1024];
	int buf_index = 0;
	char current_quote = 0;
	int i = 0;

	while (input[i] != '\0')
	{
		t_char_type char_type = get_char_type(input[i]);
		if (current_quote != 0)
		{
			if (input[i] == current_quote)
				current_quote = 0;
			else
				buffer[buf_index++] = input[i];
			i++;
		}
		else if (char_type == CHAR_QUOTE)
			current_quote = input[i++];
		else if (char_type == CHAR_WHITESPACE)
		{
			if (buf_index > 0)
			{
				buffer[buf_index] = '\0';
				tokens[token_count++] = create_token(TOKEN_ARGUMENT, buffer);
				buf_index = 0;
			}
			i++;
		}
		else if (char_type == CHAR_SPECIAL)
		{
		
			if (buf_index > 0)
			{
				buffer[buf_index] = '\0';
				tokens[token_count++] = create_token(TOKEN_ARGUMENT, buffer);
				buf_index = 0;
			}
			if (input[i] == '|')
				tokens[token_count++] = create_token(TOKEN_PIPE, "|");
			else if (input[i] == '>')
				tokens[token_count++] = create_token(TOKEN_REDIRECT_OUT, ">");
			else if (input[i] == '<')
				tokens[token_count++] = create_token(TOKEN_REDIRECT_IN, "<");
			i++;
		}
		else
			buffer[buf_index++] = input[i++];
	}
	if (buf_index > 0)
	{
		buffer[buf_index] = '\0';
		tokens[token_count++] = create_token(TOKEN_ARGUMENT, buffer);
	}
	tokens[token_count] = NULL;
	return tokens;
}

int main(int argc, char **argv)
{
	int i;
	t_token **tokens;

	if (argc < 2) {
		ft_printf("Usage: %s <command>\n", argv[0]);
		return (1);
	}
	tokens = tokenize_input(argv[1]);
	i = 0;
	while (tokens[i] != NULL)
	{
		ft_printf("Token Type: %d, Value: %s\n", tokens[i]->type, tokens[i]->value);
		i++;
	}

	free_tokens(tokens);
	return (0);
}
