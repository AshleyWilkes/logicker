#pragma once
#include "../value_extractor.hpp"
#include "../../../elements/value/boolean.hpp"

namespace logicker::core::constraint::extractor::value {
  template<typename GridSlotId>
  class slot_value_determined : public value_extractor<core::value::boolean, grid<GridSlotId>> {
    public:
      slot_value_determined( GridSlotId slot_id );
      core::value::boolean extract( const grid<GridSlotId>& grid, bool throw_on_undecided = true ) const override;
      std::string to_string() const override;
    private:
      const GridSlotId slot_id_;
  };

  template<typename GridSlotId>
  slot_value_determined<GridSlotId>::slot_value_determined( GridSlotId slot_id ) : slot_id_{ slot_id } {}

  template<typename GridSlotId>
  core::value::boolean
  slot_value_determined<GridSlotId>::extract( const grid<GridSlotId>& grid, bool throw_on_undecided ) const {
    auto& slot = grid.get_slot( slot_id_ );
    if ( slot.is_decided() ){
      return true;
    } else if ( throw_on_undecided ){
      throw "core/constraint/extractor/value/slot_value_determined throwing on undecided";
    } else {
      return false;
    }
  }

  template<typename GridSlotId>
  std::string
  slot_value_determined<GridSlotId>::to_string() const {
    return "SlotValueDetermined: [" + slot_id_.first.to_string() + ", " + slot_id_.second + "]";
  }
}
