#include "preprocessor.h"

FILE* tmpF;

typedef Array(FILE*) Array_FILE;

Array_char buffer;
Array_FILE userFiles;
Array_chars names;
Array_char userFileName;
bool skipsEnter = false;
bool freeExp = false;


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
    clearBuffer();
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
    clearBuffer();
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
    clearBuffer();
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
    clearBuffer();
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
                                    while(c != '\n'){
                                        c = getChar();
                                    }
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
    char c = buffer.data[buffer.used-1];
    if(c != '\0'){
        buffer.data[buffer.used-1] = '\0';
    }
    if(strcmp(buffer.data,"include") == 0){
        dir = INCLUDE;
    } else if(strcmp(buffer.data,"define") == 0){
        dir = DEFINE;
    }
    buffer.data[buffer.used-1] = c;
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
        // This first part changes di and trigraphs
        if(inChar ==  '<'){
            inChar = getChar();
            inChar = digraphReplacementLess(inChar);
        } else if(inChar ==  ':'){
            inChar = getChar();
            inChar = digraphReplacementColon(inChar);
        } else if(inChar ==  '%'){
            inChar = getChar();
            inChar = digraphReplacementPerc(inChar);
        } else if(inChar ==  '?'){
            if(inChar == '?'){
                inChar = getChar();
                inChar = trigraphReplacement(inChar);
            }
        }

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
                while(inChar != '\n' && inChar != EOF){
                    inChar = getChar();
                }
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
        } else if(inChar ==  '#'){
            clearBuffer();
            for(inChar = getChar(); isalpha(inChar); inChar = getChar()){}
            bool error = false;
            switch(checkDirective()){
                case DEFINE:
                    inChar = skipSpaces(inChar);
                    Array_char name;
                    Array_char value;
                    Array_chars vars = { NULL, 0, 0 };
                    initArray(name, char, 20);
                    initArray(value, char, 20);
                    for(;inChar != ' '; inChar = getChar()){
                        if(inChar == '('){
                            initArray(vars, Array_char, 10);
                            do{
                                Array_char v;
                                initArray(v, char, 10);
                                inChar = skipSpaces(inChar);
                                for(inChar=getChar();inChar != ',' && inChar != ')'; inChar = getChar()){
                                    insertArray(v, char, inChar);
                                }
                                insertArray(v, char, '\0');
                                insertArray(vars, Array_char, v);
                                inChar = skipSpaces(inChar);
                                if(inChar == '\n'){
                                    error = true;
                                    break;
                                }
                            }while(inChar != ')');
                        } else {
                            insertArray(name, char, inChar);
                        }
                    }
                    inChar = skipSpaces(inChar);
                    bool isHashtag = false;
                    Array_char hashtagTrue;
                    initArray(hashtagTrue, char, 10);
                    for(;inChar != '\n'; inChar = getChar()){
                        if(isHashtag && inChar != ' '){
                            insertArray(hashtagTrue, char, inChar);
                        } else if(isHashtag && inChar == ' '){
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
                                while(inChar != '\n' && inChar != EOF){
                                    inChar = getChar();
                                }
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
                    if(isHashtag){
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
                    if(error || value.used == 0 || name.used == 0){
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
                            insertArray(userFileName, char, inChar);
                        }
                        insertArray(userFileName, char, '\0');
                        if(!openUserFile(userFileName.data)){
                            fputc('#', tmpF);
                            insertBufferInFile();
                        }
                    } else {
                        fputc('#', tmpF);
                        insertBufferInFile();
                    }
                    deleteAllArray(userFileName);
                    break;
                case NODIRECTIVE:
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
    while(isspace(c) || c == '\t')
        c = getChar();

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
    if(buffer.data[buffer.used-1] != '\0'){
        insertArray(buffer, char, '\0');
    }
    for(int i = 0; i < buffer.used-1; i++){
        fputc(buffer.data[i], tmpF);
    }
    clearBuffer();
}

// Retrieves every char from the user files and if
// there is a backslash symbol and an enter it skips
// them
char getChar(){
    skipsEnter = false;
    char c = getc(userFiles.data[userFiles.used-1]);
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
    return c;
}

// Returns last char in the buffer to the file
void ungetChar(){
    buffer.used--;
    ungetc(buffer.data[buffer.used], userFiles.data[userFiles.used-1]);
    buffer.data[buffer.used] = '\0';
}
