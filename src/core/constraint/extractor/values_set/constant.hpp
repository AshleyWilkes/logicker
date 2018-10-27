#pragma once
#include "../values_set_extractor.hpp"

namespace logicker::core::constraint::extractor::values_set {
  template<typename Grid, typename ValueType, template<typename> typename ValuesSetType = value_set>
  class constant : public values_set_extractor<ValueType, Grid, ValuesSetType> {
    public:
      constant( ValuesSetType<ValueType> constant_value );
      ValuesSetType<ValueType> extract( const Grid& grid, bool throw_on_undecided = true ) const override;
      std::string to_string() const;
    private:
      ValuesSetType<ValueType> value_;
  };

  template<typename Grid, typename ValueType, template<typename> typename ValuesSetType>
  constant<Grid, ValueType, ValuesSetType>::constant( ValuesSetType<ValueType> constant_value ) : value_{ constant_value } {}

  template<typename Grid, typename ValueType, template<typename> typename ValuesSetType>
  ValuesSetType<ValueType>
  constant<Grid, ValueType, ValuesSetType>::extract( const Grid& grid, bool throw_on_undecided ) const {
    return value_;
  }

  template<typename Grid, typename ValueType, template<typename> typename ValuesSetType>
  std::string
  constant<Grid, ValueType, ValuesSetType>::to_string() const {
    return "Constant: " + value_.to_string();
  }
}
