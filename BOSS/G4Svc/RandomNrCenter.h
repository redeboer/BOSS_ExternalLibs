#ifndef RandomNrCenter_H
#define RandomNrCenter_H

#include <CLHEP/Random/Random.h>
#include <string>
using namespace CLHEP;

class RandomNrCenter {
private:
	static HepRandomEngine *theDefaultEngine;
	static HepRandomEngine *theCurrentEngine;
public:
	void SaveEngineStatus(std::string);
	void RestoreEngineStatus(std::string);
	void SetEngineSeed(long int);
	long int GetEngineSeed();
	void SetEngine();
	void ResetEngine();
	HepRandomEngine *GetEngine();
	
	static double Rngaus(double mean, double sigma);
};

#endif
