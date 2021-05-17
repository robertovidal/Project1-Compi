scanner: main.c preprocessor.c trie.c scanner.c lex.yy.c
	flex scanner.l && gcc main.c preprocessor.c trie.c scanner.c lex.yy.c -lfl -Wall -o scanner -ggdb3