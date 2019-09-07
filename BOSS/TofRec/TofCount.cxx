#include "TofRec/TofTrack.h"
#include "TofRec/TofCount.h"
#include <iostream>
#include <iomanip>

TofCount::TofCount() {
  extTrkNum = 0;
  barrelTrk = 0;
  endcapTrk = 0;
  inner1  = 0;
  outer1  = 0;
  double1 = 0;
  east1   = 0;
  west1   = 0;
  nohit1  = 0;
  unchanged1 = 0;
  changed1   = 0;
  inner2  = 0;
  outer2  = 0;
  double2 = 0;
  east2   = 0;
  west2   = 0;
  nohit2  = 0;
  unchanged2 = 0;
  changed2   = 0;
  inner3  = 0;
  outer3  = 0;
  double3 = 0;
  east3   = 0;
  west3   = 0;
  nohit3  = 0;
  unchanged3 = 0;
  changed3   = 0;
  inner4  = 0;
  outer4  = 0;
  double4 = 0;
  east4   = 0;
  west4   = 0;
  nohit4  = 0;
  unchanged4 = 0;
  changed4   = 0;

  for( unsigned int i=0; i<nParBhabha; i++ ) {
    nbb[i] = 0;
  }

  return;
}


void TofCount::init() {
  extTrkNum = 0;
  barrelTrk = 0;
  endcapTrk = 0;
  inner1  = 0;
  outer1  = 0;
  double1 = 0;
  east1   = 0;
  west1   = 0;
  nohit1  = 0;
  unchanged1 = 0;
  changed1   = 0;
  inner2  = 0;
  outer2  = 0;
  double2 = 0;
  east2   = 0;
  west2   = 0;
  nohit2  = 0;
  unchanged2 = 0;
  changed2   = 0;
  inner3  = 0;
  outer3  = 0;
  double3 = 0;
  east3   = 0;
  west3   = 0;
  nohit3  = 0;
  unchanged3 = 0;
  changed3   = 0;
  inner4  = 0;
  outer4  = 0;
  double4 = 0;
  east4   = 0;
  west4   = 0;
  nohit4  = 0;
  unchanged4 = 0;
  changed4   = 0;

  return;
}


void TofCount::setExtTrackNum( unsigned int ntrk ) {
  extTrkNum = extTrkNum + ntrk;
  return;
}


void TofCount::setTrack1( TofTrack*& tof ) {
  if( tof->hitCase() == InnerLayer )       inner1++;
  else if( tof->hitCase() == OuterLayer )  outer1++;
  else if( tof->hitCase() == DoubleLayer ) double1++;
  else if( tof->hitCase() == EastEndcap )  east1++;
  else if( tof->hitCase() == WestEndcap )  west1++;
  else if( tof->hitCase() == EastEndcapMRPC )  east1++;
  else if( tof->hitCase() == WestEndcapMRPC )  west1++;
  else                                     nohit1++;
  return;
}


void TofCount::setTrack1Col( std::vector<TofTrack*>*& tofTrackVec ) {
  TofTrackVec::iterator iter_tof = tofTrackVec->begin();
  for( ; iter_tof!=tofTrackVec->end(); iter_tof++ ) {
    setTrack1( *iter_tof );
  }
  return;
}


void TofCount::setTrack2( TofTrack*& tof ) {
  if( tof->hitCase() == InnerLayer )       inner2++;
  else if( tof->hitCase() == OuterLayer )  outer2++;
  else if( tof->hitCase() == DoubleLayer ) double2++;
  else if( tof->hitCase() == EastEndcap )  east2++;
  else if( tof->hitCase() == WestEndcap )  west2++;
  else if( tof->hitCase() == EastEndcapMRPC )  east2++;
  else if( tof->hitCase() == WestEndcapMRPC )  west2++;
  else                                     nohit2++;
  //  if( tof->changed() ) changed2++;
  //  else                 unchanged2++;
  return;
}


void TofCount::setTrack3( TofTrack*& tof ) {
  if( tof->hitCase() == InnerLayer )       inner3++;
  else if( tof->hitCase() == OuterLayer )  outer3++;
  else if( tof->hitCase() == DoubleLayer ) double3++;
  else if( tof->hitCase() == EastEndcap )  east3++;
  else if( tof->hitCase() == WestEndcap )  west3++;
  else if( tof->hitCase() == EastEndcapMRPC )  east3++;
  else if( tof->hitCase() == WestEndcapMRPC )  west3++;
  else                                     nohit3++;
  //  if( tof->changed() ) changed3++;
  //  else                 unchanged3++;
  return;
}


void TofCount::setTrack4( TofTrack*& tof ) {
  if( tof->hitCase() == InnerLayer )       inner4++;
  else if( tof->hitCase() == OuterLayer )  outer4++;
  else if( tof->hitCase() == DoubleLayer ) double4++;
  else if( tof->hitCase() == EastEndcap )  east4++;
  else if( tof->hitCase() == WestEndcap )  west4++;
  else if( tof->hitCase() == EastEndcapMRPC )  east4++;
  else if( tof->hitCase() == WestEndcapMRPC )  west4++;
  else                                     nohit4++;
  //  if( tof->changed() ) changed4++;
  //  else                 unchanged4++;
  return;
}

/*
void TofCount::final() {
   std::cout<<"############################# TofRec Summary #####################################" << std::endl;
   std::cout<<" ===> The Extend tracks in total:  " << extTrkNum <<std::endl;
   std::cout<<"     ===> The valid Extend tracks in total:  "<< (inner4+outer4+double4+east4+west4) <<std::endl;
   std::cout<<"          ===> The TofRec barrel tracks in total:  " << (inner4+outer4+double4) <<std::endl;
   std::cout<<"              ===> The Inner Layer hit only in total:   " << inner1 << "   " << inner2 << "   " << inner3 << "   " << inner4 << std::endl;
   std::cout<<"              ===> The Outer Layer hit only in total:   " << outer1 << "   " << outer2 << "   " << outer3 << "   " << outer4 << std::endl;
   std::cout<<"              ===> The Double Layer hit in total:       " << double1 << "   " << double2 << "   " << double3 << "   " << double4 << std::endl;
   std::cout<<"          ===> The TofRec endcap tracks in total:  " << (east4+west4) <<std::endl;
   std::cout<<"              ===> The TofRec East endcap tracks in total: " << east1 << "   " << east2 << "   " << east3 << "   " << east4 << std::endl;
   std::cout<<"              ===> The TofRec West endcap tracks in total: " << west1 << "   " << west2 << "   " << west3 << "   " << west4 << std::endl;
   std::cout<<"     ===> The invalid Extend tracks in total:  "<< nohit1 << "   " << nohit2 << "   " << nohit3 << "   " << nohit4 <<std::endl;
   std::cout<<"#############################  End Summary #######################################" <<std::endl;

}
*/

void TofCount::final() {
   std::cout<<"############################# TofRec Summary #####################################" << std::endl;
   std::cout<<" ===> Number of Extrapolated tracks:  " << extTrkNum <<std::endl;
   std::cout<<"   ===> Number of valid Extraploated tracks:  " << (inner1+outer1+double1+east1+west1) << "  Barrel: " << (inner1+outer1+double1) << "  Endcap: " << (east1+west1) << std::endl;
   std::cout<<"     ===> TofRec: number of barrel tracks:  " << (inner3+outer3+double3) << "  Efficiency:  " << setprecision(4) << (inner3+outer3+double3)*100.0/((inner1+outer1+double1)*1.0) << "%" << std::endl;
   std::cout<<"       ===> Inner Layer hit only in total:   " << inner1 << " \t" << inner2 << " \t" << inner3 << " \t" << inner4 << std::endl;
   std::cout<<"       ===> Outer Layer hit only in total:   " << outer1 << "\t" << outer2 << " \t" << outer3 << " \t" << outer4 << std::endl;
   std::cout<<"       ===> Double Layer hit in total:       " << double1 << " \t" << double2 << " \t" << double3 << " \t" << double4 << std::endl;
   std::cout<<"     ===> TofRec: number of endcap tracks:  " << (east3+west3) << "  Efficiency:  " << setprecision(4) << (east3+west3)*100.0/((east1+west1)*1.0) << "%" << std::endl;
   std::cout<<"       ===> East endcap tracks in total: " << east1 << " \t" << east2 << " \t" << east3 << " \t" << east4 << std::endl;
   std::cout<<"       ===> West endcap tracks in total: " << west1 << " \t" << west2 << " \t" << west3 << " \t" << west4 << std::endl;
   std::cout<<"     ===> The invalid Extend tracks in total:  "<< nohit1 << " \t" << nohit2 << " \t" << nohit3 << " \t" << nohit4 <<std::endl;
   std::cout<<"#############################  End Summary #######################################" <<std::endl;

}


void TofCount::initBhabha() {
 for( unsigned int i=0; i<nParBhabha; i++ ) {
   nbb[i] = 0;
 }
 return;
}


void TofCount::addNumber(unsigned int i) {
  if( i<nParBhabha ) {
    nbb[i]++;
  }
  return;
}


void TofCount::finalBhabha( std::string calibData ) {
  std::cout<<"$$$$$$$ Summary of Bhabha Selection Criteria $$$$$$$$$$$$$$$$$$$$$"<<std::endl;
  std::cout <<"0 Total umber of events:  "<<nbb[0]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[0])*100.0/double(nbb[0])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[0])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"1 Event Start Time:       "<<nbb[1]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[1])*100.0/double(nbb[0])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[1])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"2 N_ext_track = 2:        "<<nbb[2]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[2])*100.0/double(nbb[1])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[2])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"3 N_mdc_track = 2:        "<<nbb[3]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[3])*100.0/double(nbb[2])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[3])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"4 EMC reconstructed:      "<<nbb[4]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[4])*100.0/double(nbb[3])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[4])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"5 N_shower >= 2:          "<<nbb[5]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[5])*100.0/double(nbb[4])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[5])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"6 Total charge = 0:       "<<nbb[6]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[6])*100.0/double(nbb[5])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[6])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"7 Back to back:           "<<nbb[7]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[7])*100.0/double(nbb[6])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[7])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"8 Kalman Filter:          "<<nbb[8]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[8])*100.0/double(nbb[7])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[8])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"9 Distance of ext-shower: "<<nbb[9]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[9])*100.0/double(nbb[8])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[9])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"10 Vertex cut of 1st trk: "<<nbb[10]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[10])*100.0/double(nbb[9])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[10])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"11 Vertex cut of 2nd trk: "<<nbb[11]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[11])*100.0/double(nbb[10])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[11])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"12 Delta phi:             "<<nbb[12]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[12])*100.0/double(nbb[11])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[12])*100.0/double(nbb[0])<<"%"<<std::endl;
  std::cout <<"13 Energy of Shower:      "<<nbb[13]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[13])*100.0/double(nbb[12])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[13])*100.0/double(nbb[0])<<"%"<<std::endl;
  if( calibData=="Bhabha" ) {
    std::cout <<"14 Energy of others:      "<<nbb[14]<<"\t"<<"Ratio1: "<<setprecision(4)<<double(nbb[14])*100.0/double(nbb[13])<<"%"<<"\t"<<"Ratio2: "<<setprecision(4)<<double(nbb[14])*100.0/double(nbb[0])<<"%"<<std::endl;
  }
  else if( calibData=="Dimu") {
  }
  std::cout<<"$$$$$$$ End of Summary $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<std::endl;
  return;
}
