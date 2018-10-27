#pragma once
#include <stdexcept>
#include <string>

namespace logicker::core::value {
  class integer {
    public:
      integer( int value );
      int get() const { return value_; }
      static std::vector<integer> range( int min, int max );
      std::string to_string() const;
    private:
      int value_;
  };

  integer::integer( int value ) : value_{ value } {}

  std::vector<integer>
  integer::range( int min, int max ) {
    if ( min > max ) {
      throw std::invalid_argument( "min: " + std::to_string( min ) + ", max: " + std::to_string( max ));
    }
    //prepsat pomoci STL
    std::vector<integer> result;
    for ( int i = min; i <= max; ++i ) {
      result.push_back( i );
    }
    return result;
  }

  std::string
  integer::to_string() const {
    return std::to_string( value_ );
  }

  bool operator<( const integer& lhs, const integer& rhs ) {
    return lhs.get() < rhs.get();
  }

  //myslim, ze Scott Meyers doporucuje implementovat rovnost
  //jako negaci nerovnosti anebo naopak. Coz zde neni dodrzeno
  bool operator!=( const integer& lhs, const integer& rhs ) {
    return lhs.get() != rhs.get();
  }

  bool operator==( const integer& lhs, const integer& rhs ) {
    return lhs.get() == rhs.get();
  }
}
