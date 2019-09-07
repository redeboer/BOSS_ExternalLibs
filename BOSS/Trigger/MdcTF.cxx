//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     /
////
////---------------------------------------------------------------------------/
////
////Description:
////Author:   Caogf
////Created:  Feb, 2006
////Modified:
////Comment:
////
//
#include "Trigger/MdcTF.h"
#include <iostream>
#include <fstream>
#include <TString.h>

using namespace std;

MdcTF::MdcTF()
{
  m_MdcTSF = MdcTSF::get_Mdc();
  ReadTFLut();
  //Dump();
}
MdcTF::~MdcTF()
{
}
void MdcTF::GetTrack()
{
  intrkSL1.clear();
  intrkSL2.clear();

  m_MdcTSF->GetTS();
  for(int i=0;i<128;i++) { strack[i] = 0; ltrack[i] = 0; }
  
  for(int i=0; i<48; i++) {
    if(m_MdcTSF->GetSL1TSF(i)==1)  intrkSL1.push_back(i);
  }
  for(int i=0; i<80; i++) {
    if(m_MdcTSF->GetSL2TSF(i)==1)  intrkSL2.push_back(i); 
  }

  std::vector<std::vector<int> >::iterator viter=TF.begin();
  std::vector<int>::iterator iter;
  for(int i=0;i<128;i++)
  {
    int tracId5 = m_MdcTSF->GetSL5TSF(i);
    if(tracId5!=0)
    {
      iter = (*(viter + i)).begin();
      for(;iter!=(*(viter + i)).end();iter+=4)
      {
	int cellId3 = *(iter+3)-1;
	int cellId4 = *(iter+2)-1;
	int cellId10 =*(iter)-1;
	int trackId3 = m_MdcTSF->GetSL3TSF(cellId3);
	int trackId4 = m_MdcTSF->GetSL4TSF(cellId4);
	int trackId10 = m_MdcTSF->GetSL10TSF(cellId10); 
	if(trackId3 > 0 && trackId4 > 0) { strack[i] = 1;  }
	if(trackId3 > 0 && trackId4 > 0 && trackId10 > 0) ltrack[i] = 1; 
      }
    }
  }
  //debug
/*
  std::cout<<"*************IN TF.cxx ************"<<std::endl;
  for(int j = 0; j < 128; j++) {
    if(strack[j] == 1) std::cout<<"short track : "<<j<<std::endl;
  }
  for(int j = 0; j < 128; j++) {
    if(ltrack[j] == 1) std::cout<<"long track : "<<j<<std::endl;
  }
*/
}
void MdcTF::ReadTFLut()
{
  fstream readin;
  TF.clear();
  int a,b,l1,l2,l3,l4;
  for(int i=1;i<129;i++)
  {
    int num=0;
    std::vector<int> tmp;
    tmp.clear();
    TString filename = TString(getenv( "TRIGGERROOT" ));
    filename+="/data/tf/ptd5-2/PTD-SL5C-L19-C";
    filename+=i;
    filename+=".pat";
    readin.open(filename,ios_base::in);
    if(!readin) cerr<<"can not open the file "<<filename<<endl;
    while(readin.good())
    {
      readin>>a>>b>>l4>>l3>>l2>>l1;
      num++;
    }
    if(i==1) std::cout<<"number of line is "<<num-1<<std::endl;
    readin.close();
    readin.clear();
    readin.open(filename,ios_base::in);
    if(!readin) std::cerr<<"can not open the file again "<<filename<<std::endl;
    for(int j=0;j<(num-1);j++)
    {
      readin>>a>>b>>l4>>l3>>l2>>l1;
      tmp.push_back(l4);
      tmp.push_back(l3);
      tmp.push_back(l2);
      tmp.push_back(l1);
    }
    readin.close();
    TF.push_back(tmp);
  }
}
void MdcTF::Dump()
{
  std::vector<int> combine_size;
  cout<<endl;
  cout<<"     ***************check data file tf: *****************      "<<endl;
  cout<<"*--------------------------BEGIN-------------------------------*"<<endl;
  cout<<endl;
  combine_size.clear();
  for(unsigned int i=0; i<TF.size(); i++) {
    cout<<"Cell ID --> "<<i<<endl;
    combine_size.push_back(TF[i].size());
    for(unsigned int j=0; j<TF[i].size(); j++) {
      cout<<TF[i][j]<<" ";
      if((j+1)%4==0) cout<<endl;
    }  
    cout<<endl;
  } 
  sort(combine_size.begin(),combine_size.end());
  cout<<"The Max Number of Combination is "<<combine_size[combine_size.size()-1]/4<<endl;
  cout<<endl;
  cout<<"*----------------------------END-------------------------------*"<<endl;
}
