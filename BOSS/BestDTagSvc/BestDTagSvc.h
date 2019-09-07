#ifndef BEST_DTAG_SVC_H
#define BEST_DTAG_SVC_H

#include "GaudiKernel/Service.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "VertexFit/IVertexDbSvc.h"
#include "BestDTagSvc/IBestDTagSvc.h"

class IDataProviderSvc;
template <class TYPE> class CnvFactory;

class BestDTagSvc : public Service, virtual public IBestDTagSvc
{
   friend class CnvFactory<BestDTagSvc>;

   public :

      BestDTagSvc(const std::string& name, ISvcLocator* svcLoc);
      virtual ~BestDTagSvc();

      virtual StatusCode initialize();
      virtual StatusCode finalize();
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

      EvtRecDTag*  getSingleTag(int modeid, int charm = 0);
      EvtRecDTag** getDoubleTag(int modeid1, int modeid2, int charm = 0);

      bool        isCosmicOrLepton();


   private :
	  bool m_isPidTight;
      bool isGoodTrack(EvtRecTrack* atrk);
      bool isElectron(EvtRecTrack* atrk);
      bool isMuon(EvtRecTrack* atrk);


   private :

      IDataProviderSvc*  eventSvc_;
      IVertexDbSvc*      vtxSvc_;
};

#endif
