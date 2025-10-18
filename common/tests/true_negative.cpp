#include "../logger.hpp"

char *make_leak() {
  char *leak = new char[100];
  return leak;
}

int main() {
  auto leak = make_leak();
  return 0;
}
