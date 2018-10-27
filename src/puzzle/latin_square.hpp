#pragma once
#include "../core/condition_instance.hpp"
#include "../core/elements/field_type/integer_field_type.hpp"
#include "../core/field.hpp"
#include "../core/grid.hpp"
#include "../core/elements/topology/square.hpp"

#include <algorithm>
#include <vector>

namespace logicker::puzzle {
  using condition_description = std::pair<std::vector<core::CoordsMetaGroup>, core::condition_v1::ConditionDescription>;
  class latin_square {
    public:
      using value_type = int;
      using field_type = core::field_type<value_type>;
      using topology = core::square;
      using condition = core::condition_v1::each_value_once_in_rows_and_cols;
      /*using latin_square_condition = 
        condition_group<
          condition::each_value_once, 
          rectangle::rows_and_cols>;
      using each_value_once = 
        condition::sets_equal<
          
      using conditions =  */

      //tady teda ma byt using conditions = ASDF,
      //kde ASDF je typ, ktery obsahuje functor classu s
      //bool operator()(grid)
      //kde prosimTe vysledek false znamena reseni je ok,
      //true znamena reseni neni ok
      //

      static const std::vector<condition_description>& get_condition_descriptions();
  };

  const std::vector<condition_description>&
  latin_square::get_condition_descriptions() {
    static std::vector<condition_description> cond_descs_{
      { { "Rows", "Cols" }, "EachValueOnce" }
    };
    return cond_descs_;
  }
}

//definice puzzle_typu parametrizuje tovarny (v checker a solver), z kterezto 
//parametrizace ty tovarny poznaji, jaky ocekavaji format vstupu (tedy
//_minimalni_ format, momentalne nevidim, jak by parametrizace mohla 
//zabranit existenci nepotrebne a nepouzite extra informace ve vstupu)
//
//takovahle tovarna tedy musi mit takovou implementaci, aby umoznovala volani
//s ruznymi typy a pocty parametru; zaroven nechci vzdavat typovou safety,
//takze plan je brat token nejakeho stromoveho typu s multiple typy leafu.
//Udelam to zalozene na jsoncpp, ale obalim to svym typem, at se ten json
//da snadno vymenit, kdyby to bylo treba.
//
//co muze delat takova tovarna (ruzne tovarny (checker, solver, generator)
//  delaji ruzne veci):
//1) vytvorit instanci field_typu, ktera se pak pouzije pro inicializaci kazdeho policka;
//    tj. urci hodnoty parametru konstruktoru teto instance field_typu
//2) vytvorit instanci gridu, ktery bude predstavovat assignment_grid;
//    tj. urci topology::size, jez bude parametrem konstruktoru teto instance
//3) vytvorit instanci podminky, ktera bude predstavovat, umm, podminky
//4) nasetovat zadanym fieldum zadane hodnoty
//    tj. rozparsovat [coords]:value a zavolat set_value
//
//Vsechny body 1)-3) se muzou vyskytovat v multiple verzich a 4) uz je vlastne
//multiple takto (krom toho kdovi, jestli ty givensy nakonec nebude treba
//taky nejak zobecnit, coz by umoznilo odlisne formaty a zpracovani vstupu).
//
//Novy plan je implementovat !!vsechny!! tyto vlastnosti jako typedefy v PuzzleInstance;
//to by melo jit, multiplicnost by melo jit resit pres templaty s variabilnim
//poctem parametru. Domnivam se, ze se tim vyhnu tune straslivackeho kodu typu
//  if (xxx_desc == "yyy") {
//    xxx = new yyy();
//  }
//a ze ziskam naopak pekny a dobre strukturovany kod v templatach.
