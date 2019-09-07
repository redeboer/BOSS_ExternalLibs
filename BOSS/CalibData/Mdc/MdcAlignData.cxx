#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "CalibData/Mdc/MdcAlignData.h"

using namespace std;
namespace CalibData {

   StatusCode MdcAlignData::update(CalibBase1& other, MsgStream* log)
   {
      MdcAlignData& other1 = dynamic_cast<MdcAlignData& >(other);
      cout<<"\n"<<"here is the update in the MdcCal Alignment"<<std::endl;
      CalibBase1::update(other, log);
      for(int i=0;i<16;i++){
	 dxEP[i]=other1.dxEP[i];
	 dyEP[i]=other1.dyEP[i];
	 dzEP[i]=other1.dzEP[i];
	 rxEP[i]=other1.rxEP[i];
	 ryEP[i]=other1.ryEP[i];
	 rzEP[i]=other1.rzEP[i];
      }
      for(int i=0;i<6796;i++){
	 dxWireEast[i]=other1.dxWireEast[i];
	 dyWireEast[i]=other1.dyWireEast[i];
	 dzWireEast[i]=other1.dzWireEast[i];
	 dxWireWest[i]=other1.dxWireWest[i];
	 dyWireWest[i]=other1.dyWireWest[i];
	 dzWireWest[i]=other1.dzWireWest[i];
	 tension[i]=other1.tension[i];
      }
      
      return StatusCode::SUCCESS;
   }

   
void MdcAlignData::readPar(char* p1, char* p2, char* p3){
     int i;
     string strtmp;
     
     istringstream  falign;
   string aa1 = p1;
     falign.str(aa1);   
     for(i=0; i<7; i++) falign >> strtmp;
     for(i=0; i<16; i++){
	  falign >> strtmp >> dxEP[i] >> dyEP[i] >> dzEP[i] >> rxEP[i] >> ryEP[i] >> rzEP[i];
     }

     istringstream fwpos;
     string aa2 = p2;
     fwpos.str(aa2);
     for(i=0; i<7; i++) fwpos >> strtmp;
     for(i=0; i<6796; i++){
	  fwpos >> strtmp >> dxWireEast[i] >> dyWireEast[i] >> dzWireEast[i]
		>> dxWireWest[i] >> dyWireWest[i] >> dzWireWest[i];
     }

     istringstream ften;
     string aa3 = p3;
     ften.str(p3);
     for(i=0; i<6796; i++) ften >> strtmp >> tension[i];
}
}
