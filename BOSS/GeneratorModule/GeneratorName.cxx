#include "GeneratorModule/GeneratorName.h"
#include <iostream>

GeneratorName
generator_name (const int& index)
{
  switch (index) {
  case 1000000  : return PYTHIA;
  case 2000000  : return HERWIG;
  case 3000000  : return ISAJET;
  case 4000000  : return SINGLE;
  case 5000000  : return HIJING;
  case 6000000  : return PHOJET;
  case 100000   : return COMPHEP;
  case 200000   : return USER;
  case 300000   : return ACERMC;
  case 400000   : return ALPGEN;
  case 10000    : return TAUOLA;
  case 20000    : return PHOTOS;
  case 30000    : return TAUOLA_PHOTOS;
  }
  return UNKNOWN;
}

GeneratorName
first_generator (int& index)
{
  int gen1 = 1000000*(index/1000000);
  return generator_name(gen1);
}

GeneratorName
second_generator (int& index)
{
  int gen1 = 1000000*(index/1000000);
  int gen2 = index - gen1;
  gen2 = 100000*(gen2/100000);
  return generator_name(gen2);
}

GeneratorName
third_generator (int& index)
{
  int gen1 = 1000000*(index/1000000);
  int gen2 = index - gen1;
  gen2 = 100000*(gen2/100000);
  int gen3 = index - gen1 - gen2;
  gen3 = 10000*(gen3/10000);
  return generator_name(gen3);
}

int
generator_process (int& index)
{ return index%10000; }

void
GeneratorName_print(int& id)
{
  std::cout << first_generator(id) << " "
	    << second_generator(id) << " "
	    << third_generator(id) << " + "
	    << generator_process(id);
}


std::string
generator_string (const int& index)
{
  switch (index) {
  case 1000000  : return "PYTHIA";
  case 2000000  : return "HERWIG";
  case 3000000  : return "ISAJET";
  case 4000000  : return "SINGLE";
  case 5000000  : return "HIJING";
  case 6000000  : return "PHOJET";
  case 100000   : return "COMPHEP";
  case 200000   : return "USER";
  case 300000   : return "ACERMC";
  case 400000   : return "ALPGEN";
  case 10000    : return "TAUOLA";
  case 20000    : return "PHOTOS";
  case 30000    : return "TAUOLA_PHOTOS";
  }
  return " ";
}

std::ostream& operator<<(std::ostream& lhs, GeneratorName rhs) {
  lhs << generator_string(rhs);
  return lhs;
}
