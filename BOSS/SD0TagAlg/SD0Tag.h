#ifndef Physics_Analysis_SD0Tag_H
#define Physics_Analysis_SD0Tag_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
 
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "McTruth/McParticle.h"
typedef std::vector<int> Vint;

class SD0Tag : public Algorithm {

public:
  SD0Tag(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  


private:

  int m_MC_sample;
  int PID_flag;

  int Seperate_Charge; int Charge_default;

  int p_run[3467]; double p_Ecm[3467];

  NTuple::Tuple*  m_tuple1;
  NTuple::Item<int>      m_tagmode;
  NTuple::Item<double>   m_mass_bc;
  NTuple::Item<double>   m_delE_tag;
  NTuple::Item<double>   m_EGam_max_0;
  NTuple::Item<int>      m_nGood;
  NTuple::Item<int>      m_nGam;
  NTuple::Item<int>      m_runNo;
  NTuple::Item<int>      m_event;
  NTuple::Item<int>      m_cosmic_ok;

};

#endif 
