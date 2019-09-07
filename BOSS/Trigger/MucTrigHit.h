#ifndef MucTrigHit_h
#define MucTrigHit_h 1
 
#include "MucRawEvent/MucDigi.h"
#include "Trigger/TrigPara.h"

using namespace TrigConf;
 
class MucTrigHit{
public:
        MucTrigHit();
        ~MucTrigHit();
        void getMucDigi(MucDigiCol* mucDigiCol);
        void getFastOr();

        inline int getfoBR(int i, int j, int k) { return foBR[i][j][k]; }
        inline int getfoEC(int i, int j, int k, int l) { return foEC[i][j][k][l]; }

        static MucTrigHit* get_Muc(void);
 
private:
        static MucTrigHit* muc_Pointer;
      
        int hitBR[8][4][112]; //barrel seg, gap, strip
        int hitEC[2][4][4][64]; //endcap part, seg, gap, strip
        
        int foBR[8][4][7]; //barrel fastOr signal, seg, gap, fastOr
        int foEC[2][4][4][4]; //endcaps fastOr signal, part, seg, gap, fastOr
};
#endif
