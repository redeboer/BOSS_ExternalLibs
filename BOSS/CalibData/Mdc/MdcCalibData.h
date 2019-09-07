#ifndef MDCCALIBDATA_H
#define MDCCALIBDATA_H

#include <map>
#include <vector>
#include "TTree.h" 
#include "TObjArray.h" 
#include "TBufferFile.h" 
#include "GaudiKernel/DataObject.h"
#include "CalibData/CalibModel.h"
#include "CalibData/CalibBase1.h"
typedef std::map<int, double>::value_type valType;
namespace CalibData {
  class MdcCalibData:  public CalibBase1{
public:
     MdcCalibData();
     ~MdcCalibData();
     //virtual ~MdcCalibData() {}
     // Re-implemented from DataObject
     inline virtual const CLID& clID() const { return classID(); } 
    
     inline static  const CLID& classID() { return CLID_Calib_MdcCal; }

     virtual StatusCode   update(CalibBase1& other, MsgStream* log);

     void setXtpar(int xtkey,double val);//huangb add 
     void setNewXtpar(TObjArray* newXtTrees);//yzhang add 2011-12-12 
     void setR2tpar(TObjArray* r2tTrees);//yzhang add 2011-12-15 
     void setT0(double val);
     void setDelT0(double val);
     void setQtpar0(double val);
     void setQtpar1(double val);
     // parameters of sigma vs. distance
     void setSdpar(int sdkey,double val);     //huangb add 

     double getXtpar(int layid, int entr, int lr, int order); /* wulh updated on 08-3-24 */
     TTree* getNewXtpar(int layid, int entr, int lr); /* yzhang updated on 2011-12-12 */
     TTree* getR2tpar(int layid); /* yzhang updated on 2011-12-15 */
     double getT0(int wireid) { return m_t0[wireid]; }
     double getDelT0(int wireid) { return m_delt0[wireid]; }
     double getQtpar0(int layid) { return m_qtpar0[layid]; }
     double getQtpar1(int layid) { return m_qtpar1[layid]; }
     double getSdpar(int layid, int entr, int lr, int bin); /* wulh updated on 08-3-24 */
     void clear();

     int getXtKey(int layid, int entr, int lr, int order); /* wulh updated on 08-3-24 */
     void setXtBegin();
     int getNextXtpar(int& key, double& xtpar);

     int getSdKey(int layid, int entr, int lr, int bin); /* wulh updated on 08-3-24 */
     void setSdBegin();
     int getNextSdpar(int& key, double& sdpar);
        
    
     //acquire the size of the vectors and maps 
     //huangb add
     int getqtparNo() {return m_qtpar0.size();}
     int getxtNo() { return m_xtmap.size(); }
     int gett0No() { return m_t0.size(); }
   
private:
     std::map<int, double> m_xtmap;

     TBufferFile* newXtTreesBuf;
     TBufferFile* r2tTreesBuf;
     TObjArray* newXtTreesArray;
     TObjArray* r2tTreesArray;
     TTree* m_newxt[43][18][2];
     TTree* m_r2t[43];
     //TBufferFile* newXtTreesBuf;
     //TBufferFile* r2tTreesBuf;
     //TObjArray* newXtTreesArray;
     //TObjArray* r2tTreesArray;
     //TTree* m_newxt[43][18][2];
     //TTree* m_r2t[43];

     std::vector<double> m_t0;
     std::vector<double> m_delt0;
     std::vector<double> m_qtpar0;
     std::vector<double> m_qtpar1;
     std::map<int, double> m_sdmap;
   
     std::map<int, double>::iterator m_xtiter;
     std::map<int, double>::iterator m_sditer;
     static const int XTLAYER_INDEX    = 11;
     static const int XTLAYER_MASK     = 0x1F800;

     static const int XTENTRA_INDEX    = 6;
     static const int XTENTRA_MASK     = 0x7C0;

     static const int XTLR_INDEX       = 4;
     static const int XTLR_MASK        = 0x30;

     static const int XTORDER_INDEX    = 0;
     static const int XTORDER_MASK     = 0xF;


     static const int SDLAYER_INDEX    = 10;
     static const int SDLAYER_MASK     = 0xFC00;

     static const int SDENTRA_INDEX    = 7;
     static const int SDENTRA_MASK     = 0x380;

     static const int SDLR_INDEX       = 5;
     static const int SDLR_MASK        = 0x60;

     static const int SDBIN_INDEX      = 0;
     static const int SDBIN_MASK       = 0x1F;
};
}
#endif /* MDCCALIBCONST_H */
