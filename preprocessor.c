#include "preprocessor.h"

FILE* tmpF;

typedef Array(FILE*) Array_FILE;

Array_char buffer;
Array_FILE userFiles;
Array_chars names;
Array_char userFileName;
bool skipsEnter = false;
bool freeExp = false;
bool isStringOrChar = false;
bool ungetCalled = false;
int triOrDiChanged = 1;


struct Trie* head;

// Clears the buffer used for checking directives
void clearBuffer(){
    buffer.data[0] = '\0';
    deleteAllArray(buffer);
}

// Stores a char in the buffer which is used for
// checking directives
void bufferChar(char c){
    insertArray(buffer, char, c);
}

// Changes trigraphs which have two interrogation
// symbols (??) at the beginning
char trigraphReplacement(char c){
    triOrDiChanged = 3;
    switch(c){
        case '=':
            return '#';
        case '/':
            return '\\';
        case '\'':
            return '^';
        case '(':
            return '[';
        case ')':
            return ']';
        case '!':
            return '|';
        case '<':
            return '{';
        case '>':
            return '}';
        case '-':
            return '~';
        
    }
    triOrDiChanged = 2;
    ungetChar();
    return buffer.data[buffer.used-1];
}

// Changes digraphs with the less than (<) symbol
// at the begining
char digraphReplacementLess(char c){
    triOrDiChanged = 2;
    switch(c){
        case ':':
            return '[';
        case '%':
            return '{';
    }
    triOrDiChanged = 1;
    ungetChar();
    return buffer.data[buffer.used-1];
}

// Changes digraphs with the colon (:) symbol
// at the begining
char digraphReplacementColon(char c){
    triOrDiChanged = 2;
    switch(c){
        case '>':
            return ']';
    }
    triOrDiChanged = 1;
    ungetChar();
    return buffer.data[buffer.used-1];
}

// Changes digraphs with the percentage (%) symbol
// at the begining
char digraphReplacementPerc(char c){
    triOrDiChanged = 2;
    switch(c){
        case '>':
            return '}';
        case ':':
            return '#';
    
    }
    triOrDiChanged = 1;
    ungetChar();
    return buffer.data[buffer.used-1];
}

// After having found an alpha character it checks wether a
// whole token corresponds to a macro defined by the user
Array_char macroExpansion(Array_char macro){
    Array_char error = {NULL, 0, 0};
    char c;
    struct Trie* s = searchTrie(head, macro);
    if(s != NULL){
        if(s->vars.used > 0){
            Array_chars valsUser;
            initArray(valsUser, Array_char, 10);
            c = getChar(); // previously we ungot so we bring it back
            c = skipSpaces(c);
            if(c == '('){
                bool err = false;
                Array_char valM;
                initArray(valM, char, 10);
                do{
                    Array_char val;
                    initArray(val, char, 10);
                    int parenthesis = 1;
                    bool isDoubleQuote = false;
                    for(c = getChar(); !err && c != ',' && parenthesis > 0 && c != '\n' && c != EOF; c = getChar()){
                        if(c == '"'){
                            isDoubleQuote = !isDoubleQuote;
                        } else if(c == '('){
                            parenthesis++;
                        } else if(c == ')'){
                            parenthesis--;
                            if(parenthesis == 0){
                                break;
                            }
                        }
                        if(isalpha(c) && !isDoubleQuote){
                            for(; isalnum(c) || c == '_'; c = getChar()){
                                insertArray(valM, char, c);
                            }
                            insertArray(valM, char, '\0');
                            ungetChar(); // necessary if searchTrie returns NULL
                            Array_char m = macroExpansion(valM);
                            if(m.used > 0){
                                for(int i = 0; i < m.used && m.data[i] != '\0'; i++){
                                    insertArray(val, char, m.data[i]);
                                }
                                if(freeExp){
                                    freeArray(m);
                                    freeExp = false;
                                }
                            } else {
                                err = true;
                            }
                            deleteAllArray(valM);
                        } else {
                            insertArray(val, char, c);
                        }
                    }
                    insertArray(val, char, '\0'); // needed to use strcmp
                    insertArray(valsUser, Array_char, val);
                }while(c == ',' && !err);
                freeArray(valM);
                if(c == ')' && valsUser.used == s->vars.used && !err){
                    Array_char inserted;
                    Array_char replacement;
                    initArray(inserted, char, 10);
                    initArray(replacement, char, 10);
                    bool written = false;
                    bool isHashtag = false;
                    bool isDoubleQuote = false;
                    for(int i = 0; i < s->value.used; i++){
                        if(s->value.data[i] == '#'){
                            isHashtag = true;
                            i++;
                        } else if(s->value.data[i] == '"'){
                            isDoubleQuote = !isDoubleQuote;
                        }
                        if(!isDoubleQuote){
                            for(;i < s->value.used && ((i == 0 && isalpha(s->value.data[i])) || (i > 0 && (isalnum(s->value.data[i]) || s->value.data[i] == '_'))); i++){
                                insertArray(inserted, char, s->value.data[i]);
                            }
                        }
                        if(inserted.used > 0){
                            insertArray(inserted, char, '\0'); // needed to use strcmp
                            written = false;
                            for(int j = 0; j < s->vars.used; j++){
                                // variable of macro is found
                                if(strcmp(s->vars.data[j].data,inserted.data) == 0){
                                    written = true;
                                    if(isHashtag){
                                        insertArray(replacement, char, '"');
                                    }
                                    for(int k = 0; k < valsUser.data[j].used && valsUser.data[j].data[k] != '\0'; k++){
                                        insertArray(replacement, char, valsUser.data[j].data[k]);
                                    }
                                    if(s->value.used > i && s->value.data[i] != '\0'){
                                        insertArray(replacement, char, s->value.data[i]);
                                    }
                                    if(isHashtag){
                                        insertArray(replacement, char, '"');
                                        isHashtag = false;
                                    }
                                    break;
                                }
                            }
                            if(!written){
                                if(isHashtag){
                                    jumpToEnd(c);
                                    freeArray(inserted);
                                    freeArray(replacement);
                                    freeArrayP(valsUser);
                                    return error;
                                }
                                for(int k = 0; k < inserted.used && inserted.data[k] != '\0'; k++){
                                    insertArray(replacement, char, inserted.data[k]);
                                }
                                if(s->value.used > i && s->value.data[i] != '\0'){
                                    insertArray(replacement, char, s->value.data[i]);
                                }
                            }
                            deleteAllArray(inserted);
                        } else {
                            if(s->value.data[i] != '\0'){
                                insertArray(replacement, char, s->value.data[i]);
                            }
                        }
                    }
                    freeArray(inserted);
                    freeExp = true;
                    freeArrayP(valsUser);
                    return replacement;
                } else {
                    freeArrayP(valsUser);
                    return error;
                }
                freeArrayP(valsUser);
            } else {
                return error;
            }
        } else if(s->value.used > 0){
            return s->value;
        }
    }
    return macro;
}

// Returns the corresponding directive acording to
// the buffered string
directive checkDirective(){
    directive dir = NODIRECTIVE;
    ungetChar();
    if(strcmp(buffer.data,"include") == 0){
        dir = INCLUDE;
    } else if(strcmp(buffer.data,"define") == 0){
        dir = DEFINE;
    }

    return dir;
}

// Does the preprocess
void preprocess(){
    if(userFiles.used == 0){
        return;
    }

    char inChar;
    Array_char macro;
    initArray(macro, char, 10);

    while ((inChar = getChar()) != EOF){

        // This part is for the other types of expressions
        // that can be found in the preprocessing
        if (isalpha(inChar)){
            for(; isalnum(inChar) || inChar == '_'; inChar = getChar()){
                insertArray(macro, char, inChar);
            }
            insertArray(macro, char, '\0'); 
            ungetChar(); // necessary if searchTrie returns NULL
            Array_char replacement = macroExpansion(macro);
            if(replacement.used > 0){
                for(int i = 0; i < replacement.used && replacement.data[i] != '\0'; i++){
                    fputc(replacement.data[i], tmpF);
                }
                if(freeExp){
                    freeArray(replacement);
                    freeExp = false;
                }
            } else {
                deleteAllArray(macro);
                jumpToEnd(inChar);
                insertBufferInFile();
            }
            clearBuffer();
            deleteAllArray(macro);
            continue;
        } else if(inChar ==  '/'){
            inChar = getChar();
            if(inChar == '/'){
                do{
                    inChar = getChar();
                }while(inChar != '\n');
            } else if(inChar == '*'){
                do{
                    inChar = getChar();
                    if(inChar == '*'){
                        inChar = getChar();
                        if(inChar == '/')
                            break;
                    }
                } while(inChar != EOF);
                clearBuffer();
                continue;
            } else {
                fputc('/', tmpF);
            }
            clearBuffer();
        } else if(inChar ==  '#'){
            clearBuffer();
            for(inChar = getChar(); isalpha(inChar); inChar = getChar()){}
            bool error = false;
            switch(checkDirective()){
                case DEFINE:
                    inChar = skipSpaces(inChar);
                    Array_char name = { NULL, 0, 0 };
                    Array_char value = { NULL, 0, 0 };
                    Array_chars vars = { NULL, 0, 0 };
                    bool hasParameter = false;
                    if(inChar != '\n'){
                        initArray(name, char, 20);
                        initArray(value, char, 20);
                        for(;inChar != ' ' && !error; inChar = getChar()){
                            if(inChar == '('){
                                hasParameter = true;
                                if(name.used > 0){
                                    initArray(vars, Array_char, 10);
                                    do{
                                        Array_char v;
                                        initArray(v, char, 10);
                                        if(inChar == '(' || inChar == ','){
                                            inChar = getChar();
                                        } else {
                                            freeArray(v);
                                            freeArray(vars);
                                            error = true;
                                            break;
                                        }
                                        inChar = skipSpaces(inChar);
                                        for(;isalnum(inChar); inChar = getChar()){
                                            insertArray(v, char, inChar);
                                        }
                                        insertArray(v, char, '\0');
                                        insertArray(vars, Array_char, v);
                                        inChar = skipSpaces(inChar);
                                        if(inChar == '\n' || v.used == 1){
                                            freeArray(v);
                                            freeArray(vars);
                                            error = true;
                                            break;
                                        }
                                    }while(inChar != ')');
                                } else {
                                    error = true;
                                    break;
                                }
                            } else {
                                if((name.used == 0 && isalpha(inChar)) || (name.used > 0 && isalnum(inChar))){
                                    insertArray(name, char, inChar);
                                } else if((name.used == 0 && !isalpha(inChar)) || inChar == ')'){
                                    error = true;
                                }
                            }
                        }
                        inChar = skipSpaces(inChar);
                        bool isHashtag = false;
                        Array_char hashtagTrue;
                        initArray(hashtagTrue, char, 10);
                        for(;inChar != '\n' && !error; inChar = getChar()){
                            if(isHashtag && inChar != ' ' && hasParameter){
                                insertArray(hashtagTrue, char, inChar);
                            } else if(isHashtag && inChar == ' ' && hasParameter){
                                insertArray(hashtagTrue, char, '\0');
                                for(int i = 0; i < vars.used; i++){
                                    if(strcmp(vars.data[i].data, hashtagTrue.data) == 0){
                                        isHashtag = false;
                                    }
                                }
                                if(isHashtag){
                                    error = true;
                                    break;
                                }
                            } else if(isalpha(inChar)){
                                for(; isalnum(inChar) || inChar == '_'; inChar = getChar()){
                                    insertArray(macro, char, inChar);
                                }
                                insertArray(macro, char, '\0');
                                ungetChar(); // necessary if searchTrie returns NULL
                                Array_char replacement = macroExpansion(macro);
                                if(replacement.used > 0){
                                    for(int i = 0; i < replacement.used && replacement.data[i] != '\0'; i++){
                                        insertArray(value, char, replacement.data[i]);
                                    }
                                    if(freeExp){
                                        freeArray(replacement);
                                        freeExp = false;
                                    }
                                } else {
                                    deleteAllArray(macro);
                                    error = true;
                                    break;
                                }
                                deleteAllArray(macro);
                                continue;
                            }
                            if(inChar == '#'){
                                isHashtag = true;
                            }
                            if(skipsEnter){
                                insertArray(value, char, '\n');    
                            }
                            insertArray(value, char, inChar);
                        }
                        if(isHashtag && hasParameter){
                            insertArray(hashtagTrue, char, '\0');
                            for(int i = 0; i < vars.used; i++){
                                if(strcmp(vars.data[i].data, hashtagTrue.data) == 0){
                                    isHashtag = false;
                                }
                            }
                            if(isHashtag){
                                error = true;
                            }
                        }
                        freeArray(hashtagTrue);
                    } else {
                        error = true;
                    }
                    if(error || value.used == 0 || name.used == 0){
                        jumpToEnd(inChar);
                        fputc('#', tmpF);
                        insertBufferInFile();
                        freeArrayP(vars);
                        freeArray(name);
                        freeArray(value);
                    } else {
                        insertArray(names, Array_char, name);
                        insertTrie(head, name, value, vars);
                    }
                    break;
                case INCLUDE:
                    inChar = skipSpaces(inChar);
                    if(inChar == '<' || inChar == '"'){
                        char end = '>';
                        if(inChar == '"'){
                            end = '"';
                        }
                        for(inChar = getChar();
                            inChar != end; inChar = getChar()){
                            if(inChar == '\n'){
                                error = true;
                                break;
                            }
                            insertArray(userFileName, char, inChar);
                        }
                        insertArray(userFileName, char, '\0');
                        if(!error){
                            if(!openUserFile(userFileName.data)){
                                jumpToEnd(inChar);
                                fputc('#', tmpF);
                                insertBufferInFile();
                            }
                        }
                    } else {
                        error = true;
                    }
                    if(error){
                        jumpToEnd(inChar);
                        fputc('#', tmpF);
                        insertBufferInFile();
                    }
                    deleteAllArray(userFileName);
                    break;
                case NODIRECTIVE:
                    jumpToEnd(inChar);
                    fputc('#', tmpF);
                    insertBufferInFile();
                    break;
            }
            continue;
        }
        fputc(inChar, tmpF);
        clearBuffer();
    }
    freeArray(macro);
    closeLastFile();
    preprocess();
}

// Skips every space and tab found in the current file
char skipSpaces(char c){
    while(c != '\n' && (isspace(c) || c == '\t')){
        c = getChar();
    }
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
    initArray(buffer, char, 10);
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
    freeArray(buffer);
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

// Frees every array inside the array of names and 
// clears the trie
void freeNames(){
    for(int i = 0; i < names.used; i++){
        deletionTrie(&head, names.data[i].data);
        freeArray(names.data[i]);
    }
    freeArray(names);
}

// Inserts in the file all chars stored in the buffer 
// and clears the buffer
void insertBufferInFile(){
    for(int i = 0; i < buffer.used; i++){
        fputc(buffer.data[i], tmpF);
    }
    clearBuffer();
}

// Retrieves every char from the user files and if
// there is a backslash symbol and an enter it skips
// them
char getChar(){
    skipsEnter = false;
    triOrDiChanged = 1;
    char c = getc(userFiles.data[userFiles.used-1]);
    if((c == '"' || c == '\'') && !ungetCalled){
        isStringOrChar = !isStringOrChar;
    }
    ungetCalled = false;
    if(c == '\\'){
        char c2 = getc(userFiles.data[userFiles.used-1]);
        if(c2 == '\n'){
            c = getc(userFiles.data[userFiles.used-1]);
            skipsEnter = true;
        } else {
            fputc(c, tmpF);
            return c2;
        }
    }
    bufferChar(c);
    if(!isStringOrChar){
        // This part changes di and trigraphs
        if(c ==  '<'){
            c = getc(userFiles.data[userFiles.used-1]);
            bufferChar(c);
            c = digraphReplacementLess(c);
        } else if(c ==  ':'){
            c = getc(userFiles.data[userFiles.used-1]);
            bufferChar(c);
            c = digraphReplacementColon(c);
        } else if(c ==  '%'){
            c = getc(userFiles.data[userFiles.used-1]);
            bufferChar(c);
            c = digraphReplacementPerc(c);
        } else if(c ==  '?'){
            c = getc(userFiles.data[userFiles.used-1]);
            bufferChar(c);
            if(c == '?'){
                c = getc(userFiles.data[userFiles.used-1]);
                bufferChar(c);
                c = trigraphReplacement(c);
            } else {
                ungetChar();
                c = buffer.data[buffer.used-1];
            }
        }
    }
    return c;
}

// Returns last char in the buffer to the file
void ungetChar(){
    ungetCalled = true;
    for(int i = 1; i <= triOrDiChanged; i++){
        ungetc(buffer.data[buffer.used - i], userFiles.data[userFiles.used-1]);
    }
    buffer.data[buffer.used - triOrDiChanged] = '\0';
    buffer.used = buffer.used - triOrDiChanged;
    triOrDiChanged = 1;
}

// Skips every char until it finds the end of the line
// or the end of the file
void jumpToEnd(char c){
    while(c != '\n' && c != EOF){
        c = getChar();
    }
}