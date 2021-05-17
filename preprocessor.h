#include "global.h"
#include "trie.h"
#include "array.h"

void clearBuffer(void);

void bufferChar(char c);

char trigraphReplacement(char c);

char digraphReplacementLess(char c);

char digraphReplacementColon(char c);

char digraphReplacementPerc(char c);

void tokenization();

Array_char macroExpansion(Array_char macro);

void preprocess();

void createTempFile();

char skipSpaces(char c);

void createTempFile();

FILE* startPreprocess();

void closeLastFile();

bool openUserFile(char *fileName);

void freeNames();

void insertBufferInFile();

char getChar();

void ungetChar();

typedef enum {
    DEFINE, INCLUDE, NODIRECTIVE
} directive;