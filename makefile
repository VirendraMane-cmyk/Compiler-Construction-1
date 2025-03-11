lexer: lexer.o keyword_table.o driver.o
	 gcc lexer.o keyword_table.o driver.o -o myprogram

lexer.o: lexer.c lexer.h lexerDef.h
	 gcc -w -c lexer.c

keyword_table.o: keyword_table.c keyword_table.h keyword_tableDef.h
	 gcc -w -c keyword_table.c

driver.o: driver.c lexer.h keyword_table.h
	 gcc -w -c driver.c
