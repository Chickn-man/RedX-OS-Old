#include "math.h"

// sqare root
double sqrt(double number) {
  double temp = 0;
  double root = number / 2;
  int i = 0;
  while (root != temp && i < 10000) {
    temp = root;
    root = (number / temp + temp) / 2;
    i++;
  }
  return root;
}

/*
** returns a random number between 1 and 0
*/
double rand() {
  return 0;
}

// round to int
int roundd(double x) {
  if (x < 0.0)
    return (int)(x - 0.5);
  else
    return (int)(x + 0.5);
}