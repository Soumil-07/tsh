%{
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "tsh.h"
#include "ansi_colors.h"
#include <readline/readline.h>

int yylex(void);
int yyerror(const char* s);
void set_input_string(const char* in);
void end_lexical_scan(const char*);

int status;
int history_expanded;
char* expansion;
%}

%token WORD NEWLINE GREAT GREATGREAT GREATGREATAMPERSAND LESS AMPERSAND PIPE NOTOKEN GREATAMPERSAND EXCLAMATION

%union
{
    char* strval;
    int intval;
}

%type<strval> WORD EXCLAMATION;

%%
goal: command_list;

command_list:
            command_list command_line
            | /* empty */
                ;

command_line:
        pipe_list io_modifier_list background_optional NEWLINE { status = execute(); }
        | EXCLAMATION {
            history_expand($1, &expansion);
            history_expanded = 1;
            
        }
        | NEWLINE
        | error NEWLINE { yyerrok; }
            ;

pipe_list:
         pipe_list PIPE cmd_and_args
         | cmd_and_args
            ;

cmd_and_args:
        WORD arg_list {
            /* counterintuitively, the cmd is found *after* all the arguments, so here we need to add the cmd to the front of the arglist
            and then add it to the command array. after that, it's time to reset the simpleCurrCommand for the next list of arguments */
            add_curr_simple_cmd_name($1);
        }
            ;

arg_list:
        arg_list WORD { add_arg($2); }
        | /* empty */
            ;


io_modifier:
           GREATGREAT WORD { printf(">> %s\n", $2); }
           | GREAT WORD { set_outfile($2); }
           | GREATGREATAMPERSAND WORD { printf(">>& %s\n", $2); }
           | GREATAMPERSAND WORD { printf(">& %s\n", $2); }
           | LESS WORD { set_infile($2); }
            ;

io_modifier_list:
            io_modifier_list io_modifier
            | /* empty */
                ;

background_optional:
            AMPERSAND | /* empty */
                ;

%%

int main()
{
    do {
        // get the current working dir for the shell prompt
        char *cwd = getcwd(NULL, 0);

        // usually shells replace $HOME with ~ in shell prompts. Here we try to
        // replicate the same behavior in tsh.
        char *homedir = getenv("HOME");
        if (homedir != NULL) {
          // replacing a substr in C has to be much easier than this.
          size_t cwdlen = strlen(cwd);
          size_t homedirlen = strlen(homedir);
          // since we're replacing the home directory with a single character "~",
          // the new buffer should be CWDLEN - HOMEDIRLEN + LEN("~") + 1 (null
          // terminator)
          char *buffer = malloc(cwdlen - homedirlen + 1 + 1);
          buffer[0] = '~';
          int ptr = 1;    // the current position in the new buffer.
          int cwdpos = 0; // the current position in the cwd buffer.

          while (homedirlen > 0) {
            homedirlen--;
            cwdpos++;
          }

          for (int i = cwdpos; i < cwdlen; i++) {
            buffer[ptr++] = cwd[i];
          }
          buffer[ptr] = '\0';
          cwd = buffer;
        }

        char str[500];
        sprintf(str, FGBLUE("%s > "), cwd);
        char* line = readline(str);
        int n = strlen(line);

        line = realloc(line, strlen(line) + 1);
        line[n] = '\n';
        line[n + 1] = '\0';

        set_input_string(line);
        int result = yyparse();
        
        if (result == 0 && history_expanded == 1) {
            set_input_string(expansion);
            yyparse();
            add_history(expansion);
            history_expanded = 0;
            free(expansion);
            expansion = NULL;
        } else {
            add_history(line);
        }

        /* end_lexical_scan(line); */
        reset();
        /* free(homedir);
        free(cwd); */
    } while (status);
}

int yyerror(const char* s)
{
    fprintf(stderr, "%s", s);
}
