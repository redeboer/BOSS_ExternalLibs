#ifndef ROOTCNVSVC_H
#define ROOTCNVSVC_H 1

#include <map>

#include "GaudiKernel/ConversionSvc.h"
#include "GaudiKernel/DataObject.h"
#include "commonData.h"
#include "Util.h"

template <class TYPE> class SvcFactory;
class RootInterface;
class DigiCnv;
class DstCnv;
class EvtHeaderCnv;
class EvtNavigatorCnv;
class EventCnv;
class McCnv;
class RootEvtSelector;
class RecTrackCnv;
class EvtRecCnv;  //zoujh
class TrigCnv; //caogf
class HltCnv;  //fucd

static const InterfaceID IID_IRootCnvSvc("IRootCnvSvc", 1, 0);

/** @class RootCnvSvc
 * @brief  Root Event Conversion Service which coordinates all of our converters.
 *
 * When a component requests an item not yet available on the TDS, 
 * the RootcnvSvc is called to find the appropriiate converter to gain
 * access to the data and put it on the TDS.
 * Based on SICb service written by Markus Frank.
 * If an item is mentioned in the jobOptions as one to be written out, it 
 * will be converted from TDS to ROOT by the appropriate converter
 *
 * Based on the RootCnvSvc of GLast.
 */

class RootCnvSvc  : virtual public ConversionSvc	{

  friend class SvcFactory<RootCnvSvc>;

 public:
/** @class Leaf
 * @brief object regrouping CLID and pathname with treename/branchname
 */
  class Leaf : public std::vector<Leaf*>    {
  public:
    std::string path;
    std::string treename;
    std::string branchname;
    CLID        clid;
    Leaf(const std::string& p, const CLID& c, const std::string& tree,  const std::string& branch)
      : path(p), treename(tree), branchname(branch),clid(c){}

    bool operator==(const Leaf& copy) const  {
      return  path          == copy.path && 
        treename      == copy.treename && 
        branchname    == copy.branchname && 
	clid          == copy.clid ;
    }
  };

 private:
  /// Add converters to the service  
  StatusCode addConverters ();
 
  bool m_selectFromTag;
  std::vector<std::string> m_tagInputFile;
  //int m_tagFileFormat;

  /// access to the RootInterface
    RootInterface* m_rootInterface;
  /// access to the EventSelector
    RootEvtSelector* m_evtsel;
  /// Map with leaf entries
  typedef std::map<std::string, Leaf*> LeafMap;
  LeafMap m_leaves;

  /// properties
  /// minimal split required
  const int m_minSplit;

  /// digi input filename
  std::vector<std::string> m_difileName;
  //  std::string m_difileName;
  /// digi output filename
  std::string m_dofileName;

  
  ///digi split mode
  int m_dsplitMode;

  /// buffer size for digi file
  int  m_dbufSize;

  /// compression level for digix file
  int  m_dcompressionLevel;

  /// digi treename
  std::string m_dtreeName;

  /// pointers to top converters
  DigiCnv *m_dCnv;
  DstCnv *m_dstCnv;
  McCnv  *m_mcCnv;
  TrigCnv *m_trigCnv; //caogf
  HltCnv  *m_hltCnv; //fucd
  EvtRecCnv  *m_evtRecCnv;  //zoujh

  RecTrackCnv *m_rectrackCnv;   //*********liangyt
  EvtHeaderCnv *m_evtheaderCnv;
  EvtNavigatorCnv *m_evtnavigatorCnv;
  EventCnv     *m_eventCnv;
  
  /// relational maps to be cleared
  commonData m_common;

  StatusCode initFiles();

public:

  RootInterface * getRootInterface() {return m_rootInterface;}

  virtual StatusCode initialize();

  virtual StatusCode finalize();

  virtual StatusCode updateServiceState(IOpaqueAddress* pAddress);

  /// Override inherited queryInterface due to enhanced interface
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  /// Associates a path on TDS with a particular converter
  virtual StatusCode declareObject(const Leaf& leaf);

  /// Commit pending output (fill the TTrees).
  virtual StatusCode commitOutput(const std::string& output, bool do_commit);

  /// create address containing ROOT treename, branchname, entry number
  virtual StatusCode createAddress( long int svc_type,
				    const CLID& clid,
				    const std::string* par, 
				    const unsigned long* ip,
				    IOpaqueAddress*& refpAddress);

  virtual StatusCode createAddress(DataObject *obj, IOpaqueAddress*& refpAddress);
  virtual StatusCode createAddress(std::string path, IOpaqueAddress*& refpAddress);

  DigiCnv *getDigiCnv() {return m_dCnv;}
  DstCnv *getDstCnv() {return m_dstCnv;}
  McCnv  *getMcCnv() {return m_mcCnv;}
  TrigCnv *getTrigCnv() { return m_trigCnv; } //caogf
  HltCnv  *getHltCnv() { return m_hltCnv; } //fucd
  EvtRecCnv *getEvtRecCnv() { return m_evtRecCnv; } //zoujh

  void setDigiCnv(DigiCnv *reccnv){m_dCnv=reccnv;}
  void setDstCnv(DstCnv *dstcnv){m_dstCnv=dstcnv;}
  void setMcCnv (McCnv  *mccnv) {m_mcCnv=mccnv;}
  void setTrigCnv (TrigCnv *trigcnv) { m_trigCnv = trigcnv; } //caogf
  void setHltCnv (HltCnv *hltcnv) { m_hltCnv = hltcnv; } //fucd
  void setEvtRecCnv (EvtRecCnv *evtreccnv) { m_evtRecCnv = evtreccnv; } //zoujh
  void setRecTrackCnv (RecTrackCnv  *rectrackcnv) {m_rectrackCnv=rectrackcnv;}
  void setEvtHeaderCnv (EvtHeaderCnv *evtheadercnv){m_evtheaderCnv=evtheadercnv;}
  void setEvtNavigatorCnv (EvtNavigatorCnv *evtnavigatorcnv){m_evtnavigatorCnv=evtnavigatorcnv;}
  void setEventCnv(EventCnv* eventCnv){m_eventCnv=eventCnv;}

  
  RecTrackCnv *getRecTrackCnv() {return m_rectrackCnv;}   //*******liangyt
  EvtHeaderCnv *getEvtHeaderCnv(){return m_evtheaderCnv;}
  EvtNavigatorCnv *getEvtNavigatorCnv(){return m_evtnavigatorCnv;}
  EventCnv     *getEventCnv(){return m_eventCnv;}
protected:


  RootCnvSvc(const std::string& name, ISvcLocator* svc);

  virtual ~RootCnvSvc() { };


};

#endif // ROOTCNVSVC_H
