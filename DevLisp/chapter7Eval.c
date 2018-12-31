/*
Pesara Amarasekera
2018-11-10
This file contains the program for evaluating the expressions in
the tree structure
This is specifically an abstract syntax tree

TODO:   complete debugging for segmentation fault 
        Also finish Bonus questions for chapter 7
*/

/*The structure inside the header file

typedef struct mpc_ast_t {
    char* tag; 
    char* contents;
    mpc_state_t state;
    int children_num;
    struct mpc_ast_t** children;
} mpc_ast_t;

*/

/*#include <stdio.h>
#include <stdlib.h>*/
#include "mpc.h"
#ifdef _WIN32
    #include <string.h>

    static char buffer[2048];
    char* readline(char* prompt){
        fputs(prompt,stdout);
        fgets(buffer, 2048, stdin);
        char* cpy = malloc(strlen(buffer)+1);
        strcpy(cpy,buffer);
        cpy[strlen(cpy)-1] = '\0';
        return cpy;
    }

    void add_history(char* unused){}
#else
    #include <editline/readline.h>
    #include <editline/history.h>
#endif

long eval(mpc_ast_t* t);
long eval_op(long x, char* op, long y);
long countBranches(mpc_ast_t* t);
long countleaves(mpc_ast_t* t);

/*count number of children at each layer*/
/*long countBranches(mpc_ast_t* t){
    return 0;
}*/

/*count number of children in the last layer*/
long countLeaves(mpc_ast_t* t){
    if(t->children_num==0){
        return 1;
    }

    int x = 0;
    int i = 0;
    while(t->children_num>0){
        x += countLeaves(t->children[i]);
        i++;
    }
    return x;
}

long eval(mpc_ast_t* t){
    /*If tagged as number return it directly*/
    if(strstr(t->tag,"number")){
        return atoi(t->contents);
    }

    /*The operator is always the second child*/
    char* op = t->children[1]->contents;

    /*We store the third child in 'x'*/
    long x = eval(t->children[2]);

    /*Iterate the remaining children and combining*/
    int i = 3;
    while(strstr(t->children[i]->tag,"expr")){
        x = eval_op(x,op, eval(t->children[i]));
        i++;
    }

    return x;
}

long eval_op(long x, char* op, long y){
    if ( strcmp(op,"+") == 0 ){ return x+y; }
    if ( strcmp(op,"-") == 0 ){ return x-y; }
    if ( strcmp(op,"*") == 0 ){ return x*y; }
    if ( strcmp(op,"/") == 0 ){ return x/y; }
    return 0;
}

int main (int argc, char** argv){
    /*Arithmetic grammars*/
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Maitri = mpc_new("maitri");

    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                   \
    number  : /-?[0-9]+[.]?[0-9]*/;                  \
    operator: '+'|'-'|'*'|'/'|'%'|/add/|/sub/|/mul/|/div/;   \
    expr    : <number>  | '('  <operator> <expr>+ ')' | \
    '(' <expr>+ <operator> <expr>+ ')'; \
    maitri  : /^/ <operator> <expr>+  /$/ | /^/  <expr>+  <operator> <expr>+ /$/;         \
    ",
    Number, Operator, Expr, Maitri);

    puts("Maitri Version 0.0.0.0.3");
    puts("Press Ctrl+c to Exit");
    puts("This language was built from an online tutorial");

    while(1){

        mpc_result_t r;

        char* input = readline("Input> ");
        add_history(input);

        if(mpc_parse("<stdin>", input, Maitri, &r)){

            mpc_ast_print(r.output);
            /*mpc_ast_delete(r.output);*/

            puts("#########################################");
            
            
            /* Load AST from output */
            mpc_ast_t* a = r.output;
            printf("Tag %s\n",a->tag);
            printf("Contents: %s\n",a->contents);
            printf("Number of children: %i\n", a->children_num);
            

            /* Get First Child */
            mpc_ast_t* c0 = a->children[0];
            printf("First Child Tag: %s\n", c0->tag);
            printf("First Child Contents: %s\n", c0->contents);
            printf("First Child Number of children: %i\n", c0->children_num);
            

            puts("#########################################");

            long result = eval(r.output);
            /*long numLeaves = 0L;countLeaves(r.output);*/
            printf("%li\n",result/*,numLeaves*/);
            mpc_ast_delete(r.output);

        }else{

            mpc_err_print(r.error);
            mpc_err_delete(r.error);

        }

        free(input);

    }

    mpc_cleanup(4, Number, Operator, Expr, Maitri);
    return 0;
}
