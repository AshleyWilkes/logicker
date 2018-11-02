#pragma once
#include "../value_constraint.hpp"
#include "../../extractor/value/constant.hpp"

namespace logicker::core::constraint::value {
  template<typename ValueType, typename Grid>
  class values_equal : public value_constraint<ValueType, Grid> {
    public:
      template<typename Extractor>
      values_equal(
          const Extractor& value_1,
          const Extractor& value_2 );
      template<typename Extractor>
      values_equal(
          const Extractor& value_1,
          const ValueType& value_2 );

      constraint_status is_satisfied_by( const Grid& grid ) const override;
    private:
      using constant = extractor::value::constant<ValueType, Grid>;
      using extractor = extractor::value_extractor<ValueType, Grid>;
      using extractor_ptr = std::shared_ptr<extractor>;
      extractor_ptr extractor_1_;
      extractor_ptr extractor_2_;
      //chtel bych to mit takto, s referencemi a bez pointeru, ale 
      //neumim to tak udelat a nevim, jestli to jde
      //const extractor::value_extractor<ValueType, Grid>& extractor_1_;
      //const extractor::value_extractor<ValueType, Grid>& extractor_2_;
  };

  template<typename ValueType, typename Grid>
  template<typename Extractor>
  values_equal<ValueType, Grid>::values_equal(
      const Extractor& value_1,
      const Extractor& value_2 ):
    extractor_1_{ std::make_shared<Extractor>( value_1 )},
    extractor_2_{ std::make_shared<Extractor>( value_2 )} {}

  template<typename ValueType, typename Grid>
  template<typename Extractor>
  values_equal<ValueType, Grid>::values_equal(
      const Extractor& value_1,
      const ValueType& value_2) : 
    extractor_1_{ std::make_shared<Extractor>( value_1 )},
    extractor_2_{ std::make_shared<constant>( value_2 )} {}
    //extractor_1_{ value1 }, 
    //extractor_2_{ extractor::value::constant<ValueType, Grid>{ value2 }} {}

  template<typename ValueType, typename Grid>
  constraint_status
  values_equal<ValueType, Grid>::is_satisfied_by( const Grid& grid ) const {
    ValueType extracted_value_1 = extractor_1_->extract( grid );
    ValueType extracted_value_2 = extractor_2_->extract( grid );

    constraint_status result;
    if ( extracted_value_1 != extracted_value_2 ) {
      constraint_status_message msg;
      msg.push_back( "ValuesEqual is broken:" );
      msg.push_back( "Extractor1: " + extractor_1_->to_string() );
      msg.push_back( "ExtractedValue1: " + extracted_value_1.to_string() );
      msg.push_back( "Extractor2: " + extractor_2_->to_string() );
      msg.push_back( "ExtractedValue2: " + extracted_value_2.to_string() );
      result.add_message( msg );
      result.set_broken( true );
    }
    return result;
  }
}
