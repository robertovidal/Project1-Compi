# Project1-Compi
Students:
Jose Pablo Fernández Cubillo, 2019047740
Roberto Vidal Patiño, 2019065537

First of all youn need to have installed Latex
and all its functionalities, which can be done
with the command:

sudo apt install texlive-full

Then there is a makefile which compiles the project.
First the part related to flex is compiled (the scanner)
which results in a lex.yy.c file, following that this file
along with the other c files are compiled using gcc.

If you don't have flex installed already it can be done
with:

sudo apt-get install flex

We are using the files program.c and 