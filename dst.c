typedef enum e_state {
	INITIAL,
	READING_ARGUMENT,
	IN_SINGLE_QUOTE,
	IN_DOUBLE_QUOTE,
	CHECK_APPEND,
	CHECK_HERE_DOC,
	READING_SPECIAL,
	READING_WHITESPACE,
	END
} t_state;

t_state	handle_initial(char c)
{

}

t_state handle_transition(t_state current_state, char c)
{

    switch (current_state)
	{
		case INITIAL:
			if (isspace(c))
				current_state = READING_WHITESPACE;
			if (c == '\'')
				current_state = (IN_SINGLE_QUOTE);
			if (c == '"')
				current_state = (IN_DOUBLE_QUOTE);
			if (c == '>')
				current_state = (CHECK_APPEND);
			if (c == '<')
				current_state = (CHECK_HERE_DOC);
			if (c == '|')
			{
				/*create pipe token*/
				current_state = (INITIAL);
			}
				current_state = (READING_ARGUMENT);
			break;
		case READING_ARGUMENT:
			current_state = handle_reading(input[i]);
		break;
	  	case IN_SINGLE_QUOTE:
			current_state = handle_in_single_quote(input[i]);
		break;
		case IN_DOUBLE_QUOTE:
			current_state = handle_in_double_quote(input[i]);
		break;
		case CHECK_APPEND:
			current_state = handle_check_append(input[i]);
		case CHECK_HERE_DOC:
			current_state = handle_check_here_doc(input[i]);
		case READING_SPECIAL:
			current_state = handle_reading_special(input[i]);
		case READING_WHITESPACE:
			current_state = handle_reading_whitespace(input[i]);
    }
	return (current_state);
}

t_state	handle_reading(char c)
{

}

t_state	handle_in_single_quote(char c)
{

}

t_state	handle_in_double_quote(char c)
{

}

t_state	handle_check_append(char c)
{

}

t_state	handle_check_here_doc(char c)
{

}

t_state	handle_reading_special(char c)
{

}

t_state	handle_reading_whitespace(char c)
{
	
}

int	main(viod)
{
	int i;
	char c;
	t_state	current_state;

	current_state = INITIAL;

	char input[1024] = {0};

	ft_strcat(input, "echo 123");

	i = -1;
	c = input[++i];
	while (c != '\0')
	{
		current_state = handle_transition(current_state, c);
		c = input[++i];
	}

}