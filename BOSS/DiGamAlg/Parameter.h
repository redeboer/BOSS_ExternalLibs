#ifndef PARAMETER_H
#define PARAMETER_H

class Parameter
{
public:
	double m_E_start;
	double m_E_stop;

	double m_CrossSection;
	double m_MCEff;
	double m_MCEffBoost;
	double m_boostMinEmin;
	double m_boostMinEmax;
	
	void parameters(double E_cms);
};

#endif
