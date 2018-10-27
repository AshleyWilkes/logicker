#pragma once
#include "../values_sets_constraint.hpp"
#include "../../extractor/values_set_extractor.hpp"
#include "../../extractor/values_set/constant.hpp"

namespace logicker::core::constraint::values_sets {
  template<typename ValueType, typename Grid, template<typename> typename ValueSetType = value_set>
  class values_sets_equal : public values_sets_constraint<ValueType, Grid, ValueSetType> {
    public:
      values_sets_equal(
          const extractor::values_set_extractor<ValueType, Grid, ValueSetType>& value1,
          const extractor::values_set_extractor<ValueType, Grid, ValueSetType>& value2);
      template<typename Extractor>
      values_sets_equal(
          const Extractor& value1,
          const ValueSetType<ValueType>& value2);

      constraint_status is_satisfied_by( const Grid& grid) const override;
    private:
      using constant = extractor::values_set::constant<Grid, ValueType, ValueSetType>;
      //ty prohozeny parametry mezi values_set_extractor a jeho potomky jsou maso!
      using extractor = extractor::values_set_extractor<ValueType, Grid, ValueSetType>;
      using extractor_ptr = std::shared_ptr<extractor>;
      extractor_ptr extractor_1_;
      extractor_ptr extractor_2_;
      //const extractor::values_set_extractor<ValueType, ValueSetType>& extractor_1_;
      //const extractor::values_set_extractor<ValueType, ValueSetType>& extractor_2_;
  };

  template<typename ValueType, typename Grid, template<typename> typename ValueSetType>
  template<typename Extractor>
  values_sets_equal<ValueType, Grid, ValueSetType>::values_sets_equal(
      const Extractor& value1,
      const ValueSetType<ValueType>& value2) :
    extractor_1_{ std::make_shared<Extractor>( value1 )},
    extractor_2_{ std::make_shared<constant>( value2 )} {}

  template<typename ValueType, typename Grid, template<typename> typename ValueSetType>
  constraint_status
  values_sets_equal<ValueType, Grid, ValueSetType>::is_satisfied_by( const Grid& grid ) const {
    ValueSetType<ValueType> extracted_value_set_1 = extractor_1_->extract( grid );
    ValueSetType<ValueType> extracted_value_set_2 = extractor_2_->extract( grid );

    constraint_status result;
    if ( extracted_value_set_1 != extracted_value_set_2 ) {
      constraint_status_message msg;
      msg.push_back( "ValuesSetsEqual is broken:" );
      msg.push_back( "Extractor1: " + extractor_1_->to_string() );
      msg.push_back( "ExtractedSet1: " + extracted_value_set_1.to_string() );
      msg.push_back( "Extractor2: " + extractor_2_->to_string() );
      msg.push_back( "ExtractedSet2: " + extracted_value_set_2.to_string() );
      result.add_message( msg );
      result.set_broken( true );
    }
    return result;
  }
}
