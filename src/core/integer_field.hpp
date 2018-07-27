#pragma once
#include <boost/optional.hpp>

namespace logicker::core {
  template<int Min, int Max>
  class integer_field {
    public:
      typedef int value_type;

      void set(value_type value);
      value_type get() const;
    private:
      boost::optional<value_type> set_value_ { boost::none };
  };

  template<int Min, int Max>
  void
  integer_field<Min, Max>::set(value_type value) {
    if (set_value_) {
      throw std::domain_error("field already set");
    } else if (value < Min || value > Max) {
      throw std::domain_error("value out of range");
    } else {
      set_value_ = value;
    }
  }

  template<int Min, int Max>
  typename integer_field<Min, Max>::value_type
  integer_field<Min, Max>::get() const {
    if (set_value_) {
      return set_value_.get();
    } else {
      throw std::domain_error("value not set");
    }
  }
}
