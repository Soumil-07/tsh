tsh : tsh.tab.o tsh.lex.o
	$(CC) -g -o $@ tsh.tab.o tsh.lex.o -ly -ll -lreadline

# tell make which files yacc will generate
#
# an explanation of the arguments:
# -b tsh  -  name the files tsh.tab.*
# -d        -  generate a .tab.h file too

tsh.tab.h tsh.tab.c : tsh.y
	$(YACC) -d -b tsh $?

# the object file relies on the generated lexer, and
# on the token constants 

tsh.lex.o : tsh.tab.h tsh.lex.c

# can't use the default suffix rule because we're
# changing the name of the output to .lex.c

tsh.lex.c : tsh.l
	$(LEX) -t $? > $@
