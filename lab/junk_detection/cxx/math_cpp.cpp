int f(int a, int b) {
  if (a && b) {
    b++;
  } else {
    a = a + b;
  }

  if (a || b) {
    b = b + 1;
  } else {
    a += b;
  }

  int x = a + b;
  int y = x + b + a;

  a++;
  b++;
  ++a;
  ++b;

  return y;
}

