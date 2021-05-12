void clearBuffer(void);

void bufferChar(char c);

char trigraphReplacement(char c);

char digraphReplacementLess(char c);

char digraphReplacementColon(char c);

char digraphReplacementPerc(char c);

void tokenization();

void macroExpansion(char c);

void preprocess();

void createTempFile();

char skipSpaces(char c);

void createTempFile();

FILE* startPreprocess();

void closeLastFile();

bool openUserFile(char *fileName);

void freeNames();

typedef enum {
    DEFINE, INCLUDE, NODIRECTIVE
} directive;