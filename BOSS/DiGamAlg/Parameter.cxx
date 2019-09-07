#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include "DiGamAlg/Parameter.h"

void Parameter::parameters(double E_cms)
{
	std::string digamPath = getenv("DIGAMALGROOT");
	digamPath += "/DiGamAlg/parameters.txt";
	std::ifstream fin;
	fin.open(digamPath.c_str());

	int i = 0;
	std::string temp;						
	while (getline(fin, temp))
	{
		std::istringstream word(temp);
		word >> m_E_start >> m_E_stop >> m_CrossSection >> m_MCEff >> m_MCEffBoost >> m_boostMinEmin >> m_boostMinEmax;
		if(m_E_start <= E_cms && m_E_stop > E_cms)
			break;
		i++;
	}										
	fin.close();
}

