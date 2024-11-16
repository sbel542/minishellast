typedef struct s_token {
	t_token_type type;
	char *value;
} t_token;

typedef enum e_token_type {
	TOKEN_COMMAND,
	TOKEN_ARGUMENT,
	TOKEN_PIPE,
	TOKEN_REDIRECT_OUT,
	TOKEN_REDIRECT_IN,
	TOKEN_HERE_DOC,
	TOKEN_APPEND,
	TOKEN_STRING,
} t_token_type;

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

t_state	handle_initial(char c)
{
	if (isspace(c))
	{
		/*create blank token*/
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
	  /*create pipe token*/
	  return (INITIAL);
	}
	return (READING_ARGUMENT);
}

t_state	handle_reading_word(char c)
{
	if (isspace(c))
	{
		/*create word token*/
		/*create blank token*/
		return (INITIAL);
	}
	else if (c == '\'')
	{
		/*create word token*/
		return (IN_SINGLE_QUOTE);
	}
	else if (c == '"')
	{
		/*create word token*/
		return (IN_DOUBLE_QUOTE);
	}
	else if (c == '>')
	{
		/*create word token*/
		return (CHECK_APPEND);
	}
	else if (c == '<')
	{
		/*create word token*/
		return (CHECK_HERE_DOC);
	}
	else if (c == '|')
	{
		/*create word token*/
		/*create pipe token*/
		return (INITIAL);
	}
	return (READING_WORD);
}

t_state	handle_in_single_quote(char c)
{
	if (c == '\'')
	{
		/*create single quoted string token*/
		return (INITIAL);
	}
	return (IN_SINGLE_QUOTE);
}

t_state	handle_in_double_quote(char c)
{
	if (c == '"')
	{
		
		/*create single quoted string token*/
		return (INITIAL);
	}
	return (IN_DOUBLE_QUOTE);
}

t_state	handle_check_append(char c)
{
	if (c == '>')
	{
		/*create >> token*/
		return (INITIAL);
	}
	/*create > token*/
	return (handle_initial(char c));
}

t_state	handle_check_here_doc(char c)
{
	if (c == '<')
	{
		/*create << token*/
		return (INITIAL);
	}
	/*create < token*/
	return (handle_initial(char c));
}

t_state	handle_reading_whitespace(char c)
{
	if (!isspace(c))
		return (handle_initial(char c));
	else
		return READING_WHITESPACE;
}

int	main(viod)
{
	t_state	current_state;
	char buffer[1024];

	current_state = INITIAL;
	while (input[i] != '\0')
	{
		if (current_state == INITIAL)
			current_state = handle_initial(input[i]);
		else if (current_state == READING_ARGUMENT)
			current_state = handle_reading(input[i]);
		else if (current_state == IN_SINGLE_QUOTE)
			current_state = handle_in_single_quote(input[i]);
		else if (current_state == IN_DOUBLE_QUOTE)
			current_state = handle_in_double_quote(input[i]);
		else if (current_state == CHECK_APPEND)
			current_state = handle_check_append(input[i]);
		else if (current_state == CHECK_HERE_DOC)
			current_state = handle_check_here_doc(input[i]);
		else if (current_state == READING_WHITESPACE)
			current_state = handle_reading_whitespace(input[i]);
		i++;
	}
}
