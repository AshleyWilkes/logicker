#pragma once
#include "../value_constraint.hpp"

namespace logicker::core::constraint::value {
  template<typename Grid>
  class not_both_true : public value_constraint<core::value::boolean, Grid> {
    private:
      using extractor = extractor::value_extractor<core::value::boolean, Grid>;
      using extractor_ptr = std::shared_ptr<extractor>;
    public:
      template<typename Extractor>
      not_both_true( const Extractor& value1, const Extractor& value2 );

      constraint_status is_satisfied_by( const Grid& grid ) const override;
    private:
      extractor_ptr extractor_1_;
      extractor_ptr extractor_2_;
  };

  template<typename Grid>
  template<typename Extractor>
  not_both_true<Grid>::not_both_true( const Extractor& value1, const Extractor& value2 ) : extractor_1_{ std::make_shared<Extractor>( value1 ) }, extractor_2_{ std::make_shared<Extractor>( value2 ) } {}

  template<typename Grid>
  constraint_status
  not_both_true<Grid>::is_satisfied_by( const Grid& grid ) const {
    core::value::boolean extracted_value_1 = extractor_1_->extract( grid );
    core::value::boolean extracted_value_2 = extractor_2_->extract( grid );

    constraint_status result;
    if ( extracted_value_1.get() && extracted_value_2.get() ) {
      constraint_status_message msg;
      msg.push_back( "NotBothTrue is broken:" );
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
