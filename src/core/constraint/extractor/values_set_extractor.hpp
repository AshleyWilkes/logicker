#pragma once

namespace logicker::core::constraint::extractor {
  template<typename ExtractedValueType, typename Grid, template<typename> typename ValuesSetType = value_set>
  class values_set_extractor {
    public:
      virtual ValuesSetType<ExtractedValueType> extract( const Grid& grid, bool throw_on_undecided = true ) const = 0;
      virtual std::string to_string() const = 0;
      virtual ~values_set_extractor() = default;
      /*template<typename GridSlotId, typename GridValueType>
      void set_source( grid<GridSlotId, GridValueType> grid );

      virtual ValuesSetType<ExtractedValueType> extract( bool throw_on_undecided = true ) const = 0;*/
  };
}
