#include "mapped_buffer.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"

// static
MappedBuffer MappedBuffer::create(const std::string& font_file) {
  int fd = open(font_file.c_str(), O_RDONLY);
  CHECK(fd != -1);
  struct stat st = {};
  CHECK(fstat(fd, &st) == 0);

  uint32_t size = st.st_size;
  const uint8_t* buffer = reinterpret_cast<const uint8_t*>(
      mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0));
  CHECK(buffer != nullptr);
  return MappedBuffer(buffer, size);
}

MappedBuffer::~MappedBuffer() {
  if (data_ != nullptr) {
    munmap((void*)(data_), size_);
  }
}
