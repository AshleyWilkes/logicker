#pragma once

namespace logicker::core::constraint::extractor::value {
  template<typename GridSlotId, typename SlotValueType, typename ExtractedValueType>
  class count_if : public value_extractor<ExtractedValueType, grid<GridSlotId>> {
    public:
      using ValueSlot = core::constraint::value_slot<SlotValueType>;
      using UnaryPredicate = std::function<bool (const ValueSlot&)>;
      count_if( std::vector<GridSlotId> slot_ids, UnaryPredicate predicate );
      ExtractedValueType extract( const grid<GridSlotId>& grid, bool throw_on_undecided = true ) const override;
      std::string to_string() const override;
    private:
      std::vector<GridSlotId> slot_ids_;
      UnaryPredicate predicate_;
  };

  template<typename GridSlotId, typename SlotValueType, typename ExtractedValueType>
  count_if<GridSlotId, SlotValueType, ExtractedValueType>::count_if( std::vector<GridSlotId> slot_ids, UnaryPredicate predicate ) : slot_ids_{ slot_ids }, predicate_{ predicate } {}

  template<typename GridSlotId, typename SlotValueType, typename ExtractedValueType>
  ExtractedValueType
  count_if<GridSlotId, SlotValueType, ExtractedValueType>::extract( const grid<GridSlotId>& grid, bool throw_on_undecided ) const {
    int result = 0;
    for ( auto slot_id : slot_ids_ ) {
      if ( grid.has( slot_id ) ) {
        auto& slot = grid.template get_slot<SlotValueType>( slot_id );
        if ( predicate_( slot ) ) {
          ++result;
        }
      }
    }
    return result;
  }

  template<typename GridSlotId, typename SlotValueType, typename ExtractedValueType>
  std::string
  count_if<GridSlotId, SlotValueType, ExtractedValueType>::to_string() const {
    std::string result{ "CountIf: " };
    for ( auto slot_id : slot_ids_ ) {
      result += "[" + slot_id.first.to_string() + ", " + slot_id.second + "];";
    }
    return result;
  }
}
