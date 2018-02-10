class FooBar {
public:
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

  int f1(int a, int b) {
    if (a) {
      return b;
    }

    if (b || a) {
      return a;
    }

    if (b && a) {
      return 42;
    }

    int x = a ? (b * b) : 6;

    return x;
  }

};

int x(int xx) {
  FooBar f;
  f.f(2, 4);
  if (f.f1(xx | 143, xx)) {
    throw xx;
  }

  return 117;
}

