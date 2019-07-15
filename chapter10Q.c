/*

Linux
cc -std=c99 -Wall -ggdb prompt.c mpc.c -ledit -lm -o prompt
Windows
cc -std=c99 -Wall -ggdb prompts.c mpc.c -prompts

Pesara Amarasekera
2019-07-09

In this file Quoted Expressions will be implemented

The language is beginning to look much like a functional programming language
*/
#include "mpc.h"
#ifdef _WIN32
    #include <string.h>

    static char buffer[2048];

    char* readline(char* prompt){
        fputs(prompt,stdout);
        fgets(buffer,2048,stdin);
        char* cpy = malloc(strlen(buffer)+1);
        strcpy(cpy,buffer);
        cpy[strlen(buffer)] ='\0';
        return cpy;
    }

    char* add_history(char* unused) {}
#else
    #include <editline/readline.h>
    #include <editline/history.h>
#endif

#define LASSERT(args, cond, err)                                                                    \
    if(!(cond)){lval_del(args); return lval_err(err);}


#define INCORRECTNUMARGS(args)                                                                     \
    if(args->count != 1){lval_del(args);return lval_err("Function passed too many arguments!");}
#define EMPTYLIST(args)                                                                            \
    if(args->cell[0]->count == 0){lval_del(args);return lval_err("Function passed {}!");}

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };

typedef struct lval{
    int type;
    long num;
    /* Error and Symbol types have some string data */
    char* err;
    char* sym;
    /* Count and Pointer to a list of "lval" */
    int count;
    struct lval** cell;
} lval;

void lval_print(lval* v);
lval* lval_eval(lval* v);

/*  Construct a pointer to a new Number lval */
lval* lval_num(long x){
    lval* v  = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

/* Construct a pointer to a new Error lval */
lval* lval_err(char* m){
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m)+1);
    strcpy(v->err,m);
    return v;
}

/* Construct a pointer to a new Symbol lval */
lval* lval_sym(char* s){
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s)+1);
    strcpy(v->sym,s);
    return v;
}

lval* lval_sexpr(void){
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}
/* A pointer to a new empty Qexpr lval */
lval* lval_qexpr(void){
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

void lval_del(lval* v){
    switch (v->type){
        case LVAL_NUM: break;
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;

        /*If Qexpr or Sexpr then delete all elements inside*/
        case LVAL_QEXPR:
        case LVAL_SEXPR:
            for(int i=0;i<v->count;i++){
                lval_del(v->cell[i]);
            }
            /* Also free the memory allocated to contain the pointers */
            free(v->cell);
        break;
    }
    
    free(v);
}

lval* lval_read_num(mpc_ast_t* t){
    errno = 0;
    long x = strtol(t->contents,NULL,10);
    return errno != ERANGE ? 
        lval_num(x) : lval_err("invalid number");
}

lval* lval_add(lval* v,lval* x){
    v->count++;
    v->cell = realloc(v->cell,sizeof(lval*) * v->count);
    v->cell[v->count-1] = x;
    return v;
}

lval* lval_read(mpc_ast_t* t){
    /*If Symbol or Number return conversion to that type*/
    if(strstr(t->tag,"number")) {return lval_read_num(t);}
    if(strstr(t->tag,"symbol")) {return lval_sym(t->contents);}

    /*If root(>), sexpr, or qexpr then create empty list*/
    lval* x = NULL;
    if(strcmp(t->tag,">") == 0) { x = lval_sexpr();}
    if(strstr(t->tag,"sexpr"))  { x = lval_sexpr();}
    if(strstr(t->tag,"qexpr"))  { x = lval_qexpr();}

    /*Fill this list with any valid expression contained within*/
    for(int i=0;i<t->children_num;i++){
        if(strcmp(t->children[i]->contents,"(") == 0){continue;}
        if(strcmp(t->children[i]->contents,")") == 0){continue;}
        if(strcmp(t->children[i]->contents,"{") == 0){continue;}
        if(strcmp(t->children[i]->contents,"}") == 0){continue;}
        if(strcmp(t->children[i]->tag,"regex")  == 0){continue;}
        x = lval_add(x,lval_read(t->children[i]));
    }

    return x;
}

void lval_expr_print(lval* v,char open, char close){
    putchar(open);
    for(int i=0;i<v->count;i++){
        /*Print value contained within*/
        lval_print(v->cell[i]);

        /*Don't print trailing space if last element*/
        if(i != v->count-1){
            putchar(' ');
        }
    }
    putchar(close);
}

void lval_print(lval* v){
    switch (v->type) {
        case LVAL_NUM: printf("%li",v->num); break;
        case LVAL_ERR: printf("Error: %s",v->err); break;
        case LVAL_SYM: printf("%s", v->sym); break;
        case LVAL_SEXPR: lval_expr_print(v,'(',')'); break;
        case LVAL_QEXPR: lval_expr_print(v,'{','}'); break;
    }
}

void lval_println(lval* v){lval_print(v); putchar('\n');}

lval* lval_pop(lval* v, int i){
    /* Find the item at i */
    lval* x = v->cell[i];

    /* Shift memory after the item "i" is over the top */
    memmove(&v->cell[i],&v->cell[i+1],
        sizeof(lval*) * (v->count-i-1));

    /* Decrease the count of items in the list */
    v->count--;
    
    /*reallocate the memory used*/
    v->cell = realloc(v->cell,sizeof(lval*) * v->count);

    return x;
}

lval* lval_take(lval* v, int i){
    lval* x = lval_pop(v,i);
    lval_del(v);
    return x;
}

lval* lval_join(lval* x, lval* y){
    /*For each cell in 'y' add it to 'x'*/
    while(y->count){
        x = lval_add(x,lval_pop(y,0));
    }
    lval_del(y);
    return x;
}

long lval_len(lval* y){
    long x = 0;
    while(y->count){
        lval* temp = lval_pop(y,0);
        x++; 
        lval_del(temp);
    }
    lval_del(y);
    return x;
}

lval* lval_init(lval* x, lval* y){
    while(y->count-1>0){
        x = lval_add(x,lval_pop(y,0));
    }
    lval_del(y);
    return x;
}

lval* builtin_op(lval* a,char* op){
    /* Ensure all arguments are numbers */
    for(int i=0;i<a->count;i++){
        if(a->cell[i]->type != LVAL_NUM){
            lval_del(a);
            return lval_err("Cannot operate on non-number");
        }
    }
    
    /*Pop the first element*/
    lval* x = lval_pop(a,0);

    /*If no arguments and subexpressions perform unary negation*/
    if((strcmp(op,"-") == 0) && a->count == 0){
        x->num = -x->num;
    }

    /*While there are still elements remaining*/
    while(a->count > 0){
        /*Pop the next element*/
        lval* y = lval_pop(a,0);
        if(strcmp(op,"+")==0){ x->num += y->num;}
        if(strcmp(op,"-")==0){ x->num -= y->num;}
        if(strcmp(op,"*")==0){ x->num *= y->num;}
        if(strcmp(op,"%")==0){
            if(y->num==0){
                lval_del(x);lval_del(y);
                x = lval_err("Modulo By Zero!");break;
            }
             x->num = (long) x->num % (long) y->num;
        }
        if(strcmp(op,"/")==0){
            if(y->num==0){
                lval_del(x); lval_del(y);
                x = lval_err("Division By Zero!"); break;
            }
            x->num /= y->num;
        }
        lval_del(y);
    }

    lval_del(a); return x;
}

lval* builtin_head(lval* a){
    /*Check Error Conditions*/
    LASSERT(a,a->count == 1,
        "Function 'head' passed too many arguments!");
    LASSERT(a,a->cell[0]->type == LVAL_QEXPR,
        "Function 'head' passed incorrect type!");
    LASSERT(a,a->cell[0]->count != 0,
        "Function 'head' passed {}!");

    /* Otherwise take first argument */
    lval* v = lval_take(a,0);

    /* Delete all elements that are not head and return */
    while(v->count > 1) {lval_del(lval_pop(v,1));}
    return v;
}

lval* builtin_tail(lval* a){
    /* Check Error Conditions */
    LASSERT(a,a->count == 1,
        "Function 'tail' passed too many arguments!");
    LASSERT(a,a->cell[0]->type == LVAL_QEXPR,
        "Function 'tail' passed incorrect type!");
    LASSERT(a,a->cell[0]->count != 0,
        "Function 'tail' passed {}!");

    /* Take first argument */
    lval* v = lval_take(a,0);

    /* Delete first element and return */
    lval_del(lval_pop(v,0));
    return v;
}

lval* builtin_list(lval* a){
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_join(lval* a){
    for(int i = 0;i<a->count;i++){
        LASSERT(a,a->cell[i]->type == LVAL_QEXPR,
            "Function 'join' passed incorrect type!");
    }

    lval* x = lval_pop(a,0);
    while(a->count){
        x = lval_join(x, lval_pop(a,0));
    }

    lval_del(a);
    return x;
}

lval* builtin_cons(lval* a){
    LASSERT(a,a->count == 2,
        "Function 'cons' passed too many arguments!");
    LASSERT(a,a->cell[0]->type != LVAL_ERR,
        "Function 'cons' passed incorrect type!");
    LASSERT(a,a->cell[1]->type == LVAL_QEXPR,
        "Function 'cons' passed incorrect type!");

    lval* x = lval_qexpr();
    x = lval_add(x,lval_pop(a,0));
    while(a->count){
        x = lval_join(x,lval_pop(a,0));
    }

    lval_del(a);
    return x;
}

lval* builtin_len(lval* a){
    LASSERT(a, a->count == 1,
        "Function 'len' passed too many arguments!")
    LASSERT(a,a->cell[0]->type == LVAL_QEXPR,
        "Function 'len' passed incorrect type!");

    long x = 0;
    x = lval_len(lval_pop(a,0));

    lval* y = lval_num(x);
    lval_del(a);
    return y;
}

lval* builtin_init(lval* a){
    LASSERT(a,a->count == 1,
        "Function 'int' passed too many arguments!");
    LASSERT(a,a->cell[0]->type == LVAL_QEXPR,
        "Function 'init' passed incorrect type!");
    LASSERT(a,a->cell[0]->count != 0,
        "Function 'init' passed {}!");

    lval* x = lval_qexpr();
    x = lval_init(x,lval_pop(a,0));

    lval_del(a);
    return x;
}

lval* builtin_eval(lval* a){
    LASSERT(a,a->count == 1,
        "Function 'eval' passed too many arguments!");
    LASSERT(a,a->cell[0]->type == LVAL_QEXPR,
        "Function 'eval' passed incorrect type");
    
    lval* x = lval_take(a,0);
    x->type = LVAL_SEXPR;
    return lval_eval(x);
}

lval* builtin(lval* a, char* func){
    if(strcmp("list",func)==0){return builtin_list(a);}
    if(strcmp("head",func)==0){return builtin_head(a);}
    if(strcmp("tail",func)==0){return builtin_tail(a);}
    if(strcmp("join",func)==0){return builtin_join(a);}
    if(strcmp("cons",func)==0){return builtin_cons(a);}
    if(strcmp("len",func)==0){return builtin_len(a);}
    if(strcmp("init",func)==0){return builtin_init(a);}
    if(strcmp("eval",func)==0){return builtin_eval(a);}
    if(strstr("+-/*%",func)){return builtin_op(a,func);}
    lval_del(a);
    return lval_err("Unknown Function!");
}

lval* lval_eval_sexpr(lval* v){
    /*Evaluate Children*/
    for(int i =0;i<v->count;i++){
        v->cell[i] = lval_eval(v->cell[i]);
    }

    /* Error checking */
    for(int i=0;i<v->count;i++){
        if(v->cell[i]->type==LVAL_ERR){return lval_take(v,i);}
    }

    /*Empty Expression*/
    if(v->count==0){return v;}

    /*Single Expression*/
    if(v->count==1){return lval_take(v,0);}

    /* Ensure First element is Symbol */
    lval* f = lval_pop(v,0);
    if(f->type!=LVAL_SYM){
        lval_del(f);lval_del(v);
        return lval_err("S-expression Does not start with symbol!");
    }

    /* Call builtin with operator */
    lval* result = builtin(v,f->sym);
    lval_del(f);

    return result;
}

lval* lval_eval(lval* v){
    /*Evaluate S-expressions*/
    if(v->type == LVAL_SEXPR) {return lval_eval_sexpr(v);}
    /*All other lval types remain the same*/
    return v;
}

int main(int argc, char** argv){
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* Sexpr = mpc_new("sexpr");
    mpc_parser_t* Qexpr = mpc_new("qexpr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                       \
        number : /-?[0-9]+/ ;                               \
        symbol : \"list\"|\"head\"|\"tail\"                 \
               |\"join\"|\"cons\"|\"len\"|\"init\"|\"eval\" \
               |'+'|'-'|'*'|'/'|'%' ;                       \
        sexpr  : '(' <expr>* ')' ;                          \
        qexpr  : '{' <expr>* '}' ;                          \
        expr   :  <number> | <symbol> | <sexpr> | <qexpr> ; \
        lispy  : /^/<expr>*/$/ ;                            \
    ",
    Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    puts("Lispy version 0.0.0.0.6");
    puts("Press Ctrl-c to Exit\n");

    while(1){
        mpc_result_t r;
        char* input  = readline("Input> ");
        add_history(input);

        if(mpc_parse("<stdin>",input,Lispy,&r)){
            lval* x = lval_eval(lval_read(r.output));
            lval_println(x);
            lval_del(x);
            mpc_ast_delete(r.output);
        }else{
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
    }

    mpc_cleanup(6,Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
    return 0;
}