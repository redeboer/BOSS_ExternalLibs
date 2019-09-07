//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     /
//// 
////---------------------------------------------------------------------------/
////
////Description: muon trigger research
////Author:   Caogf
////Created:  March, 2007
////Modified:
////Comment:
////
//
#include "Trigger/BesMucTrig.h"
#include "Trigger/MucTrigHit.h"

#include "Trigger/IBesGlobalTrigSvc.h"
#include "Trigger/BesGlobalTrigSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include <iostream>
#include <fstream>

using namespace std;

BesMucTrig::BesMucTrig()
{
  m_MucTrigHit = MucTrigHit::get_Muc();

  readIndex();
}

BesMucTrig::~BesMucTrig()
{
}

void BesMucTrig::startMucTrig()
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("BesGlobalTrigSvc", m_tmpSvc);
  m_pIBGT = dynamic_cast<BesGlobalTrigSvc* >(m_tmpSvc);

  //reset values
  for(int i=0; i<8; i++)
  { 
    nhitSegBR[i] = 0; 
    for(int j=0; j<4; j++)
    {
      nfireLayerBR[i][j] = 0;
      nhitLayerBR[i][j] = 0;
    }
  }
  nhitBR = 0;

  for(int i=0; i<2; i++)
  {
    nhitEC[i] = 0;
    for(int j=0; j<4; j++)
    {
      nhitSegEC[i][j] = 0;
      for(int k=0; k<4; k++)
      {
        nfireLayerEC[i][j][k] = 0;
        nhitLayerEC[i][j][k] = 0;
      }
    }
  }

  vIndex.clear();

  m_MucTrigHit->getFastOr(); //run fastOr algorithm

  //reset values and get fastOr signal from class MucTrigHit
  for(int i=0; i<8; i++)
    for(int j=0; j<4; j++)
      for(int k=0; k<7; k++)
        {
          foBR[i][j][k] = 0;
          foBR[i][j][k] = m_MucTrigHit->getfoBR(i, j, k);
          if(foBR[i][j][k] == 1) {
            nfireLayerBR[i][j] = 1; //fired layer Id of barrel
            nhitLayerBR[i][j]+=1; //Hit number in each layer of barrel 
            nhitSegBR[i]+=1; //hit number in each segment of barrel 
            nhitBR+=1;  //hit number in barrel
            //1000: barrel, i*100: segment, j*10: gap, k: strip
            int index = 1000 + i*100 + j*10 + k;
            vIndex.push_back(index);
          }
        }

  for(int i=0; i<2; i++)
    for(int j=0; j<4; j++)
      for(int k=0; k<4; k++)
        for(int l=0; l<4; l++)
        {
          foEC[i][j][k][l] = 0; 
          foEC[i][j][k][l] = m_MucTrigHit->getfoEC(i, j, k, l);
          if(foEC[i][j][k][l]==1) {
            nfireLayerEC[i][j][k] = 1; //fired layer Id of endcaps
            nhitLayerEC[i][j][k]+=1; //hit number in each layer of endcaps
            nhitSegEC[i][j]+=1; //hit number in each segment of endcaps
            nhitEC[i]+=1; //hit number in endcaps
            int index;
            //0: east endcap, 2000: west endcap, j*100: segment, k*10: gap, l: strip
            if(i==0) index = 0 + j*100 + k*10 + l;
            if(i==1) index = 2000 + j*100 + k*10 +l;
            vIndex.push_back(index);
          }
        }

  setIndex();
  findTrack();
}
void BesMucTrig::findTrack()
{
  //find tracks in barrel
  for(int i=0; i<8; i++)
  {
    trackBR[i] = 0;
    track3lBR[i] = 0;
    if(i!=2) {
      for(unsigned int j=0; j<vindexb.size(); j++)
      {
        int ly1,ly2,ly3,ly4;
        ly1 = vindexb[j][0];
        ly2 = vindexb[j][1]-10;
        ly3 = vindexb[j][2]-20;
        ly4 = vindexb[j][3]-30;
        if((foBR[i][0][ly1]+foBR[i][1][ly2]+foBR[i][2][ly3]+foBR[i][3][ly4])>=3) {
          trackBR[i]++; 
          break; 
        }
        if(foBR[i][0][ly1]==1&&foBR[i][2][ly3]==1) { trackBR[i]++; break; }
        if(foBR[i][1][ly2]==1&&foBR[i][3][ly4]==1) { trackBR[i]++; break; }
      }
     
      for(unsigned int j=0; j<vindexb.size(); j++)
      {
        int ly1,ly2,ly3,ly4;
        ly1 = vindexb[j][0];
        ly2 = vindexb[j][1]-10;
        ly3 = vindexb[j][2]-20;
        ly4 = vindexb[j][3]-30;
        if((foBR[i][0][ly1]+foBR[i][1][ly2]+foBR[i][2][ly3]+foBR[i][3][ly4])>=3) {
          track3lBR[i]++;
          break;
        }
      }
    }
    if(i==2) {
      for(unsigned int j=0; j<vindexb3.size(); j++)
      {
        int ly1,ly2,ly3,ly4;
        ly1 = vindexb3[j][0];
        ly2 = vindexb3[j][1]-10;
        ly3 = vindexb3[j][2]-20;
        ly4 = vindexb3[j][3]-30;
        if((foBR[i][0][ly1]+foBR[i][1][ly2]+foBR[i][2][ly3]+foBR[i][3][ly4])>=3) { 
          trackBR[i]++; 
          break;
        }
        if(foBR[i][0][ly1]==1&&foBR[i][2][ly3]==1) { trackBR[i]++; break; }
        if(foBR[i][1][ly2]==1&&foBR[i][3][ly4]==1) { trackBR[i]++; break; }
      }

      for(unsigned int j=0; j<vindexb3.size(); j++)
      {
        int ly1,ly2,ly3,ly4;
        ly1 = vindexb3[j][0];
        ly2 = vindexb3[j][1]-10;
        ly3 = vindexb3[j][2]-20;
        ly4 = vindexb3[j][3]-30;
        if((foBR[i][0][ly1]+foBR[i][1][ly2]+foBR[i][2][ly3]+foBR[i][3][ly4])>=3) {
          track3lBR[i]++;
          break;
        }
      }
    }
  }
  
  //find tracks in endcaps
  for(int i=0; i<4; i++) 
  {
    trackEE[i] = 0;
    trackWE[i] = 0;
    track3lEE[i] = 0;
    track3lWE[i] = 0; 
    for(unsigned int j=0; j<vindexe.size(); j++)
    {
      int ly1,ly2,ly3,ly4;
      ly1 = vindexe[j][0];
      ly2 = vindexe[j][1]-10;
      ly3 = vindexe[j][2]-20;
      ly4 = vindexe[j][3]-30;
      //for east endcap
      if((foEC[0][i][0][ly1]+foEC[0][i][1][ly2]+foEC[0][i][2][ly3]+foEC[0][i][3][ly4])>=3) { 
        trackEE[i]++; 
        break; 
      }
      if(foEC[0][i][0][ly1]==1&&foEC[0][i][2][ly3]==1) { trackEE[i]++; break; }
      if(foEC[0][i][1][ly2]==1&&foEC[0][i][3][ly4]==1) { trackEE[i]++; break; }
    }

    for(unsigned int j=0; j<vindexe.size(); j++)
    {
      int ly1,ly2,ly3,ly4;
      ly1 = vindexe[j][0];
      ly2 = vindexe[j][1]-10;
      ly3 = vindexe[j][2]-20;
      ly4 = vindexe[j][3]-30;
      //for west endcap
      if((foEC[1][i][0][ly1]+foEC[1][i][1][ly2]+foEC[1][i][2][ly3]+foEC[1][i][3][ly4])>=3) { 
        trackWE[i]++;
        break; 
      }
      if(foEC[1][i][0][ly1]==1&&foEC[1][i][2][ly3]==1) { trackWE[i]++; break; }
      if(foEC[1][i][1][ly2]==1&&foEC[1][i][3][ly4]==1) { trackWE[i]++; break; }
    }

    for(unsigned int j=0; j<vindexe.size(); j++)
    {
      int ly1,ly2,ly3,ly4;
      ly1 = vindexe[j][0];
      ly2 = vindexe[j][1]-10;
      ly3 = vindexe[j][2]-20;
      ly4 = vindexe[j][3]-30;
      //for east endcap
      if((foEC[0][i][0][ly1]+foEC[0][i][1][ly2]+foEC[0][i][2][ly3]+foEC[0][i][3][ly4])>=3) {
        track3lEE[i]++;
        break;
      }
    }
    for(unsigned int j=0; j<vindexe.size(); j++)
    {
      int ly1,ly2,ly3,ly4;
      ly1 = vindexe[j][0];
      ly2 = vindexe[j][1]-10;
      ly3 = vindexe[j][2]-20;
      ly4 = vindexe[j][3]-30;
      //for west endcap
      if((foEC[1][i][0][ly1]+foEC[1][i][1][ly2]+foEC[1][i][2][ly3]+foEC[1][i][3][ly4])>=3) {
        track3lWE[i]++;
        break;
      }
    } 
  }

  //count the tracks in barrel and endcaps
  btrack = 0;
  etrack = 0;
  int b3ltrack = 0, e3ltrack = 0;
  for(int i=0; i<8; i++)  { btrack = btrack + trackBR[i]; b3ltrack = b3ltrack + track3lBR[i]; }
  for(int i=0; i<4; i++)  { etrack = etrack + trackEE[i] + trackWE[i]; e3ltrack = e3ltrack + track3lWE[i] + track3lEE[i]; }

  //cosmic ray experiment, MUC trigger, 3v7 or 4v8; Or use 3v7 only; Note: use 3/4 logic in both cases
  bool mucBB3478 = false, mucBB37 = false;
  if((track3lBR[2] > 0 && track3lBR[6] > 0) || (track3lBR[3] > 0 && track3lBR[7] > 0)) mucBB3478 = true;
  if(track3lBR[2] > 0 && track3lBR[6] > 0) mucBB37 = true;
   
  //set muc trigger conditions
  m_pIBGT->setMucBB3478(mucBB3478);
  m_pIBGT->setMucBB37(mucBB37);

  //set value in service
  m_pIBGT->setMucNtrackBR(btrack);
  m_pIBGT->setMucNtrackEC(etrack);
  m_pIBGT->setMucN3ltrackTotal(b3ltrack+e3ltrack);
}

std::vector<int> BesMucTrig::getNlayerSeg()
{
  vfireLayer.clear();

  for(int i=0; i<8; i++)
  {
    int nlayer = 0;
    for(int j=0; j<4; j++)
    {
      if(nfireLayerBR[i][j] == 1) {
        nlayer++;
      }
    }
    if(nlayer != 0) {
      nlayer = 100 + i*10 + nlayer; //100: part(barrel), i*10: segment
      vfireLayer.push_back(nlayer);
    }
  }
  
  for(int i=0; i<2; i++)
    for(int j=0; j<4; j++)
    {
      int nlayer = 0;
      for(int k=0; k<4; k++)
      {
        if(nfireLayerEC[i][j][k] == 1) {
          nlayer++;
        }
      }
      if(nlayer != 0) {
        if(i==0) nlayer = 0 + j*10 + nlayer; //0: east endcaps, j*10: jth segment
        else nlayer = 200 + j*10 + nlayer; //200: west endcap, j*10: jth segment
        vfireLayer.push_back(nlayer); 
      }
    } 
  return vfireLayer;
}

int BesMucTrig::getNlayerPart(int part)
{
  int nLayerBR = 0, nLayerWE = 0, nLayerEE = 0;
  //count the barrel fired layer number
  for(int i=0; i<8; i++)
    for(int j=0; j<4; j++)
    {
      if(nfireLayerBR[i][j] == 1) {
        nLayerBR++;
      }
    }
   //count each endcap fired layer number
   for(int i=0; i<2; i++)
    for(int j=0; j<4; j++)
      for(int k=0; k<4; k++)
      {
        if(nfireLayerEC[i][j][k] == 1) {
          if(i==0) nLayerEE++;
          if(i==1) nLayerWE++;
        }
      }

  if(part == 0) return nLayerEE;
  if(part == 1) return nLayerBR;
  if(part == 2) return nLayerWE;

  return -1;
}

std::vector<int> BesMucTrig::getNhitLayer()
{
  vhitLayer.clear();

  for(int i=0; i<8; i++)
    for(int j=0; j<4; j++)
    {
      if(nhitLayerBR[i][j] != 0) {
        //1000: barrel, i*100: ith segment, j*10: jth gap, nhitLayerBR[i][j]: the number of hits
        int hit  = 1000 + i*100 + j*10 + nhitLayerBR[i][j]; 
        vhitLayer.push_back(hit);
      }
    }

  for(int i=0; i<2; i++)
    for(int j=0; j<4; j++)
      for(int k=0; k<4; k++)
      {
        if(nhitLayerEC[i][j][k] != 0) {
          int hit;
          //0: east endcaps, j*100: jth segment, k*10: kth gap, nhitLayerEC[i][j][k]: the number of hits
          if(i==0) hit = 0 + j*100 + k*10 + nhitLayerEC[i][j][k]; 
          //2000: west endcap, j*100: jth segment, k*10: kth gap, nhitLayerEC[i][j][k]: the number of hits
          else hit = 2000 + j*100 + k*10 + nhitLayerEC[i][j][k];
          vhitLayer.push_back(hit);
        }
      }
  return vhitLayer;
}

std::vector<int> BesMucTrig::getNhitSeg()
{
  vhitSeg.clear();

  for(int i=0; i<8; i++) 
  {
    if(nhitSegBR[i] != 0) {
      //1000: barrel, i*100: ith segment,tSegBR[i]: the number of hits 
      int hit = 1000 + i*100 + nhitSegBR[i];
      vhitSeg.push_back(hit);
    }
  }

  for(int i=0; i<2; i++)
    for(int j=0; j<4; j++)
    {
      if(nhitSegEC[i][j] != 0) {
        int hit;
        //0: east endcaps, j*100: jth segment, nhitSegEC[i][j]: the number of hits
        if(i==0) hit = 0 + j*100 + nhitSegEC[i][j];
        //2000: west endcap, j*100: jth segment, nhitSegEC[i][j]: the number of hits
        else hit = 2000 + j*100 + nhitSegEC[i][j];
        vhitSeg.push_back(hit);
      }
    }
  return vhitSeg;
}

int BesMucTrig::getNhitPart(int i)
{ 
  if(i == 1) return nhitBR;
  if(i == 0) return nhitEC[0];
  if(i == 2) return nhitEC[1];

  return -1;
}

int BesMucTrig::getNhitTotal()
{
  int total = nhitBR + nhitEC[0] + nhitEC[1];
  return total;
}

void BesMucTrig::setIndex()
{
  m_pIBGT->setMucIndex(vIndex);
}

void BesMucTrig::readIndex()
{
  fstream infile1;
  fstream infile2;
  fstream infile3;

  vindexb.clear();
  vindexb3.clear();
  vindexe.clear();

  std::string filename = std::string(getenv( "TRIGGERROOT" ));
  std::string filename1 = filename + "/data/mufield0.5_1.5GeV_cos0.76_phi456thseg_index_select.txt";
  std::string filename2 = filename + "/data/mufield0.5_1.5GeV_cos0.76_phi234thseg_index_select.txt";
  std::string filename3 = filename + "/data/mufield0.5_1.5GeV_ee2_index_select.txt";
  infile1.open(filename1.c_str(), ios_base::in);
  infile2.open(filename2.c_str(), ios_base::in);
  infile3.open(filename3.c_str(), ios_base::in);

  char line[255];
  char* token;
  std::string str;
 
  if(!infile1) cout<<"open file1 failture"<<endl;
  while(infile1) {
    infile1.getline( line, 255 );
    token = strtok( line, " " );
    int num = 0; 
    std::vector<int> tmp;
    tmp.clear();
    do{
      if(token) {
        str = token;
        token = strtok(NULL," ");
        int Index = atoi(str.c_str());
        if(num != 0) tmp.push_back(Index);
        num++;
      }
      else continue;
    }while (token!=NULL);
    if(tmp.size() == 4) vindexb.push_back(tmp);
  }
  cout<<"total "<<vindexb.size()<<" index in barrel have been read!"<<endl;
  

  if(!infile2) cout<<"open file2 failture"<<endl;
  while(infile2) {
    infile2.getline( line, 255 );
    token = strtok( line, " " );
    int num = 0; 
    std::vector<int> tmp;
    tmp.clear();
    do{
      if(token) {
        str = token;
        token = strtok(NULL," ");
        int Index = atoi(str.c_str());
        if(num != 0) tmp.push_back(Index);
        num++;
      }
      else continue;
    }while (token!=NULL);
    if(tmp.size() == 4) vindexb3.push_back(tmp);
  }
  cout<<"total "<<vindexb3.size()<<" index in barrel 3 have been read!"<<endl; 

  if(!infile3) cout<<"open file3 failture"<<endl;
  while(infile3) {
    infile3.getline( line, 255 );
    token = strtok( line, " " );
    int num = 0; 
    std::vector<int> tmp;
    tmp.clear();
    do{
      if(token) {
        str = token;
        token = strtok(NULL," ");
        int Index = atoi(str.c_str());
        if(num != 0) tmp.push_back(Index);
        num++;
      }
      else continue;
    }while (token!=NULL);
    if(tmp.size() == 4) vindexe.push_back(tmp);
  }
  cout<<"total "<<vindexe.size()<<" index in endcap have been read!"<<endl;
  
  infile1.close();
  infile2.close();
  infile3.close();
}
