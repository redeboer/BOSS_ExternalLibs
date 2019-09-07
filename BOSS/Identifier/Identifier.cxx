#include "Identifier/Identifier.h"
#include <stdarg.h>
#include <stdio.h>
#include <algorithm>

#include <iostream>
#include <iomanip>

//-----------------------------------------------
void Identifier::set (const std::string& id)
{
  sscanf (id.c_str(), "0x%x", &m_id);
}


//-----------------------------------------------
std::string Identifier::getString() const
{
   std::string result;
   char temp[20];
   
   sprintf (temp, "0x%x", (unsigned int)m_id);
   result += temp;
   result.insert(2,10-result.length(),'0');
   
   return (result);
}

//-----------------------------------------------
void Identifier::show () const
{
    const Identifier& me = *this;
    std::cout << me.getString();
}

std::ostream& operator<<(std::ostream & os, const Identifier& Id)
{
   return (os<<Id.getString());
}
