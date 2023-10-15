#include <stdio.h>

int sum(int a, int b) {
  return a + b;
}

int main(void)
{
  int x = 2;
  int y = 0;
  printf("%d\n", sum(x, y));
  return 0;
}
