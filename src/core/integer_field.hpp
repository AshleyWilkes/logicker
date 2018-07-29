#pragma once
#include <boost/dynamic_bitset.hpp>
#include <boost/optional.hpp>

namespace logicker::core {
  class integer_field {
    public:
      typedef int value_type;
      void set_limits(value_type min, value_type max);

      void set(value_type value);
      value_type get() const;
      void eliminate(value_type value);
      bool is_set() const;
      bool is_value_option(value_type value) const;
    private:
      boost::optional<value_type> set_value_ { boost::none };
      value_type min_, max_;
      boost::dynamic_bitset<> options_;
  };

  void
  integer_field::set_limits(value_type min, value_type max) {
    //assert max > min
    //assert options_.size() == 0
    min_ = min; max_ = max;
    unsigned size{ (unsigned) max - min + 1 };
    options_ = ~boost::dynamic_bitset<>{size};
  }

  void
  integer_field::set(value_type value) {
    //assert value >= min && value <= max
    //assert options_.test(value - min_)
    //assert set_value_ == boost::none
    if (set_value_) {
      throw "field already set";
    } else if (value < min_ || value > max_) {
      throw "value out of range";
    } else if (! options_.test(value - min_)) {
      throw "value not available"; 
    } else {
      set_value_ = value;
      //mozna setnout options_ tak, aby umoznovaly jenom tuto jedinou hodnotu??
    }
  }

  typename integer_field::value_type
  integer_field::get() const {
    if (set_value_) {
      return set_value_.get();
    } else {
      throw "value not set";
    }
  }

  void
  integer_field::eliminate(value_type value) {
    //assert options_.test(value - min_)
    //assert set_value_ == boost::none -- jiank eliminuju posledni hodnotu,
    //    coz zatim nechci implementovat
    unsigned index { (unsigned) value - min_ };
    if (options_.test(index)) {
      throw "value already eliminated";
    } else if (set_value_) {
      throw "value already set";
    } else {
      options_.flip(index);
      if (options_.count() == 1) {
        set_value_ = min_ + options_.find_first();
      }
    }
  }

  bool
  integer_field::is_set() const {
    return set_value_ != boost::none;
  }

  bool
  integer_field::is_value_option(value_type value) const {
    //assert value >= min_ && value <= max_
    return options_.test(value - min_);
  }
}
