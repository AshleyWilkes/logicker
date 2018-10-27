#pragma once
#include <typeinfo>//std::bad_cast
#include "polymorphic_map.hpp"
#include "value_slot.hpp"

namespace logicker::core::constraint {
  //wouldn't transporting her be easier if she were dead?
  //tedy nemit zde (a v givensech taky) slots_ jako membera,
  //ale proste polymorphic_map privatne podedit?
  template<typename ValueSlotId, typename ValueSlotBase = grid_slot>
  class grid {
    public:
      using ValSlotId = ValueSlotId;

      template<typename ValueType, template<typename> typename ValueSlotType = value_slot>
      void add_value_slot( const ValueSlotId& slot_id, const ValueSlotType<ValueType>& slot );

      template<typename ValueType, template<typename> typename ValueSlotType = value_slot>
      const ValueSlotType<ValueType>& get_slot( const ValueSlotId& slot_id ) const;

      template<typename ValueType, template<typename> typename ValueSlotType = value_slot>
      ValueSlotType<ValueType>& get_slot( const ValueSlotId& slot_id );

      const ValueSlotBase& get_slot( const ValueSlotId& slot_id ) const;
      ValueSlotBase& get_slot( const ValueSlotId& slot_id );

      bool has( const ValueSlotId& slot_id ) const;
    private:
      polymorphic_map<ValueSlotId, ValueSlotBase> slots_;
  };

  template<typename ValueSlotId, typename ValueSlotBase>
  template<typename ValueType, template<typename> typename ValueSlotType>
  void
  grid<ValueSlotId, ValueSlotBase>::add_value_slot( const ValueSlotId& slot_id, const ValueSlotType<ValueType>& slot ){
    slots_.add( slot_id, slot );
  }

  template<typename ValueSlotId, typename ValueSlotBase>
  template<typename ValueType, template<typename> typename ValueSlotType>
  const ValueSlotType<ValueType>&
  grid<ValueSlotId, ValueSlotBase>::get_slot( const ValueSlotId& slot_id ) const {
    try {
      return slots_.template get<const ValueSlotType<ValueType>&>( slot_id );
    } catch ( const std::bad_cast& e) {
      throw "bad cast inside grid::get_slot const";
    }
  }

  template<typename ValueSlotId, typename ValueSlotBase>
  template<typename ValueType, template<typename> typename ValueSlotType>
  ValueSlotType<ValueType>&
  grid<ValueSlotId, ValueSlotBase>::get_slot( const ValueSlotId& slot_id ) {
    try {
      return slots_.template get<ValueSlotType<ValueType>&>( slot_id );
    } catch ( const std::bad_cast& e ) {
      throw "bad cast inside grid::get_slot";
    }
  }

  template<typename ValueSlotId, typename ValueSlotBase>
  const ValueSlotBase&
  grid<ValueSlotId, ValueSlotBase>::get_slot( const ValueSlotId& slot_id ) const {
    return slots_.get( slot_id );
  }

  template<typename ValueSlotId, typename ValueSlotBase>
  ValueSlotBase&
  grid<ValueSlotId, ValueSlotBase>::get_slot( const ValueSlotId& slot_id ) {
    return slots_.get( slot_id );
  }

  template<typename ValueSlotId, typename ValueSlotBase>
  bool
  grid<ValueSlotId, ValueSlotBase>::has( const ValueSlotId& slot_id ) const {
    return slots_.has( slot_id );
  }
}
