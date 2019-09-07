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
#include "Trigger/EmcCCount.h"
#include <iostream>

#include "Trigger/IBesGlobalTrigSvc.h"
#include "Trigger/BesGlobalTrigSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "CLHEP/Random/RandGauss.h"

#include <map>
#include <vector>
#include <complex>

using namespace std;

EmcCCount::EmcCCount()
{
  m_EmcTCFinder = EmcTCFinder::get_Emc();
}
EmcCCount::~EmcCCount()
{
}
void EmcCCount::getClusterId()
{
  //reset values
  EClus_Z = false;
  WClus_Z = false;
  //barrel
  for(int j=0;j<TrigConf::TCPHINO_B;j++)
  {
    BClusterPhi[j] = 0;
    for(int i=0;i<TrigConf::TCTHETANO_B;i++)
    {
      BClusterId[i][j] = 0;
    }
  }
  //endcaps
  for(int j =0;j<TrigConf::TCPHINO_E/2;j++)
  {
    if(j < TrigConf::TCPHINO_E/2) {
      WEClusterPhi[j] = 0;
      EEClusterPhi[j] = 0;
    }
    for(int i =0;i<TrigConf::TCTHETANO_E;i++)
    {
      EEClusterId[i][j] = 0;
      WEClusterId[i][j] = 0;
    }
  }

  std::vector<double> emcClusE;
  emcClusE.clear();

  for(int i=0;i<TrigConf::TCTHETANO_B;i++)
    for(int j=0;j<TrigConf::TCPHINO_B;j++)
    {
      ///Find cluster position in barral
      if(findCluster(1,i,j))
      {
        BClusterId[i][j] = 1;
	//check cluster z balance for charge track
        int block_phi = int (j/5);
	if(((block_phi%2 == 0) && i > 5) || ((block_phi%2 == 1) && i > 4)) WClus_Z = true;
	else EClus_Z = true;
        emcClusE.push_back(m_EmcTCFinder->getBTCEnergy(i,j));
      }
    }
  //Find cluster position in endcap
  for(int i =0;i<TrigConf::TCTHETANO_E;i++)
    for(int j =0;j<TrigConf::TCPHINO_E/2;j++)
    {
      if(findCluster(0,i,j)) { EEClusterId[i][j] = 1; EClus_Z = true; emcClusE.push_back(m_EmcTCFinder->getEETCEnergy(i,j)); }
      if(findCluster(2,i,j)) { WEClusterId[i][j] = 1; WClus_Z = true; emcClusE.push_back(m_EmcTCFinder->getWETCEnergy(i,j)); }
    }
  //Find phi position of cluster in barrel
  for(int j = 0;j<TrigConf::TCPHINO_B;j++)
    for(int i =0;i<TrigConf::TCTHETANO_B;i++)
    {
      if(BClusterId[i][j]==1)
      {
	BClusterPhi[j] = 1;
	break;
      }
    }
  //Find phi position of cluster in west endcap 
  for(int j =0;j<TrigConf::TCPHINO_E/2;j++)
    for(int i =0;i<TrigConf::TCTHETANO_E;i++)
    {
      if(WEClusterId[i][j] == 1)
      {
	WEClusterPhi[j] = 1;
      }
    }
  //Find phi position of cluster in east endcap
  for(int j =0;j<TrigConf::TCPHINO_E/2;j++)
    for(int i =0;i<TrigConf::TCTHETANO_E;i++)
    {
      if(EEClusterId[i][j] == 1)
      {
	EEClusterPhi[j] = 1;
      }
    }
 // for(int i=0;i<TrigConf::TCPHINO_E/2;i++) cout<<"ClusterId output "<<EEClusterPhi[i]<<" "<<WEClusterPhi[i]<<endl;
    
    //set number of cluster in barrel and endcaps for service
    int nbCluster = 0, neeCluster = 0, nweCluster = 0;
    
    map<int,vector<complex<int> >, greater<int> > mClusId;
    std::vector<complex<int> > vClusIdBR;
    std::vector<complex<int> > vClusIdEE;
    std::vector<complex<int> > vClusIdWE;
    typedef pair<int, vector<complex<int> > > vpair;
    vClusIdBR.clear();
    vClusIdEE.clear();
    vClusIdWE.clear();
    mClusId.clear();

    for(int i=0;i<TrigConf::TCTHETANO_B;i++)
      for(int j=0;j<TrigConf::TCPHINO_B;j++)
      {
        int BClusId = BClusterId[i][j];
        if(BClusId == 1) {
          nbCluster++;
          complex<int> clusId(i,j);
          vClusIdBR.push_back(clusId);
        }
      }
    for(int i=0; i<TrigConf::TCTHETANO_E; i++)
      for(int j=0; j<TrigConf::TCPHINO_E/2; j++)
      {
        if(EEClusterPhi[j]) {
          neeCluster++;
          complex<int> clusId(i,j);
          vClusIdEE.push_back(clusId); 
        }
        if(WEClusterPhi[j]) {
          nweCluster++;
          complex<int> clusId(i,j);
          vClusIdWE.push_back(clusId);
        }
      }

     mClusId.insert(vpair(1,vClusIdBR));
     mClusId.insert(vpair(0,vClusIdEE));
     mClusId.insert(vpair(2,vClusIdWE));
    
     ISvcLocator* svcLocator = Gaudi::svcLocator();
     StatusCode sc = svcLocator->service("BesGlobalTrigSvc", m_tmpSvc);
     m_pIBGT = dynamic_cast<BesGlobalTrigSvc* >(m_tmpSvc);

     m_pIBGT->setEmcNbCluster(nbCluster);
     m_pIBGT->setEmcNweCluster(nweCluster);
     m_pIBGT->setEmcNeeCluster(neeCluster); 
     m_pIBGT->setEmcClusId(mClusId);  
     m_pIBGT->setEmcClusE(emcClusE);  
}

bool EmcCCount::findCluster(int partId,int TCThetaNb,int TCPhiNb)
{
  //double L1TC_THRESH = m_pIBGT->getL1TC_THRESH();
  /*for(int i = 0; ; i++) {
    double theshold = RandGauss::shoot(m_pIBGT->getL1TC_THRESH(),3.3);
    if(theshold < m_pIBGT->getL1TC_THRESH()) {
      L1TC_THRESH = theshold;
      break;
    } 
  }*/
  
  //double energy0,energy1,energy2,energy3,energy4,energy5,energy6,energy7,energy8;
  int clus0,clus1,clus3,clus4,clus8;
 /* if(partId==1)
  {
// Cluster Finder Logic
//              Phi--->
//              |---|----|----|
//		| 5 |  1 | 6  |
//              |---|----|----|
//              | 2 |  0 | 3  |   
//              |---|----|----|   ^
//              | 7 |  4 | 8  |   |
//              |---|----|----| Theta
     energy0 = m_EmcTCFinder->getBTCEnergy(TCThetaNb,TCPhiNb);
     if(energy0>L1TC_THRESH)
     {
       //trigger cell index from 0, not 1, so TrigConf::TCTHETANO_B - 1 or TrigConf::TCPHINO_B - 1 needed 
       if(TCThetaNb!=(TrigConf::TCTHETANO_B-1)) energy1 = m_EmcTCFinder->getBTCEnergy(TCThetaNb+1,TCPhiNb);
       else energy1 = 0.0;
			
       if(TCPhiNb!=0) energy2 = m_EmcTCFinder->getBTCEnergy(TCThetaNb,TCPhiNb-1);
       else energy2 = m_EmcTCFinder->getBTCEnergy(TCThetaNb,TrigConf::TCPHINO_B-1);
			
       if(TCPhiNb!=(TrigConf::TCPHINO_B-1)) energy3 = m_EmcTCFinder->getBTCEnergy(TCThetaNb,TCPhiNb+1);
       else energy3 = m_EmcTCFinder->getBTCEnergy(TCThetaNb,0);
			
       if(TCThetaNb!=0) energy4 = m_EmcTCFinder->getBTCEnergy(TCThetaNb-1,TCPhiNb);
       else energy4 = 0.0;
			
       if(TCThetaNb!=(TrigConf::TCTHETANO_B-1)&&TCPhiNb!=0) energy5 = m_EmcTCFinder->getBTCEnergy(TCThetaNb+1,TCPhiNb-1);
       if(TCThetaNb==(TrigConf::TCTHETANO_B-1)) energy5 = 0.0;
       if(TCThetaNb!=(TrigConf::TCTHETANO_B-1)&&TCPhiNb==0) energy5 = m_EmcTCFinder->getBTCEnergy(TCThetaNb+1,TrigConf::TCPHINO_B-1);
			
       if(TCThetaNb!=(TrigConf::TCTHETANO_B-1)&&TCPhiNb!=(TrigConf::TCPHINO_B-1)) energy6 = m_EmcTCFinder->getBTCEnergy(TCThetaNb+1,TCPhiNb+1);
       if(TCThetaNb==(TrigConf::TCTHETANO_B-1)) energy6 = 0.0;
       if(TCThetaNb!=(TrigConf::TCTHETANO_B-1)&&TCPhiNb==(TrigConf::TCPHINO_B-1)) energy6 = m_EmcTCFinder->getBTCEnergy(TCThetaNb+1,0);
 
       if(TCThetaNb!=0&&TCPhiNb!=0) energy7 = m_EmcTCFinder->getBTCEnergy(TCThetaNb-1,TCPhiNb-1);
       if(TCThetaNb==0) energy7=0;
       if(TCThetaNb!=0&&TCPhiNb==0) energy7 = m_EmcTCFinder->getBTCEnergy(TCThetaNb-1,TrigConf::TCPHINO_B-1);
 
       if(TCThetaNb!=0&&TCPhiNb!=(TrigConf::TCPHINO_B-1)) energy8 = m_EmcTCFinder->getBTCEnergy(TCThetaNb-1,TCPhiNb+1);
       if(TCThetaNb==0) energy8 = 0.0;
       if(TCThetaNb!=0&&TCPhiNb==(TrigConf::TCPHINO_B-1)) energy8 = m_EmcTCFinder->getBTCEnergy(TCThetaNb-1,0);
 
       if((energy3>=L1TC_THRESH)||(energy1>=L1TC_THRESH)||((energy4>=L1TC_THRESH)&&(energy8>=L1TC_THRESH)))
       {	
         return false;
       }
       else return true;
    }
    else return false;
  }*/
  if(partId==1)
  {
// Cluster Finder Logic
//              <---Theta
//              |---|----|----|
//              | 5 |  1 | 6  |
//              |---|----|----|
//              | 2 |  0 | 3  |  Phi 
//              |---|----|----|   |
//              | 7 |  4 | 8  |   |
//              |---|----|----|   ^
     clus0 = m_EmcTCFinder->getBTC(TCThetaNb,TCPhiNb);
     if(clus0 > 0)
     {
       //trigger cell index from 0, not 1, so TrigConf::TCTHETANO_B - 1 or TrigConf::TCPHINO_B - 1 needed 
       if(TCPhiNb != 0) clus1 = m_EmcTCFinder->getBTC(TCThetaNb,TCPhiNb - 1);
       else clus1 = m_EmcTCFinder->getBTC(TCThetaNb,TrigConf::TCPHINO_B-1);

//       if(TCPhiNb!=0) clus2 = m_EmcTCFinder->getBTC(TCThetaNb,TCPhiNb-1);
//       else clus2 = m_EmcTCFinder->getBTC(TCThetaNb,TrigConf::TCPHINO_B-1);

       if(TCThetaNb!=0) clus3 = m_EmcTCFinder->getBTC(TCThetaNb - 1,TCPhiNb);
       else clus3 = 0;

       if(TCPhiNb!=(TrigConf::TCPHINO_B-1)) clus4 = m_EmcTCFinder->getBTC(TCThetaNb,TCPhiNb+1);
       else clus4 = m_EmcTCFinder->getBTC(TCThetaNb,0);

//       if(TCThetaNb!=(TrigConf::TCTHETANO_B-1)&&TCPhiNb!=0) clus5 = m_EmcTCFinder->getBTC(TCThetaNb+1,TCPhiNb-1);
//       if(TCThetaNb==(TrigConf::TCTHETANO_B-1)) clus5 = 0;
//       if(TCThetaNb!=(TrigConf::TCTHETANO_B-1)&&TCPhiNb==0) clus5 = m_EmcTCFinder->getBTC(TCThetaNb+1,TrigConf::TCPHINO_B-1);

//       if(TCThetaNb!=(TrigConf::TCTHETANO_B-1)&&TCPhiNb!=(TrigConf::TCPHINO_B-1)) clus6 = m_EmcTCFinder->getBTC(TCThetaNb+1,TCPhiNb+1);
//       if(TCThetaNb==(TrigConf::TCTHETANO_B-1)) clus6 = 0;
//       if(TCThetaNb!=(TrigConf::TCTHETANO_B-1)&&TCPhiNb==(TrigConf::TCPHINO_B-1)) clus6 = m_EmcTCFinder->getBTC(TCThetaNb+1,0);

//       if(TCThetaNb!=0&&TCPhiNb!=0) clus7 = m_EmcTCFinder->getBTC(TCThetaNb-1,TCPhiNb-1);
//       if(TCThetaNb==0) clus7 = 0;
//       if(TCThetaNb!=0&&TCPhiNb==0) clus7 = m_EmcTCFinder->getBTC(TCThetaNb-1,TrigConf::TCPHINO_B-1);

       if(TCThetaNb!=0&&TCPhiNb!=(TrigConf::TCPHINO_B-1)) clus8 = m_EmcTCFinder->getBTC(TCThetaNb-1,TCPhiNb+1);
       if(TCThetaNb==0) clus8 = 1;
       if(TCThetaNb!=0&&TCPhiNb==(TrigConf::TCPHINO_B-1)) clus8 = m_EmcTCFinder->getBTC(TCThetaNb-1,0);

       //if((clus3 > 0)||(clus1 > 0)||((clus4 > 0)&&(clus8 > 0)))
       if(!((clus3 > 0)||(clus1 > 0)) && !((clus4 > 0)&&(clus8 > 0)))
       {
         return true;
       }
       else return false;
    }
    else return false;
  }
  //------------------------------------------------------------------------------------
/* Cluster Finder Logic (1)
             <---phi
      |---|
      | 1 |      
      |---|---| /|\
      | 0 | 2 |  |
      |---|---| theta
  if(partId==0)
  {
    if(TCThetaNb==0)
    {
      energy0 = m_EmcTCFinder->GetEETCEnergy(TCThetaNb,TCPhiNb);
      energy1 = m_EmcTCFinder->GetEETCEnergy(TCThetaNb+1,TCPhiNb);
      if(TCPhiNb!=0) energy2 =m_EmcTCFinder->GetEETCEnergy(TCThetaNb,TCPhiNb-1);
      else energy2 = m_EmcTCFinder->GetEETCEnergy(TCThetaNb,TrigConf::TCPHINO_E-1);
      if((energy0>=L1TC_THRESH)&&(energy1<L1TC_THRESH)&&(energy2<L1TC_THRESH)) return true;
      else return false;
    }
    if(TCThetaNb==(TrigConf::TCTHETANO_E-1))
    {
      energy0 = m_EmcTCFinder->GetEETCEnergy(TCThetaNb,TCPhiNb);
      if(TCPhiNb!=0) energy1 = m_EmcTCFinder->GetEETCEnergy(TCThetaNb,TCPhiNb-1);
      else energy1 = m_EmcTCFinder->GetEETCEnergy(TCThetaNb,TrigConf::TCPHINO_E-1);
      if((energy0>=L1TC_THRESH)&&(energy1<L1TC_THRESH)) return true;
      else return false;
    }
  }
  if(partId==2)
  {
    if(TCThetaNb==0)
    {
      energy0 = m_EmcTCFinder->GetWETCEnergy(TCThetaNb,TCPhiNb);
      energy1 = m_EmcTCFinder->GetWETCEnergy(TCThetaNb+1,TCPhiNb);
      if(TCPhiNb!=0) energy2 = m_EmcTCFinder->GetWETCEnergy(TCThetaNb,TCPhiNb-1);
      else energy2 = m_EmcTCFinder->GetWETCEnergy(TCThetaNb,TrigConf::TCPHINO_E-1);
      if((energy0>=L1TC_THRESH)&&(energy1<L1TC_THRESH)&&(energy2<L1TC_THRESH)) return true;
      else return false;
    }
    if(TCThetaNb==(TrigConf::TCTHETANO_E-1))
    {
      energy0 = m_EmcTCFinder->GetWETCEnergy(TCThetaNb,TCPhiNb);
      if(TCPhiNb!=0) energy1 = m_EmcTCFinder->GetWETCEnergy(TCThetaNb,TCPhiNb-1);
      else energy1 = m_EmcTCFinder->GetWETCEnergy(TCThetaNb,TrigConf::TCPHINO_E-1);
      if((energy0>=L1TC_THRESH)&&(energy1<L1TC_THRESH)) return true;
      else return false;
    }
  }
*/
  //------------------------------------------------------------------------------------
/*Cluster Finder Logic (1) 
             <---phi
            
      |---|---| /|\
      | 0 | 1 |  |
      |---|---| theta
  //if(partId==0)
  //{
  //  energy0 = m_EmcTCFinder->getEETCEnergy(TCThetaNb,TCPhiNb);
  //  if(TCPhiNb!=0) {
  //    energy1 = m_EmcTCFinder->getEETCEnergy(TCThetaNb,TCPhiNb-1);
  //  }
  //  else {
  //    energy1 = m_EmcTCFinder->getEETCEnergy(TCThetaNb,(TrigConf::TCPHINO_E-1));
  //  }
  //  if((energy0>=L1TC_THRESH)&&(energy1<L1TC_THRESH)) return true;
  //  else return false;
  //}

  if(partId==0)
  {
    clus0 = m_EmcTCFinder->getEETC(TCThetaNb,TCPhiNb);
    if(TCPhiNb!=0) {
      clus1 = m_EmcTCFinder->getEETC(TCThetaNb,TCPhiNb-1);
    }
    else {
      clus1 = m_EmcTCFinder->getEETC(TCThetaNb,(TrigConf::TCPHINO_E-1));
    }
    if((clus0 > 0)&&(clus1 == 0)) return true;
    else return false;
  }

  //if(partId==2)
  //{
  //  energy0 = m_EmcTCFinder->getWETCEnergy(TCThetaNb,TCPhiNb);
  //  if(TCPhiNb!=0) {
  //    energy1 = m_EmcTCFinder->getWETCEnergy(TCThetaNb,TCPhiNb-1);
  //  }
  //  else {
  //    energy1 = m_EmcTCFinder->getWETCEnergy(TCThetaNb,(TrigConf::TCPHINO_E-1));
  //  }
  //  if((energy0>=L1TC_THRESH)&&(energy1<L1TC_THRESH)) return true;
  //  else return false;
  //}

  if(partId==2)
  {
    clus0 = m_EmcTCFinder->getWETC(TCThetaNb,TCPhiNb);
    if(TCPhiNb!=0) {
      clus1 = m_EmcTCFinder->getWETC(TCThetaNb,TCPhiNb-1);
    }
    else {
      clus1 = m_EmcTCFinder->getWETC(TCThetaNb,(TrigConf::TCPHINO_E-1));
    }
    if((clus0 > 0)&&(clus1 == 0)) return true;
    else return false;
  }
*/

/*Cluster Finder Logic (2) 
             <---phi
            
      |---|---| /|\
      | 0 | 1 |  |
      |---|---| theta
*/ 
  if(partId==0)
  {
    int etc1,etc2,etc3,etc4;
    etc1 = m_EmcTCFinder->getEETC(TCThetaNb,TCPhiNb*2);
    etc2 = m_EmcTCFinder->getEETC(TCThetaNb,TCPhiNb*2+1);
    if(etc1 > 0 || etc2 > 0) clus0 = 1;
    else clus0 = 0;

    if(TCPhiNb!=0) {
      etc3 = m_EmcTCFinder->getEETC(TCThetaNb,(TCPhiNb-1)*2);
      etc4 = m_EmcTCFinder->getEETC(TCThetaNb,(TCPhiNb-1)*2+1);
      if(etc3 > 0 || etc4 > 0) clus1 = 1;
      else clus1 = 0;
    }
    else {
      etc3 = m_EmcTCFinder->getEETC(TCThetaNb,(TrigConf::TCPHINO_E/2-1)*2);
      etc4 = m_EmcTCFinder->getEETC(TCThetaNb,(TrigConf::TCPHINO_E/2-1)*2+1);
      if(etc3 > 0 || etc4 > 0) clus1 = 1;
      else clus1 = 0;
    }

    if((clus0 > 0)&&(clus1 == 0)) return true;
    else return false;
  }

/*Cluster Finder Logic (2) 
             <---phi
            
      |---|---| /|\
      | 1 | 0 |  |
      |---|---| theta
*/
  if(partId==2)
  {
    int etc1,etc2,etc3,etc4;
    etc1 = m_EmcTCFinder->getWETC(TCThetaNb,TCPhiNb*2);
    etc2 = m_EmcTCFinder->getWETC(TCThetaNb,TCPhiNb*2+1);
    if(etc1 > 0 || etc2 > 0) clus0 = 1;
    else clus0 = 0;

    if(TCPhiNb!= (TrigConf::TCPHINO_E/2-1)) {
      etc3 = m_EmcTCFinder->getWETC(TCThetaNb,(TCPhiNb+1)*2);
      etc4 = m_EmcTCFinder->getWETC(TCThetaNb,(TCPhiNb+1)*2+1);
      if(etc3 > 0 || etc4 > 0) clus1 = 1;
      else clus1 = 0;
    }
    else {
      etc3 = m_EmcTCFinder->getWETC(TCThetaNb,0);
      etc4 = m_EmcTCFinder->getWETC(TCThetaNb,1);
      if(etc3 > 0 || etc4 > 0) clus1 = 1;
      else clus1 = 0;
    }

    if((clus0 > 0)&&(clus1 == 0)) return true;
    else return false;
  }

  return false;
}
