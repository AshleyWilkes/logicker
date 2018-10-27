#pragma once
#include <boost/optional.hpp>
#include <set>
#include "value_set.hpp"

namespace logicker::core::constraint {
  class grid_slot {
    public:
      /*template<typename ValueType>
      void eliminate( const ValueType& value );

      void eliminate_empty();

      template<typename ValueType>
      void decide_on( const ValueType& value );

      void decide_on_empty();*/

      virtual bool is_decided() const = 0;

      /*template<typename ValueType>
      boost::optional<ValueType> get_decided_value() const;*/

      virtual ~grid_slot() = default;
  };

  template<class ValueType>
  class value_slot : public grid_slot {
    public:
      using Container = std::set<ValueType>;
      value_slot( value_set<ValueType> set );
      void eliminate( const ValueType& value );
      void eliminate_empty();
      void decide_on( const ValueType& value );
      void decide_on_empty();

      bool is_decided() const override;
      boost::optional<ValueType> get_decided_value() const;
    private:
      Container avails_, eliminateds_;
      bool is_empty_avail_;
      bool is_decided_;

      void update_is_decided();
  };

  template<class ValueType>
  value_slot<ValueType>::value_slot( value_set<ValueType> set) {
    for ( auto& set_value : set ) {
      avails_.insert( set_value.first );
    }
    is_empty_avail_ = set.get_num_empties() > 0;
    is_decided_ = ( is_empty_avail_ ? avails_.size() == 0 : avails_.size() == 1 );
  }

  template<class ValueType>
  void
  value_slot<ValueType>::eliminate( const ValueType& value ) {
    auto in_avails = avails_.find( value );
    if ( in_avails != avails_.end() && ! is_decided_ ) {
      avails_.erase( in_avails );
      eliminateds_.insert( value );
      update_is_decided();
    } else {
      throw "core/constraint/value_slot:eliminate error\n";
    }
  }

  template<class ValueType>
  void
  value_slot<ValueType>::decide_on( const ValueType& value ) {
    if ( avails_.find( value ) != avails_.end() && ! is_decided_ ) {
      for ( auto avail : avails_ ) {
        if ( avail != value ) {
          eliminateds_.insert( avail );
        }
      }
      avails_.clear();
      avails_.insert( value );
      is_decided_ = true;
    } else {
      throw "core/constraint/value_slot:decide_on error\n";
    }
  }

  template<class ValueType>
  bool
  value_slot<ValueType>::is_decided() const {
    return is_decided_;
  }

  template<class ValueType>
  boost::optional<ValueType>
  value_slot<ValueType>::get_decided_value() const {
    if ( is_decided_ ) {
      return ( is_empty_avail_ ? boost::none : boost::optional<ValueType>( *( avails_.begin() )));
    } else {
      throw "core/constraint/value_slot throwing on get when undecided";
    }
  }
}
