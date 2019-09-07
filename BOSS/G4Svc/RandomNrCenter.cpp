#include "G4Svc/RandomNrCenter.h"
#include <CLHEP/Random/Random.h>

HepRandomEngine* RandomNrCenter::theDefaultEngine=HepRandom::getTheEngine();
HepRandomEngine* RandomNrCenter::theCurrentEngine=HepRandom::getTheEngine();

void RandomNrCenter::SaveEngineStatus(std::string s)
{
	HepRandom::saveEngineStatus(s.c_str());
}
void RandomNrCenter::RestoreEngineStatus(std::string s)
{
	HepRandom::restoreEngineStatus(s.c_str());
}
void RandomNrCenter::SetEngineSeed(long int i)
{
	HepRandom::setTheSeed(i,3);
}
long int RandomNrCenter::GetEngineSeed()
{
	return HepRandom::getTheSeed();
}
void RandomNrCenter::SetEngine()
{
}

void RandomNrCenter::ResetEngine()
{
	theCurrentEngine=theDefaultEngine;
}

HepRandomEngine* RandomNrCenter::GetEngine()
{
	return theCurrentEngine;
}

#include <CLHEP/Random/RandGauss.h>
double RandomNrCenter::Rngaus(double mean, double sigma)
{
	return RandGauss::shoot(mean,sigma);
}
