#ifndef McTestAlg_h
#define McTestAlg_h 1


#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class IDataProviderSvc;

class McTestAlg:public Algorithm 
{
  public:
    
    McTestAlg (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

    void RetrieveMcParticle();
    void RetrieveMdc();
    void MdcInit();
    void RetrieveTof();
    void TofInit();
    void RetrieveEmc();
    void RetrieveMuc();
    
  private:
    IDataProviderSvc* m_evtSvc;

    bool m_particleRootFlag;
    NTuple::Tuple* tupleParticle;
    NTuple::Item<double> me;

    bool m_mdcRootFlag;
    NTuple::Tuple* tupleMdc1;  // MdcTruth
    NTuple::Tuple* tupleMdc2;  // MdcDigit
 
    NTuple::Item<long> truthMdcIndex;
    NTuple::Item<long> truthMdcParId;
    NTuple::Item<long> truthMdcLayer;
    NTuple::Item<long> truthMdcWire;
    NTuple::Item<double> truthMdcEdep;
    NTuple::Item<double> truthMdcDriftD;
    NTuple::Item<double> truthMdcX;
    NTuple::Item<double> truthMdcY;
    NTuple::Item<double> truthMdcZ;
    NTuple::Item<double> ntruthMdc;
    NTuple::Item<double> m_layer;
    NTuple::Item<double> m_cell;
    NTuple::Item<double> m_charge;
    NTuple::Item<double> m_time;

    bool m_tofRootFlag;
    NTuple::Tuple* tupleTof;
    NTuple::Item<double> truthIndex;
    NTuple::Item<double> truthPartId;
    NTuple::Item<double> truthLayer;
    NTuple::Item<double> truthScinNb;
    NTuple::Item<double> truthX;
    NTuple::Item<double> truthY;
    NTuple::Item<double> truthZ;
    NTuple::Item<double> ntruth;
    NTuple::Item<double> tleft;
    NTuple::Item<double> tright;
    NTuple::Item<double> qleft;
    NTuple::Item<double> qright;

};

#endif
