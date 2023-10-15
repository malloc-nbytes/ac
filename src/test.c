#include <stdio.h>

int sum(int x, int y)
{
  return x + y;
}

int main(void)
{
  printf("hello world\n");
  int s = sum(1, 2);
  printf("sum: %d\n", s);
  return 0;
}
