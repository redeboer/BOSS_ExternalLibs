// Convert Data Format from Online data to TofData
//
#ifndef TOFDATA_H
#define TOFDATA_H
#include "Identifier/Identifier.h"
#include <vector>
#include <map>

class TofValue{
 public:
  TofValue();
  virtual ~TofValue();

  // Assignment constructor.
  TofValue& operator=(const TofValue& source);

  int channel()            const { return  m_channel;     }
  int times()              const { return  m_times;       }
  int number()             const { return  m_number;      }
  int clock()              const { return  m_clock;       }
  double value()           const { return  m_value;       }
  bool used()              const { return  m_used;        }

  void timespp();
  void timesmm();
  void setNumber( int number )   { m_number = number;     }
  //  virtual void setValue( int value ) = 0;
  void setUsed( bool used )      { m_used   = used;       }

 protected:
  int m_clock;
  int m_channel;
  double m_value;

 private:
  int m_times;
  int m_number;
  bool m_used;

};


class Adc : public TofValue {
 public:
  Adc();

  void setCorr()                 { m_corr   = true;       }
  void setElec()                 { m_elec   = true;       }

  void setValue( Identifier identify, int value );

  double qtc()             const { return  m_qtc;         }

 private:
  double m_qtc;
  bool m_corr, m_elec, m_mrpc;
};


class Tdc : public TofValue {
 public:
  void setValue( int value );
 private:
};


class PmtData{
 public:
  PmtData();
  ~PmtData();

  // Assignment constructor.
  PmtData& operator=(const PmtData& source);

  unsigned int iden()      const { return  m_iden_value;        }
  unsigned int times()     const { return  m_times;             }
  int adcChannel();
  double qtc();
  double adc();
  int qclock();
  int tdcChannel();
  double tdc();
  int tclock();
  unsigned int quality()   const { return  m_quality;           }

  void timespp();
  void timesmm();
  int  qtimes();
  void qtimespp();
  void qtimesmm();
  int  ttimes();
  void ttimespp();
  void ttimesmm();

  int  qnumber();
  int  tnumber();

  bool qused();
  bool tused();
  bool used();

  void setIdentify( unsigned int iden )  { m_iden_value = iden; }
  void setAdc( Adc* adc );
  void setTdc( Tdc* tdc );
  void setUsed();

  void clear();

 private:
  unsigned int m_iden_value;
  unsigned int m_times;
  unsigned int m_quality;      // 0x 0 0 1 (Q) 1 (T)
  Adc* m_adc;
  Tdc* m_tdc;

};


class TofData{
 public:
  TofData();
  ~TofData();

  // Assignment constructor.
  TofData& operator=(const TofData& source);

  unsigned int identify()   const { return  m_identify;        }
  bool is_mrpc()            const { return  m_mrpc;            }
  bool barrel()             const { return  m_barrel;          }
  int  tofId()              const { return  m_tofId;           }
  int  layer()              const { return  m_layer;           }
  int  strip()              const { return  m_strip;           }
  int  tofTrackId()         const { return  m_tofTrackId;      }

  unsigned int times()      const { return  m_times;           }

  void data( double& adcEast, double& tdcEast, double& adcWest, double& tdcWest );
  void data( double& adc, double& tdc );

  PmtData* forward()        const { return  m_forward;         }
  PmtData* backward()       const { return  m_backward;        }

  double adc1();
  double tdc1();
  double adc2();
  double tdc2();
  double adc();
  double tdc();

  double qtc1();
  double qtc2();
  double qtc();

  int adcChannelEast();
  int tdcChannelEast();
  int adcChannelWest();
  int tdcChannelWest();
  int adcChannel();
  int tdcChannel();

  int qclock1();
  int tclock1();
  int qclock2();
  int tclock2();
  int qclock();
  int tclock();

  unsigned int quality()      const { return  m_quality;                }
  bool tmatched()             const { return  m_tmatched;               }

  int qtimes1();
  int ttimes1();
  int qtimes2();
  int ttimes2();
  unsigned int eastTimes();
  unsigned int westTimes();

  int  qnumber1();
  int  tnumber1();
  int  qnumber2();
  int  tnumber2();

  bool qused1();
  bool tused1();
  bool qused2();
  bool tused2();
  bool used1();
  bool used2();
  bool used();

  double ztdc()            const { return  m_ztdc;        }
  double zadc()            const { return  m_zadc;        }

  double zpos()            const { return  m_zpos;        }
  double energy()          const { return  m_energy;      }

 public:
  void setIdentify( Identifier identify );
  void setMisLable()                        {  m_misLable   = true;       }
  void setTofTrackId( int tofTrackId )      {  m_tofTrackId = tofTrackId; }
  void setTimes( unsigned int times )       {  m_times      = times;      }
  void setForward( PmtData* pmt );
  void setBackward( PmtData* pmt );
  void setQuality( unsigned int quality )   {  m_quality    = quality;    }
  void setTMatched( bool tmatched )         {  m_tmatched   = tmatched;   }
  void setUsed();
  void setZTdc( double ztdc )               {  m_ztdc   = ztdc;           }
  void setZAdc( double zadc )               {  m_zadc   = zadc;           }
  void setZpos( double zpos )               {  m_zpos   = zpos;           }
  void setEnergy( double energy )           {  m_energy = energy;         }

  void clear();

 private:

  unsigned int  m_identify;
  bool  m_mrpc;
  bool  m_barrel;
  int   m_tofId;
  int   m_layer;
  int   m_strip;
  int   m_tofTrackId;

  bool  m_misLable;

  double m_ztdc;
  double m_zadc;

  double m_zpos;        //z-position after calibration
  double m_energy;      //energy deposit after calibration

  unsigned int m_times;
  unsigned int m_quality;
  // 0x 0...0  1 (East Q) 1 (East T) 1 (West Q) 1 (West T)
  // Forward == East ;  Backward = West
  bool m_tmatched;
  // true, will used for Estime and TOF, false, will used only for EMC

  PmtData* m_forward;
  PmtData* m_backward;

};

typedef std::multimap<unsigned int, TofData*> TofDataMap;
typedef std::multimap<unsigned int, TofData*>::iterator IterTofDataMap;

typedef std::vector<TofData*> TofDataVector;
typedef std::vector<TofData*>::iterator IterTofDataVector;

#endif
