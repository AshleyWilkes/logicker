#pragma once
#include <map>
#include <vector>

namespace logicker::core {
  //toto musi mit template parametr ValueType
  //jsou dva druhy field_typu:
  //-- budto jsem schopen rozhodnout, kolik hodnot pro policko pripada v uvahu
  //-- nebo je pocet moznosti neomezeny (??napr libovolne vysoky integer??); proted me nenapada priklad ani ted implemetaci nepotrebuju, takze predpokladejme, ze toto neni treba
  //podle techto moznosti je treba vybrat i field, ktery se s danym field_typem pouzije, soucasny integer_field potrebuje vedet, pro kolik hodnot si ma alokovat bitset
  template<class ValueType>
  class field_type {};

  template<>
  class field_type<int> {
    public:
      typedef int value_type;
      field_type(value_type min, value_type max);
      field_type(std::vector<value_type> values);

      std::size_t size() const;
      int value_to_index(value_type value) const;
      value_type index_to_value(int index) const;
    private:
      std::vector<value_type> values_;
      std::map<value_type, int> indices_;

      static std::vector<value_type> limits_to_range(value_type min, value_type max);
  };

  field_type<int>::field_type(value_type min, value_type max) : field_type{ field_type<int>::limits_to_range(min, max) } {}

  field_type<int>::field_type(std::vector<value_type> values) : values_{values} {
    for (int i = 0; i < values_.size(); ++i) {
      indices_.insert({ values_[i], i });
    }
  }

  std::size_t
  field_type<int>::size() const {
    return values_.size();
  }

  int
  field_type<int>::value_to_index(value_type value) const {
    auto it = indices_.find(value);
    if (it != indices_.end()) {
      return it->second;
    } else {
      throw "Unknown value";
    }
  }

  typename field_type<int>::value_type
  field_type<int>::index_to_value(int index) const {
    return values_.at(index);
  }

  std::vector<typename field_type<int>::value_type>
  field_type<int>::limits_to_range(value_type min, value_type max) {
    std::vector<value_type> result;
    for (int i = min; i <= max; ++i) {
      result.push_back(i);
    }
    return result;
  }
}
