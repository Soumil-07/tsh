%{

#include <stdio.h>
#include "tsh.tab.h"
%}

%%

\n  {
        return NEWLINE;
    }
[ \t] {

    } 
">"     { 
                return GREAT;
        } 
"<"     { 
                return LESS;
        } 
">>"    { 
                return GREATGREAT;
        } 
">>&"    { 
                return GREATGREATAMPERSAND;
        } 
">&"    { 
                return GREATAMPERSAND;
        } 
"|"     { 
                return PIPE;
        } 
"&"     { 
                return AMPERSAND;
        } 
^"!"[!|0-9]* {
                yylval.strval = strdup(yytext);
                return EXCLAMATION;
        }
[^ \t\n][^ \t\n]*  { 
                /* Assume that file names have only alpha chars */ 
                yylval.strval = strdup(yytext);
                return WORD;
        } 

.       { 
                /* Invalid character in input */ 
                return NOTOKEN;
        } 

%%

void set_input_string(const char* in) {
  YY_BUFFER_STATE sbuf = yy_scan_string(in);
  yy_switch_to_buffer(sbuf);
}

void end_lexical_scan(const char* in) {
  yy_delete_buffer(in);
}
