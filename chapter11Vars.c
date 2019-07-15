/*

Linux
cc -std=c99 -Wall -ggdb prompt.c mpc.c -ledit -lm -o prompt
Windows
cc -std=c99 -Wall -ggdb prompts.c mpc.c -prompts

Pesara Amarasekera
2019-07-09

In this file the concept of function pointers in c is introduced
TODO: check lval_err
      check builtin_cons
*/

#include "mpc.h"
#ifdef _WIN32
    #include <string.h>
    static char buffer[2048]

    char* readline(char* prompt){
        fputs(prompt,stdout);
        fgets(buffer,2048,stdin);
        char* cpy = malloc(strlen(buffer)+1);
        strcpy(cpy,buffer);
        cpy[strlen(buffer)] = '\0';
        return cpy;
    }

    char* add_history(char* unsused){}
#else
    #include <editline/readline.h>
    #include <editline/history.h>
#endif
#define LASSERT(args, cond, fmt, ...) \
    if(!(cond)){ \
        lval* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args); \
        return err; \
    } 

/* Forward Declarations */
struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

/* Lisp Value */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM,
       LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

typedef lval*(*lbuiltin)(lenv*,lval*);

struct lval{
    int type;
    long num;
    /* Error and Symbol types have some string data */
    char* err;
    char* sym;
    lbuiltin fun;
    /* Count and Pointer to a list of "lval" */
    int count;
    struct lval** cell;
};

struct lenv{
    int count;
    char** syms;
    lval** vals;
};

void lval_print(lval* v);
lval* lval_eval(lenv* e, lval* v);

/*  Construct a pointer to a new Number lval */
lval* lval_num(long x){
    lval* v  = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

/* Construct a pointer to a new Error lval */
lval* lval_err(char* fmt, ...){
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;

    /* Create a va list and initialize it */
    va_list va;
    va_start(va, fmt);

    /* Allocate 512 bytes of space */
    v->err = malloc(512);

    /* printf the error string with a maximum of 511 */
    vsnprintf(v->err, 511, fmt, va);

    /* Reallocate to number of bytes actually used */
    v->err = realloc(v->err, strlen(v->err)+1);

    /*Cleanup our va list*/
    va_end(va);
    
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

lval* lval_fun(lbuiltin func){
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FUN;
    v->fun = func;
    return v;
}

void lval_del(lval* v){
    switch (v->type){
        case LVAL_NUM: break;
        case LVAL_FUN: break;
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

lval* lval_copy(lval* v){

    lval* x = malloc(sizeof(lval));
    x->type = v->type;

    switch (v->type){
        /* Copy Functions and Numbers Directly */
        case LVAL_FUN: x->fun = v->fun; break;
        case LVAL_NUM: x->num = v->num; break;

        /* Copy Strings using malloc and strcpy */
        case LVAL_ERR:
            x->err = malloc(strlen(v->err)+1);
            strcpy(x->err,v->err); break;
        
        case LVAL_SYM:
            x->sym = malloc(strlen(v->sym)+1);
            strcpy(x->sym,v->sym); break;
        
        /* Copy Lists by copying each sub-expression */
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            x->count = v->count;
            x->cell = malloc(sizeof(lval*) * x->count);
            for(int i = 0; i < x->count; i++){
                x->cell[i] = lval_copy(v->cell[i]);
            }
        break;
    }
    return x;
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
        case LVAL_FUN: printf("<function>"); break;
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
    while(y->count-1 != 0){
        x = lval_add(x,lval_pop(y,0));
    }
    lval_del(y);
    return x;
}

char* ltype_name(int t){
    switch(t){
        case LVAL_FUN: return "Function";
        case LVAL_NUM: return "Number";
        case LVAL_ERR: return "Error";
        case LVAL_SYM: return "Symbol";
        case LVAL_SEXPR: return "S-Expression";
        case LVAL_QEXPR: return "Q-Expression";
        default: return "Unknown";
    }
}

lenv* lenv_new(void){
    lenv* e = malloc(sizeof(lenv));
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

void lenv_del(lenv* e){
    for(int i=0;i<e->count;i++){
        free(e->syms[i]);
        lval_del(e->vals[i]);
    }
    free(e->syms);
    free(e->vals);
    free(e);
}

lval* lenv_get(lenv* e, lval* k){
    /* Iterate over all items in environment */
    for(int i = 0;i<e->count;i++){
        /*Check if the stored string matches the symbol string*/
        /* If it does, return a copy of the value */
        if (strcmp(e->syms[i],k->sym)==0){
            return lval_copy(e->vals[i]);
        }
    }
    /* If no symbol found return error */
    return lval_err("Unbound Symbol '%s'",k->sym);
}

void lenv_put(lenv* e, lval* k, lval* v){
    /* Iterate over all items in environment */
    /* This is to see if variable already exists */
    for(int i=0;i<e->count;i++){

        /* If variable is found delete item at that position */
        /* And replace with variable supplied by user */
        if(strcmp(e->syms[i],k->sym) == 0){
            lval_del(e->vals[i]);
            e->vals[i] = lval_copy(v);
            return;
        }
    }

    /* If no existing entry found allocate space for new entry */
    e->count++;
    e->vals = realloc(e->vals, sizeof(lval*) * e->count);
    e->syms = realloc(e->syms, sizeof(char*) * e->count);

    /* Copy contents of lval and symbol string into new location */
    e->vals[e->count-1] = lval_copy(v);
    e->syms[e->count-1] = malloc(strlen(k->sym)+1);
    strcpy(e->syms[e->count-1],k->sym);
}

lval* builtin_def(lenv* e, lval* a){
    LASSERT(a,a->cell[0]->type == LVAL_QEXPR,
        "Function 'def' passed incorrect type for argument 0. "
        "Got %s, Expected %s.",
        ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
    
    /* First argument is symbol */
    lval* syms = a->cell[0];

    /* Ensure all elements of first list are symbols */
    for(int i=0;i<syms->count;i++){
        LASSERT(a,syms->cell[i]->type == LVAL_SYM,
            "Function 'def' cannot define non-symbol");
    }

    /* Check correct number of symbols and values */
    LASSERT(a, syms->count == a->count-1,
        "Function 'def' cannot define incorrect "
        "number of values to symbols");
    
    /* Assign copies of values to symbols */
    for(int i =0;i<syms->count;i++){
        lenv_put(e,syms->cell[i],a->cell[i+1]);
    }

    lval_del(a);
    return lval_sexpr();
}

lval* builtin_op(lenv* e, lval* a,char* op){
    /* Ensure all arguments are numbers */
    for(int i=0;i<a->count;i++){
        LASSERT(a,a->cell[i]->type == LVAL_NUM,
            "Function '%s' passed incorrect type for argument %i. "
            "Got %s, Expected %s.",
            op, i,ltype_name(a->cell[i]->type), ltype_name(LVAL_NUM));
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
    
    lval_del(a); 
    return x;
}

lval* builtin_add(lenv* e, lval* a){
    return builtin_op(e,a,"+");
}

lval* builtin_sub(lenv* e, lval* a){
    return builtin_op(e,a,"-");
}

lval* builtin_mul(lenv* e, lval* a){
    return builtin_op(e,a,"*");
}

lval* builtin_div(lenv* e, lval* a){
    return builtin_op(e,a,"/");
}

lval* builtin_mod(lenv* e, lval* a){
    return builtin_op(e,a,"%");
}

lval* builtin_list(lenv* e, lval* a){
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_head(lenv* e,lval* a){
    /*Check Error Conditions*/
    LASSERT(a,a->count == 1,
        "Function 'head' passed too many arguments. "
        "Got %i, Expected %i.",
        a->count, 1);
    LASSERT(a,a->cell[0]->type == LVAL_QEXPR,
        "Function 'head' passed incorrect type for argument 0. "
        "Got %s, Expected %s.",
        ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
    LASSERT(a,a->cell[0]->count != 0,
        "Function 'head' passed {}!");

    /* Otherwise take first argument */
    lval* v = lval_take(a,0);

    /* Delete all elements that are not head and return */
    while(v->count > 1) {lval_del(lval_pop(v,1));}
    return v;
}

lval* builtin_tail(lenv* e, lval* a){
    /* Check Error Conditions */
    LASSERT(a,a->count == 1,
        "Function 'tail' passed too many arguments "
        "Got %i, Expected %i.",
        a->count, 1);
    LASSERT(a,a->cell[0]->type == LVAL_QEXPR,
        "Function 'tail' passed incorrect type for argument 0. "
        "Got %s, Expected %s.",
        ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
    LASSERT(a,a->cell[0]->count != 0,
        "Function 'tail' passed {}!");

    /* Take first argument */
    lval* v = lval_take(a,0);

    /* Delete first element and return */
    lval_del(lval_pop(v,0));
    return v;
}

lval* builtin_join(lenv* e, lval* a){
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

lval* builtin_cons(lenv* e, lval* a){
    LASSERT(a,a->count == 2,
        "Function 'cons' passed too many arguments. "
        "Got %i, Expected %i.",
        a->count, 2);
    /* Maybe update this error to give more options */
    LASSERT(a,a->cell[0]->type != LVAL_ERR,
        "Function 'cons' passed incorrect type for argument 0. "
        "Got %s, Expected %s.",
        ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
    
    LASSERT(a,a->cell[1]->type == LVAL_QEXPR,
        "Function 'cons' passed incorrect type for argument 1. "
        "Got %s, Expected %s.",
        ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));

    lval* x = lval_qexpr();
    x = lval_add(x,lval_pop(a,0));
    while(a->count){
        x = lval_join(x,lval_pop(a,0));
    }

    lval_del(a);
    return x;
}

lval* builtin_len(lenv* e, lval* a){
    LASSERT(a, a->count == 1,
        "Function 'len' passed too many arguments. "
        "Got %i, Expected %i.",
        a->count, 1)
    LASSERT(a,a->cell[0]->type == LVAL_QEXPR,
        "Function 'len' passed incorrect type for argument 0. "
        "Got %s, Expected %s.",
        ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));

    long x = 0;
    x = lval_len(lval_pop(a,0));

    lval* y = lval_num(x);
    lval_del(a);
    return y;
}

lval* builtin_init(lenv* e, lval* a){
    LASSERT(a,a->count == 1,
        "Function 'int' passed too many arguments. "
        "Got %i, Expected %i.",
        a->count, 1);
    LASSERT(a,a->cell[0]->type == LVAL_QEXPR,
        "Function 'init' passed incorrect type for argument 0. "
        "Got %s, Expected %s.",
        ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
    LASSERT(a,a->cell[0]->count != 0,
        "Function 'init' passed {}!");

    lval* x = lval_qexpr();
    x = lval_init(x,lval_pop(a,0));

    lval_del(a);
    return x;
}

lval* builtin_eval(lenv* e, lval* a){
    LASSERT(a,a->count == 1,
        "Function 'eval' passed too many arguments. "
        "Got %i, Expected %i.",
        a->count, 1);
    LASSERT(a,a->cell[0]->type == LVAL_QEXPR,
        "Function 'eval' passed incorrect type for argument 0. "
        "Got %s, Expected %s.",
        ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
    
    lval* x = lval_take(a,0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func){
    lval* k = lval_sym(name);
    lval* v = lval_fun(func);
    lenv_put(e,k,v);
    lval_del(k);lval_del(v);
}

void lenv_add_builtins(lenv* e){
    /* List Functions */
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);
    lenv_add_builtin(e, "def", builtin_def);

    /*Mathematical Functions*/
    lenv_add_builtin(e,"+",builtin_add);
    lenv_add_builtin(e,"-",builtin_sub);
    lenv_add_builtin(e,"*",builtin_mul);
    lenv_add_builtin(e,"/",builtin_div);
}

lval* lval_eval_sexpr(lenv* e,lval* v){
    /*Evaluate Children*/
    for(int i =0;i<v->count;i++){
        v->cell[i] = lval_eval(e, v->cell[i]);
    }

    /* Error checking */
    for(int i=0;i<v->count;i++){
        if(v->cell[i]->type==LVAL_ERR){return lval_take(v,i);}
    }

    if(v->count==0){return v;}
    if(v->count==1){return lval_take(v,0);}

    /* Ensure First element is a function after evaluation */
    lval* f = lval_pop(v,0);
    if(f->type!=LVAL_FUN){
        lval_del(v);lval_del(f);
        return lval_err("first element is not a function");
    }

    /* If so call function to get result */
    lval* result = f->fun(e,v);
    lval_del(f);
    return result;
}

lval* lval_eval(lenv* e, lval* v){
    if(v->type == LVAL_SYM) {
        lval* x = lenv_get(e,v);
        lval_del(v);
        return x;
    }
    if(v->type == LVAL_SEXPR){ return lval_eval_sexpr(e, v);}
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
        symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/;          \
        sexpr  : '(' <expr>* ')' ;                          \
        qexpr  : '{' <expr>* '}' ;                          \
        expr   :  <number> | <symbol> | <sexpr> | <qexpr> ; \
        lispy  : /^/<expr>*/$/ ;                            \
    ",
    Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    lenv* e = lenv_new();
    lenv_add_builtins(e);

    puts("Lispy version 0.0.0.0.7"); 
    puts("Press Ctrl-c to Exit\n");

    while(1){

        mpc_result_t r;
        char* input  = readline("Input> ");
        add_history(input);

        if(mpc_parse("<stdin>",input,Lispy,&r)){

            lval* x = lval_eval(e, lval_read(r.output));
            lval_println(x);
            lval_del(x);

            mpc_ast_delete(r.output);
        }else{
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }
    lenv_del(e);
    mpc_cleanup(6,Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
    return 0;
}