#pragma once
#include "../core/input_node.hpp"
#include "../core/puzzle_instance.hpp"
#include "../core/elements/topology/rectangle.hpp"
#include "../core/elements/value/integer.hpp"
#include "../core/puzzle.hpp"

#include "../core/constraint/extractor/value/slot_value_determined.hpp"
#include "../core/constraint/extractor/value/slot_value.hpp"
#include "../core/constraint/extractor/values_set/values_set.hpp"
#include "../core/constraint/constraint/value/values_equal.hpp"
#include "../core/constraint/constraint/values_sets/values_sets_equal.hpp"

namespace logicker::puzzle::simples {
}
  /*class ABC {
    Grid grid{ "square", "#Size" };
    ValueSet value_set{ "char", "A", "..", "#Max_char", "#Size" };

    ConditionType one_value_in_field(field){ "ValueInSet", "field[Value]", value_set };
    ConditionGroup{ field in "all_fields", one_value_in_field(field) };

    ConditionType given_in_field(field, value){ "ValuesEqual", "field[Value]", value };
    ConditionGroup{ [field, value] in "#Givens", given_in_field(field, value) };

    Value2ValueSetCollector region_set(region){ field in region, "field[Value]" };
    ConditionType set_of_values(region){ "ValueSetsEqual", region_set(region), value_set };
    ConditionGroup{ region in "rows", set_of_values(region) };
    ConditionGroup{ region in "cols", set_of_values(region) };

    ConditionType first_value(region, value){ "FirstValueEquals", region, value };
    ConditionGroup{ region in "
  };*/

//The very newest plan about conditions:
//
//Inicializace probehne ve trech krocich: interpretace souboru se zadanim (nikoli
//definici) ulohy, kompaktizace popisu podminek a samotne vytvoreni instanci
//run-time reprezentaci podminek, schopnych odvozovat dedukce.
//
//1) Reading input
//(Rozhodl jsem se, ze pujdu postupne, kde prvni krok bude, ze kazdy PuzzleType
//ma inicializacni fazi implementovanu primo v C++ jako metody tridy onomu
//PuzzleType odpovidajici. Vyhledove by bylo moc pekne mit inicializaci
//zmaknutou tak, ze bude mozne mit postup inicializace zapsany jako konfiguracni
//soubor v lidsky citelnem formatu.)
//
//Pri prvnim kroku, interpretaci zadani, vytvorim ??vector?? s prvky typu
//std::pair<??std::string??, std::set<grid::coords>>. Jeden prvek symbolicky
//popisuje nejakou podminku, ktere reseni musi splnovat. Je to kombinace
//descriptoru a seznamu policek, nad nimiz ma descriptorem popsana podminka
//platit. Seznam podporovanych descriptoru je poskytovan by condition_factory,
//ktera mi v druhem kroku vyrobi run-time instance. Je otazka, jestli tento
//prvni krok ma vubec descriptory nejak resit (existenci, whatever) nebo je
//ma pouze predavat kroku 3.
//
//2) Compacting descriptions
//V druhem kroku je treba "dat dohromady podminky nad stejnymi mnozinami
//policek". Resp. podobnymi mnozinami, hm. Neni to trivialni, jiste jsou
//dve veci (a jeste to, ze tyto dve veci nejsou vsechno, co je potreba
//implementovat):
//-- pokud dve podminky jsou obe nad toutez mnozinou policek, ma je
//condition_factory zpracovavat zaroven
//-- pokud jedna podminka je nad podmnozinou policek druhe podminky,
//ma condition_factory zpracovat jednak prvni podminku samotnou
//a dvak obe podminky najednou
//
//(Neni to vsechno, jezto mam silny pocit, ze se ukaze, ze nekdy je zadouci
//vytvorit kombinovanou podminku i v pripade, ze mnoziny policek nejsou
//usporadane inkluzi, ale maji pouze neprazdny prunik. Jako priklad
//mam predstavu nejakeho Sudoku se soucty v tom smyslu, ze v obrazci
//jsou zvyraznene oblasti, v nichz ma platit scitani pod sebe. Tam bych
//asi mel brat v uvahu mj. napr. kombinaci vsech radku, do kterych
//urcita souctova oblast zasahuje. ??Je to tak easy, jakoze kdykoli dve
//podminky jsou nad mnozinami s neprazdnym prunikem, je potreba brat 
//tyto dve i dohromady??)
//
//V tomto druhem kroku tedy prevadim ??vector?? z kroku 1 v ??mapu??
//std::map<std::set<grid::coords>, std::vector<??std::string??>>. Zatim
//tedy podle dvou vyse uvedenych "podmnozinovych kriterii".
//
//3) Creating instances
//Ve tretim kroku posilam jednotlive polozky z ??mapy?? vytvorene v druhem
//kroku to condition_factory, ta mi vrati vytvorene run-time instance
//podminek, ktere si ulozim jako soucasti zadani ulohy. Zminene zadani
//ulohy, tj. nova verze puzzle_instance, je ??kompletne tvoreno temito
//run-time instancemi podminek??. 
//
//4) Creating workers
//
