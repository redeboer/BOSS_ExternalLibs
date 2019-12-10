//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtDecayBase.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTDECAYBASE_HH
#define EVTDECAYBASE_HH

#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtId.hh"
#include <string>
#include "EvtGenBase/EvtSpinType.hh"
#include <stdlib.h>
#include <vector>
class EvtParticle;
class EvtSpinType;

class EvtDecayBase{
 public:

  //These pure virtual methods has to be implemented
  //by any derived class
  virtual void getName(std::string& name)=0;
  virtual void decay(EvtParticle *p)=0;
  virtual void makeDecay(EvtParticle *p)=0;
  virtual EvtDecayBase* clone()=0;


  //These virtual methods can be implemented by the 
  //derived class to implement nontrivial functionality.
  virtual void init();
  virtual void initProbMax();
  virtual std::string commandName();
  virtual void command(std::string cmd);


  double getProbMax( double prob );
  double resetProbMax( double prob );

  EvtDecayBase();
  virtual ~EvtDecayBase();

  virtual bool matchingDecay(const EvtDecayBase &other) const;

  EvtId getParentId() {return _parent;}
  double getBranchingFraction() {return _brfr;}
  void disableCheckQ() {_chkCharge=0;};
  void checkQ();
  int getNDaug() {return _ndaug;}
  EvtId* getDaugs() {return _daug;}
  EvtId getDaug(int i) {return _daug[i];}
  int getNArg() {return _narg;}
  int getPHOTOS() {return _photos;}
  void setPHOTOS() {_photos=1;}
  void setVerbose() {_verbose=1;}
  void setSummary() {_summary=1;}
  double* getArgs();
  std::string* getArgsStr() {return _args;}
  double getArg(int j); 
  std::string getArgStr(int j) {return _args[j];}
  std::string   getModelName() {return _modelname; }
  int getDSum() {return _dsum; }
  int summary() {return _summary; }
  int verbose() {return _verbose; }

  void saveDecayInfo(EvtId ipar, int ndaug,EvtId *daug,
		     int narg, std::vector<std::string>& args, 
		     std::string name, double brfr);
  void printSummary();

  
  //Does not really belong here but I don't have a better place.
  static void findMasses(EvtParticle *p, int ndaugs, 
                            EvtId daugs[10], double masses[10]);
  static void findMass(EvtParticle *p);
  static double findMaxMass(EvtParticle *p);

  //Methods to set the maximum probability.
  void setProbMax(double prbmx);
  void noProbMax();

  void checkNArg(int a1, int a2=-1, int a3=-1, int a4=-1);
  void checkNDaug(int d1, int d2=-1);

  void checkSpinParent(EvtSpinType::spintype sp);
  void checkSpinDaughter(int d1, EvtSpinType::spintype sp);

  // lange - some models can take more daughters
  // than they really have to fool aliases (VSSBMIX for example)
  virtual int nRealDaughters() { return _ndaug;}

protected:

  bool _daugsDecayedByParentModel;
  bool daugsDecayedByParentModel() {return _daugsDecayedByParentModel;}

private:


  int _photos;
  int _ndaug;
  EvtId _parent;
  int _narg;
  EvtId *_daug;
  double *_argsD;
  std::string *_args;
  std::string _modelname;
  double _brfr;
  int _dsum;
  int _summary;
  int _verbose;


  int defaultprobmax;
  double probmax;
  int ntimes_prob;

  //Should charge conservation be checked when model is 
  //created? 1=yes 0 no.
  int _chkCharge;


  //These are used for gathering statistics.
  double sum_prob;
  double max_prob;

};

#endif

