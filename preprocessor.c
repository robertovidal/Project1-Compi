#include "global.h"
#include "preprocessor.h"
#include "trie.h"
#include "array.h"

FILE* tmpF;

typedef Array(Array_char) Array_chars;
typedef Array(FILE*) Array_FILE;

char buffer[9] = "";
Array_FILE userFiles;
Array_chars names;
Array_char macro;
Array_char userFileName;

struct Trie* head;

// Clears the buffer used for checking directives
void clearBuffer(){
    strcpy(buffer, "");
}

// Stores a char in the buffer which is used for
// checking directives
void bufferChar(char c){
    size_t buffer_length = strlen(buffer);
    buffer[buffer_length] = c;
    buffer[buffer_length + 1] = '\0';
}

// Changes trigraphs which have two interrogation
// symbols (??) at the beginning
char trigraphReplacement(char c){
    switch(c){
        case '=':
            return '#';
            break;
        case '/':
            return '\\';
            break;
        case '\'':
            return '^';
            break;
        case '(':
            return '[';
            break;
        case ')':
            return ']';
            break;
        case '!':
            return '|';
            break;
        case '<':
            return '{';
            break;
        case '>':
            return '}';
            break;
        case '-':
            return '~';
            break;
        
    }
    fputc('?', tmpF);
    fputc('?', tmpF);
    return c;
}

// Changes digraphs with the less than (<) symbol
// at the begining
char digraphReplacementLess(char c){
    switch(c){
        case ':':
            return '[';
            break;
        case '%':
            return '{';
            break;
    }
    fputc('<', tmpF);
    return c;
}

// Changes digraphs with the colon (:) symbol
// at the begining
char digraphReplacementColon(char c){
    switch(c){
        case '>':
            return ']';
            break;
    }
    fputc(':', tmpF);
    return c;
}

// Changes digraphs with the percentage (%) symbol
// at the begining
char digraphReplacementPerc(char c){
    switch(c){
        case '>':
            return '}';
            break;
        case ':':
            return '#';
            break;
    
    }
    fputc('%', tmpF);
    return c;
}

// After having found an alpha character it checks wether a
// whole token corresponds to a macro defined by the user
void macroExpansion(char c){
    for(; isalnum(c) || c == '_';
        c = getc(userFiles.data[userFiles.used-1])){
        insertArray(macro, char, c);
    }
    ungetc(c, userFiles.data[userFiles.used-1]);
    insertArray(macro, char, '\0');
    Array_char s = searchTrie(head, macro);
    if(s.data != NULL){
        for(int i = 0; i < s.used; i++){
            fputc(s.data[i], tmpF);
        }
    } else {
        for(int i = 0; i < macro.used-1; i++){
            fputc(macro.data[i], tmpF);
        }
    }
    deleteAllArray(macro);
}

// Returns the corresponding directive acording to
// the buffered string
directive checkDirective(){
    directive dir = NODIRECTIVE;
    if(strcmp(buffer,"include") == 0){
        dir = INCLUDE;
    } else if(strcmp(buffer,"define") == 0){
        dir = DEFINE;
    }
    clearBuffer();
    return dir;
}

// Does the preprocess
void preprocess(){
    char inChar;
    if(userFiles.used == 0){
        return;
    }
    while ((inChar = getc(userFiles.data[userFiles.used-1])) != EOF){
        // This first part changes di and trigraphs
        if(inChar ==  '<'){
            inChar = getc(userFiles.data[userFiles.used-1]);
            inChar = digraphReplacementLess(inChar);
        } else if(inChar ==  ':'){
            inChar = getc(userFiles.data[userFiles.used-1]);
            inChar = digraphReplacementColon(inChar);
        } else if(inChar ==  '%'){
            inChar = getc(userFiles.data[userFiles.used-1]);
            inChar = digraphReplacementPerc(inChar);
        } else if(inChar ==  '?'){
            if(inChar == '?'){
                inChar = getc(userFiles.data[userFiles.used-1]);
                inChar = trigraphReplacement(inChar);
            }
        }

        // This part is for the other types of expressions
        // that can be found in the preprocessing
        if (isalpha(inChar)){
            macroExpansion(inChar);
            continue;
        } else if(inChar ==  '\\'){
            inChar = getc(userFiles.data[userFiles.used-1]);
            if(inChar ==  '\n')
                continue;
            else
                fputc('\\', tmpF);
        } else if(inChar ==  '/'){
            inChar = getc(userFiles.data[userFiles.used-1]);
            if(inChar == '/'){
                bool next = false;
                do{
                    if(next) next = false;
                    inChar = getc(userFiles.data[userFiles.used-1]);
                    if(inChar == '\\'){
                        next = true;
                    }
                }while(inChar != '\n' || next);
            } else if(inChar == '*'){
                do{
                    inChar = getc(userFiles.data[userFiles.used-1]);
                    if(inChar == '*'){
                        inChar = getc(userFiles.data[userFiles.used-1]);
                        if(inChar == '/')
                            break;
                    }
                } while(inChar != EOF);
                continue;
            } else {
                fputc('/', tmpF);
            }
        } else if(inChar ==  '#'){
            for(inChar = getc(userFiles.data[userFiles.used-1]);
                isalpha(inChar); inChar = getc(userFiles.data[userFiles.used-1])){
                bufferChar(inChar);
            }
            switch(checkDirective()){
                case DEFINE:
                    inChar = skipSpaces(inChar);
                    Array_char name;
                    Array_char value;
                    initArray(name, char, 20);
                    initArray(value, char, 20);
                    for(;inChar != ' '; inChar = getc(userFiles.data[userFiles.used-1])){
                        insertArray(name, char, inChar);
                    }
                    inChar = skipSpaces(inChar);
                    for(;inChar != '\n'; inChar = getc(userFiles.data[userFiles.used-1])){
                        insertArray(value, char, inChar);
                    }
                    insertArray(names, Array_char, name);
                    insertTrie(head, name, value);
                    break;
                case INCLUDE:
                    inChar = skipSpaces(inChar);
                    if(inChar == '<' || inChar == '"'){
                        char end = '>';
                        if(inChar == '"') end = '"';
                        for(inChar = getc(userFiles.data[userFiles.used-1]);
                            inChar != end; inChar = getc(userFiles.data[userFiles.used-1])){
                            insertArray(userFileName, char, inChar);
                        }
                        insertArray(userFileName, char, '\0');
                        if(!openUserFile(userFileName.data)){
                            fputs("#include ", tmpF);
                            if(end == '"')
                                fputc('"', tmpF);
                            else
                                fputc('<', tmpF);
                            for(int i = 0; i < userFileName.used-1; i++){
                                fputc(userFileName.data[i], tmpF);
                            }
                            fputc(end, tmpF);
                        }
                    }
                    deleteAllArray(userFileName);
                    break;
                case NODIRECTIVE:
                    fputc('#', tmpF);
                    for(int i = 0; i < 100; i++){
                        fputc(buffer[i], tmpF);
                    }
                    break;
            }
            continue;
        }
        fputc(inChar, tmpF);
    }
    closeLastFile();
    preprocess();
}

// Skips every space and tab found in the current file
char skipSpaces(char c){
    while(isspace(c) || c == '\t')
        c = getc(userFiles.data[userFiles.used-1]);
    fputc(' ', tmpF);
    return c;
}

// Creates the temporal file
void createTempFile(){
    tmpF = tmpfile();
    if(tmpF == NULL){
        printf("Could not create temporal file.\n");
        exit(EXIT_FAILURE);
    }
}

// Initializes and frees all global arrays, the trie and
// starts the preprocessing
FILE* startPreprocess(char *fileName){
    head = getNewTrieNode();
    initArray(macro, char, 10);
    initArray(userFileName, char, 10);
    initArray(names, Array_char, 10);
    initArray(userFiles, FILE*, 10);
    openUserFile(fileName);
    createTempFile();
    preprocess();
    closeLastFile();
    freeNames();
    freeArray(userFiles);
    freeArray(userFileName);
    freeArray(macro);
    return tmpF;
}

// Closes the last file in the array of files
void closeLastFile(){
    if(userFiles.used > 0){
        userFiles.used--;
        fclose(userFiles.data[userFiles.used]);
    }
}

// Opens a file given its name
bool openUserFile(char *fileName){
    FILE *userFile = fopen(fileName, "r");
    if(userFile == NULL){
        printf("Could not read %s\n", fileName);
        return false;
    }
    insertArray(userFiles, FILE*, userFile);
    return true;
}

// Frees every array inside the array of names
void freeNames(){
    for(int i = 0; i < names.used; i++){
        freeArray(names.data[i]);
    }
    freeArray(names);
}