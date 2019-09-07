#ifndef BesMucTrig_h
#define BesMucTrig_h 1

#include <vector>

class MucTrigHit;
class IBesGlobalTrigSvc;
class BesGlobalTrigSvc;

class BesMucTrig{
public:
       BesMucTrig();
       ~BesMucTrig();

       void startMucTrig();

       //get the fired layer number
       std::vector<int> getNlayerSeg();

       //get the total fired layer number in barrel and endcaps
       int getNlayerPart(int part); 
 
       //get the hits in each layer
       std::vector<int> getNhitLayer();

       //get the hits in each segment
       std::vector<int> getNhitSeg();

       //get the hits in each part
       int getNhitPart(int i);

       //get the hits including barrel and endcaps
       int getNhitTotal();

       void setIndex();

       void readIndex();

       void findTrack();

private:
       MucTrigHit* m_MucTrigHit;
 
       int nfireLayerBR[8][4], nhitLayerBR[8][4], nhitSegBR[8], nhitBR;
       int nfireLayerEC[2][4][4], nhitLayerEC[2][4][4], nhitSegEC[2][4], nhitEC[2];

       std::vector<int> vfireLayer;
       std::vector<int> vhitLayer;
       std::vector<int> vhitSeg;

       std::vector<int> vIndex; //container of fired strip number
      
       int foBR[8][4][7]; //barrel fastOr signal, seg, gap, fastOr
       int foEC[2][4][4][4]; //endcaps fastOr signal, part, seg, gap, fastOr

       int trackBR[8]; //track in each segment in barrel
       int trackEE[4]; //track in each segment in east endcap
       int trackWE[4]; //track in each segment in west endcap

       int track3lBR[8];
       int track3lEE[4];
       int track3lWE[4];

       int btrack, etrack;

       std::vector<std::vector<int> > vindexb;
       std::vector<std::vector<int> > vindexb3;
       std::vector<std::vector<int> > vindexe;

       BesGlobalTrigSvc* m_pIBGT;
       IBesGlobalTrigSvc* m_tmpSvc;
};
#endif 
