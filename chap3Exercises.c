#include <stdio.h>

void helloWorld(void);
int atoi(char* ptr);

int main(int argc, char** argv){
  /*part 1*/
  for (int i = 0; i < 10; i++) {
    puts("Hello World");
  }

  /*part two*/
  int j = 10;

  while(j>0){
    puts("Hello World");
    j=j-1;
  }

  /*part 3*/
  if(argc == 2 ){
  int n = atoi(argv[1]);

    while(n>0){
      helloWorld();
      n=n-1;
    }
  }

  /*
    part 4
  --built in types of C--
  char,
  signed char,
  unsigned char,
  short,
  short int,
  signed short int,
  signed short int,
  unsigned short,
  unsigned short int,
  int,
  signed ,
  signed int,
  unsigned,
  unsigned int,
  long,
  long int,
  signed long,
  signed long int,
  unsigned long,
  unsigned long int,
  long long,
  long long int,
  signed long long,
  signed long long int,
  unsigned long long,
  unsigned long long int,
  float,
  double,
  long double
  */

  /*
    part 5
  --Mathematical operators in C--
  +
  -
  *
  /
  %
  */

  /*
    part 6
  --Conditional operators in C--
  >
  <
  ==
  !=
  >=
  <=
  */

  /*
    part 7
  --What it += ? --
  += is an assignment operator that adds the value on the
  right of the operator to the current value of the
  variable on th left
  */

  /*
    part 8
  --What is the do loop?--
  the do loop is a loop with a terminating condition at the
  end (this ensures that the loop runs at least once).
  */

  /*
    part 9
  --What is the switch statement?--
  the switch statement allows the execution of the code
  that is defined by the specified case of the variable
  passed into the switch statement.

  The switch statement is a multi-way decision that tests
  whether an expression matches the constant integers and branches
  according to the cases
  */

  /*
    part 10
  --What is the break keyword?--
  break is used to forcefully exit the enclosing do,while,
  for, or switch
  */

  /*
    part 11
  --What is the continue keyword ?--
  continue causes the next iteration of the enclosing
  do,while or for loop to begin
  */

  /*
    part 12
  --What is the typedef keyword ?--
  typedef is used to create new data type names. It helps
  define data-types in regards to their use.
  */
}

void helloWorld(){
  puts("Hello World");
}

int atoi(char* ptr){
  int i;
  for(i = 0; *ptr != '\0';ptr++){
    i = 10 * i + ( *ptr - '0');
  }
  return i;
}
