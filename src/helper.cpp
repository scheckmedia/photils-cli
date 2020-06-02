#include "helper.h"

using namespace photils;

fs::path photils::get_execution_path() {
  auto app_path = fs::path();
#if __APPLE__
  char buff[1024];
  uint32_t size = sizeof(buff);
  if (_NSGetExecutablePath(buff, &size) == 0) {
    app_path.assign(buff).remove_filename();
  }
#else
  app_path = fs::read_symlink("/proc/self/exe").remove_filename();
#endif

  return app_path;
}
