#pragma once
#include "../core/condition_instance.hpp"
#include "../core/integer_field.hpp"
#include "../core/grid.hpp"
#include "../core/rectangle.hpp"

#include <algorithm>
#include <vector>

namespace logicker::puzzle {
  using condition_description = std::pair<std::vector<core::CoordsMetaGroup>, core::ConditionDescription>;
  template<int size>
  class latin_square {
    public:
      typedef core::integer_field field_type;
      typedef core::rectangle topology;

      bool is_solved_by(const core::grid<field_type, topology>& grid) const;
      topology::size grid_size() const;
      std::vector<std::unique_ptr<core::condition_instance<field_type, topology>>>& get_condition_instances() const;
    private:
      topology::size grid_size_{ size, size };
      //nemam tady mit primo instance condition, tady mam mit jenom nejaky predpis,
      //z nehoz nekdo jiny umi vygenerovat potrebne instancializace
      //tj. ma tu byt napsano neco jako
      //(radky+sloupce) X (kazda hodnota prave jednou)
      //[ROWS+COLS, EACH_VALUE_ONCE]
      //prvni hodnota je id CoordsGroupy nebo CoordsMetaGroupy
      //druha hodnota je id ConditionTypu
      //vyhodnocovani:
      //z prvni hodnoty dam dohromady std::vector<std::vector<Coords>>
      //kde vnejsi vector je proste kontejner, jehoz hodnoty jsou
      //soubory policek; tyto soubory muzou mit ruzne topologie, takze
      //vnitrni vector mozna nema byt std::vector, ale pro ted to staci
      mutable std::vector<std::unique_ptr<core::condition_instance<field_type, topology>>> grid_conds_;
      std::vector<condition_description> cond_descs_{ { { "Rows", "Cols" }, "EachValueOnce" } };

      void process_condition_description(condition_description cond_desc) const;
  };

  template<int size>
  bool
  latin_square<size>::is_solved_by(const core::grid<field_type, topology>& grid) const {
    if (grid_conds_.empty()) {
      this->get_condition_instances();
    }
    if (grid_conds_.empty()) {
      throw "latin_square::get_condition_instances returned nothing\n";
    }
    return std::all_of(grid_conds_.begin(), grid_conds_.end(),
        [grid](auto& cond){ return cond->is_satisfied_by( grid ); });
  }

  template<int size>
  typename latin_square<size>::topology::size
  latin_square<size>::grid_size() const {
    return grid_size_;
  }

  template<int size>
  std::vector<std::unique_ptr<
      core::condition_instance<
      typename latin_square<size>::field_type,
      typename latin_square<size>::topology>>>&
  latin_square<size>::get_condition_instances() const {
    if (grid_conds_.empty()) {
      for (auto cond_desc : cond_descs_) {
        this->process_condition_description(cond_desc);
      }
    }
    return grid_conds_;
  }

  template<int size>
  void
  latin_square<size>::process_condition_description(condition_description cond_desc) const {
    //z CoordsMetaGroupy udelat vector vectoru coordsu
    for (auto group : topology::get_all_coords_groups(grid_size_, cond_desc.first)) {
      //z ConditionDescriptiony a vectoru coordsu delat condition_instance a pushovat je do _grid_conds_
      auto coords_vec = topology::get_coords_in_group(grid_size_, group);
      grid_conds_.push_back(core::condition_instance<field_type, topology>::create_instance(cond_desc.second, coords_vec));
    }
  }
}
