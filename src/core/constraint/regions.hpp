/*#pragma once
#include "polymorphic_map.hpp"
#include <vector>

namespace logicker::core::constraint {
  template<typename RegionId, typename RegionElementType, template<typename...> typename Container = std::vector>
  class regions {
    public:
      using Region = Container<RegionElementType>;

      template<typename Topology>
      regions( const Topology& topology, const std::vector<RegionId>& ids_vector );

      void add( const RegionId& region_id, bool throw_on_exists = true );
      void add( const RegionId& region_id, const RegionElementType& element );

      const Region& get( const RegionId& region_id ) const;
      Region& get( const RegionId& region_id );

      typename std::map<RegionId, std::shared_ptr<Region>>::const_iterator begin() const { return map_.begin(); }
      typename std::map<RegionId, std::shared_ptr<Region>>::const_iterator end() const { return map_.end(); }
    private:
      polymorphic_map<RegionId, Region> map_;
  };

  //overengineered?? sice predavam typename RegionElementType, abych mohl mit
  //i regiony tvorene ValueSloty, ne jen Coordsy, ale implementace
  //konstruktoru predpoklada, ze region tvori Coordsy, a to navic
  //jen Coordsy z CG_Center
  template<typename RegionId, typename RegionElementType, template<typename...> typename Container>
  template<typename Topology>
  regions<RegionId, RegionElementType, Container>::regions( const Topology& topology, const std::vector<RegionId>& ids_vector ) : map_{} {
    //checkout, ze ids_vector ma stejny pocet prvku jako topology coords pro Centers
    auto coords_vector = topology.get_coords_in_group( topology::CG_Centers );
    if ( coords_vector.size() != ids_vector.size() ) {
      throw "regions::regions input error; there are " + std::to_string( coords_vector.size() )
        + " coords but " + std::to_string( ids_vector.size() ) + " region_ids";
    }
    for ( auto coords_it = coords_vector.cbegin(), region_it = ids_vector.cbegin();
        coords_it != coords_vector.cend(); ++coords_it, ++region_it ) {
      auto coords = *coords_it;
      auto region_id = *region_it;
      //pokud region s danym id neexistuje, zalozit
      if ( ! map_.has( region_id ) ) {
        map_.template add<Region>( region_id, {} );
      }
      //sehnat region s danym id
      auto& region = map_.get( region_id );
      //pridat aktualni coordsy do sehnaneho regionu
      region.push_back( coords );
    }
  }
}*/
