#pragma once
#include<map>
#include<memory>

namespace logicker::core::constraint {
  template<typename Key, typename BaseT>
  class polymorphic_map {
    private:
      using BaseTPtr = std::shared_ptr<BaseT>;
    public:
      template<typename T>
      void add( const Key& id, const T& content );

      template<typename T>
      const T& get( const Key& id ) const;

      template<typename T>
      T& get( const Key& id);

      const BaseT& get( const Key& id ) const;
      BaseT& get( const Key& id );

      bool has (const Key& id ) const;

      typename std::map<Key, BaseTPtr>::const_iterator begin() const { return map_.begin(); }
      typename std::map<Key, BaseTPtr>::const_iterator end() const { return map_.end(); }
    private:
      std::map<Key, BaseTPtr> map_;
  };

  template<typename Key, typename BaseT>
  template<typename T>
  void
  polymorphic_map<Key, BaseT>::add( const Key& id, const T& content ){
    BaseTPtr slot_ptr = std::make_shared<T>( content );
    map_.insert({ id, slot_ptr });
  }

  template<typename Key, typename BaseT>
  template<typename T>
  const T&
  polymorphic_map<Key, BaseT>::get( const Key& id ) const {
    return const_cast<polymorphic_map *>(this)
      ->template get<T>( id );
  }

  template<typename Key, typename BaseT>
  template<typename T>
  T&
  polymorphic_map<Key, BaseT>::get( const Key& id ) {
    BaseT& elem_ref = get( id );
    return dynamic_cast<T&>( elem_ref );
  }

  template<typename Key, typename BaseT>
  const BaseT&
  polymorphic_map<Key, BaseT>::get( const Key& id ) const {
    return const_cast<polymorphic_map *>(this)
      ->get( id );
  }

  template<typename Key, typename BaseT>
  BaseT&
  polymorphic_map<Key, BaseT>::get( const Key& id ) {
    return *(map_.find( id )->second);
  }

  template<typename Key, typename BaseT>
  bool
  polymorphic_map<Key, BaseT>::has( const Key& id ) const {
    return map_.find( id ) != map_.end();
  }
}
