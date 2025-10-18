#include "../logger.hpp"
#include <unistd.h> // Required for getpid()

int main() {
  zap::logger log;

  for (int i = 0; i < 10000; i++) {
    log << "msg"
        << "some long string that produces some message that is longer than "
           "100 characters and is also not a number. This string have some "
           "meaning and will probably help with debugging"
        << "intvalue" << rand() % 10000 << "pid" << 12 << "version"
        << "1.0.0";
  }
}