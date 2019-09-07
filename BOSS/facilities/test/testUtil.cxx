/// @file testUtil.cxx
#include <string>
#include <iostream>
#include "facilities/Util.h"
/** Miniscule program to test a couple modes of Util::expandEnvVar
 *  Caller should have an environment variable SRC with some
 *  sensible definition.
 */
int main(int, char**) {
  std::string name = std::string("${GLAST_EXT}/xerces");
  std::string nameAgain = std::string("${GLAST_EXT}/xerces");
  std::string oDelim = std::string ("${");
  std::string cDelim = std::string ("}");
  int ntrans;

  std::cout << "Test expandEnvVar: " << std::endl;
  try {
    ntrans = facilities::Util::expandEnvVar(&name, oDelim, cDelim);
    std::cout << "Translated name (right delimiters) is " 
              << name << std::endl;  
    ntrans = facilities::Util::expandEnvVar(&nameAgain);
    std::cout << "Translated name (wrong delimiters) is " 
              << nameAgain << std::endl;  
  }
  catch (facilities::Untranslatable err) {
    std::cout << "Failed to completely translate " << name << std::endl;
  }



  // Process running this must have environment variable SRC
  std::string multi = std::string("$(FACILITIESROOT)/$(SRC)");

  try {
    ntrans = facilities::Util::expandEnvVar(&multi);

    std::cout << "Translated name is " << multi << std::endl;
    std::cout << ntrans << " variables were translated." << std::endl;
  }
  catch (facilities::Untranslatable err) {
    std::cout << "Failed to completely translate " << multi << std::endl;
  }

  std::cout << std::endl << "Test itoa " << std::endl;
  // Test the new itoa routine
  std::string intStr;
  facilities::Util::itoa(12, intStr);
  std::cout << "My String is " << intStr << std::endl;

  // basename & stringTokenize
  std::string unixname("/a/path/myUnixFile.txt");
  std::string wname("C:\\a\\longer\\path\\myWindowsFile.txt");

  std::vector<std::string> tokens;
  unsigned int i;

  std::cout << std::endl << "Test stringTokenize and basename" << std::endl;
  facilities::Util::stringTokenize(unixname, "/", tokens);

  std::cout << "Processing string " << unixname << std::endl;
  for (i = 0; i < tokens.size(); i++) {
    std::cout << "Token " << i << " is: " << tokens[i] << std::endl;
  }

  std::cout << "basename is " << facilities::Util::basename(unixname) << std::endl;

  facilities::Util::stringTokenize(wname, "\\", tokens);

  std::cout << "Processing string " << wname << std::endl;
  for (i = 0; i < tokens.size(); i++) {
    std::cout << "Token " << i << " is: " << tokens[i] << std::endl;
  }

  std::cout << "basename is " << facilities::Util::basename(wname) << std::endl;

  //test keyValueTokenize to map routine
  std::cout << std::endl << "Test keyValueTokenize " << std::endl;
  std::string input1("apple=green,lemon=yellow,blueberry=blue");
  std::cout << "Input string: '" << input1 << "'" << std::endl;
  std::map<std::string,std::string> maptokens;
  facilities::Util::keyValueTokenize(input1,",",maptokens,"=", false);
  std::map<std::string,std::string>::const_iterator tokens_itr=maptokens.begin();
  while (tokens_itr!=maptokens.end()) {
    std::cout << "Token key " <<(*tokens_itr).first  << " and value: " 
	      << (*tokens_itr).second << std::endl;
    tokens_itr++;
  }

  std::cout <<"appending to previous map:"<<std::endl;
  std::string input2("apple2/green2,lemon2/yellow2,blueberry2/blue2");
  std::cout << "New string is '" << input2 << "'" << std::endl;
  facilities::Util::keyValueTokenize(input2,",",maptokens,"/",false);
  tokens_itr=maptokens.begin();
  while (tokens_itr!=maptokens.end()) {
    std::cout << "Token key " <<(*tokens_itr).first  << " and value: " 
	      << (*tokens_itr).second << std::endl;
    tokens_itr++;
  }
  
  std::cout <<"clearing the map first:"<<std::endl;
  facilities::Util::keyValueTokenize(input2,",",maptokens,"/",true);
  tokens_itr=maptokens.begin();
  while (tokens_itr!=maptokens.end()) {
    std::cout << "Token key " <<(*tokens_itr).first  << " and value: " 
	      << (*tokens_itr).second << std::endl;
    tokens_itr++;
  }

  std::cout << "Use a multi-character pairDelimiter argument " << std::endl;
  std::string input3("apple2:=green2 lemon2:=yellow2 blueberry2:=blue2");
  std::cout << "input is: '" << input3 << "'" << std::endl;
  facilities::Util::keyValueTokenize(input3," ",maptokens,":=");
  tokens_itr=maptokens.begin();
  while (tokens_itr!=maptokens.end()) {
    std::cout << "Token key " <<(*tokens_itr).first  << " and value: " 
	      << (*tokens_itr).second << std::endl;
    tokens_itr++;
  }


  // Test stringToDouble routine
  std::cout << std::endl << "Test stringToDouble " << std::endl;
  std::string okDouble("3.14159");
  std::string badDouble("3.garbage56");

  double result = -1;

  try {
    result = facilities::Util::stringToDouble(okDouble);
    std::cout << "Converted (string) " << okDouble << " to (double) " 
              << result << std::endl;
  }
  catch (facilities::WrongType ex) {
    std::cout << "Failed with exception   " << ex.getMsg() 
              << std::endl;
  }

  try {
    result = facilities::Util::stringToDouble(badDouble);
    std::cout << "Converted (string) " << badDouble << " to (double) " 
              << result << std::endl;
  }
  catch (facilities::WrongType ex) {
    std::cout << "Failed with exception   " << ex.getMsg() 
              << std::endl;
  }

  // Test stringToInt routine
  std::cout << std::endl << "Test stringToInt " << std::endl;

  std::string okInt("33550");
  std::string badInt1("3garbage56");
  std::string badInt2("garbage356");

  int intResult = -1;

  try {
    intResult = facilities::Util::stringToInt(okInt);
    std::cout << "Converted (string) " << okInt << " to (int) " 
              << intResult << std::endl;
  }
  catch (facilities::WrongType ex) {
    std::cout << "Failed with exception   " << ex.getMsg() 
              << std::endl;
  }

  try {
    intResult = facilities::Util::stringToInt(badInt1);
    std::cout << "Converted (string) " << badInt1 << " to (int) " 
              << intResult << std::endl;
  }
  catch (facilities::WrongType ex) {
    std::cout << "Failed with exception  " << ex.getMsg() 
              << std::endl;
  }

  try {
    intResult = facilities::Util::stringToInt(badInt2);
    std::cout << "Converted (string) " << badInt2 << " to (int) " 
              << intResult << std::endl;
  }
  catch (facilities::WrongType ex) {
    std::cout << "Failed with exception  " << ex.getMsg() 
              << std::endl;
  }

  // Try out trimTrailing method
  std::cout << std::endl << "Test trimTrailing " << std::endl;
  std::string string1("ends with 2 blanks  ");
  std::string string2("ends with newline\n");
  std::string string3("no trailing whitespace");

  unsigned nTrimmed = facilities::Util::trimTrailing(&string1);
  std::cout << "Trimmed " << nTrimmed << " from string1; has new value : "
            << string1 << "*EOS*" << std::endl;

  nTrimmed = facilities::Util::trimTrailing(&string2);
  std::cout << "Trimmed " << nTrimmed << " from string2; has new value : "
            << string2 << "*EOS" <<  std::endl;

  nTrimmed = facilities::Util::trimTrailing(&string3);
  std::cout << "Trimmed " << nTrimmed << " from string3; has new value : "
            << string3 << "*EOS" << std::endl;



  return 0;
}

  
