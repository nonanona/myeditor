#pragma once

#include <sstream>
#include <string>

template <typename T>
class Point {
 public:
  Point() : x_(0), y_(0) {}
  Point(T x, T y) : x_(x), y_(y) {}

  T x() const { return x_; }
  T y() const { return y_; }

  void set(T x, T y) {
    x_ = x;
    y_ = y;
  }
  void set_x(T x) { x_ = x; }
  void set_y(T y) { y_ = y; }

  Point<T>& operator=(const Point<T>& o) {
    x_ = o.x_;
    y_ = o.y_;
    return *this;
  }

  Point<T> operator+=(const Point<T>& o) {
    x_ += o.x_;
    y_ += o.y_;
    return *this;
  }

  Point<T> operator+(const Point<T>& o) {
    return Point<T>(x_ + o.x_, y_ + o.y_);
  }

  std::string toString() const {
    std::stringstream ss;
    ss << x() << "," << y();
    return ss.str();
  }

 private:
  T x_;
  T y_;
};

template <typename T>
class Size {
 public:
  Size() : width_(0), height_(0) {}
  Size(T width, T height) : width_(width), height_(height) {}

  T width() const { return width_; }
  T height() const { return height_; }

  T size() const { return width_ * height_; }

  void set(T width, T height) {
    width_ = width;
    height_ = height;
  }
  void set_width(T width) { width_ = width; }
  void set_height(T height) { height_ = height; }

  Size<T>& operator=(const Size<T>& o) {
    width_ = o.width_;
    height_ = o.height_;
    return *this;
  }

  std::string toString() const {
    std::stringstream ss;
    ss << width() << "x" << height();
    return ss.str();
  }

 private:
  T width_;
  T height_;
};

template <typename T>
class Rect {
 public:
  Rect(T x, T y, T w, T h) : pos_(x, y), size_(w, h) {}
  Rect(const Point<T>& pos, const Size<T>& size) : pos_(pos), size_(size) {}
  Rect() : pos_(0, 0), size_(0, 0) {}

  T l() const { return pos_.x(); }
  T t() const { return pos_.y(); }
  T r() const { return pos_.x() + size_.width(); }
  T b() const { return pos_.y() + size_.height(); }
  float l_ratio() const { return (float)l() / (float)width(); }
  float t_ratio() const { return (float)t() / (float)height(); }
  float r_ratio() const { return (float)r() / (float)width(); }
  float b_ratio() const { return (float)b() / (float)height(); }
  T width() const { return size_.width(); }
  T height() const { return size_.height(); }
  Size<T> size() const { return size_; }
  Point<T> origin() const { return pos_; }

  void set(Point<T> pos, Size<T> size) {
    pos_ = pos;
    size_ = size;
  }
  void set_l(T l) { pos_.set_x(l); }
  void set_t(T t) { pos_.set_y(t); }
  void set_width(T w) { size_.set_width(w); }
  void set_height(T h) { size_.set_height(h); }

  Rect<T>& operator=(const Rect<T>& o) {
    pos_ = o.pos_;
    size_ = o.size_;
    return *this;
  }

  std::string toString() const {
    std::stringstream ss;
    ss << l() << "," << t() << "-" << width() << "x" << height();
    return ss.str();
  }

 private:
  Point<T> pos_;
  Size<T> size_;
};

using ISize = Size<uint32_t>;
using IRect = Rect<uint32_t>;
using IPoint = Point<uint32_t>;

using FSize = Size<float>;
using FRect = Rect<float>;
using FPoint = Point<float>;
