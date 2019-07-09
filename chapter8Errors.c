/*

Linux
cc -std=c99 -Wall prompt.c mpc.c -ledit -lm -o prompt
Windows
cc -std=c99 -Wall prompts.c mpc.c -prompts

Pesara Amarasekera
2019-07-07
This file contains the error handling code for Maitri 
Programming Language
Watching for Heisenbugs (Mutating Bugs???)

LOOK: valgrind and gdb will come in handy after this point

gdb: you can use gdb when you create an executable with either -g or -ggdb(most expressive debugging information) 
    flag, then running the executable inside gdb

enum: enums can be named from the start or using typedef

union: a variable that at different times hold different types

*/

#include "mpc.h"
#ifdef _WIN32
    #include <string.h>

    static char buffer[2048];

    char* readline(char* prompt){
        fputs(prompt,stdout);
        fgets(buffer, 2048, stdin);
        char* cpy = malloc(strlen(buffer)+1);
        strcpy(cpy,buffer);
        cpy[strlen(buffer)] ='\0';
        return cpy;
    }

    void add_history(char* unused){}
#else
    #include <editline/readline.h>
    #include <editline/history.h>
#endif

/*Create Enumeration of number Types*/
/*enum { NUM_LONG, NUM_DOUBLE};*/

/*Create Enumeration of Possible lval Types*/
enum { LVAL_NUM, LVAL_ERR };

/*Create Enumeration of Possible Error Types*/
enum { LERR_DIV_ZERO, LERR_MOD_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/*New lval Struct Declaration*/
typedef struct{
    int type;
    /* long num;*/
    double num; 
    int err;
} lval;

/*Create a new number type lval*/
lval lval_num(double x /*long x*/){
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

/*Create a new error type lval*/
lval lval_err(int x){
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

/* Print an "lval" */
void lval_print(lval v){
    switch(v.type){
        /*In the case the type is a number print it*/
        /*Then 'break' out of the switch.*/
        case LVAL_NUM: printf("%.2lf",v.num)/*printf("%li",v.num)*/; break;

        /*In the case the type is an error*/
        case LVAL_ERR:
            /*Check what type of error it is and print it*/
            if(v.err == LERR_DIV_ZERO){
                printf("Error: Division By Zero!");
            }
            if(v.err == LERR_MOD_ZERO){
                printf("Error: Modulo By Zero!");
            }
            if(v.err == LERR_BAD_OP){
                printf("Error: Invalid Operator!");
            }
            if(v.err == LERR_BAD_NUM){
                printf("Error: Invalid Number!");
            }
        break;
    }
}

/*Print an "lval" followed by a newline*/
void lval_println(lval v) {lval_print(v);putchar('\n');}

lval eval_op(lval x, char* op,lval y) {
    /*If either value is an error return it*/
    if(x.type == LVAL_ERR) { return x; }
    if(y.type == LVAL_ERR) { return y; }

    /*Otherwise do math on the number values*/
    if(strcmp(op,"+") == 0) {return lval_num(x.num+y.num);}
    if(strcmp(op,"-") == 0) {return lval_num(x.num-y.num);}
    if(strcmp(op,"*") == 0) {return lval_num(x.num*y.num);}
    if(strcmp(op,"%") == 0) {
        return y.num == 0
        ? lval_err(LERR_MOD_ZERO)
        : lval_num( (long)(x.num) % (long)(y.num) );
    }
    if(strcmp(op,"/") == 0) {
        return y.num==0
        ? lval_err(LERR_DIV_ZERO)
        : lval_num(x.num/y.num);
    }

    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t){

    if(strstr(t->tag,"number")){
        errno = 0;
        /*long x = strtol(t->contents,NULL,10);*/
        double x = atof(t->contents);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    char* op = t->children[1]->contents;
    lval x = eval(t->children[2]);

    int i = 3;
    while(strstr(t->children[i]->tag,"expr")){
        x = eval_op(x,op,eval(t->children[i]));
        i++;
    }
    return x;
}

int main(int argc, char** argv){
    /*Arithmetic grammars*/
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
    "                                             \
    number : /-?[0-9]+[.]?[0-9]*/;                \
    operator : '+'|'-'|'*'|'/'|'%';               \
    expr : <number> | '(' <operator> <expr>+ ')'; \
    lispy : /^/ <operator> <expr>+ /$/;           \
    ",
    Number,Operator,Expr,Lispy);

    puts("Lispy Version 0.0.0.0.4");
    puts("Press Ctrl-c to Exit\n");

    while(1){
        mpc_result_t r;

        char* input = readline("Input> ");
        add_history(input);

        if(mpc_parse("<stdin>", input, Lispy, &r)){
            
            lval result = eval(r.output);
            lval_println(result);
            mpc_ast_delete(r.output);

        }else{

            mpc_err_print(r.error);
            mpc_err_delete(r.error);

        }

        free(input);

    }

    mpc_cleanup(4,Number,Operator,Expr,Lispy);
    return 0;
}