#include <math.h>
#include "GaudiKernel/Algorithm.h"
#include "TruthHelper/GenAccessIO.h"

class IHistogram1D;                    //Forward declaration
class IHistogram2D;                    //Forward declaration
class StoreGateSvc;

class TruthDemo:public Algorithm {
public:
  TruthDemo(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  bool        m_produceHistogram;

//histograms, used if m_produceHistogram is true = 1
  IHistogram1D*   m_hgenerated;
  IHistogram1D*   m_pxBalance;
  IHistogram1D*   m_pyBalance;
  IHistogram1D*   m_totEnergy;
  IHistogram1D*   m_negStatus;
  //   StoreGateSvc* m_sgSvc;
  GenAccessIO*        m_tesIO;

};
