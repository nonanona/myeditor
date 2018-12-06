#pragma once

#define DISALLOW_COPY_AND_ASSIGN(Type) \
  Type(const Type&) = delete;          \
  Type& operator=(const Type&) = delete;
