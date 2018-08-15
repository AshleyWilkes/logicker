#pragma once
#include <boost/dynamic_bitset.hpp>
#include <boost/optional.hpp>
#include <iostream>

namespace logicker::core {
  template<class FieldType>
  class field;

  template<class FieldType>
  std::ostream& operator<<(std::ostream& os, const field<FieldType>& field);

  template<class FieldType>
  class field {
    public:
      typedef typename FieldType::value_type value_type;

      field(const FieldType& type);
      void set(value_type value);
      value_type get() const;
      void eliminate(value_type value);
      bool is_set() const;
      bool is_value_option(value_type value) const;
      std::vector<value_type> available_values() const;

      void clear();

      bool is_subset_of(const field& other) const;

      friend std::ostream& operator<< <>(std::ostream& os, const field<FieldType>& field);
    private:
      const FieldType type_;
      boost::optional<value_type> set_value_ { boost::none };
      boost::dynamic_bitset<> options_;
  };

  template<class FieldType>
  field<FieldType>::field(const FieldType& type) : type_{ type }, options_{ type.size() } {
    options_ = ~options_;
  }

  template<class FieldType>
  void
  field<FieldType>::set(value_type value) {
    if (set_value_ && set_value_ != value) {
      throw "field already set to another value";
    }
    int index = type_.value_to_index(value);
    //index should be ok here, value_to_index should throw if not
    if (options_.test(index)) {
      set_value_ = value;
      options_.clear();
      options_.resize( type_.size() );
      options_.flip(index);
    } else {
      throw "value not available anymore";
    }
    //mozna setnout options_ tak, aby umoznovaly tuto jedinou hodnotu??
  }

  template<class FieldType>
  typename field<FieldType>::value_type
  field<FieldType>::get() const {
    if (set_value_) {
      return set_value_.get();
    } else {
      throw "value not set";
    }
  }

  template<class FieldType>
  void
  field<FieldType>::eliminate(value_type value) {
    int index = type_.value_to_index(value);
    if (!options_.test(index)) {
      throw "value already eliminated";
    } else if (set_value_) {
      throw "value already set";
    } else {
      options_.flip(index);
      if (options_.count() == 1) {
        set_value_ = type_.index_to_value(options_.find_first());
      }
    }
  }

  template<class FieldType>
  bool
  field<FieldType>::is_set() const {
    return set_value_ != boost::none;
  }

  template<class FieldType>
  bool
  field<FieldType>::is_value_option(value_type value) const {
    return options_.test(type_.value_to_index(value));
  }

  template<class FieldType>
  void
  field<FieldType>::clear() {
    set_value_ = boost::none;
    options_.clear();
    options_.resize( type_.size(), true );
  }

  template<class FieldType>
  bool
  field<FieldType>::is_subset_of(const field& other) const {
    return options_.is_subset_of( other.options_ );
  }

  template<class FieldType>
  std::ostream&
  operator<<(std::ostream& os, const field<FieldType>& field) {
    if (field.set_value_) {
      os << " " << field.set_value_.get() << " ";
    } else {
      os << "(" << field.options_.count() << ")";
    }
    return os;
  }
}
