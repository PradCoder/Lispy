#include <stdio.h>

/*
compile command
gcc -std=c99 -Wall HelloAgain.c -o HelloAgain

1.When no main there is an error for undefined reference to main
2.puts is like fputc with output to stdout (screen) and appends \n
3.gdb (flagged as -ggdb) is used to debug it gives no different
  information
*/

int main(int argc,char** argv){
  puts("Hello Again C My friend.");
  return 0;
}
