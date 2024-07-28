# Lispy
A lisp built based on ["Build your own Lisp"](http://buildyourownlisp.com) by Daniel Holden.

## Lispy in Action
To see Lispy in action use `compileLispy.sh` (you will first have change permissions for this file).The executable `Lispy` will be generated. I've written the Bash script to make use of the `cc` compiler, if you have `gcc` you can change the initial `cc` to `gcc` to compile.

Arithmetic operations are applied to arguments much like how functions in a regular programming language is applied (i.e. Polish notation is used for arithmetic).
```
Input> (+ 1 1)
2
```

There are a few example files provided (those ending with `lspy`) to run these pass the filename as an argument to the executable.

```
./Lispy stdlib.lspy
```

The following function to compute the _nth_ Fibonacci number is provided in the `stdlib.lspy` file
(this is currently the standard library of the language).
```lisp
; Fibonacci
(fun {fib n} {
	select
		{ (== n 0) 0 }
		{ (== n 1) 1 }
		{ otherwise (+ (fib (- n 1)) (fib (- n 2)))}
})
```
To use this function simply load the `stdlib.lspy` file and call the function with a number.

```
Input> fib 3
2
```

The syntax for Lispy is almost the same as Common-Lisp (look at the `.lspy` files to understand the syntax). **PLEASE NOTE** Lispy is very picky about white-space so make sure to get the spacing right!.

## Further steps
I'm planning on using what I've done here to implement a completely new language on my own. As you might've noticed Garbage Collection is currently being worked on.

## To be implemented
- Native Types
- User Defined Types
- List Literal
- Operating System interaction
- Macros
- Variable Hashtable
- Pool Allocation
- Garbage Collection
- Tail Call Optimisation
- Lexical Scoping
- Static Typing and more....
