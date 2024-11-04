#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft/libft.h"

typedef enum e_node_type {
	COMMAND,
	ARGUMENT,
	PIPE,
	REDIRECTION
} t_node_type;

typedef struct s_ast_node {
	t_node_type type;
	char *value;
	struct s_ast_node *left;
	struct s_ast_node *right;
} t_ast_node;

t_ast_node *create_node(t_node_type type, const char *value)
{
	t_ast_node *node = malloc(sizeof(t_ast_node));
	if (!node) return NULL;
	node->type = type;
	node->value = ft_strdup(value);
	node->left = NULL;
	node->right = NULL;
	return (node);
}

void free_ast(t_ast_node *node) {
	if (!node) return;
	free(node->value);
	free_ast(node->left);
	free_ast(node->right);
	free(node);
}

t_ast_node *parse_command(char **tokens, int *token_pos)
{
	const char *token = tokens[*token_pos];
	if (!token) return NULL;
	t_ast_node *command_node = create_node(COMMAND, token);
	(*token_pos)++;
	t_ast_node *arg_list = NULL;
	t_ast_node *last_arg = NULL;

	while (tokens[*token_pos] && ft_strncmp(tokens[*token_pos], "|", ft_strlen(tokens[*token_pos])) != 0 && ft_strncmp(tokens[*token_pos], ">", ft_strlen(tokens[*token_pos])) != 0)
	{
		t_ast_node *arg_node = create_node(ARGUMENT, tokens[*token_pos]);
		(*token_pos)++;
		if (!arg_list)
			arg_list = arg_node;
		else
			last_arg->right = arg_node;
		last_arg = arg_node;
	}
	command_node->left = arg_list;
	return (command_node);
}

t_ast_node *parse_redirection(char **tokens, int *token_pos)
{
	t_ast_node *command_node = parse_command(tokens, token_pos);
	if (!command_node)
		return (NULL);
	if (tokens[*token_pos] && strcmp(tokens[*token_pos], ">") == 0)
	{
		t_ast_node *redir_node = create_node(REDIRECTION, ">");
		(*token_pos)++;
		if (tokens[*token_pos])
		{
			redir_node->right = create_node(ARGUMENT, tokens[*token_pos]);
			(*token_pos)++;
		}
		redir_node->left = command_node;
		return (redir_node);
	}
	return (command_node);
}

t_ast_node *parse_pipe(char **tokens, int *token_pos)
{
	t_ast_node *left_node = parse_redirection(tokens, token_pos);
	if (!left_node)
		return (NULL);
	while (tokens[*token_pos] && strcmp(tokens[*token_pos], "|") == 0)
	{
		t_ast_node *pipe_node = create_node(PIPE, "|");
		(*token_pos)++;
		pipe_node->left = left_node;
		pipe_node->right = parse_redirection(tokens, token_pos);
		left_node = pipe_node;
	}
	return (left_node);
}

t_ast_node *parse_expression(char **tokens, int *token_pos)
{
	return (parse_pipe(tokens, token_pos));
}

void print_ast(t_ast_node *node, int depth)
{
    if (!node)
		return;
    for (int i = 0; i < depth; i++)
        ft_printf("  ");
    ft_printf("%s: %s\n", 
        node->type == COMMAND ? "COMMAND" :
        node->type == ARGUMENT ? "ARGUMENT" :
        node->type == PIPE ? "PIPE" :
        node->type == REDIRECTION ? "REDIRECTION" : "UNKNOWN",
        node->value ? node->value : "NULL");
    if (node->left)
	{
        for (int i = 0; i < depth + 1; i++)
            ft_printf("  ");
        ft_printf("Left Child -> ");
        print_ast(node->left, depth + 2);
    }
	else
	{
        for (int i = 0; i < depth + 1; i++)
            ft_printf("  ");
        ft_printf("Left Child -> NULL\n");
    }
    if (node->right)
	{
        for (int i = 0; i < depth + 1; i++)
            ft_printf("  ");
        ft_printf("Right Child -> ");
        print_ast(node->right, depth + 2);
    }
	else
	{
        for (int i = 0; i < depth + 1; i++)
            ft_printf("  ");
        ft_printf("Right Child -> NULL\n");
    }
}

int main(int argc, char **argv)
{
	char	**tokens;
	int		token_pos;

	if (argc < 2)
	{
		ft_printf("Usage: %s <command>\n", argv[0]);
		return (1);
	}
	tokens = ft_split(argv[1], ' ');
	token_pos = 0;
	t_ast_node *ast = parse_expression(tokens, &token_pos);
	ft_printf("Abstract Syntax Tree:\n");
	print_ast(ast, 0);
	free_ast(ast);
	for (int i = 0; tokens[i] != NULL; i++)
		free(tokens[i]);
	free(tokens);

	return (0);
}
