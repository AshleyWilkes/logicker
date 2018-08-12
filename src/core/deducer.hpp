#pragma once
#include <algorithm>
#include <boost/optional.hpp>
#include <vector>

namespace logicker::core {
  template<class ValueType>
  struct basic_deduction {
    int index_;
    ValueType value_;
  };

  template<class ValueType>
  class deducer {
    public:
      virtual boost::optional<basic_deduction<ValueType>> try_deduce(std::vector<field<field_type<ValueType>>> fields) const = 0;
  };

  template<class ValueType>
  class neq_deducer : public deducer<ValueType> {
    public:
      boost::optional<basic_deduction<ValueType>> try_deduce(std::vector<field<field_type<ValueType>>> fields) const override;
  };

  template<class ValueType>
  boost::optional<basic_deduction<ValueType>>
  neq_deducer<ValueType>::try_deduce(std::vector<field<field_type<ValueType>>> fields) const {
    if (fields.size() != 2) {
      throw "neq_deducer::try_deduce -- only 2 fields deductioning impled";
    }
    //tady predpokladam, ze kdyz je field is_set, tak obsahuje hodnotu, ono vubec integer field je na pikacu
    field<field_type<ValueType>> f1 = fields[0];
    field<field_type<ValueType>> f2 = fields[1];
    if (f1.is_set() && !f2.is_set() && f2.is_value_option(f1.get())) {
      return basic_deduction<ValueType>{ 1, f1.get() };
    } else if (f2.is_set() && !f1.is_set() && f1.is_value_option(f2.get())) {
      return basic_deduction<ValueType>{ 0, f2.get() };
    }
    return boost::none;
  }
}
