#pragma once
#include <stdexcept>
#include <string>

namespace logicker::core::value {
  class boolean {
    public:
      boolean( bool value );
      bool get() const { return value_; }
      static std::vector<boolean> range();
      std::string to_string() const;
    private:
      bool value_;
  };

  boolean::boolean( bool value ) : value_{ value } {}

  std::vector<boolean>
  boolean::range() {
    std::vector<boolean> result{ false, true };
    return result;
  }

  std::string
  boolean::to_string() const {
    return std::to_string( value_ );
  }

  bool operator<( const boolean& lhs, const boolean& rhs ) {
    return lhs.get() < rhs.get();
  }

  //myslim, ze Scott Meyers doporucuje implementovat rovnost
  //jako negaci nerovnosti anebo naopak. Coz zde neni dodrzeno
  bool operator!=( const boolean& lhs, const boolean& rhs ) {
    return lhs.get() != rhs.get();
  }

  bool operator==( const boolean& lhs, const boolean& rhs ) {
    return lhs.get() == rhs.get();
  }
}
