#ifndef TrigGTDProvider_H
#define TrigGTDProvider_H
#include "GaudiKernel/ContainedObject.h"
#include <map>
#include <vector>
#include <string>

using namespace std;

//extern const CLID& CLID_TrigGTDProvider;
/*
 Global trigger data structure (GTDProvider)
*/
class TrigGTDProvider : virtual public ContainedObject {
public:
  // Constructor
  TrigGTDProvider(const unsigned int boardIndex, const unsigned int dataType, const unsigned int timeWindow, 
                  const unsigned int GTDProviderTiming,  const bool preScale, 
                  const std::map<unsigned int, std::vector<unsigned int> > GTDProviderData, 
                  const std::vector<unsigned int> GTDProviderEvtType, 
                  const std::map<unsigned int, std::vector<unsigned int> > Num_One, 
                  const std::map<unsigned int, std::vector<unsigned int> > Num_Zero);

  TrigGTDProvider() {}

  //destructor
  virtual ~TrigGTDProvider() {}

  // Retrieve reference to class definition structure
//  virtual const CLID& clID() const { return TrigGTDProvider::classID(); }
//  static const CLID& classID()     { return CLID_TrigGTDProvider; }

  //set related information
  void setDataType(const unsigned int dataType);
  void setTimeWindow(const unsigned int timeWindow);
  void setBoardIndex(const unsigned int boardIndex);
  void setTrigTiming(const unsigned int GTDProviderTiming);
  void setPreScale(const bool preScale);
  void setTrigCondList(const std::map<unsigned int, std::vector<unsigned int> > GTDProviderData);
  void setTrigEvtType(const std::vector<unsigned int> GTDProviderEvtType);
  void setNumOne(const std::map<unsigned int, std::vector<unsigned int> > Num_One);
  void setNumZero(const std::map<unsigned int, std::vector<unsigned int> > Num_Zero);

  //get related information
  unsigned int getDataType() const;
  unsigned int getTimeWindow() const;
  unsigned int getBoardIndex() const;
  unsigned int getTrigTiming() const;
  bool getPreScale() const;
  const std::map<unsigned int, std::vector<unsigned int> >& getTrigCondList() const;
  const std::vector<unsigned int>& getTrigEvtType() const;
  const std::map<unsigned int, std::vector<unsigned int> >& getNumOne() const;
  const std::map<unsigned int, std::vector<unsigned int> >& getNumZero() const;

protected:
  //data structure of GTDProvider
  unsigned int m_boardIndex;
  unsigned int m_dataType;
  unsigned int m_timeWindow;
  unsigned int m_GTDProviderTiming;
  bool m_preScale;
  std::map<unsigned int, std::vector<unsigned int> > m_GTDProviderData;
  std::vector<unsigned int> m_GTDProviderEvtType;
  
  std::map<unsigned int, std::vector<unsigned int> > m_Num_One; // number of "1" for each trigger condition or each bit in time window
  std::map<unsigned int, std::vector<unsigned int> > m_Num_Zero; // number of "0" before the first "1" for each condition or each bit in time window
  
};

typedef ObjectVector<TrigGTDProvider> TrigGTDProviderCol;

#endif 
