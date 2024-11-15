/*=======================================================================*/
/* Project: mini shell                                                   */
/* Module : shell scanner                                                */
/*                                                                       */
/* This program is free software: you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation, either version 3 of the License, or     */
/* any later version.                                                    */
/*=======================================================================*/

%top {
/* ===================================================================== */
/* Project: minishell                                                    */
/* Module : shell scanner                                                */
/* ===================================================================== */
}

%{
#include <stdlib.h>
enum yytokentype
{
	WORD = 258,
	NOTOKEN = 259,
	NEWLINE = 260,
	PIPE = 261,
	DLESS = 262,
	DGREAT = 263,
	LESS = 264,
	GREAT = 265
};

typedef struct
{
	int token_id;
	const char *str;
} lt_entry;

lt_entry lt[12] = {
	{ .token_id = WORD, .str =    "WORD" },
	{ .token_id = NOTOKEN, .str = "NOTOKEN" },
	{ .token_id = NEWLINE, .str = "NEWLINE" },
	{ .token_id = PIPE, .str =    "PIPE" },
	{ .token_id = DLESS, .str =   "DLESS" },
	{ .token_id = DGREAT, .str =  "DGREAT" },
	{ .token_id = LESS, .str =    "LESS" },
	{ .token_id = GREAT, .str =    "GREAT" }
};

/* Value type.  */
union YYSTYPE
{
	char *string_val;
} yylval;

%}

/* option batch OU interactive */
%option nostdinit
%option noyywrap
%option noinput
%option nounput

%%

[ \t]* 				{	/* Discard spaces and tabs */ }
\n	 				{ yylval.string_val = NULL; return NEWLINE; }
">"					{ yylval.string_val = NULL; return GREAT; }
">>"				return DGREAT;
"<"					return LESS;
"<<"				return DLESS;
"|"					return PIPE;
"exit"				exit (0);
[a-zA-Z0-9\[\]]*    {
						yylval.string_val = strdup(yytext);
						return WORD;
					}

\'[^\n\']*\'		{
						/* handle arguments in quotes by stripping the quotes */
						//yytext = (char *)(yytext+1); // drop the front quotes
						//yytext[strlen(yytext) - 1] = '\0'; // drop the end quotes
						yylval.string_val = strdup(yytext);
						return WORD;
					}
.					{ return NOTOKEN; }
<<EOF>>				{ fprintf(yyout, "%s\n", "Hey-ho!"); yyterminate(); }
%%


const char *get_string(int token)
{
	int i;
	const char *str = NULL;

	i = -1;
	while (++i < 8)
	{
		if (lt[i].token_id == token)
		{
			str = lt[i].str;
			break ;
		}
	}
	return str;
}

int	main(void)
{
	int tok;
	tok = yylex();
	while(tok)
	{
		printf("\"%s\": \"%s\"\n", get_string(tok), yylval.string_val);
		tok = yylex();
	}
	return (EXIT_SUCCESS);
}
