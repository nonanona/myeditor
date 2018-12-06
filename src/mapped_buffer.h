#pragma once

#include <string>

class MappedBuffer {
 public:
  MappedBuffer() : data_(nullptr), size_(0) {}
  ~MappedBuffer();

  MappedBuffer(MappedBuffer&& o) {
    data_ = o.data_;
    size_ = o.size_;
    o.data_ = nullptr;
    o.size_ = 0;
  }

  MappedBuffer& operator=(MappedBuffer&& o) {
    data_ = o.data_;
    size_ = o.size_;
    o.data_ = nullptr;
    o.size_ = 0;
    return *this;
  }

  static MappedBuffer create(const std::string& data);

  const uint8_t* data() const { return data_; }
  const uint32_t size() const { return size_; }

 private:
  MappedBuffer(const MappedBuffer&) = delete;
  MappedBuffer& operator=(const MappedBuffer&) = delete;

  MappedBuffer(const uint8_t* data, uint32_t size) : data_(data), size_(size) {}

  const uint8_t* data_;
  uint32_t size_;
};
