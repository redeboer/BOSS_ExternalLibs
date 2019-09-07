#ifndef GENERATORMODULE_GENERATORNAME_H
#define GENERATORMODULE_GENERATORNAME_H

#include <iosfwd>
#include <string>

enum  GeneratorName
{
  PYTHIA          = 1000000,
  HERWIG          = 2000000,
  ISAJET          = 3000000,
  SINGLE          = 4000000,
  HIJING          = 5000000,
  PHOJET          = 6000000,
  COMPHEP         = 100000,
  USER            = 200000,
  ACERMC          = 300000,
  ALPGEN          = 400000,
  TAUOLA          = 10000,
  PHOTOS          = 20000,
  TAUOLA_PHOTOS   = 30000,
  UNKNOWN         = -999
};

GeneratorName  generator_name        (const int& index);
GeneratorName  first_generator       (int& index);
GeneratorName  second_generator      (int& index);
GeneratorName  third_generator       (int& index);
int            generator_process     (int& index);

void           GeneratorName_print   (int& id);

std::string    generator_string      (const int& index);

std::ostream   &operator<<           (std::ostream& lhs, GeneratorName rhs);

#endif // GENERATORMODULE_GENERATORNAME_H
