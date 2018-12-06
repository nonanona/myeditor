#include "option_parser.h"

#include <cstdio>
#include <cstdlib>

// static
Option& Option::getInstance() {
  static Option option;
  return option;
}

// static
const Option& Option::get() {
  return getInstance();
}

// static
void Option::init(int argc, char** argv) {
  Option& r = getInstance();

  for (int i = 1; i < argc; ++i) {
    const char* arg = argv[i];
    if (arg[0] == '-') {  // option
      switch (arg[1]) {
        case 'v':
          r.verbose_ = true;
          break;
        case 'd':
          r.debug_ = true;
          break;
        case 'p':
          if (i + 1 < argc) {
            r.text_size_pt_ = atof(argv[i + 1]);
          } else {
            printf("-p option needs argument.\n");
            exit(1);
          }
          break;
        case 'w':
          if (i + 1 < argc) {
            uint32_t width;
            uint32_t height;
            if (sscanf(argv[i + 1], "%dx%d", &width, &height) != 2) {
              printf("Unknown format for window size: %s\n", argv[i + 1]);
              exit(1);
            }
            r.window_size_ = ISize(width, height);
          } else {
            printf("-p option needs argument.\n");
            exit(1);
          }
          break;
        default:
          printf("Unknown option: %s\n", arg);
          exit(1);
      }
    }
  }
}
