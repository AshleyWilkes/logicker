#pragma once
#include <algorithm>
#include <cstdlib>//std::abs
#include <iostream>
#include <string>
#include <tuple>
#include <utility>//std::pair
#include <vector>

namespace logicker::core::topology {
  //ad vytvareni teto veci: vidim dve cesty
  //a) vytvaret vzdy plnou coords_groupu (napr vsechny Centers)
  //  a pak z ni filtrama udelat, co potrebuju, napr row1
  //b) mit tovaren vic a pouzivat je jemneji rovnou k tvorbe
  //  toho, co potrebuju (nebo alespon jemnejsiho priblizeni)
  //
  //neverim, ze by se tohle mohlo stat vykonnostnim bottleneckem
  //  plus Premature Optimization
  //
  //na druhou stranu, pokud uzivatel potrebuje row1, nechci ho
  //  asi nutit pouzit k tomu vic nez jedno volani metody
  //  takze i kdybych sel cestou a), stejne budu chtit
  //  vyrobit i dalsi convenience metody
  //
  //takze rozdil mezi a) a b) je implementacni detail
  //  a problem, ktery zde resim, je jak ma vypadat interface
  //  techto (eventualne convenience) metod
  template<typename Coords, template<typename...> typename Container = std::vector>
  class coords_group {
    public:
      using const_iterator = typename Container<Coords>::const_iterator;

      coords_group( const std::string& id );

      void add( const Coords& coords );

      const_iterator begin() const;
      const_iterator end() const;

      std::size_t size() const;

      std::string id() const;

      template<typename CoordsFilter>
      coords_group& apply_filter( CoordsFilter filter );
    private:
      Container<Coords> elems_;
      std::string id_;
  };

  template<typename Coords, template<typename...> typename Container>
  coords_group<Coords, Container>::coords_group( const std::string& id ) : id_{ id } {}

  template<typename Coords, template<typename...> typename Container>
  void
  coords_group<Coords, Container>::add( const Coords& coords ) {
    elems_.push_back( coords );
  }

  template<typename Coords, template<typename...> typename Container>
  typename coords_group<Coords, Container>::const_iterator
  coords_group<Coords, Container>::begin() const {
    return elems_.begin();
  }

  template<typename Coords, template<typename...> typename Container>
  typename coords_group<Coords, Container>::const_iterator
  coords_group<Coords, Container>::end() const {
    return elems_.end();
  }

  template<typename Coords, template<typename...> typename Container>
  std::size_t
  coords_group<Coords, Container>::size() const {
    return elems_.size();
  }

  template<typename Coords, template<typename...> typename Container>
  std::string
  coords_group<Coords, Container>::id() const {
    return id_;
  }

  template<typename Coords, template<typename...> typename Container>
  template<typename CoordsFilter>
  coords_group<Coords, Container>&
  coords_group<Coords, Container>::apply_filter( CoordsFilter filter ) {
    elems_.erase( std::remove_if( elems_.begin(), elems_.end(), filter ), elems_.end() );
    return *this;
  }

  template<typename Coords, template<typename...> typename Container = std::vector>
  class coords_meta_group {
    public:
      using CoordsGroup = coords_group<Coords, Container>;
      using const_iterator = typename Container<CoordsGroup>::const_iterator;

      coords_meta_group() = default;

      template<typename Topology>
      coords_meta_group( const Topology& topology, const std::vector<std::string>& region_ids );

      void add( const CoordsGroup& cgroup );

      const_iterator begin() const;
      const_iterator end() const;

      template<typename CoordsFilter>
      coords_meta_group& apply_filter( CoordsFilter filter );
    private:
      Container<CoordsGroup> elems_;
  };

  template<typename Coords, template<typename...> typename Container>
  template<typename Topology>
  coords_meta_group<Coords, Container>::coords_meta_group( const Topology& topology, const std::vector<std::string>& region_ids ) {
    coords_group<Coords> centers{ topology.cg_centers() };
    if ( centers.size() != region_ids.size() ) {
      throw "coords_meta_group from regions sizes error";
    }
    std::map<std::string, CoordsGroup> map;
    for ( auto centers_it = centers.begin(), regions_it = region_ids.begin();
        centers_it != centers.end(); ++centers_it, ++regions_it ) {
      auto coords = *centers_it;
      auto region_id = *regions_it;
      if ( map.find( region_id ) == map.end() ) {
        map.insert({ region_id, coords_group<Coords>{ "Region " + region_id } } );
      }
      map.find( region_id )->second.add( coords );
    }
    for ( auto map_it : map ) {
      elems_.push_back( map_it.second );
    }
  }

  template<typename Coords, template<typename...> typename Container>
  void
  coords_meta_group<Coords, Container>::add( const CoordsGroup& cgroup ) {
    elems_.push_back( cgroup );
  }

  template<typename Coords, template<typename...> typename Container>
  typename coords_meta_group<Coords, Container>::const_iterator
  coords_meta_group<Coords, Container>::begin() const {
    return elems_.begin();
  }

  template<typename Coords, template<typename...> typename Container>
  typename coords_meta_group<Coords, Container>::const_iterator
  coords_meta_group<Coords, Container>::end() const {
    return elems_.end();
  }

  //tento design rectangle_coordsu zaslouzi nekolik poznamek:
  //1) definice tridy uvnitr jine tridy je odpornost
  //2) soucasti coords je nove index; ten usnadnuje implementaci
  //condition::is_satisfied_by(std::vector<value_type>); tohle nebylo
  //v planu a ted neni jasne, zda je to dobry napad
  //2a) je to vyborny napad, oddeluje to logiku toho, ze policka maji
  //mezi sebou vztahy (treba jsou sousedni) od toho, ze grid je skupina
  //policek. condition_instance maji obsahovat jen indexy svych policek,
  //coz by mel byt prvni krok k odstraneni templaty Topology z condition
  //instance
  //!!!!!!!!!!!!!!!!!!!!!!!!!
  //3) zavedeni indexu implikuje nove invarianty:
  //-- get_all_coords() musi coordsy vracet vzdy ve stejnem poradi
  //    (pri kazdem volani se stejnym size parametrem musi vratit
  //    stejne poradi coordsu)
  //-- get_all_coords() musi coordsy vracet v poradi indexu
  //    0 .. max_index
  //!!!!!!!!!!!!!!!!!!!!!!!!!
  //4) soucasti coords je nove size: size zabira misto
  //a aktualne se vubec nepouziva, ale je dobre mozne, ze
  //se to bude hodit pri nejakem budoucim debugovani;
  //do konstruktoru rectangle_coords se to posilat musi kvuli vypoctu
  //indexu, ale nemuselo by se ukladat dovnitr coords tridy
  class rectangle {
    public:
      typedef std::pair<int, int> topology_size_t;
      class rectangle_coords {
        public:
          //rectangle_coords(topology_size_t size, int row, int col);
          rectangle_coords(int row, int col);
          int row() const { return row_; }
          int col() const { return col_; }
          /*int index() { return index_; }
          topology_size_t get_size() { return size_; }*/
          friend bool operator<(const rectangle_coords& lhs, const rectangle_coords& rhs);
          friend std::ostream& operator<<(std::ostream& os, const rectangle_coords& coords);
          std::string to_string() const;
          //friend bool operator==(const rectangle_coords& lhs, const rectangle_coords& rhs);
        private:
          int row_;
          int col_;
          /*int index_;
          topology_size_t size_;*/
      };

      //je treba se kvalitne vyporadat s temito coords_groupami etc.
      //1) to, cemu rikam coords_group se ma jmenovat tak, aby z nazvu 
      //  bylo poznat, ze jde o popis, nikoli o instanci skupiny policek
      //2) ma ten popis z bodu 1) vubec existovat jako pojmenovana entita
      //  jeho smyslem beztak pouze je byt predan jako parametr nejake
      //  tovarne, ktera vyrobi prave instanci skupiny policek
      //3) takze ja myslim, ze reseni je mit overloadnutou
      //  tovarnu, ktera bere urcite parametry (jez odpovidaji
      //  popisu coords_groupy) a vraci objekt typu coords_groupa
      //4) tento objekt reprezentuje skupinu policek, umi se nechat
      //  pouzit jako range ve for cyklu, a rovnez umi nechat na sebe
      //  aplikovat filtr, ktery potencialne zmensi skupinu policek,
      //  jiz groupa reprezentuje
      //5) Mno a ta tovarna, ta ma byt zrejme soucasti topologie;
      //  zakladni parametry (Centers, Borders, Corners) maji stat
      //  dokonce nad jednotlivou topologii; nicmene spolecny predek
      //  topologii je overkill, dokud topologii predavam jako templatovy
      //  parametr; staci tedy mit pro tyto zakladni parametry spravne
      //  overloady tovarni metody
      //6) Kazda topologie dale podporuje vlastni parametry, ktere je mozne
      //  tovarne predat; u rectanglu jsou to rows, col, diags; u hexa
      //  to budou nejake directions; tyto vlastni parametry uz si specifikuje
      //  konkretni topologie sama a sama taky zajistuje prislusne
      //  overloady tovarni metody
      //7) Dale zde mame prozatim neuvazovane zakladni parametry
      //  NeighsOverBorder, NeighsOverCorner; ty by pro Centery
      //  specifikovaly jine Centery
      //8) A dale NeighCenters, NeighBorders, NeighCorners;
      //  ty by pro policka jineho zakladniho typu specifikovaly
      //  jejich sousedy; pro policko stejneho typu nejspis nedavaji rozumny
      //  smysl (Centers maji svoje NeighsOver; Corner zadne sousedni
      //  Cornery nema a u Borderu je to taky podivne), predpoklad tedy je,
      //  ze pouziti techto parametru s polickem stejneho typu neni definovano
      //  a jedna se tedy o error
      //!!A nerikejme Borderum Border, ale Side, to je mnohem konzistentnejsi!!
      using coords = rectangle_coords;
      using coords_range_old = std::vector<coords>;
      using coords_meta_group_old = std::string;
      using Index = int;
      using Direction = bool;//true for normal direction, false for reverse

      using coords_group_old = std::tuple<coords_meta_group_old, Index, Direction>;
      //ma bejt coords_groups_range!
      using coords_group_range_old = std::vector<coords_group_old>;

      /*template<typename Coords>
      using coords_meta_group = std::vector<coords_group<Coords>>;*/

      explicit rectangle(topology_size_t size);
      explicit rectangle(int width, int height);
      topology_size_t size() { return size_; }

      int coords_index(const rectangle_coords& coords) const;
      int fields_count() const;

      bool are_neighs( const coords& c1, const coords& c2, bool incl_corner, bool incl_side ) const;

      coords_group<coords> cg_centers() const;
      coords_group<coords> cg_row( int index, bool reverse_dir = false ) const;
      coords_group<coords> cg_col( int index, bool reverse_dir = false ) const;
      coords_meta_group<coords> cmg_rows_cols() const;
      //zde incl_corner urcuje, zda do vysledku patri policka, ktera sousedi rohem
      //incl_side urcuje, zda do vysledku patri policka, ktera sousedi stranou
      //!!nedobry design, dva bool parametry, u kterych neni jasne, ktery znamena co!!
      coords_group<coords> cg_neighs( const coords& root, bool incl_corner = true, bool incl_side = true ) const;
      //vraci dvojice policek; ordered urcuje, jestli je kazda dvojice ve vysledku jednou (false)
      //nebo dvakrat s opacnym poradim policek (true)
      //je to nepekny navrh, ordered jako prvni parametr musi byt vzdy specifikovany, kdyz chci
      //nedefaultni hodnoty ostatnich parametru
      //a budu vubec nekdy k necemu potrebovat verzi s ordered == true
      //(krom toho kazdy klient by si ty dvojice mohl otocit sam, kdyz uz by je potreboval)
      //nabizi se dat topologii propertu neighs_policy, ktera by urcovala hodnoty tech incl_XXX parametru (i pro cg_neighs)
      //!!takhle (3 bool parametry, kde na prvni pohled neni poznat, ktery znamena co), to nemuze zustat!!
      coords_meta_group<coords> cmg_neigh_pairs( bool ordered = false, bool incl_corner = true, bool incl_side = true ) const;

      coords_group_range_old get_all_coords_groups(const std::vector<coords_meta_group_old>& meta_group) const;
      coords_range_old get_coords_in_group(coords_group_old group) const;

      friend bool operator==(const rectangle& lhs, const rectangle& rhs) {
        return lhs.size_ == rhs.size_;
      }
    private:
      topology_size_t size_;
  };

  static rectangle::coords_meta_group_old CMG_Rows = "Rows";
  static rectangle::coords_meta_group_old CMG_Cols = "Cols";
  static rectangle::coords_meta_group_old CMG_Centers = "Centers";
  static const std::vector<rectangle::coords_meta_group_old> coords_meta_groups { "Rows", "Cols" };
  static std::vector<rectangle::coords_meta_group_old> CMG_rows_cols { CMG_Rows, CMG_Cols };

  //sem by asi patrilo pridat i CMGs, ktery jsou nahore, ted ale neni vhodna chvile
  static rectangle::coords_group_old CG_Centers = { CMG_Centers, 0, false };

  rectangle::rectangle(rectangle::topology_size_t size) : size_{ size } {}

  rectangle::rectangle(int width, int height) : size_{ width, height } {}

  bool operator<(const rectangle::rectangle_coords& lhs, const rectangle::rectangle_coords& rhs) {
    if (lhs.row_ < rhs.row_) return true;
    if (lhs.row_ == rhs.row_) return lhs.col_ < rhs.col_;
    return false;
  }

  std::ostream& operator<<(std::ostream& os, const rectangle::rectangle_coords& coords) {
    os << "[" << coords.row_ << ", " << coords.col_ << "]";
    return os;
  }

  std::string 
  rectangle::rectangle_coords::to_string() const {
    return "[" + std::to_string( row_ ) + ", " + std::to_string( col_ ) + "]";
  }

  /*bool operator==(const rectangle::rectangle_coords& lhs, const rectangle::rectangle_coords& rhs) {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_);
  }*/

  /*rectangle::rectangle_coords::rectangle_coords(rectangle::topology_size_t size, int row, int col) : row_{row}, col_{col}, index_{row * size.second + col}, size_{size} {}*/
  
  rectangle::rectangle_coords::rectangle_coords(int row, int col) : row_{row}, col_{col} {}
  
  int
  rectangle::coords_index(const rectangle_coords& coords) const {
    return coords.row() * size_.first + coords.col();
    //throw "core::elements::topology::rectangle::coords_index: not impled";
  }

  int
  rectangle::fields_count() const {
    return size_.first * size_.second;
  }

  bool
  rectangle::are_neighs( const coords& c1, const coords& c2, bool incl_corner, bool incl_side ) const {
    int rDist = std::abs( c1.row() - c2.row() );
    int cDist = std::abs( c1.col() - c2.col() );
    int min = std::min( rDist, cDist );
    int max = std::max( rDist, cDist );
    if ( min == 0 && max == 1 && incl_side ) {
      return true;
    }
    if ( min == 1 && max == 1 && incl_corner ) {
      return true;
    }
    return false;
  }

  coords_group<rectangle::coords>
  rectangle::cg_centers() const {
    coords_group<coords> result{ "Centers" };
    for (int row = 0; row < size_.second; ++row) {
      for (int col = 0; col < size_.first; ++col) {
        result.add({ row, col });
      }
    }
    return result;
  }

  coords_group<rectangle::coords>
  rectangle::cg_row( int index, bool reverse_dir ) const {
    //sanity check index!
    coords_group<coords> result{ "Row " + std::to_string( index ) };
    if ( reverse_dir ) {
      for ( int col = size_.first - 1; col >= 0; --col ) {
        result.add({ index, col });
      }
    } else {
      for ( int col = 0; col < size_.first; ++col ) {
        result.add({ index, col });
      }
    }
    return result;
  }

  coords_group<rectangle::coords>
  rectangle::cg_col( int index, bool reverse_dir ) const {
    //sanity check index!
    coords_group<coords> result{ "Col " + std::to_string( index ) };
    if ( reverse_dir ) {
      for ( int row = size_.second - 1; row >= 0; --row ) {
        result.add({ row, index });
      }
    } else {
      for ( int row = 0; row < size_.second; ++row ) {
        result.add({ row, index });
      }
    }
    return result;
  }

  coords_meta_group<rectangle::coords>
  rectangle::cmg_rows_cols() const {
    coords_meta_group<coords> result;
    for ( int row = 0; row < size_.second; ++row ) {
      result.add( cg_row( row ) );
    }
    for ( int col = 0; col < size_.first; ++col ) {
      result.add( cg_col( col ) ) ;
    }
    return result;
  }

  coords_group<rectangle::coords>
  rectangle::cg_neighs( const coords& root, bool incl_corner, bool incl_side ) const {
    coords_group<coords> result{ "Neighs of " + root.to_string() };
    for ( coords coords : cg_centers() ) {
      if ( are_neighs( root, coords, incl_corner, incl_side ) ) {
        result.add( coords );
      }
    }
    return result;
  }

  coords_meta_group<rectangle::coords>
  rectangle::cmg_neigh_pairs( bool ordered, bool incl_corner, bool incl_side ) const {
    coords_meta_group<coords> result;
    auto centers = cg_centers();
    for ( auto coords_it_1 = centers.begin(); coords_it_1 != centers.end(); ++coords_it_1 ) {
      for ( auto coords_it_2 = coords_it_1; coords_it_2 != centers.end(); ++coords_it_2 ) {
        if ( are_neighs( *coords_it_1, *coords_it_2, incl_corner, incl_side ) ) {
          coords_group<coords> cgroup{ "Neighs " + coords_it_1->to_string() + " and " + coords_it_2->to_string() };
          cgroup.add( *coords_it_1 );
          cgroup.add( *coords_it_2 );
          result.add( cgroup );
        }
      }
    }
    return result;
  }

  rectangle::coords_group_range_old
  rectangle::get_all_coords_groups(const std::vector<coords_meta_group_old>& meta_groups) const {
    std::vector<coords_group_old> result_vec;
    for (auto group : meta_groups) {
      if (group == "Rows") {
        for (int i = 0; i < size_.second; ++i) {
          result_vec.push_back( { CMG_Rows, i, true } );
        }
      } else if (group == "Cols") {
        for (int i = 0; i < size_.first; ++i) {
          result_vec.push_back( { CMG_Cols, i, true } );
        }
      } else {
        throw "unsupported meta group";
      }
    }
    return result_vec;
  }

  rectangle::coords_range_old
  rectangle::get_coords_in_group(coords_group_old group) const {
    std::vector<coords> result_vec;
    coords_meta_group_old cmg{ std::get<0>(group) };
    Index index{ std::get<1>(group) };
    //Direction dir{ std::get<2>(group) };
    if (cmg == CMG_Rows) {
      for (int col = 0; col < size_.first; ++col) {
        //result_vec.push_back({size_, index, col});
        result_vec.push_back({index, col});
      }
    } else if (cmg == CMG_Cols) {
      for (int row = 0; row < size_.second; ++row) {
        //result_vec.push_back({size_, row, index});
        result_vec.push_back({row, index});
      }
    } else if (cmg == CMG_Centers) {
      for (int row = 0; row < size_.second; ++row) {
        for (int col = 0; col < size_.first; ++col) {
          result_vec.push_back({ row, col });
        }
      }
    }
    return result_vec;
  }
}
