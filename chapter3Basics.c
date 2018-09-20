#include <stdio.h>

int main(int argc,char** argv){
  int count = 10;
  int added = add_together(10,20);
  return 0;
}

int add_together(int x,int y){
  int result = x+y;
  return result;
}

typedef struct{
  float c;
  float y;
}point;

point p;
p.x=10;
p.y=10;

float length = sqrt(p.x*p.x + p.y*p.y);

/*
Pointer to a pointer declared as value**
the levels of having a pointer to a pointer can extend
to infinitum (if a computer's memory doesn't run out)
*/

if(x>10&&x<100){
  puts("x is greater than 10 and less than a 100!");
}else{
  puts("x is less than 11 and greater than 99!");
}

int i = 10;

while(i>0){
  puts("Loop Iteration");
  i = i - 1;
}

for(int i = 0; i < 10; i++){
  puts("Loop Iteration");
}
