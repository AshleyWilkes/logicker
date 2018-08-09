#pragma once
#include <string>
#include <tuple>
#include <utility>//std::pair
#include <vector>

namespace logicker::core {
  //!!usingy a staticy jsou WTF a how-to!!
  using CoordsMetaGroup = std::string;
  using Index = int;
  using Direction = bool;//true for normal direction, false for reverse

  static CoordsMetaGroup CMG_Rows = "Rows";
  static CoordsMetaGroup CMG_Cols = "Cols";
  static std::vector<CoordsMetaGroup> coords_meta_groups { CMG_Rows, CMG_Cols };

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
          rectangle_coords(topology_size_t size, int row, int col);
          int row() { return row_; }
          int col() { return col_; }
          int index() { return index_; }
          topology_size_t get_size() { return size_; }
          friend bool operator<(const rectangle_coords& lhs, const rectangle_coords& rhs);
          //friend bool operator==(const rectangle_coords& lhs, const rectangle_coords& rhs);
        private:
          int row_;
          int col_;
          int index_;
          topology_size_t size_;
      };
      typedef rectangle_coords coords;
      typedef std::vector<coords> coords_range;
      typedef std::tuple<CoordsMetaGroup, Index, Direction> coords_group;
      typedef std::vector<coords_group> coords_group_range;

      rectangle(topology_size_t size);
      topology_size_t size() { return size_; }

      //tohle vsechno udelat static; topologie nebudou instanciovatelne, neni proc
      //wrong! je proc, to posilani size objektu jako parametru vsech techto metod
      //je ugly a kdyz uz si grid musi uchovavat instanci size, to uz si stejne
      //dobre muze uchovavat instanci Topology. Taky je to citelnejsi.
      int fields_count() const;

      coords_group_range get_all_coords_groups(const std::vector<CoordsMetaGroup>& meta_group);
      coords_range get_coords_in_group(coords_group group);
    private:
      topology_size_t size_;
  };

  rectangle::rectangle(rectangle::topology_size_t size) : size_{ size } {}

  bool operator<(const rectangle::rectangle_coords& lhs, const rectangle::rectangle_coords& rhs) {
    if (lhs.row_ < rhs.row_) return true;
    if (lhs.row_ == rhs.row_) return lhs.col_ < rhs.col_;
    return false;
  }

  /*bool operator==(const rectangle::rectangle_coords& lhs, const rectangle::rectangle_coords& rhs) {
    return (lhs.row_ == rhs.row_) && (lhs.col_ == rhs.col_);
  }*/

  rectangle::rectangle_coords::rectangle_coords(rectangle::topology_size_t size, int row, int col) : row_{row}, col_{col}, index_{row * size.second + col}, size_{size} {}
  
  int
  rectangle::fields_count() const {
    return size_.first * size_.second;
  }

  rectangle::coords_group_range
  rectangle::get_all_coords_groups(const std::vector<CoordsMetaGroup>& meta_groups) {
    std::vector<coords_group> result_vec;
    for (auto group : meta_groups) {
      if (group == "Rows") {
        for (int i = 0; i < size_.first; ++i) {
          result_vec.push_back( { CMG_Rows, i, true } );
        }
      } else if (group == "Cols") {
        for (int i = 0; i < size_.second; ++i) {
          result_vec.push_back( { CMG_Cols, i, true } );
        }
      } else {
        throw "unsupported meta group";
      }
    }
    return result_vec;
  }

  rectangle::coords_range
  rectangle::get_coords_in_group(coords_group group) {
    std::vector<coords> result_vec;
    CoordsMetaGroup cmg{ std::get<0>(group) };
    Index index{ std::get<1>(group) };
    //Direction dir{ std::get<2>(group) };
    if (cmg == CMG_Rows) {
      for (int col = 0; col < size_.second; ++col) {
        result_vec.push_back({size_, index, col});
      }
    } else if (cmg == CMG_Cols) {
      for (int row = 0; row < size_.first; ++row) {
        result_vec.push_back({size_, row, index});
      }
    }
    return result_vec;
  }
}
