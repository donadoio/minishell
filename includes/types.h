#ifndef TYPES_H
#define TYPES_H

typedef enum s_type
{
	COMMAND,
	PIPE,
	INPUT_REDIRECTION,
	OUTPUT_REDIRECTION
}	t_type;

typedef struct s_token
{
	t_type type;
	char *content;
}	t_token;

typedef struct s_node
{
	t_token token;
	struct s_node *next;
}	t_node;

#endif