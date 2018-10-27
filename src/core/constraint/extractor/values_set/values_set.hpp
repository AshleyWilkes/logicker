#pragma once
#include "../values_set_extractor.hpp"

namespace logicker::core::constraint::extractor::values_set {
  template<typename GridSlotId, typename ExtractedValueType, template<typename> typename ValuesSetType = value_set>
  class values_set : public values_set_extractor<ExtractedValueType, grid<GridSlotId>, ValuesSetType> {
    public:
      values_set( std::vector<GridSlotId> slot_ids );
      ValuesSetType<ExtractedValueType> extract( const grid<GridSlotId>& grid, bool throw_on_undecided = true ) const override;
      std::string to_string() const;
    private:
      std::vector<GridSlotId> slot_ids_;
  };

  template<typename GridSlotId, typename ExtractedValueType, template<typename> typename ValuesSetType>
  values_set<GridSlotId, ExtractedValueType, ValuesSetType>::values_set( std::vector<GridSlotId> slot_ids ) : slot_ids_{ slot_ids } {}

  template<typename GridSlotId, typename ExtractedValueType, template<typename> typename ValuesSetType>
  ValuesSetType<ExtractedValueType>
  values_set<GridSlotId, ExtractedValueType, ValuesSetType>::extract( const grid<GridSlotId>& grid, bool throw_on_undecided ) const {
    std::set<ExtractedValueType> set;
    for ( auto slot_id : slot_ids_ ) {
      auto& slot = grid.template get_slot<ExtractedValueType>( slot_id );
      if ( slot.is_decided() ) {
        auto slot_value = slot.get_decided_value();
        set.insert( slot_value.get() );
        if ( slot_value ) {
        } else {
          throw "constraint/extractor/values_set/values_set::extract: empty value not impled";
        }
      } else if ( throw_on_undecided ) {
        throw "constraint/extractor/values_set/values_set::extract: not impled";
      } else {
        throw "constraint/extractor/value/slot_value::extract: not throwing on undecided not impled";
      }
    }
    return ValuesSetType<ExtractedValueType>{ set, 0 };
  }

  template<typename GridSlotId, typename ExtractedValueType, template<typename> typename ValuesSetType>
  std::string
  values_set<GridSlotId, ExtractedValueType, ValuesSetType>::to_string() const {
    std::string result{ "ValuesSet: " };
    for ( auto slot_id : slot_ids_ ) {
      result += "[" + slot_id.first.to_string() + ", " + slot_id.second + "];";
    }
    return result;
  }
}
