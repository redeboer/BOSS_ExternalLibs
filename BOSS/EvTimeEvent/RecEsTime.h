/*
 *
 * This class is a Event data model for the EsTimeAlg  output 
 * Created by max
 * 
 */

#ifndef  EVTIME_H
#define  EVTIME_H
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h" 
#include "EventModel/EventModel.h"
#include "GaudiKernel/ObjectVector.h"
#include "Identifier/Identifier.h"

extern const  CLID &CLID_RecEsTime;


class RecEsTime : virtual public ContainedObject {

public:
  RecEsTime (){}
  virtual ~RecEsTime(){}

  virtual const CLID& clID()  const { return RecEsTime::classID();}
  static  const CLID& classID() { return CLID_RecEsTime; }

  //Extractors
 public:
  
  double getTest()          const { return  m_Test;}
  int   getStat()          const { return  m_Stat;}
  double getQuality()       const { return  m_Quality;}
  // int   getNbunch()        const { return  m_nbunch;}

  void  setTest(double Test)             { m_Test = Test;}
  void  setStat(int Stat)		{ m_Stat = Stat;}
  void  setQuality(double Quality)        { m_Quality = Quality;}
  // void  setNbunch(int nbunch)           { m_nbunch = nbunch;}

private:

  double m_Test;         // event start time 
  int   m_Stat;         // 1/2/3: from TOF;/ danger events from TOF; /change events before and after MDC recon
                        // 4/5/6: from MDC / danger  / change              
                        // 7:from segment fit;
                        // 8:six parameter fit
			// 9:cosmic event
  double m_Quality;      // value between measured time and bunch time   
  // int  m_nbunch;     //bunches before the collide  bunch 
};
 
typedef ObjectVector<RecEsTime> RecEsTimeCol;

#endif //EVTIME_H

