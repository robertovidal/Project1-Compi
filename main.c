#include "global.h"
#include "preprocessor.h"
#include "scanner.h"

int main(int argc, char *argv[]){
    if(argc == 1)
        printf("The name of the file is missing.");
    if(argc == 2){
        FILE *f = startPreprocess(argv[1]);
        fclose(f);
        startScan(f);
        system("cd latex && pdflatex scanner.tex && mv ./scanner.pdf ../result.pdf && cd .. && evince result.pdf");
    }
    return 0;
}