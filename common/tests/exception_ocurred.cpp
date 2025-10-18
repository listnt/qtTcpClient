#include "../logger.hpp"

int throw_exception() {
  throw 1;
  return 1;
}

int main() {
  zap::logger log;

  try {
    log << "msg" << throw_exception() << "pid" << 12 << "version"
        << "1.0.0";
  } catch (...) {
    std::cout << "first exception ocurred" << std::endl;
  }

  try {
    log << throw_exception();
  } catch (...) {
    std::cout << "second exception ocurred" << std::endl;
  }

  return 0;
}
