/*Reconstruct prompt*/

/*to compile: use filname.c mpc.c */
#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
#ifdef _WIN32
  #include <string.h>

  static char buffer[2048];

  char* readline(char* prompt){
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
  }

  void add_history(char* unused){}

#else
  #include <editline/readline.h>
  #include <editline/history.h>
#endif

int main (int argc, char** argv){
  /*Arithmetic grammars*/
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Maitri = mpc_new("maitri");

  /*Shiba Inu Grammars*/
  mpc_parser_t* Adjective = mpc_new("adjective");
  mpc_parser_t* Noun = mpc_new("noun");
  mpc_parser_t* Phrase = mpc_new("phrase");
  mpc_parser_t* Doge = mpc_new("doge");

  mpca_lang(MPCA_LANG_DEFAULT,
  "                                                   \
    number  : /-?[0-9]+[.?[0-9]*]*/;                  \
    operator: '+'|'-'|'*'|'/'|'%'|/add/|/sub/|/mul/|/div/;   \
    expr    : <number>  | '('  <operator> <expr>+ ')' | \
    '(' <expr>+ <operator> <expr>+ ')'; \
    maitri  : /^/ <operator> <expr>+  /$/ | /^/  <expr>+  <operator> <expr>+ /$/;         \
  ",/*% mark is highlighted as an error is because the
  Syntax highlighting is wrong*/
  Number,  Operator, Expr, Maitri);

  mpca_lang(MPCA_LANG_DEFAULT,
  "                                             \
    adjective: \"wow\"  | \"many\"              \
              | \"so\" |  \"such\";             \
    noun     : \"lisp\" | \"language\"        \
              | \"book\" |  \"build\" | \"c\";  \
    phrase   : <adjective> <noun>;              \
    doge     : /^/ <phrase>* /$/;                       \
  ",
  Adjective,  Noun, Phrase, Doge);

  puts("Maitri Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit");
  puts("This language was built from an online tutorial");

  while(1){
    /*Attempt to Parse the user Input*/

    mpc_result_t r;/*output result*/

    char* input = readline("Input> ");
    add_history(input);

    if(mpc_parse("<stdin>", input, Maitri, &r)){
      /*On Success Print the AST*/
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else if (mpc_parse("<stdin>", input, Doge, &r)){
      /*On Success Print the AST*/
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      /*Otherwise Print the Error*/
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
    /*char* input = readline("Input> ");

    add_history(input);

    printf("Output> %s\n",input);

    free(input);*/
  }

  mpc_cleanup(8, Number, Operator, Expr, Maitri, Adjective, Noun, Phrase, Doge);

  return 0;

  /*
  Write a regular expression matching strings of all a or b such
  as aababa or bbaa.
  */
  /*[a*b*]*/

  /*Write a regular expression matching strings of consecutive a
  and b such ababab or aba.
  */
  /*[[b?][ab]*[a?]]**/

  /*Write a regular
  expression matching pit, pot and respite but not peat,spit,part*/
  /*(res)?(pi?o?t)*(e)?*/
}
