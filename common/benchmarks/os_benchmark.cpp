#include <cstring>
#include <unistd.h>

int main() {
  const char *msg =
      "{timestamp=1760762439900 lvl=INFO msg=\" some long string that produces"
      "some message that is longer than 100 characters and"
      "is also not a number.This string have some meaning and"
      "will probably help with debugging"
      "\" intvalue=9430 pid=12 version=\" 1.0.0 \"}";
  for (int i = 0; i < 10000; i++) {
    write(STDOUT_FILENO, msg, strlen(msg));
  }
}