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
      typedef std::pair<int, int> size;
      class rectangle_coords {
        public:
          rectangle_coords(size size, int row, int col);
          int row() { return row_; }
          int col() { return col_; }
          int index() { return index_; }
          size get_size() { return size_; }
          friend bool operator<(const rectangle_coords& lhs, const rectangle_coords& rhs);
        private:
          const int row_;
          const int col_;
          const int index_;
          const size size_;
      };
      typedef rectangle_coords coords;
      typedef std::vector<coords> coords_range;
      typedef std::tuple<CoordsMetaGroup, Index, Direction> coords_group;
      typedef std::vector<coords_group> coords_group_range;

      //tohle vsechno udelat static; topologie nebudou instanciovatelne, neni proc
      static coords_range get_all_coords(size size);

      static coords_group_range get_all_coords_groups(size size, const std::vector<CoordsMetaGroup>& meta_group);
      static coords_range get_coords_in_group(size size, coords_group group);
  };

  bool operator<(const rectangle::rectangle_coords& lhs, const rectangle::rectangle_coords& rhs) {
    return lhs.index_ < rhs.index_;
  }

  rectangle::rectangle_coords::rectangle_coords(rectangle::size size, int row, int col) : row_{row}, col_{col}, index_{row * size.second + col}, size_{size} {}

  rectangle::coords_range
  rectangle::get_all_coords(size size) {
    std::vector<coords> result_vec;
    for (int row = 0; row < size.first; ++row) {
      for (int col = 0; col < size.second; ++col) {
        result_vec.push_back({size, row, col});
      }
    }
    return result_vec;
  }

  rectangle::coords_group_range
  rectangle::get_all_coords_groups(size size, const std::vector<CoordsMetaGroup>& meta_groups) {
    std::vector<coords_group> result_vec;
    for (auto group : meta_groups) {
      if (group == "Rows") {
        for (int i = 0; i < size.first; ++i) {
          result_vec.push_back( { CMG_Rows, i, true } );
        }
      } else if (group == "Cols") {
        for (int i = 0; i < size.second; ++i) {
          result_vec.push_back( { CMG_Cols, i, true } );
        }
      } else {
        throw "unsupported meta group";
      }
    }
    return result_vec;
  }

  rectangle::coords_range
  rectangle::get_coords_in_group(size size, coords_group group) {
    std::vector<coords> result_vec;
    CoordsMetaGroup cmg{ std::get<0>(group) };
    Index index{ std::get<1>(group) };
    Direction dir{ std::get<2>(group) };
    if (cmg == CMG_Rows) {
      for (int col = 0; col < size.second; ++col) {
        result_vec.push_back({size, index, col});
      }
    } else if (cmg == CMG_Cols) {
      for (int row = 0; row < size.first; ++row) {
        result_vec.push_back({size, row, index});
      }
    }
    return result_vec;
  }
}
