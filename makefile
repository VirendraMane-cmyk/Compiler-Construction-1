all:
	gcc -g -o compiler driver.c lexer.c parser.c keyword_table.c nary_tree.c stack.c ast.c symbol_table.c