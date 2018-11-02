#pragma once

namespace logicker::core::constraint::extractor::value {
  template<typename GridSlotId, typename ComparedValueType>
  class slot_values_equal : public value_extractor<core::value::boolean, grid<GridSlotId>> {
    public:
      slot_values_equal( std::vector<GridSlotId> slot_ids );
      core::value::boolean extract( const grid<GridSlotId>& grid, bool throw_on_undecided = true ) const override;
      std::string to_string() const override;
    private:
      std::vector<GridSlotId> slot_ids_;
      using InnerExtractor = slot_value<GridSlotId, ComparedValueType>;
      std::vector<InnerExtractor> slot_extractors_;
  };

  template<typename GridSlotId, typename ComparedValueType>
  slot_values_equal<GridSlotId, ComparedValueType>::slot_values_equal( std::vector<GridSlotId> slot_ids ) : slot_ids_{ slot_ids } {
    for ( auto slot_id : slot_ids ) {
      slot_extractors_.push_back( InnerExtractor{ slot_id } );
    }
  }

  template<typename GridSlotId, typename ComparedValueType>
  core::value::boolean
  slot_values_equal<GridSlotId, ComparedValueType>::extract( const grid<GridSlotId>& grid, bool throw_on_undecided ) const {
    auto extractor_it = slot_extractors_.begin();
    ComparedValueType value = extractor_it->extract( grid, throw_on_undecided );
    for ( ++extractor_it; extractor_it != slot_extractors_.end(); ++extractor_it ) {
      ComparedValueType other = extractor_it->extract( grid, throw_on_undecided );
      if ( other != value ) {
        return false;
      }
    }
    return true;
  }

  template<typename GridSlotId, typename ComparedValueType>
  std::string
  slot_values_equal<GridSlotId, ComparedValueType>::to_string() const {
    std::string result{ "SlotValuesEqual: " };
    for ( auto slot_id : slot_ids_ ) {
      result += "[" + slot_id.first.to_string() + ", " + slot_id.second + "];";
    }
    return result;
  }
}
