#ifndef MDCCALIBCONST_H
#define MDCCALIBCONST_H

#include <map>
#include <vector>

class MdcCalibConst {

public:
     MdcCalibConst();
     MdcCalibConst(const MdcCalibConst& calconst);

     void initCalibConst();

     void   fillXtpar(int key, double val);
     void   resetXtpar(int lay, int entr, int lr, int order, double val);
     int    getXtSize() const { return m_xtmap.size(); }
     void   setXtBegin();
     int    getNextXtpar(int& key, double& xtpar);
     double getXtpar(int lay, int entr, int lr, int order);
     int  getXtKey(int lay, int entr, int lr, int order) const;

     void   fillT0(double val)    { m_t0.push_back( val ); }
     void   fillDelT0(double val) { m_delt0.push_back( val ); }
     void   resetT0(int wireid, double val)    { m_t0[wireid] = val; }
     void   resetDelT0(int wireid, double val) { m_delt0[wireid] = val; }
     double getT0(int wireid)    const { return m_t0[wireid]; }
     double getDelT0(int wireid) const { return m_delt0[wireid]; }
     int    getT0Size() const { return m_t0.size(); }

     void   fillQtpar(int order, double val);
     void   fillQtpar0(double val) { m_qtpar0.push_back( val ); }
     void   fillQtpar1(double val) { m_qtpar1.push_back( val ); }
     void   resetQtpar(int lay, int order, double val);
     void   resetQtpar0(int lay, double val) { m_qtpar0[lay] = val; }
     void   resetQtpar1(int lay, double val) { m_qtpar1[lay] = val; }
     double getQtpar(int lay, int order) const;
     double getQtpar0(int lay) const { return m_qtpar0[lay]; }
     double getQtpar1(int lay) const { return m_qtpar1[lay]; }
     int    getQtSize() const { return m_qtpar0.size(); }

     void   fillSdpar(int key, double val);
     void   resetSdpar(int lay, int entr, int lr, int bin, double val);
     int    getSdSize() const { return m_sdmap.size(); }
     void   setSdBegin();
     int    getNextSdpar(int& key, double& sdpar);
     double getSdpar(int lay, int entr, int lr, int bin);
     int getSdKey(int lay, int entr, int lr, int bin) const;

     void   clear();

private:
     std::map<int, double> m_xtmap;
     std::vector<double>   m_t0;
     std::vector<double>   m_delt0;
     std::vector<double>   m_qtpar0;
     std::vector<double>   m_qtpar1;
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

#endif /* MDCCALIBCONST_H */
