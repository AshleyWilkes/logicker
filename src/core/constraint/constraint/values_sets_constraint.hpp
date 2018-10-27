#pragma once

namespace logicker::core::constraint {
  template<typename ValueType, typename Grid, template<typename> typename ValueSetType = value_set>
  class values_sets_constraint : public constraint<Grid> {
    public:
      constraint_status is_satisfied_by( const Grid& grid ) const override = 0;
  };
}
