#pragma once
#include <boost/optional.hpp>

namespace logicker::core::constraint::extractor::value {
  template<typename GridSlotId, typename ExtractedValueType>
  class slot_value : public value_extractor<ExtractedValueType, grid<GridSlotId>> {
    public:
      slot_value( GridSlotId slot_id );
      ExtractedValueType extract( const grid<GridSlotId>& grid, bool throw_on_undecided = true ) const override;
      std::string to_string() const override;
    private:
      GridSlotId slot_id_;
  };

  template<typename GridSlotId, typename ExtractedValueType>
  slot_value<GridSlotId, ExtractedValueType>::slot_value( GridSlotId slot_id ) : slot_id_{ slot_id } {}

  template<typename GridSlotId, typename ExtractedValueType>
  ExtractedValueType
  slot_value<GridSlotId, ExtractedValueType>::extract( const grid<GridSlotId>& grid, bool throw_on_undecided ) const {
    auto& slot = grid.template get_slot<ExtractedValueType>( slot_id_ );
    if ( slot.is_decided() ){
      auto result = slot.get_decided_value();
      if ( result ) {
        return result.get();
      } else {
        throw "constraint::exractor::value::slot_value::extract: empty value not impled";
      }
    } else if ( throw_on_undecided ){
      throw "constraint::extractor::value::slot_value::extract: not impled";
    } else {
      //tady je totiz neco spatne, tahle metoda by spis mela vracet optional<optional<ExtractedValueType>>, ale to zas dava smysl, jen pokud pripoustim empties (ve slotu muze byt empty value), jinymi slovy nedomysleno
      throw "constraint::extractor::value::slot_value::extract: not throwing on undecided not impled";
    }
  }

  template<typename GridSlotId, typename ExtractedValueType>
  std::string
  slot_value<GridSlotId, ExtractedValueType>::to_string() const {
    return "SlotValue: [" + slot_id_.first.to_string() + ", " + slot_id_.second + "]";
  }
}
