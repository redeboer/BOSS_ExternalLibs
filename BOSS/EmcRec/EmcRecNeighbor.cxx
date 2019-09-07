//
//  Define the relationship of crystals
//
//  Wang.Zhe 2004, 3, 29
//
#include <iostream>

#include "EmcRec/EmcRecNeighbor.h"

// Constructors and destructors 
EmcRecNeighbor::EmcRecNeighbor()
{}

EmcRecNeighbor::~EmcRecNeighbor()
{}

// 3*3
bool EmcRecNeighbor::IsNeighbor(const Identifier& id1,
				const Identifier& id2)
{
   bool is;
   RecEmcIDVector NeighborVec;
   ci_RecEmcIDVector ci_NV;
   
   is=false;
   NeighborVec=GetNeighbors(id1);
   for(ci_NV=NeighborVec.begin();
       ci_NV!=NeighborVec.end();
       ++ci_NV) {
      if((*ci_NV)==id2) { is=true; }
   }
   return is;
}

// 5*5
bool EmcRecNeighbor::IsNextNeighbor(const Identifier& id1,
				    const Identifier& id2)
{
   bool is;
   RecEmcIDVector NeighborVec;
   ci_RecEmcIDVector ci_NV;
   
   is=false;
   NeighborVec=GetNextNeighbors(id1);
   for(ci_NV=NeighborVec.begin();
       ci_NV!=NeighborVec.end();
       ++ci_NV) {
      if((*ci_NV)==id2) { is=true; }
   }
   return is;
}

// 3*3 8 ones
RecEmcIDVector EmcRecNeighbor::GetNeighbors(const Identifier& id)
{
  unsigned int barrel_ec=EmcID::barrel_ec(id);
  unsigned int theta=EmcID::theta_module(id);
  unsigned int phi=EmcID::phi_module(id);
   
  RecEmcID aNb;
  RecEmcIDVector NeighborVec;
   
  NeighborVec.clear();
   
  if(barrel_ec==EmcID::getBARREL()) {
    unsigned int phip=999;
    unsigned int phin=999;
    if(phi==EmcID::getPHI_BARREL_MIN()) {
      phip=EmcID::getPHI_BARREL_MAX();
      phin=phi+1;
    }
    if(phi==EmcID::getPHI_BARREL_MAX()) {
      phip=phi-1;
      phin=EmcID::getPHI_BARREL_MIN();
    }
    if((phi>EmcID::getPHI_BARREL_MIN()) &&
        	 (phi<EmcID::getPHI_BARREL_MAX())) {
      phip=phi-1;
      phin=phi+1;
    }
      
    aNb=EmcID::crystal_id(barrel_ec,theta,phip);
    NeighborVec.push_back(aNb);
    aNb=EmcID::crystal_id(barrel_ec,theta,phin);
    NeighborVec.push_back(aNb);
      
    if((theta==EmcID::getTHETA_BARREL_MIN()) || (theta==EmcID::getTHETA_BARREL_MAX())) {
      unsigned int east_west=0;
      if(theta==EmcID::getTHETA_BARREL_MIN()) {   //theta=0
        aNb=EmcID::crystal_id(barrel_ec,theta+1,phip);
        NeighborVec.push_back(aNb);
        aNb=EmcID::crystal_id(barrel_ec,theta+1,phi);
        NeighborVec.push_back(aNb);
        aNb=EmcID::crystal_id(barrel_ec,theta+1,phin);
        NeighborVec.push_back(aNb);
        east_west=EmcID::getENDCAP_EAST();
      } else {                                    //theta=43
        aNb=EmcID::crystal_id(barrel_ec,theta-1,phip);
        NeighborVec.push_back(aNb);
        aNb=EmcID::crystal_id(barrel_ec,theta-1,phi);
        NeighborVec.push_back(aNb);
        aNb=EmcID::crystal_id(barrel_ec,theta-1,phin);
        NeighborVec.push_back(aNb);
        east_west=EmcID::getENDCAP_WEST();
      }
      //neighbors in endcap
      unsigned int phi1=phi*(EmcID::getPHI_ENDCAP_MAX(EmcID::getTHETA_ENDCAP_MAX())+1)
        /(EmcID::getPHI_BARREL_MAX()+1);  //phi1=phi*4/5
      aNb=EmcID::crystal_id(east_west,EmcID::getTHETA_ENDCAP_MAX(),phi1);
      NeighborVec.push_back(aNb);
      if(phi%5==0) {
        if(phi==EmcID::getPHI_BARREL_MIN()) {  //phi=0
          aNb=EmcID::crystal_id(east_west,EmcID::getTHETA_ENDCAP_MAX(),
              EmcID::getPHI_ENDCAP_MAX(EmcID::getTHETA_ENDCAP_MAX()));   //id=(0,5,95)
          NeighborVec.push_back(aNb);
        } else {
          aNb=EmcID::crystal_id(east_west,EmcID::getTHETA_ENDCAP_MAX(),phi1-1);
          NeighborVec.push_back(aNb);
        }
      } else {
        if(phi==EmcID::getPHI_BARREL_MAX()) {  //phi=119
          aNb=EmcID::crystal_id(east_west,EmcID::getTHETA_ENDCAP_MAX(),
              EmcID::getPHI_ENDCAP_MIN());                                //id=(0,5,0)
          NeighborVec.push_back(aNb);
        } else {
          aNb=EmcID::crystal_id(east_west,EmcID::getTHETA_ENDCAP_MAX(),phi1+1);
          NeighborVec.push_back(aNb);
        }
      }
    }

    else if((theta>EmcID::getTHETA_BARREL_MIN()) &&	 (theta<EmcID::getTHETA_BARREL_MAX())) {
      aNb=EmcID::crystal_id(barrel_ec,theta+1,phip);
      NeighborVec.push_back(aNb);
      aNb=EmcID::crystal_id(barrel_ec,theta+1,phi);
      NeighborVec.push_back(aNb);
      aNb=EmcID::crystal_id(barrel_ec,theta+1,phin);
      NeighborVec.push_back(aNb);
      aNb=EmcID::crystal_id(barrel_ec,theta-1,phip);
      NeighborVec.push_back(aNb);
      aNb=EmcID::crystal_id(barrel_ec,theta-1,phi);
      NeighborVec.push_back(aNb);
      aNb=EmcID::crystal_id(barrel_ec,theta-1,phin);
      NeighborVec.push_back(aNb);
    }
  }
  
  else if((barrel_ec==EmcID::getENDCAP_EAST()) ||  //endcap
       (barrel_ec==EmcID::getENDCAP_WEST())) {
    unsigned int phiInSec=(EmcID::getPHI_ENDCAP_MAX(theta)+1)/16;
    unsigned int sector=phi/phiInSec;
    unsigned int phip=999;
    unsigned int phin=999;
    unsigned int InvalidPhi = 999;
    if(phi==EmcID::getPHI_ENDCAP_MIN()) {
      phip=EmcID::getPHI_ENDCAP_MAX(theta);
      phin=phi+1;
    }
    else if(phi==EmcID::getPHI_ENDCAP_MAX(theta)) {
      phip=phi-1;
      phin=EmcID::getPHI_ENDCAP_MIN();
    }
    else if((phi==4*phiInSec-1) || (phi==12*phiInSec-1)){   //crystal next to the gap
      phip=phi-1;
      phin=phi+1; //InvalidPhi;
    }
    else if((phi==4*phiInSec) || (phi==12*phiInSec)) {
      phip=phi-1; //InvalidPhi;
      phin=phi+1;
    }
    else if(((phi>EmcID::getPHI_ENDCAP_MIN())&&(phi<4*phiInSec-1)) || 
        ((phi>4*phiInSec)&&(phi<12*phiInSec-1)) || 
        ((phi>12*phiInSec)&&(phi<EmcID::getPHI_ENDCAP_MAX(theta)))) {
      phip=phi-1;
      phin=phi+1;
    }

    if(phip!=InvalidPhi) {
      aNb=EmcID::crystal_id(barrel_ec,theta,phip);
      NeighborVec.push_back(aNb);
    }
    if(phin!=InvalidPhi) {
      aNb=EmcID::crystal_id(barrel_ec,theta,phin);
      NeighborVec.push_back(aNb);
    }

    if(theta==EmcID::getTHETA_ENDCAP_MIN()) {           //layer 0
      if(phip!=InvalidPhi) {
        aNb=EmcID::crystal_id(barrel_ec,theta+1,phip);
        NeighborVec.push_back(aNb);
      }
      aNb=EmcID::crystal_id(barrel_ec,theta+1,phi);
      NeighborVec.push_back(aNb);
      if(phin!=InvalidPhi) {
        aNb=EmcID::crystal_id(barrel_ec,theta+1,phin);
        NeighborVec.push_back(aNb);
      }
    }
    
    else if(theta==EmcID::getTHETA_ENDCAP_MAX()) {      //layer 5
      if(phip!=InvalidPhi) {
        aNb=EmcID::crystal_id(barrel_ec,theta-1,phip);
        NeighborVec.push_back(aNb);
      }
      aNb=EmcID::crystal_id(barrel_ec,theta-1,phi);
      NeighborVec.push_back(aNb);
      if(phin!=InvalidPhi) {
        aNb=EmcID::crystal_id(barrel_ec,theta-1,phin);
        NeighborVec.push_back(aNb);
      }

      unsigned int phi1=phi*(EmcID::getPHI_BARREL_MAX()+1)/(EmcID::getPHI_ENDCAP_MAX(theta)+1);
      unsigned int min_max=0;
      if(barrel_ec==EmcID::getENDCAP_EAST()){
        min_max=EmcID::getTHETA_BARREL_MIN();
      } else {
        min_max=EmcID::getTHETA_BARREL_MAX();
      }
      aNb=EmcID::crystal_id(EmcID::getBARREL(),min_max,phi1);
      NeighborVec.push_back(aNb);
      aNb=EmcID::crystal_id(EmcID::getBARREL(),min_max,phi1+1);
      NeighborVec.push_back(aNb);                                      
      if(phi%4==0) {
        if(phi==EmcID::getPHI_ENDCAP_MIN()) {
          aNb=EmcID::crystal_id(EmcID::getBARREL(),min_max,EmcID::getPHI_BARREL_MAX());
          NeighborVec.push_back(aNb);
        } else {
          aNb=EmcID::crystal_id(EmcID::getBARREL(),min_max,phi1-1);
          NeighborVec.push_back(aNb);
        }
      } else if(phi%4==3) {
        if(phi==EmcID::getPHI_ENDCAP_MAX(theta)){
          aNb=EmcID::crystal_id(EmcID::getBARREL(),min_max,EmcID::getPHI_BARREL_MIN());
          NeighborVec.push_back(aNb);
        } else {
          aNb=EmcID::crystal_id(EmcID::getBARREL(),min_max,phi1+2);
          NeighborVec.push_back(aNb);
        }
      }
    }
    
    else if((theta==EmcID::getTHETA_ENDCAP_MIN()+1) ||   //layer 1,3
        (theta==EmcID::getTHETA_ENDCAP_MIN()+3)) {
      if(phip!=InvalidPhi) {
        aNb=EmcID::crystal_id(barrel_ec,theta-1,phip);
        NeighborVec.push_back(aNb);
      }
      aNb=EmcID::crystal_id(barrel_ec,theta-1,phi);
      NeighborVec.push_back(aNb);
      if(phin!=InvalidPhi) {
        aNb=EmcID::crystal_id(barrel_ec,theta-1,phin);
        NeighborVec.push_back(aNb);
      }

      unsigned int phi1=phi*(phiInSec+1)/phiInSec;
      aNb=EmcID::crystal_id(barrel_ec,theta+1,phi1);
      NeighborVec.push_back(aNb);
      aNb=EmcID::crystal_id(barrel_ec,theta+1,phi1+1);
      NeighborVec.push_back(aNb);
      if(phi%phiInSec==0) {
        if(sector==0){
          aNb=EmcID::crystal_id(barrel_ec,theta+1,EmcID::getPHI_ENDCAP_MAX(theta+1));
          NeighborVec.push_back(aNb);
        } else {
          aNb=EmcID::crystal_id(barrel_ec,theta+1,phi1-1);
          NeighborVec.push_back(aNb);
        }
      } else if(phi%phiInSec==phiInSec-1) {
        if(sector==15){
          aNb=EmcID::crystal_id(barrel_ec,theta+1,EmcID::getPHI_ENDCAP_MIN());
          NeighborVec.push_back(aNb);
        } else {
          aNb=EmcID::crystal_id(barrel_ec,theta+1,phi1+2);
          NeighborVec.push_back(aNb);
        }
      }
    }
    
    else if((theta==EmcID::getTHETA_ENDCAP_MIN()+2) ||   //layer 2,4
        (theta==EmcID::getTHETA_ENDCAP_MIN()+4)) {
      unsigned int phi1=phi*(phiInSec-1)/phiInSec;
      aNb=EmcID::crystal_id(barrel_ec,theta-1,phi1);
      NeighborVec.push_back(aNb);
      if(phi%phiInSec==0){
        if(sector==0){
          aNb=EmcID::crystal_id(barrel_ec,theta+1,EmcID::getPHI_ENDCAP_MAX(theta-1));
          NeighborVec.push_back(aNb);
        } else {
          aNb=EmcID::crystal_id(barrel_ec,theta-1,phi1-1);
          NeighborVec.push_back(aNb);
        }
      } else {
        if(phi==EmcID::getPHI_ENDCAP_MAX(theta)){
          aNb=EmcID::crystal_id(barrel_ec,theta-1,EmcID::getPHI_ENDCAP_MIN());
          NeighborVec.push_back(aNb);
        } else {
          aNb=EmcID::crystal_id(barrel_ec,theta-1,phi1+1);
          NeighborVec.push_back(aNb);
        }
      }

      if(phip!=InvalidPhi) {
        aNb=EmcID::crystal_id(barrel_ec,theta+1,phip);
        NeighborVec.push_back(aNb);
      }
      aNb=EmcID::crystal_id(barrel_ec,theta+1,phi);
      NeighborVec.push_back(aNb);
      if(phin!=InvalidPhi) {
        aNb=EmcID::crystal_id(barrel_ec,theta+1,phin);
        NeighborVec.push_back(aNb);
      }
    }
  }
   
  return NeighborVec;
}

// 5*5 16 ones
RecEmcIDVector EmcRecNeighbor::GetNextNeighbors(const Identifier& id)
{
   unsigned int barrel_ec=EmcID::barrel_ec(id);
   unsigned int theta=EmcID::theta_module(id);
   unsigned int phi=EmcID::phi_module(id);
   
   RecEmcID aNb;
   RecEmcIDVector NeighborVec;
   
   NeighborVec.clear();

   //if(barrel_ec==EmcID::getBARREL()) {
   if(0) {  //not used now
      unsigned int phip,phipp;
      unsigned int phin,phinn;
      unsigned int thetap,thetapp;
      unsigned int thetan,thetann;      
      unsigned int invalidTheta=999999;
      if(phi==EmcID::getPHI_BARREL_MIN()) {
	 phipp=EmcID::getPHI_BARREL_MAX()-1;
	 phip=EmcID::getPHI_BARREL_MAX();
	 phin=phi+1;
	 phinn=phi+2;
      }
      if(phi==EmcID::getPHI_BARREL_MIN()+1) {
	 phipp=EmcID::getPHI_BARREL_MAX();
	 phip=phi-1;
	 phin=phi+1;
	 phinn=phi+2;
      }
      if(phi==EmcID::getPHI_BARREL_MAX()-1) {
	 phipp=phi-2;
	 phip=phi-1;
	 phin=phi+1;
	 phinn=EmcID::getPHI_BARREL_MIN();
      }
      if(phi==EmcID::getPHI_BARREL_MAX()) {
	 phipp=phi-2;
	 phip=phi-1;
	 phin=EmcID::getPHI_BARREL_MIN();
	 phinn=EmcID::getPHI_BARREL_MIN()+1;
      }
      if((phi>EmcID::getPHI_BARREL_MIN()+1) &&
	 (phi<EmcID::getPHI_BARREL_MAX()-1)) {
	 phipp=phi-2;
	 phip=phi-1;
	 phin=phi+1;
	 phinn=phi+2;
      }

      if(theta==EmcID::getTHETA_BARREL_MIN()) {
	 thetapp=invalidTheta;
	 thetap=invalidTheta;
	 thetan=theta+1;
	 thetann=theta+2;
      }
      if(theta==EmcID::getTHETA_BARREL_MIN()+1) {
	 thetapp=invalidTheta;
	 thetap=theta-1;
	 thetan=theta+1;
	 thetann=theta+2;
      }
      if((theta>EmcID::getTHETA_BARREL_MIN()+1) &&
	 (theta<EmcID::getTHETA_BARREL_MAX()-1)) {
	 thetapp=theta-2;
	 thetap=theta-1;
	 thetan=theta+1;
	 thetann=theta+2;
      }
      if(theta==EmcID::getTHETA_BARREL_MAX()-1) {
	 thetapp=theta-2;
	 thetap=theta-1;
	 thetan=theta+1;
	 thetann=invalidTheta;
      }
      if(theta==EmcID::getTHETA_BARREL_MAX()) {
	 thetapp=theta-2;
	 thetap=theta-1;
	 thetan=invalidTheta;
	 thetann=invalidTheta;
      }

      // Check them out.
      //cout<<phipp<<" "<<phip<<" "<<phi<<" "<<phin<<" "<<phinn<<endl;
      //cout<<thetapp<<" "<<thetap<<" "<<theta<<" "<<thetan<<" "<<thetann<<endl;
      
      aNb=EmcID::crystal_id(barrel_ec,theta,phipp);
      NeighborVec.push_back(aNb);
      aNb=EmcID::crystal_id(barrel_ec,theta,phinn);
      NeighborVec.push_back(aNb);
   
      if(thetapp!=invalidTheta) {
	 aNb=EmcID::crystal_id(barrel_ec,thetapp,phipp);
	 NeighborVec.push_back(aNb);
	 aNb=EmcID::crystal_id(barrel_ec,thetapp,phip);
	 NeighborVec.push_back(aNb);
	 aNb=EmcID::crystal_id(barrel_ec,thetapp,phi);
	 NeighborVec.push_back(aNb);
	 aNb=EmcID::crystal_id(barrel_ec,thetapp,phin);
	 NeighborVec.push_back(aNb);
	 aNb=EmcID::crystal_id(barrel_ec,thetapp,phinn);
	 NeighborVec.push_back(aNb);
      }
      if(thetap!=invalidTheta) {
	 aNb=EmcID::crystal_id(barrel_ec,thetap,phipp);
	 NeighborVec.push_back(aNb);
	 aNb=EmcID::crystal_id(barrel_ec,thetap,phinn);
	 NeighborVec.push_back(aNb);
      }
      if(thetan!=invalidTheta) {
	 aNb=EmcID::crystal_id(barrel_ec,thetan,phipp);
	 NeighborVec.push_back(aNb);
	 aNb=EmcID::crystal_id(barrel_ec,thetan,phinn);
	 NeighborVec.push_back(aNb);
      }
      if(thetann!=invalidTheta) {
	 aNb=EmcID::crystal_id(barrel_ec,thetann,phipp);
	 NeighborVec.push_back(aNb);
	 aNb=EmcID::crystal_id(barrel_ec,thetann,phip);
	 NeighborVec.push_back(aNb);
	 aNb=EmcID::crystal_id(barrel_ec,thetann,phi);
	 NeighborVec.push_back(aNb);
	 aNb=EmcID::crystal_id(barrel_ec,thetann,phin);
	 NeighborVec.push_back(aNb);
	 aNb=EmcID::crystal_id(barrel_ec,thetann,phinn);
	 NeighborVec.push_back(aNb);
      }
   }

   //else if((barrel_ec==EmcID::getENDCAP_EAST()) ||  //endcap
       //(barrel_ec==EmcID::getENDCAP_WEST())) {
   else {
     RecEmcIDVector tmpNeighborVec,tmpNextNeighborVec;
     ci_RecEmcIDVector ci_NV,ci_tmpNV,ci_tmpNV1,ci_tmpNNV;
     tmpNeighborVec=GetNeighbors(id);
     bool flag=false;           //whether NeighborVec already includes this crystal
     bool flagNeighbor=false;   //whether this crystal belongs to NeighborVec
    
     //------------------------------------------------------------------
     for(ci_tmpNV=tmpNeighborVec.begin();
         ci_tmpNV!=tmpNeighborVec.end();
         ci_tmpNV++){
       tmpNextNeighborVec=GetNeighbors(*ci_tmpNV);
       //================================================================
       for(ci_tmpNNV=tmpNextNeighborVec.begin();
           ci_tmpNNV!=tmpNextNeighborVec.end();
           ci_tmpNNV++){
         
         for(ci_NV=NeighborVec.begin();
             ci_NV!=NeighborVec.end();
             ci_NV++){
           if(*ci_tmpNNV==*ci_NV){  //this crystal is already included
             flag=true;
             continue;
           }
         }
         
         if(!flag){       //find a new crystal
           for(ci_tmpNV1=tmpNeighborVec.begin();
               ci_tmpNV1!=tmpNeighborVec.end();
               ci_tmpNV1++){
             if(*ci_tmpNNV==*ci_tmpNV1){  //this crystal belongs to NeighborVec
               flagNeighbor=true;
               continue;
             }
           }

           if(*ci_tmpNNV==id)
             flagNeighbor=true;
           
           if(!flagNeighbor)
             NeighborVec.push_back(*ci_tmpNNV);
           else
             flagNeighbor=false;
         }
         else
           flag=false;
       }
       //================================================================
     }
     //------------------------------------------------------------------
   }

   return NeighborVec;   
}

