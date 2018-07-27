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

  class rectangle {
    public:
      typedef std::pair<int, int> size;
      typedef std::pair<int, int> coords;
      typedef std::vector<coords> coords_range;
      typedef std::tuple<CoordsMetaGroup, Index, Direction> coords_group;
      typedef std::vector<coords_group> coords_group_range;

      //tohle vsechno udelat static; topologie nebudou instanciovatelne, neni proc
      static coords_range get_all_coords(size size);

      static coords_group_range get_all_coords_groups(size size);
      static coords_range get_coords_in_group(size size, coords_group group);
  };

  rectangle::coords_range
  rectangle::get_all_coords(size size) {
    std::vector<coords> result_vec;
    for (int row = 0; row < size.first; ++row) {
      for (int col = 0; col < size.second; ++col) {
        result_vec.push_back({row, col});
      }
    }
    return result_vec;
  }

  rectangle::coords_group_range
  rectangle::get_all_coords_groups(size size) {
    std::vector<coords_group> result_vec;
    for (int i = 0; i < size.first; ++i) {
      result_vec.push_back( {CMG_Rows, i, true } );
    }
    for (int i = 0; i < size.second; ++i) {
      result_vec.push_back( {CMG_Cols, i, true } );
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
        result_vec.push_back({index, col});
      }
    } else if (cmg == CMG_Cols) {
      for (int row = 0; row < size.first; ++row) {
        result_vec.push_back({row, index});
      }
    }
    return result_vec;
  }
}
