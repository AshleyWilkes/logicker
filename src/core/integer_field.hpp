#pragma once
#include <boost/optional.hpp>

namespace logicker::core {
  class integer_field {
    public:
      typedef int value_type;
      void set_limits(value_type min, value_type max);

      void set(value_type value);
      value_type get() const;
    private:
      boost::optional<value_type> set_value_ { boost::none };
      value_type min_, max_;
  };

  void
  integer_field::set_limits(value_type min, value_type max) {
    min_ = min; max_ = max;
  }

  void
  integer_field::set(value_type value) {
    if (set_value_) {
      throw std::domain_error("field already set");
    } else if (value < min_ || value > max_) {
      throw std::domain_error("value out of range");
    } else {
      set_value_ = value;
    }
  }

  typename integer_field::value_type
  integer_field::get() const {
    if (set_value_) {
      return set_value_.get();
    } else {
      throw std::domain_error("value not set");
    }
  }
}
