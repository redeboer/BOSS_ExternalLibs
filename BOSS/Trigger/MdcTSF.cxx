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
#include "Trigger/MdcTSF.h"

#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"

#include "RawEvent/RawDataUtil.h"
#include "RawEvent/DigiEvent.h"

#include <TString.h>
#include <fstream>
#include <vector>

using namespace std;

MdcTSF* MdcTSF::mdc_Pointer=0;
MdcTSF* MdcTSF::get_Mdc(void) {
  if(!mdc_Pointer) mdc_Pointer = new MdcTSF();
  return mdc_Pointer;
}

MdcTSF::MdcTSF()
{
  ReadLut();
  //Dump();
}
MdcTSF::~MdcTSF()
{
}
void MdcTSF::setMdcDigi(std::vector<int>& vmdcHit) 
{
  std::vector<int> vtmp;
  vtmp.clear();
  digiId.clear();

  for(std::vector<int>::iterator iter = vmdcHit.begin(); iter != vmdcHit.end(); iter += 2)
  {
    int layer = *iter;
    int wire  = *(iter + 1);
    if(layer<=19) { vtmp.push_back(layer); vtmp.push_back(wire); }
    if(layer>=36&&layer<=39) 
    { 
      wire = int(wire/2);//combine neighbor two cells
      vtmp.push_back(layer); 
      vtmp.push_back(wire); 
    }
  }

  //remove the same layer, same cell, because of combine in 10 super layer;
  for(std::vector<int>::iterator iter = vtmp.begin(); iter != vtmp.end(); iter += 2) {
    bool ifsamelem = false;
    int layerId = *(iter);
    int cellId = *(iter+1);
    for(std::vector<int>::iterator iter_tmp = iter; iter_tmp != vtmp.end(); iter_tmp += 2) {
      if((iter_tmp+2) != vtmp.end()) {
        int layerId_tmp = *(iter_tmp+2);
        int cellId_tmp = *(iter_tmp+3);
        if((layerId == layerId_tmp) && (cellId == cellId_tmp)) ifsamelem = true;
      }
    }
    if(ifsamelem == false) {
      digiId.push_back(layerId);
      digiId.push_back(cellId);
 //     std::cout<<"layer: "<<layerId<<" "<<"wire: "<<cellId<<std::endl;
    }
  }

}
void MdcTSF::CountTS()
{
//============================reset each array======================================
  // super layer one
  for(int i=0;i<48;i++)
  {
    SL1PivotCellHit[i] = 0;
    SL1TSF[i] = 0;
    for(int j=0;j<8;j++)
    {
      SL1HitCount[i][j] = 0;
    }
  }
  // super layer two
  for(int i=0;i<80;i++)
  {
    SL2PivotCellHit[i] = 0;
    SL2TSF[i] = 0;
    for(int j=0;j<12;j++)
    {
      SL2HitCount[i][j] = 0;
    }
  }
  // super layer three
  for(int i=0;i<88;i++)
  {
    SL3PivotCellHit[i] = 0;
    SL3TSF[i] = 0;
    for(int j=0;j<16;j++)
    {
      SL3HitCount[i][j] = 0;
    }
  }
  // super layer four
  for(int i=0;i<112;i++)
  {
    SL4PivotCellHit[i] = 0;
    SL4TSF[i] = 0; 
    for(int j=0;j<22;j++)
    {
      SL4HitCount[i][j] = 0;
    }
  }    
  // super layer five
  for(int i=0;i<128;i++)
  {
    SL5PivotCellHit[i] = 0;
    SL5TSF[i] = 0;
    for(int j=0;j<31;j++)
    {
      SL5HitCount[i][j] = 0;
    }
  }
  // super layer ten
  for(int i=0;i<128;i++)
  {
    SL10PivotCellHit[i] = 0;
    SL10TSF[i] = 0;
    for(int j=0;j<81;j++)
    {       
      SL10HitCount[i][j] = 0;
    }   
  } 
//=================================== end reset value ===================================

//============ counting the hit number in each super layer for each combination ========
  int layer,cell,superlayer,ln;

  for(std::vector<int>::iterator iter=digiId.begin();iter!=digiId.end();iter+=2)
  {
    layer = *iter;
    cell = *(iter+1);
    if(layer<=19)
    {
      superlayer = (int) layer/4+1;
      ln = layer%4;
      if(superlayer == 1)
      {
        if(ln==2) SL1PivotCellHit[cell] = 1;
        for(unsigned int i = 0;i < SL1.size(); i++)
        {
          for(unsigned int j = 0; j < SL1[i].size()/4; j++) {
            if(SL1[i][3-ln+j*4] == cell+1) SL1HitCount[i][j] += 1;
          }
        }
      } //end super layer1

      if(superlayer == 2)
      {
        if(ln==2) SL2PivotCellHit[cell] = 1;
        for(unsigned int i = 0;i < SL2.size(); i++)
        {
          for(unsigned int j = 0; j < SL2[i].size()/4; j++) {
            if(SL2[i][3-ln+j*4] == cell+1) SL2HitCount[i][j] += 1;
          }
        }
      } //end super layer2

      if(superlayer==3)
      {
	if(ln==2) SL3PivotCellHit[cell] = 1;
        for(unsigned int i = 0;i < SL3.size(); i++)
        {
          for(unsigned int j = 0; j < SL3[i].size()/4; j++) {
            if(SL3[i][3-ln+j*4] == cell+1) SL3HitCount[i][j] += 1;
          }
        }
      } //end super layer3

      if(superlayer==4)
      {
	if(ln==2) SL4PivotCellHit[cell] = 1;
        for(unsigned int i = 0;i < SL4.size(); i++)
        {
          for(unsigned int j = 0; j < SL4[i].size()/4; j++) {
            if(SL4[i][3-ln+j*4] == cell+1) SL4HitCount[i][j] += 1;
          }
        } 
      } //end super layer4

      if(superlayer==5)
      {
	if(ln==1) SL5PivotCellHit[cell] = 1;
        for(unsigned int i = 0;i < SL5.size(); i++)
        {
          for(unsigned int j = 0; j < SL5[i].size()/4; j++) {
            if(SL5[i][3-ln+j*4] == cell+1) SL5HitCount[i][j] += 1;
          }
        } 
      } //end super layer5
    }
    else
    { 
      superlayer = (int) layer/4+1;
      ln = layer%4;
      if(ln==1) SL10PivotCellHit[cell] = 1;
      for(unsigned int i = 0;i < SL10.size(); i++)
      {
        for(unsigned int j = 0; j < SL10[i].size()/4; j++) {
          if(SL10[i][3-ln+j*4] == cell+1) SL10HitCount[i][j] += 1;
        }
      }
    } //end super layer10
  }
}
void MdcTSF::GetTS()
{
  CountTS();
  // find TSF in each super layer
  for(int i=0; i<48; i++)
    for(int j=0; j<8; j++)
    {
      if(SL1HitCount[i][j] >= 3) { SL1TSF[i] = 1; break; } //cout<<"SL1 cell id : "<<i<<endl; break; }
    }

  for(int i=0; i<80; i++)
    for(int j=0; j<12; j++)
    {
      if(SL2HitCount[i][j] >= 3) { SL2TSF[i] = 1; break; } //cout<<"SL2 cell id : "<<i<<endl; break; }
    }

  for(int i = 0; i < 88; i++)
    for(int j = 0; j < 16; j++)
    {
      if(SL3HitCount[i][j] >= 3) { SL3TSF[i] = 1; break; } //cout<<"SL3: "<<i<<endl; break; }
    }

  for(int i = 0; i < 112; i++)
    for(int j = 0; j < 22; j++)
    {
      if(SL4HitCount[i][j] >= 3) { SL4TSF[i] = 1; break; } //cout<<"SL4: "<<i<<endl; break; }
    } 

  for(int i = 0; i < 128; i++)
    for(int j = 0; j < 31; j++)
    {
      if(SL5HitCount[i][j] >= 3) { SL5TSF[i] = 1; break; } //cout<<"SL5: "<<i<<endl; break; }
    }

  for(int i = 0; i < 128; i++)
    for(int j = 0; j < 81; j++)
    {
      if(SL10HitCount[i][j] >= 3) { SL10TSF[i] = 1; break; } //cout<<"SL10 : "<<i<<endl; break; }
    }

  // combine the continuous TSF
  


}

void MdcTSF::ReadLut()
{
  fstream readin;
  int ncell=0,pl=0;
  int a,b,l4,l3,l2,l1;
  SL1.clear();
  SL2.clear();
  SL3.clear();
  SL4.clear();
  SL5.clear();
  SL10.clear();
  for(int n=1;n<6;n++)
  {
    if(n==1) { ncell=48; pl=3; }
    if(n==2) { ncell=80; pl=7; }
    if(n==3) { ncell=88; pl=11; }
    if(n==4) { ncell=112; pl=15; }
    if(n==5) { ncell=128; pl=18; }

    for(int i=1;i<(ncell+1);i++)
    {
      std::vector<int> tmp;
      tmp.clear();
      int num=0;
      TString filename = TString(getenv( "TRIGGERROOT" ));
      filename+="/data/tsf";
      filename+=n;
      filename+="/TSF-SL";
      filename+=n;
      filename+="-L";
      filename+=pl;
      filename+="-C";
      filename+=i;
      filename+=".pat";
      readin.clear();
      readin.open(filename,ios_base::in);
      if(!readin) cerr<<"can not open the file "<<filename<<endl;
      while(readin.good())
      {
        readin>>a>>b>>l4>>l3>>l2>>l1;
        num++;
      }
      readin.close();
      readin.clear();
      readin.open(filename,ios_base::in);
      if(!readin) cerr<<"can not open the file again "<<filename<<endl;
      for(int j=0;j<(num-1);j++)
      {
        readin>>a>>b>>l4>>l3>>l2>>l1;
        tmp.push_back(l4);
        tmp.push_back(l3);
        tmp.push_back(l2);
        tmp.push_back(l1);
      }
      readin.close();
      if(n==1) SL1.push_back(tmp);
      if(n==2) SL2.push_back(tmp);
      if(n==3) SL3.push_back(tmp);
      if(n==4) SL4.push_back(tmp);
      if(n==5) SL5.push_back(tmp); 
    }
  }
  for(int i=1;i<129;i++)
  {
    std::vector<int> tmp1;
    tmp1.clear();
    int num=0;
    //TString filename = "/ihepbatch/bes/caogf/data/tsf/tsf10C/TSF-SL10C-L38-C";
    TString filename = TString(getenv( "TRIGGERROOT" ));
    filename+="/data/tsf10C/TSF-SL10C-L38-C";
    filename+=i;
    filename+=".pat";
    readin.clear();
    readin.open(filename,ios_base::in);
    if(!readin) cerr<<"can not open the file "<<filename<<endl;
    while(readin.good())
    {
      readin>>a>>b>>l4>>l3>>l2>>l1;
      num++;
    }
    readin.close();
    readin.clear();
    readin.open(filename,ios_base::in);
    if(!readin) cerr<<"can not open the file again "<<filename<<endl;

    for(int j=0;j<(num-1);j++)
    {
      readin>>a>>b>>l4>>l3>>l2>>l1;
      tmp1.push_back(l4);
      tmp1.push_back(l3);
      tmp1.push_back(l2);
      tmp1.push_back(l1);
    }
    readin.close();
    SL10.push_back(tmp1);
  }
}
void MdcTSF::Dump()
{
  std::vector<int> combine_size;
  cout<<endl;
  cout<<"     ***************check data file tsf: *****************      "<<endl;
  cout<<"*--------------------------BEGIN-------------------------------*"<<endl;
  cout<<endl;

  cout<<"      -------------------SL1----------------------         "<<endl;
  combine_size.clear();
  for(unsigned int i=0; i<SL1.size(); i++) {
    cout<<"Cell ID --> "<<i<<endl;
    combine_size.push_back(SL1[i].size());
    for(unsigned int j=0; j<SL1[i].size(); j++) {
      cout<<SL1[i][j]<<" ";
      if((j+1)%4==0) cout<<endl;
    }  
    cout<<endl;
  }
  sort(combine_size.begin(),combine_size.end());
  cout<<"The Max Number of Combination is "<<combine_size[combine_size.size()-1]/4<<endl;
  cout<<endl;

  cout<<"      -------------------SL2----------------------         "<<endl;
  combine_size.clear();
  for(unsigned int i=0; i<SL2.size(); i++) {
    cout<<"Cell ID --> "<<i<<endl;
    combine_size.push_back(SL2[i].size());
    for(unsigned int j=0; j<SL2[i].size(); j++) {
      cout<<SL2[i][j]<<" ";
      if((j+1)%4==0) cout<<endl;
    }
    cout<<endl;
  }
  sort(combine_size.begin(),combine_size.end());
  cout<<"The Max Number of Combination is "<<combine_size[combine_size.size()-1]/4<<endl;
  cout<<endl;

  cout<<"      -------------------SL3----------------------         "<<endl;
  combine_size.clear();
  for(unsigned int i=0; i<SL3.size(); i++) {
    cout<<"Cell ID --> "<<i<<endl;
    combine_size.push_back(SL3[i].size());
    for(unsigned int j=0; j<SL3[i].size(); j++) {
      cout<<SL3[i][j]<<" ";
      if((j+1)%4==0) cout<<endl;
    }
    cout<<endl;
  }
  sort(combine_size.begin(),combine_size.end());
  cout<<"The Max Number of Combination is "<<combine_size[combine_size.size()-1]/4<<endl;
  cout<<endl;

  cout<<"      -------------------SL4----------------------         "<<endl;
  combine_size.clear();
  for(unsigned int i=0; i<SL4.size(); i++) {
    cout<<"Cell ID --> "<<i<<endl;
    combine_size.push_back(SL4[i].size());
    for(unsigned int j=0; j<SL4[i].size(); j++) {
      cout<<SL4[i][j]<<" ";
      if((j+1)%4==0) cout<<endl;
    }
    cout<<endl;
  }
  sort(combine_size.begin(),combine_size.end());
  cout<<"The Max Number of Combination is "<<combine_size[combine_size.size()-1]/4<<endl;
  cout<<endl;

  cout<<"      -------------------SL5----------------------         "<<endl;
  combine_size.clear();
  for(unsigned int i=0; i<SL5.size(); i++) {
    cout<<"Cell ID --> "<<i<<endl;
    combine_size.push_back(SL5[i].size());
    for(unsigned int j=0; j<SL5[i].size(); j++) {
      cout<<SL5[i][j]<<" ";
      if((j+1)%4==0) cout<<endl;
    }
    cout<<endl;
  }
  sort(combine_size.begin(),combine_size.end());
  cout<<"The Max Number of Combination is "<<combine_size[combine_size.size()-1]/4<<endl;
  cout<<endl;

  cout<<"      -------------------SL10----------------------         "<<endl;
  combine_size.clear();
  for(unsigned int i=0; i<SL10.size(); i++) {
    cout<<"Cell ID --> "<<i<<endl;
    combine_size.push_back(SL10[i].size());
    for(unsigned int j=0; j<SL10[i].size(); j++) {
      cout<<SL10[i][j]<<" ";
      if((j+1)%4==0) cout<<endl;
    }
    cout<<endl;
  }
  sort(combine_size.begin(),combine_size.end());
  cout<<"The Max Number of Combination is "<<combine_size[combine_size.size()-1]/4<<endl;
  cout<<endl;

  cout<<"*---------------------------END------------------------------*"<<endl;
}
