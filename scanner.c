#include "global.h"
#include "scanner.h"
FILE *file;

char *token_type_text_color(token_code code){
    switch (code){
    case CONSTANT:
        return "2A1200";
        break;
    case KEYWORD:
        return "162511";
        break;
    case OPERATOR:
        return "290B26";
        break;
    case SPECIALCHAR:
        return "FFF8A0";
        break;
    case IDENT:
        return "84A8FA";
        break;
    case STRING:
        return "B5B5B5";
        break;
    case ERROR:
        return "FF0000";
        break;
    default:
        return "FF0000";
        break;
    }
}

char *token_type_color_box(token_code code){
    switch (code){
    case CONSTANT:
        return "FFA562";
        break;
    case KEYWORD:
        return "5BF62A";
        break;
    case OPERATOR:
        return "9F7BBD";
        break;
    case SPECIALCHAR:
        return "413C00";
        break;
    case IDENT:
        return "15006A";
        break;
    case STRING:
        return "161616";
        break;
    case ERROR:
        return "300707";
        break;
    default:
        return "300707";
        break;
    }
}

char *token_type_font(token_type type){
    switch (type){
    case CONSTANT:
        return "pcr";
        break;
    case KEYWORD:
        return "lmdh";
        break;
    case OPERATOR:
        return "cmtt";
        break;
    case SPECIALCHAR:
        return "qag";
        break;
    case IDENT:
        return "ptm";
        break;
    case STRING:
        return "cmss";
        break;
    case ERROR:
        return "ppl";
        break;
    default:
        return "ppl";
        break;
    }
}

token_type get_token_type(token_code code){
    switch (code){
    case INTVALUE:
    case FLOATVALUE:
    case CHARVALUE:
        return CONSTANT;
        break;
    case AUTO:
    case BREAK:
    case CASE:
    case CHAR:
    case CONST:
    case DEFAULT:
    case DO:
    case DOUBLE:
    case ELSE:
    case ENUM:
    case EXTERN:
    case FLOAT:
    case FOR:
    case GOTO:
    case IF:
    case INT:
    case LONG:
    case REGISTER:
    case RETURN:
    case SHORT:
    case SIGNED:
    case SIZEOF:
    case STATIC:
    case STRUCT:
    case SWITCH:
    case TYPEDEF:
    case UNION:
    case UNSIGNED:
    case VOID:
    case VOLATILE:
    case WHILE:
        return KEYWORD;
        break;
    case INCREMENTOP:
    case DECREMENTOP:
    case EQUALOP:
    case NOTEQUALOP:
    case GREATEROREQUALTHANOP:
    case LESSOREQUALTHANOP:
    case OPENINGANGLEBRACKET:
    case CLOSINGANGLEBRACKET:
    case ANDOP:
    case OROP:
    case LEFTSHIFTOP:
    case RIGHTSHIFTOP:
    case ADDASSIGNMENTOP:
    case SUBSTRACTASSIGNMENTOP:
    case MULTASSIGNMENTOP:
    case DIVASSIGNMENTOP:
    case MODASSIGNMENTOP:
    case LEFTSHIFTASSIGNMENTOP:
    case RIGHTSHIFTASSIGNMENTOP:
    case BITWISEASSIGNMENTOP:
    case BITWISEEXCLUSIVEASSIGNMENTOP:
    case BITWISEINCLUSIVEASSIGNMENTOP:
    case VIRGULILLA:
    case EXCLAMATIONMARK:
    case PERCENTSIGN:
    case CARET:
    case AMPERSAND:
    case ASTERISK:
    case PLUSSIGN:
    case VERTICALSIGN:
    case MINUSSIGN:
    case EQUALSIGN:
    case QUESTIONMARK:
    case SLASH:
        return OPERATOR;
        break;
    case NUMBERSIGN:
    case DOLLARSIGN:
    case LEFTPARENTHESIS:
    case RIGHTPARENTHESIS:
    case UNDERSCORE:
    case BACKSLASH:
    case APOSTROPHE:
    case LEFTBRACE:
    case RIGHTBRACE:
    case LEFTBRACKET:
    case RIGHTBRACKET:
    case COLON:
    case SEMICOLON:
    case SIMPLEQUOTATIONMARK:
    case COMMA:
    case PERIOD:
        return SPECIALCHAR;
        break;
    case IDENTIFIER:
        return IDENT;
        break;
    
    case STRINGVALUE:
        return STRING;
        break;
    case OTHER:
        return ERROR;
        break;
    default:
        return ERROR;
        break;
    }
}

void insert_string(char *token_str){
    int position = 0;
    while (token_str[position] != '\0'){
        if(token_str[position] == '{')
            fputs(" \\{ ", file);
        else if(token_str[position] == '}')
            fputs(" \\} ", file);
        else if(token_str[position] == '%')
            fputs(" \\% ", file);
        else if(token_str[position] == '$')
            fputs(" \\$ ", file);
        else if(token_str[position] == '#')
            fputs(" \\# ", file);
        else if(token_str[position] == '&')
            fputs(" \\& ", file);
        else if(token_str[position] == '_')
            fputs(" \\_ ", file);
        else if(token_str[position] == '~')
            fputs(" \\~{} ", file);
        else if(token_str[position] == '^')
            fputs(" \\^ ", file);
        else if(token_str[position] == '\\')
            fputs(" \\textbackslash ", file);
        else if(token_str[position] == '<')
            fputs(" $<$ ", file);
        else if(token_str[position] == '>')
            fputs(" $>$ ", file);
        else
            fputc(token_str[position], file);
        position++;
    }
}


void startScan(FILE *tmpF) {
    change_input(tmpF);
    file = fopen("latex/code.tex", "w");
    if(file == NULL){
        printf("Cannot open latex/code.tex");
        exit(0);
    }
    struct tokens elToken = Get_Token();
    char *token_color_text_str;
    char *token_color_box_str;
    char *token_font;
    int lines = 0;
    char latex[] = "\\section{Programa Fuente} \n\n \t\\frame{\\sectionpage} \n\n\\begin{frame}{Formato de tokens} \n\t\t\\begin{enumerate} \n\t\t\t \\item {\\textcolor[HTML]{162511}{\\colorbox[HTML]{5BF62A}{\\fontfamily{lmdh} \\selectfont \n\t\t\t\tPalabras Reservadas% \n\t\t\t}}} \n\t\t\t\\item {\\textcolor[HTML]{290B26}{\\colorbox[HTML]{9F7BBD}{\\fontfamily{cmtt}\\selectfont \n\t\t\t\tOperadores%\n\t\t\t}}} \n\t\t\t\\item {\\textcolor[HTML]{B5B5B5}{\\colorbox[HTML]{161616}{\\fontfamily{cmss}\\selectfont \n\t\t\t\tString% \n\t\t\t}}} \n\t\t\t\\item {\\textcolor[HTML]{2A1200}{\\colorbox[HTML]{FFA562}{\\fontfamily{pcr}\\selectfont \n\t\t\t\tConstantes%\n\t\t\t}}} \n\t\t\t\\item {\\textcolor[HTML]{FFF8A0}{\\colorbox[HTML]{413C00}{\\fontfamily{qag}\\selectfont \n\t\t\t\tCaracteres Especiales% \n\t\t\t}}} \n\t\t\t\\item {\\textcolor[HTML]{84A8FA}{\\colorbox[HTML]{15006A}{\\fontfamily{ptm}\\selectfont \n\t\t\t\tIdentificadores% \n\t\t\t}}} \n\t\t\t\\item {\\textcolor[HTML]{FF0000}{\\colorbox[HTML]{300707}{\\fontfamily{ppl}\\selectfont \n\t\t\t\tErrores Léxicos% \n\t\t\t}}} \n\t\t\\end{enumerate} \n\t\\end{frame} \n\n\t\\begin{frame}[allowframebreaks, noframenumbering]{Programa Fuente}\n\t\t";
    fputs(latex, file);
    char latexInsert[1000];
    token_type type;
	while(elToken.code != END_OF_FILE){
        if(elToken.code == TAB)
            fputs(" \\hspace*{1em}", file);
        else if(elToken.code == NEWLINE)
            fputs(" \\newline \n\t\t", file);
        else{
            type = get_token_type(elToken.code);
            token_color_box_str = token_type_color_box(type);
            token_color_text_str = token_type_text_color(type);
            token_font = token_type_font(type);
            fputs(" \\textcolor[HTML]{", file);
            fputs(token_color_text_str, file);
            fputs("}{\\colorbox[HTML]{", file);
            fputs(token_color_box_str, file);
            fputs("}{\\fontfamily{", file);
            fputs(token_font, file);
            fputs("}\\selectfont ", file);
            if(elToken.code == INTVALUE){
                char number[100];
                sprintf(number, "%d}}", elToken.valueint);
                fputs(number, file);
            }
            else{
                insert_string(elToken.valuechar);
                fputs("}}", file);
            }
        }
		elToken = Get_Token();
	}
    fputs("\n\t\\end{frame}", file);
    fclose(file);
    finish();
}