// $Header: /bes/bes/BossCvs/Calibration/calibUtil/calibUtil/StripSrv.h,v 1.1.1.1 2005/10/17 06:12:26 maqm Exp $ 
#ifndef CALIBUTIL_STRIPSRV_H
#define CALIBUTIL_STRIPSRV_H

#include <string>
#include <iostream>
#include <vector>
#include <xercesc/dom/DOMElement.hpp>

namespace calibUtil {

  class GenericSrv;

  /** Visitor callbacks can indicate whether traversal should continue
       or not.
         CONT        normal return: continue processing
         USER_DONE   client has all information desired; no more traversal
         ERROR       client has serious error; abort
         DONE        not used by client.  Will be returned by 
                     BadStrips::traverse   in case processing was normal.
  */
  enum eVisitorRet {CONT, USER_DONE, ERROR, DONE};

  typedef std::vector<unsigned short int> StripCol;

  /** Visitor class interface definition, required for invoking 
      StripSrv::traverse
  */
  class ClientObject {
  public:    
    /**  Handle bad tower
        @param row         zero-based row of tower
        @param col         zero-based column of tower
        @param badness     bit mask including 3 least sig. bits
        These bits are defined in ChannelStatusDef.h

    */
    virtual eVisitorRet badTower(unsigned int row, unsigned int col, 
                                 int badness)=0;

    /**  Handle bad uniplane with some or all bad strips
        @param row         zero-based row of tower
        @param col         zero-based column of tower
        @param badness     bit mask including 3 least sig. bits:
        These bits are defined in ChannelStatusDef.h (same as for tower)
        @param allBad      if true all strips are bad. @arg strips should
                           be ignored
        @param strips      vector of strips of badness @arg badness.  If
                           empty, entire plane is bad.
    */
    virtual eVisitorRet badPlane(unsigned int row, unsigned int col, 
                                 unsigned int tray, bool top,
                                 int badness, bool allBad,
                                 const StripCol& strips)=0;

  };    // end pure virtual visitor class definition


  class StripSrv {


  public:

    enum eBadType  {UNKNOWN_BADTYPE, DEAD, HOT};
    /// Clients should use as return values for readData

    typedef struct stowerRC { 
      unsigned short int row; 
      unsigned short int col;
    } towerRC;
    typedef std::vector<unsigned short int> StripCol; 

    /// constructor. Initializes strip service by creating a DOM structure 
    /// out of the XML file and filling in the internal data structures 
    StripSrv(std::string xmlFileName);

    /// Constructor to be used when creating new calibrations
    StripSrv(eBadType badType, const GenericSrv& gen);

    /// destructor. Deallocates memory
    ~StripSrv();
  
    /// returns the status (Hot or Dead) of the strip
    eBadType getBadType() const;

    /// lists all towers with bad strips 
    void getBadTowers(std::vector<towerRC>& towerIds) const;

    
    
    /// methods giving access to generic data

    /// Get instrument name
    std::string getInst() const;
    
    /// Get timestamp
    std::string getTimestamp() const;

    /// Get calibration type
    std::string getCalType() const;
    
    /// Get format Version
    std::string getFmtVer() const;

    /// call back method for client to access large data
    //maqm fix
     //eVisitorRet StripSrv::traverseInfo(ClientObject *client) const;
     eVisitorRet traverseInfo(ClientObject *client) const;

    eVisitorRet writeXml(std::ostream* out);

  private:

    // A tower just has a collection of uniplanes, 
    // each identified by tray number, top or bottom, and badness
    //

    typedef struct sUniplane {
      bool m_allBad;                 // if true, StripCol may be ignored
      int  m_howBad;
      int  m_tray;
      bool m_top;
      StripCol m_strips;
    }  Uniplane;

    typedef struct sTower {
      //      towerRC id;
      unsigned short m_row;
      unsigned short m_col;
      bool    m_allBad;         // if true, uniplanes may be ignored
      int     m_howBad;
      std::vector<Uniplane> m_uniplanes;
    } Tower;
   
    std::vector<Tower> m_towers;
    eBadType m_badType;

    enum eState {
      FROM_PERS,       // read in from persistent form; read-only
      BUILDING,        // in process of building the list
      WRITTEN };       // built and written to persistent form (read-only)

    eState m_state;

    // object to store generic data
    GenericSrv *m_genSrv;   

    
    /// this function takes in a stripList in string format and 
    /// fills a vector with corresponding strip numbers
    void strToNum(std::string strips, std::vector<unsigned short int> &v);

    ///  Handles all the messy of details of extracting information
    ///  about a single unilayer from the XML representation
    //    void fillUni(const DOM_Element& uniElt, Unilayer* uni);
    void fillUni(const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* uniElt, 
                 Uniplane* uni);

    void fillStrips(const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* badElt, 
                    StripCol& list);
    
    /// Internal utility, used when this object must be mutable (e.g. addBad)
    Tower* findTower(towerRC& towerId);
    /// const version of findTower, used by const public methods
    const Tower* findTower(const towerRC& towerId) const;

  };

}// end of namespace calibUtil

#endif











