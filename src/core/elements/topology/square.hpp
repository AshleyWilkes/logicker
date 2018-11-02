#pragma once
#include "rectangle.hpp"

namespace logicker::core::topology {
  class square : public rectangle {
    public:
      explicit square(int size) : rectangle( size, size ) {}
      explicit square(topology_size_t size) : rectangle(size) {
        if (size.first != size.second) {
          //should throw standard exception, of course
          throw "Invalid argument";
        }
    }
  };
}
