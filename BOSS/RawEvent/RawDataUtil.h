#ifndef RawDataUtil_H
#define RawDataUtil_H 
#include <iostream>

class RawDataUtil {
public:

  static double MdcTime(int timeChannel) {  return  ((double)timeChannel)*MDC_TIME_FACTOR; }    
  static int MdcTimeChannel (double time) { return (int)(time/MDC_TIME_FACTOR); }   
         
  static double MdcCharge(int chargeChannel) { return ((double)chargeChannel)*MDC_CHARGE_FACTOR; }    
  static int MdcChargeChannel (double charge) { return (int)(charge/MDC_CHARGE_FACTOR); } 
           
  static double EmcTime(int timeChannel) { return ((double)timeChannel)/EMC_TIME_FACTOR; }
  static int EmcTimeChannel (double time) { return (int)(time*EMC_TIME_FACTOR); }

  static double EmcCharge(int measure, int chargeChannel)
  {
    switch(measure) {
      case 3: //saturation of high measure
        return ((double)chargeChannel)*EMC_CHARGE_HIGH_MEASURE/EMC_CHARGE_FACTOR;
        break;
      case 2:
        return ((double)chargeChannel)*EMC_CHARGE_HIGH_MEASURE/EMC_CHARGE_FACTOR;
        break;
      case 1:
        return ((double)chargeChannel)*EMC_CHARGE_MID_MEASURE/EMC_CHARGE_FACTOR;
        break;
      case 0:
        return ((double)chargeChannel)*EMC_CHARGE_LOW_MEASURE/EMC_CHARGE_FACTOR;
        break;
      default:
        std::cout<<"Wrong measure of EMC charge channel!"<<std::endl;
    }
  }
    
  static int EmcChargeChannel(double charge)
  {
    if(charge>EMC_CHARGE_HIGH_MEASURE) {
      return (int)(EMC_CHARGE_FACTOR-1);
    } else if(charge>EMC_CHARGE_MID_MEASURE) {
      return (int)(charge/(EMC_CHARGE_HIGH_MEASURE/EMC_CHARGE_FACTOR));
    } else if(charge>EMC_CHARGE_LOW_MEASURE) {
      return (int)(charge/(EMC_CHARGE_MID_MEASURE/EMC_CHARGE_FACTOR));
    } else {
      return (int)(charge/(EMC_CHARGE_LOW_MEASURE/EMC_CHARGE_FACTOR));
    }
  }

  static int EmcChargeMeasure(double charge)
  {
    if(charge>EMC_CHARGE_HIGH_MEASURE) {
      return 3;
    } else if(charge>EMC_CHARGE_MID_MEASURE) {
      return 2;
    } else if(charge>EMC_CHARGE_LOW_MEASURE) {
      return 1;
    } else {
      return 0;
    }
  }

  static double TofTime(unsigned int timeChannel) { return ((double)timeChannel)*TOF_TIME_FACTOR; }
  static unsigned int TofTimeChannel (double time) { return (unsigned int)(time/TOF_TIME_FACTOR); } 

  static double TofCharge(unsigned int chargeChannel) 
  { 
    double ADC = chargeChannel & 0x1fff ;
    return ADC*TOF_CHARGE_FACTOR; 
  }
  
  static unsigned int TofChargeChannel (double charge, double time) 
  { 
    unsigned int ADC = (unsigned) (charge/TOF_CHARGE_FACTOR);
    unsigned int TDC = (unsigned) (time/TOF_TIME_FACTOR);
    if(ADC>=8192)
    {
      ADC = (ADC | 0x080000); // if ADC_channel>=8192, set overflow flag = 1.
      // The overlow flag is the 20th bit of the ADC_channel.
    }
    ADC =  ( (TDC & 0x07e000) | ADC) ; // Store tclock into ADC_channel as qclock
    //std::cout<<"ADC: "<<ADC<<std::endl;
    //std::cout<<"TDC: "<<TDC<<std::endl;
    //std::cout<<"qclock: "<<( ( ADC & 0x7e000 ) >> 13 );
    //std::cout<<"  tclock: "<<( ( TDC & 0x7e000 ) >> 13 )<<std::endl;
    //if ( ( ( ADC & 0x7e000 ) >> 13 ) != (( TDC & 0x7e000 ) >> 13 ) )
    //  std::cout<<"qclock!=tclock"<<std::endl;
    return ADC;
  } 

private:
  static const double MDC_TIME_FACTOR;
  static const double MDC_CHARGE_FACTOR;

  static const double TOF_TIME_FACTOR;
  static const double TOF_CHARGE_FACTOR;

  static const double EMC_CHARGE_HIGH_MEASURE;
  static const double EMC_CHARGE_MID_MEASURE;
  static const double EMC_CHARGE_LOW_MEASURE;
  static const double EMC_TIME_FACTOR;
  static const double EMC_CHARGE_FACTOR;    
};

#endif
