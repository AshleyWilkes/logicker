#pragma once
#include "../value_extractor.hpp"

namespace logicker::core::constraint::extractor::value {
  template<typename ExtractedValueType, typename Grid>
  class constant : public value_extractor<ExtractedValueType, Grid> {
    public:
      constant( ExtractedValueType constant_value );
      ExtractedValueType extract( const Grid& grid, bool throw_on_undecided = true ) const override;
      std::string to_string() const override;
    private:
      ExtractedValueType value_;
  };

  template<typename ExtractedValueType, typename Grid>
  constant<ExtractedValueType, Grid>::constant( ExtractedValueType constant_value ) : value_{ constant_value } {}

  template<typename ExtractedValueType, typename Grid>
  ExtractedValueType
  constant<ExtractedValueType, Grid>::extract( const Grid& grid, bool throw_on_undecided ) const {
    return value_;
  }

  template<typename ExtractedValueType, typename Grid>
  std::string
  constant<ExtractedValueType, Grid>::to_string() const {
    return "Constant: " + value_.to_string();
  }
}
