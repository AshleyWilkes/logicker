#pragma once
#include <memory>

namespace logicker::core::constraint::extractor {
  template<typename ExtractedValueType, typename Grid>
  class value_extractor {
    public:
      virtual ExtractedValueType extract( const Grid& grid, bool throw_on_undecided = true ) const = 0;
      virtual std::string to_string() const = 0;
      virtual ~value_extractor() = default;
  };

  //well, tenhle interface tak, jak je, nedava vubec zadny smysl
  //AFAIR rozdeleni na set/get_source na jedne a extract na druhe
  //strane vznikla proto, aby zde nemusely byt oba dva templatove
  //parametry; to bylo v dobe, kdy jsem si neuvedomoval, ze i constraint
  //pro sve is_satisfied_by musi znat typ sveho Gridu
  //
  //ted uprimne nevim, jestli chci tento interface rozdelit na dva
  //anebo zrusit set/get source a parametr grid pridat do metody
  //extract. Chci dodelat sprint, kterej uz trva pres 2 mesice,
  //cili nechavam tak pro doreseni v jinym sprintu
  //
  //proto zakomentovavam a nahrazuju interacem druheho typu,
  //nicmene ta oddelenost je zajimava vec, kterou by stalo za
  //to zaridit, pokud to jde, jakoze snad jde!
  //template<typename ExtractedValueType, typename Grid>
  //class value_extractor {
    //public:
      //void set_source( Grid grid );
      //Grid get_source();
      //virtual ExtractedValueType extract( bool throw_on_undecided = true ) const = 0;
    //private:
      //std::unique_ptr<Grid> source_ptr_;
  //};
}
