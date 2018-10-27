#pragma once
#include <typeinfo>//std::bad_cast
#include "polymorphic_map.hpp"

namespace logicker::core::constraint {
  template<typename ValueSlotId, typename ValueBase>
  class givens {
    public:
      template<typename ValueType>
      void add( const ValueSlotId& slot_id, const ValueType& value );

      template<typename ValueType>
      const ValueType& get( const ValueSlotId& slot_id ) const;

      template<typename ValueType>
      ValueType& get( const ValueSlotId& slot_id );

      bool has( const ValueSlotId& slot_id ) const;
    private:
      polymorphic_map<ValueSlotId, ValueBase> map_;
  };

  template<typename ValueSlotId, typename ValueBase>
  template<typename ValueType>
  void
  givens<ValueSlotId, ValueBase>::add( const ValueSlotId& slot_id, const ValueType& value ) {
    map_.add( slot_id, value );
  }

  template<typename ValueSlotId, typename ValueBase>
  template<typename ValueType>
  const ValueType&
  givens<ValueSlotId, ValueBase>::get( const ValueSlotId& slot_id ) const {
    try {
      return map_.get( slot_id );
    } catch ( const std::bad_cast& e) {
      throw "bad cast inside givens::get const";
    }
  }

  template<typename ValueSlotId, typename ValueBase>
  template<typename ValueType>
  ValueType&
  givens<ValueSlotId, ValueBase>::get( const ValueSlotId& slot_id ) {
    try {
      return map_.get( slot_id );
    } catch( const std::bad_cast& e) {
      throw "bad cast inside givens::get";
    }
  }

  template<typename ValueSlotId, typename ValueBase>
  bool
  givens<ValueSlotId, ValueBase>::has( const ValueSlotId& slot_id ) const {
    return map_.has( slot_id );
  }
}
