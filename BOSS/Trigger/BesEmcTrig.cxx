//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     /
//
//---------------------------------------------------------------------------/
//
//Description:
//Author:   Caogf
//Created:  Feb, 2006
//Modified:
//Comment:
//

#include "Trigger/BesEmcTrig.h"
#include "Trigger/EmcESum.h"
#include "Trigger/EmcCCount.h"
#include "Trigger/TrigPara.h"

#include "Trigger/IBesGlobalTrigSvc.h"
#include "Trigger/BesGlobalTrigSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "CLHEP/Random/RandGauss.h"
using namespace CLHEP;
using namespace TrigConf;
using namespace std;

BesEmcTrig::BesEmcTrig()
{
  m_EmcESum = new EmcESum();
  m_EmcCCount = new EmcCCount();
//  cout<<"BesEmcTrig output "<<endl;
}
BesEmcTrig::~BesEmcTrig()
{
  if(m_EmcESum) delete m_EmcESum;
  if(m_EmcCCount) delete m_EmcCCount;
}
void BesEmcTrig::startEmcTrig()
{
	m_EmcESum->getESum();
	m_EmcCCount->getClusterId();
        //reset values
        NClus1 = false;
        NClus2 = false;
        NBClus1 = false;
        NEClus1 = false;

	BClus_Phi = false;
	EClus_Phi = false;
	EClusBB = false;
	BClusBB = false;
	Clus_Z = false;
	BL_BBLK = false;
	
	Etot_L = false;
	Etot_M = false;
        BEtot_H = false;
	EEtot_H = false;
	
	BL_Z = false;
	Diff_E = false;
	Diff_B = false;
        //BL_BEMC = false; //not used
	BL_EBLK = false;
        
        ISvcLocator* svcLocator = Gaudi::svcLocator();
        StatusCode sc = svcLocator->service("BesGlobalTrigSvc", m_tmpSvc);
        m_pIBGT = dynamic_cast<BesGlobalTrigSvc* >(m_tmpSvc);

        //set cluster number trigger conditions including barrel and endcaps
        int nbcluster = 0, necluster = 0;
        nbcluster = m_pIBGT->getEmcNbCluster();
        necluster = m_pIBGT->getEmcNweCluster() + m_pIBGT->getEmcNeeCluster();
        if((nbcluster + necluster)>=1) NClus1 = true;
        if((nbcluster + necluster)>=2) NClus2 = true;
        if(nbcluster >= 1) NBClus1 = true;
        if(necluster >= 1) NEClus1 = true;

        double L1ETOT_L = double (m_pIBGT->getL1ETOT_L());
        double L1ETOT_M = double (m_pIBGT->getL1ETOT_M());
        double L1EDIFF_BR = double (m_pIBGT->getL1EDIFF_BR());
        double L1EDIFF_EC = double (m_pIBGT->getL1EDIFF_EC());
	//middle energy threshold neut_a used
        /*for(int i = 0; ; i++) {
          double theshold = RandGauss::shoot(m_pIBGT->getL1ETOT_M_Peak(),m_pIBGT->getL1ETOT_M_Sigma());
          if(theshold < m_pIBGT->getL1ETOT_M_Peak()) {
            L1ETOT_M = theshold;
            break;
          }
        }
        for(int i = 0; ; i++) {
          double theshold = RandGauss::shoot(m_pIBGT->getL1ETOT_L_Peak(),m_pIBGT->getL1ETOT_L_Sigma());
          if(theshold < m_pIBGT->getL1ETOT_L_Peak()) {
            L1ETOT_L = theshold;
            break;
          }
        }*/
        
        if((m_EmcESum->getTotE() > L1ETOT_M) ) Etot_M = true;
	//low energy threshold(charge channel used)
        if((m_EmcESum->getTotE() > L1ETOT_L) ) Etot_L = true;
        //energy balance check for reserved in barrel 
        if((std::abs(m_EmcESum->getLBTotE() - m_EmcESum->getRBTotE())<L1EDIFF_BR))
        Diff_B = true;
	//energy balance check for babar of endcap	
	if((std::abs(m_EmcESum->getWETotE() - m_EmcESum->getEETotE())<L1EDIFF_EC))
        Diff_E = true;
        //High energy threshold in barrel
        double L1ETOT_BR = double (m_pIBGT->getL1ETOT_BR());
        double L1ETOT_EC = double (m_pIBGT->getL1ETOT_EC());
        /*for(int i = 0; ; i++) {
          double theshold = RandGauss::shoot(m_pIBGT->getL1ETOT_BR_Peak(),m_pIBGT->getL1ETOT_BR_Sigma());
          if( theshold < m_pIBGT->getL1ETOT_BR_Peak()) {
            L1ETOT_BR = theshold;
            break;
          }
        }
        for(int i = 0; ; i++) {
          double theshold = RandGauss::shoot(m_pIBGT->getL1ETOT_EC_Peak(),m_pIBGT->getL1ETOT_EC_Sigma());
          if(theshold < m_pIBGT->getL1ETOT_EC_Peak()) {
            L1ETOT_EC = theshold;
            break;
          }
        }*/
        if((m_EmcESum->getBTotE()>L1ETOT_BR)) BEtot_H = true;
	//High energy threshold(for babar trigger condition) 
	if((m_EmcESum->getETotE()>L1ETOT_EC)) EEtot_H = true;
        //energy balance check for reserved in barrel
        //double L1EBL_BR = m_pIBGT->getL1EBL_BR();
        //if(m_EmcESum->GetLBTotE() > L1EBL_BR && m_EmcESum->GetRBTotE() > L1EBL_BR)
        //BL_BEMC = true;
	//energy balance check for babar of endcap
        double L1EBL_EC = double (m_pIBGT->getL1EBL_EC());
	if((m_EmcESum->getWETotE()>L1EBL_EC && m_EmcESum->getEETotE()>L1EBL_EC))
        BL_EBLK = true;

 	//check balance condition of block energy for babar trigger 
	double energy0 = m_EmcESum->getBBLKE(0);
	double energy1 = m_EmcESum->getBBLKE(1);
	double energy2 = m_EmcESum->getBBLKE(2);
	double energy3 = m_EmcESum->getBBLKE(3);
	double energy4 = m_EmcESum->getBBLKE(4);
	double energy5 = m_EmcESum->getBBLKE(5);
	double energy9 = m_EmcESum->getBBLKE(9);
	double energy10 = m_EmcESum->getBBLKE(10);
	double energy6 = m_EmcESum->getBBLKE(6);
	double energy11 = m_EmcESum->getBBLKE(11);
	double energy7 = m_EmcESum->getBBLKE(7);
	double energy8 = m_EmcESum->getBBLKE(8);
        double L1BLK_GATE = double (m_pIBGT->getL1BLK_GATE());
	if((energy0+energy1)>L1BLK_GATE && (energy9+energy10)>L1BLK_GATE)
	  BL_BBLK = true;
	if((energy1+energy2)>L1BLK_GATE && (energy10+energy11)>L1BLK_GATE)
	  BL_BBLK = true;
	if((energy2+energy3)>L1BLK_GATE && (energy11+energy6)>L1BLK_GATE)
	  BL_BBLK = true;
	if((energy3+energy4)>L1BLK_GATE && (energy6+energy7)>L1BLK_GATE)
	  BL_BBLK = true;
	if((energy4+energy5)>L1BLK_GATE && (energy7+energy8)>L1BLK_GATE)
	  BL_BBLK = true;
	if((energy5+energy0)>L1BLK_GATE && (energy8+energy9)>L1BLK_GATE)
	  BL_BBLK = true;
	//double WEenergy1 = m_EmcESum->getWEBLKE(0);
        //double WEenergy2 = m_EmcESum->getWEBLKE(1);
        //double EEenergy1 = m_EmcESum->getEEBLKE(0);
        //double EEenergy2 = m_EmcESum->getEEBLKE(1);
	//if(((WEenergy1>L1BLK_GATE || WEenergy2>L1BLK_GATE) && (EEenergy1>L1BLK_GATE || EEenergy2>L1BLK_GATE)) ) BL_BBLK = true;

        //check cluster balance in phi of barral
	for(int j =0;j<TrigConf::TCPHINO_B;j++)
	{
	  int bphi = m_EmcCCount->getBClusterPhi(j);
	  if(bphi==0) continue;
	  if(bphi==1)
	  {
	    int NCluster = 0;
	    for(int k =0;k<15;k++)
	    {
	      if((8+k+j)<TrigConf::TCPHINO_B)      
	        NCluster+=m_EmcCCount->getBClusterPhi(8+k+j);
	      else
		NCluster+=m_EmcCCount->getBClusterPhi(8+k+j-TrigConf::TCPHINO_B);
	      if(NCluster>=1)
	      {
	        BClus_Phi = true;
		break;
	       }
	    }
	    if(BClus_Phi == true)
	      break;
	  }
	}
		
        //check cluster balance in phi of endcap.
	for(int j = 0;j<TrigConf::TCPHINO_E/2;j++)
	{
	  int wephi = m_EmcCCount->getWEClusterPhi(j);
	  if(wephi == 0)  continue;
	  if(wephi == 1)
	  {
	    int NCluster = 0;
	    for(int k=0;k<7;k++)
	    {
	      if((j+5+k)<TrigConf::TCPHINO_E/2)
	        NCluster+= m_EmcCCount->getEEClusterPhi(j+5+k);
	      else 
		NCluster+= m_EmcCCount->getEEClusterPhi(j+5+k-TrigConf::TCPHINO_E/2);
	      if(NCluster>=1)
	      {
	        EClus_Phi = true;
		break;
	      }
	    }
	    if(EClus_Phi == true)
	      break;
	  }
	}

        //check barrel cluster balance in theta and phi for babar trigger
	for(int i=0;i<TrigConf::TCTHETANO_B;i++)
	  for(int j=0;j<TrigConf::TCPHINO_B;j++)
	  {
	    int BClusId = m_EmcCCount->getBClusterId(i,j);
	    int NCluster = 0;
	    if(BClusId==0) continue;
	    else
	    {
	      if(i>0&&i<(TrigConf::TCTHETANO_B-1))
	      {
	        for(int k=0;k<5;k++)
	        {
	          if(j+13+k<TrigConf::TCPHINO_B) 
	          {
		    NCluster+= m_EmcCCount->getBClusterId((TrigConf::TCTHETANO_B-1)-i,j+13+k);
		    NCluster+= m_EmcCCount->getBClusterId((TrigConf::TCTHETANO_B-1)-i+1,j+13+k);
		    NCluster+= m_EmcCCount->getBClusterId((TrigConf::TCTHETANO_B-1)-i-1,j+13+k);
		  }
		  else
		  {
		    NCluster+= m_EmcCCount->getBClusterId((TrigConf::TCTHETANO_B-1)-i,j+13+k-TrigConf::TCPHINO_B);
		    NCluster+= m_EmcCCount->getBClusterId((TrigConf::TCTHETANO_B-1)-i+1,j+13+k-TrigConf::TCPHINO_B);
		    NCluster+= m_EmcCCount->getBClusterId((TrigConf::TCTHETANO_B-1)-i-1,j+13+k-TrigConf::TCPHINO_B);
		  }
	        }
		if(NCluster>0) { BClusBB = true; break; }
	      }
	      if(i==0) 
	      {
		for(int k=0;k<5;k++)
                {
                  if(j+13+k<TrigConf::TCPHINO_B)                                      
                  {                                                  
                    NCluster+= m_EmcCCount->getBClusterId((TrigConf::TCTHETANO_B-1),j+13+k);
                    NCluster+= m_EmcCCount->getBClusterId((TrigConf::TCTHETANO_B-2),j+13+k);
                  }                                                  
                  else                                               
                  {                                                  
                    NCluster+= m_EmcCCount->getBClusterId((TrigConf::TCTHETANO_B-2),j+13+k-TrigConf::TCPHINO_B);
                    NCluster+= m_EmcCCount->getBClusterId((TrigConf::TCTHETANO_B-1),j+13+k-TrigConf::TCPHINO_B);
                  }                                                  
                }
                if(NCluster>0) { BClusBB = true; break; }
	      }
	      if(i==(TrigConf::TCTHETANO_B-1)) 
	      {
		for(int k=0;k<5;k++)
                {
                  if(j+13+k<TrigConf::TCPHINO_B)                                      
                  {                                                  
                    NCluster+= m_EmcCCount->getBClusterId(0,j+13+k);
                    NCluster+= m_EmcCCount->getBClusterId(1,j+13+k);
                  }                                                  
                  else                                               
                  {                                                  
                    NCluster+= m_EmcCCount->getBClusterId(0,j+13+k-TrigConf::TCPHINO_B);
                    NCluster+= m_EmcCCount->getBClusterId(1,j+13+k-TrigConf::TCPHINO_B);
                  }                                                  
                }
                if(NCluster>0) { BClusBB = true; break; }
              }
	    }
	  }
        //check endcap cluster balance in phi for babar trigger
        for(int j = 0;j<TrigConf::TCPHINO_E/2;j++)
        {
          int NCluster = 0; 
          int EClusId = m_EmcCCount->getWEClusterPhi(j);
	  
          if(EClusId == 0)  continue;
          else 
          {
	    
            for(int k=0;k<3;k++)
            {
              if((j+7+k)<TrigConf::TCPHINO_E/2)
                NCluster+= m_EmcCCount->getEEClusterPhi(j+7+k);
              else 
                NCluster+= m_EmcCCount->getEEClusterPhi(j+7+k-TrigConf::TCPHINO_E/2);
              if(NCluster>=1)
              {
                EClusBB = true;
                break;
              }
            }
            if(EClusBB == true)
              break;
          }
        }
        //z energy balance (B+E)
        double L1EBL_Z = double (m_pIBGT->getL1EBL_Z());
        /*for(int i = 0; ; i++) {
          double theshold = RandGauss::shoot(m_pIBGT->getL1EBL_Z_Peak(),m_pIBGT->getL1EBL_Z_Sigma());
          if(theshold < m_pIBGT->getL1EBL_Z_Peak()) {
            L1EBL_Z = theshold;
            break;
          }
        }*/
	if((m_EmcESum->getRTotE()>=L1EBL_Z && m_EmcESum->getLTotE()>=L1EBL_Z))
        BL_Z = true;
//	cout<<"RTotE and LTotE are "<<m_EmcESum->GetRTotE()<<" "<<m_EmcESum->GetLTotE()<<endl;
//
	//z cluster balance (if one cluster is found in each side ,return true)
	if(m_EmcCCount->getEClus_Z()&&m_EmcCCount->getWClus_Z())
	Clus_Z = true;

        //set trigger conditions in service

	m_pIBGT->setNClus1(NClus1);
	m_pIBGT->setNClus2(NClus2);
	m_pIBGT->setNBClus1(NBClus1);
	m_pIBGT->setNEClus1(NEClus1);

	m_pIBGT->setBClusBB(BClusBB);
	m_pIBGT->setEClusBB(EClusBB);
	m_pIBGT->setClus_Z(Clus_Z);
	m_pIBGT->setBClus_PHI(BClus_Phi);
	m_pIBGT->setEClus_PHI(EClus_Phi);
	m_pIBGT->setBEtot_H(BEtot_H);
	m_pIBGT->setEEtot_H(EEtot_H);
	m_pIBGT->setEtot_L(Etot_L);
	m_pIBGT->setEtot_M(Etot_M);
	m_pIBGT->setBL_Z(BL_Z);
	m_pIBGT->setDiff_B(Diff_B);
	m_pIBGT->setDiff_E(Diff_E);
	m_pIBGT->setBL_BBLK(BL_BBLK);
	m_pIBGT->setBL_EBLK(BL_EBLK);
}
