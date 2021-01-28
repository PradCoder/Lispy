/*
Pesara Amarasekera
Changed to handle arrowkeys in multiple OS's
*/
#include <stdio.h>
#include <stdlib.h>

/*If we are compiling on Windows compile these functions*/
#ifdef _WIN32
#include <string.h>
/*Declare a buffer for user input of size 2048*/
static char buffer[2048];

/*Fake readline function*/
char* readline(char* prompt){
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

/*Fake add history function*/
void add_history(char* unused){}

/*Otherwise include the editline headers*/
#else
#include <editline/readline.h>
/*Not needed on a Mac*/
//#include <editline/history.h>
#endif

int main (int argc, char** argv){
    /*Print Version and Exit Information*/
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctr+c to Exit");
    puts("This language was built from an online tutorial and I added more as to make the language more \"usable\"\n");
    /*In a never ending loop*/
    while(1){

      /*Output out prompt*/
      /*fputs("lispy> ",stdout);*/
      /*Now in either case readline will be correctly defined*/
      char* input = readline("Maitri Input> ");

      /*Read a line of user input of maximum size 2048*/
      /*fgets(input, 2048, stdin);*/
      add_history(input);

      /*Echo input back to user*/
      printf("Output> %s\n", input);

      /*Free retrieved input*/
      free(input);

      /*the \n is a white space character, it's
      an escape sequence representing a newline*/

      /*Other patterns that can be used with printf*/
      /*
      %d,%i - int; signed decimal
      %o - int; unsigned octal
      %x,X - int; unsigned hexadecimal notation
      %u - int; unsigned decimal notation
      %c - int; single character after conversion to unsigned char
      %s - char *; characters from a string
      %f - double; decimal notation of form [-]mmm.ddd, where the number of d's
          is specified by the precision
      %e,%E -double; decimal notation of the form [-]m.ddde(+/-)xx or
            [-]m.dddE(+/-)xx, where the number of d's is specified by the precision
      %g,%G - double; %e and E is used of the exponent is less than -4 or greater
              than or equal to the precision; otherwise %f is used.Trailing zeros
              and a trailing decimal point are not
      %p - void *; print as a pointer
      %n - int *; the number of characters written so far by this call to printf
      %% - no argument is converted; print a %;
      */

      /*
      the preprocessor command #ifndef do?
        it checks whether a name is defined.
      */

      /*the preprocessor command #define do?
        it is used to define something(a string) with a keyword(symbolic name or
      symbolic constant)
      */

      /*
        if _WIN32 is defined on Windows
        then on linux/Mac <editline/readline.h> and <editline/history.h> are
        defined
      */
    }
    return 0;
}
