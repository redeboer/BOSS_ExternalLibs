//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// ====================================================================
//
//   G4HepMCInterface.cc
//   $Id: G4HepMCInterface.cpp,v 1.21 2012/05/08 05:38:49 dengzy Exp $
//
// ====================================================================

#ifndef WIN32         // Temporarly disabled on Windows, until CLHEP
                      // will support the HepMC module

#include "G4Svc/G4HepMCInterface.h"

#include "globals.hh"
#include "G4LorentzVector.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "Randomize.hh"

#include "G4Svc/IG4Svc.h"
#include "G4Svc/G4Svc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"
////////////////////////////////////
G4HepMCInterface::G4HepMCInterface()
  : hepmcEvent(0)
////////////////////////////////////
{

  IRealizationSvc *tmpReal;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("RealizationSvc",tmpReal);
  if (!status.isSuccess())
  {
     std::cout << " Could not initialize Realization Service in G4HepMCInterface" << std::endl;
  } else {
    m_RealizationSvc=dynamic_cast<RealizationSvc*>(tmpReal);
  }

}

/////////////////////////////////////
G4HepMCInterface::~G4HepMCInterface()
/////////////////////////////////////
{
  delete hepmcEvent;
}
void G4HepMCInterface::Print(const HepMC::GenEvent* hepmcevt)
{
  G4int i=0;
  for(HepMC::GenEvent::vertex_const_iterator vitr= hepmcevt->vertices_begin();
      vitr != hepmcevt->vertices_end(); ++vitr ) { // loop for vertex ...
    G4cout<<G4endl<<"vertex:"<<i<<"  barcode:"<<(*vitr)->barcode()<<" "; 
    i++;
    //if((*vitr)->mother())
    //  G4cout<<"mother particle: "<<(*vitr)->mother()-> pdg_id()<<"  ";
    //if((*vitr)->secondMother())
    //  G4cout<<" second mother: "<<(*vitr)->secondMother()-> pdg_id()<<G4endl;
    G4LorentzVector xvtx;
    xvtx.setX( (*vitr)-> position().x());
    xvtx.setY( (*vitr)-> position().y());
    xvtx.setZ( (*vitr)-> position().z());
    xvtx.setT( (*vitr)-> position().t());    
    G4cout<<"x:"<<xvtx.x()<<" y:"<<xvtx.y()<<" z:"<<xvtx.z()
          <<" t:"<<xvtx.t()*mm/c_light<<G4endl;
    
    G4int j=0;
    for (HepMC::GenVertex::particle_iterator 
	   pitr= (*vitr)->particles_begin(HepMC::children);
	 pitr != (*vitr)->particles_end(HepMC::children); ++pitr) {
      G4int pdgcode= (*pitr)-> pdg_id();
      G4LorentzVector p;
      p.setX( (*pitr)-> momentum().x());
      p.setY( (*pitr)-> momentum().y());
      p.setZ( (*pitr)-> momentum().z());
      p.setT( (*pitr)-> momentum().t());
      G4cout<<"particle:"<<j<<" pdgcode:"<<pdgcode<<" "; 
      G4cout<<"p:"<<p.x()<<" "<<p.y()<<" "<<p.z()<<" ";
      if((*pitr)->end_vertex()) 
        G4cout<<"endVtx:"<<(*pitr)->end_vertex()->barcode()<<" ";
      G4cout<<"status:"<<(*pitr)->status()<<G4endl;
      j++;
    }
  }
}

void G4HepMCInterface::PrintHPlist()
{
  G4cout<<G4endl;
  G4cout<<"particles sent to Geant4: "<<G4endl;
  for( size_t ii=0; ii<HPlist.size(); ii++ )
  {  
    G4int pdgcode = HPlist[ii]->GetTheParticle()->GetPDGcode(); 
    G4cout<<pdgcode<<" ";
  }
  G4cout<<G4endl;
}

G4int G4HepMCInterface::CheckType(const HepMC::GenEvent* hepmcevt)
{
  G4int flag =0;
  for(HepMC::GenEvent::vertex_const_iterator vitr= hepmcevt->vertices_begin();
          vitr != hepmcevt->vertices_end(); ++vitr ) { // loop for vertex ...
    for (HepMC::GenVertex::particle_iterator
             pitr= (*vitr)->particles_begin(HepMC::children);
                pitr != (*vitr)->particles_end(HepMC::children); ++pitr) {
      if((*pitr)->end_vertex())
        {flag = 1; break;}
    }
    if(flag) break;
  }
  if(m_logLevel <= 4)
  {
    if(flag == 0)
      G4cout<<G4endl<<"generator is GENBES!"<<G4endl;
    else
      G4cout<<G4endl<<"generator is not GENBES!"<<G4endl;
  }
  return flag;
}

  //Panmh 2006.12
////////////////////////////////////////////////////////////////
void G4HepMCInterface::Boost(HepMC::GenEvent* hepmcevt)
////////////////////////////////////////////////////////////////
{
  //suppose relavtive vectoy is v(vx,vy,vz),position are (x,y,z,t)in CMS and(X,Y,Z,T)in Lab,the formulae for boost vertex position from Cms to Lab in natural units are following:
       // v2 = vx*vx + vy*vy + vz*vz;  
       // gamma = 1.0 / sqrt(1.0 - v2);
       // bp = vx*x + vy*y + vz*z;
       // X=x + gamma2*bp*vx + gamma*vx*t;
       // Y=y + gamma2*bp*vy + gamma*vy*t;
       // Z=z + gamma2*bp*vz + gamma*vz*t;
       // T=gamma*(t + bp);
  // the function of these formulae is the same as xvtx.boost(ThreeVector v)      
  //For the E_cms,need to loop and splice the momentum of all the final state particles
  G4LorentzVector ptot=0;
  double E_cms,p_Mag,e_Mass2,M_cms,theta;
  for(HepMC::GenEvent::vertex_const_iterator vitr= hepmcevt->vertices_begin();
      vitr != hepmcevt->vertices_end(); ++vitr ) { // loop for vertex ...
    
    for(HepMC::GenVertex::particle_iterator
	  vpitr = (*vitr)->particles_begin(HepMC::children);
	vpitr != (*vitr)->particles_end(HepMC::children); ++vpitr) {
      
      if((*vpitr)->status() !=1)continue;// Not final state particle
      
      G4LorentzVector p;
      p.setX( (*vpitr)-> momentum().x());
      p.setY( (*vpitr)-> momentum().y());
      p.setZ( (*vpitr)-> momentum().z());
      p.setT( (*vpitr)-> momentum().t());
      ptot = p + ptot; 
    }
  }
  E_cms=ptot.e()*GeV;
  
  //get G4Svc, allow user to access the beam momentum shift in JobOptions
    ISvcLocator* svcLocator = Gaudi::svcLocator();
  IG4Svc* tmpSvc;
  G4Svc* m_G4Svc;
  StatusCode sc=svcLocator->service("G4Svc", tmpSvc);
  m_G4Svc=dynamic_cast<G4Svc *>(tmpSvc);
  
 
  G4ThreeVector v=0;   //velocity of cms as seen from lab
  //for cms velocity 
  theta=11*mrad;     
  //theta=(m_G4Svc->GetBeamAngle())*mrad;
  //theta is half of the angle between the two beams,namely,is the angle between the beam and Z axis.
  M_cms=E_cms;  //for p_cms=0 in the CMS
  e_Mass2=electron_mass_c2*electron_mass_c2; //square of electron mass 
  p_Mag=sqrt((E_cms*E_cms-4*e_Mass2)/(2*(1-cos(pi-2*theta))));
  G4ThreeVector p_Positron(p_Mag*sin(theta),0,p_Mag*cos(theta));
  G4ThreeVector p_Electron(p_Mag*sin(pi-theta),0,p_Mag*cos(pi-theta));
  G4ThreeVector vee=p_Positron+p_Electron;
  G4ThreeVector beamshift(m_G4Svc->GetBeamShiftPx(),m_G4Svc->GetBeamShiftPy(),m_G4Svc->GetBeamShiftPz());
  if(m_G4Svc-> GetSetBeamShift()==true && fabs(M_cms-3686)<50.0) {vee = beamshift;} 

    double pee=vee.r(); 
    M_cms = sqrt(M_cms*M_cms + pee*pee);
    v=vee/M_cms;

  
  for(HepMC::GenEvent::vertex_const_iterator vitr= hepmcevt->vertices_begin();
      vitr != hepmcevt->vertices_end(); ++vitr ) { // loop for vertex ...
    
    // Boost vertex position from cms to lab 
    G4LorentzVector xvtx;
    xvtx.setX( (*vitr)-> position().x());
    xvtx.setY( (*vitr)-> position().y());
    xvtx.setZ( (*vitr)-> position().z());
    xvtx.setT( (*vitr)-> position().t());
    xvtx.boost(v);
    (*vitr)->set_position(xvtx);
    
    // Boost the particles four momentum from cms to lab
    // the transform formulae are similary to the position boost       
    for (HepMC::GenVertex::particle_iterator 
	   vpitr = (*vitr)->particles_begin(HepMC::children);
	 vpitr != (*vitr)->particles_end(HepMC::children); ++vpitr) {
      G4LorentzVector p;
      p.setX( (*vpitr)-> momentum().x());
      p.setY( (*vpitr)-> momentum().y());
      p.setZ( (*vpitr)-> momentum().z());
      p.setT( (*vpitr)-> momentum().t());
      p=p.boost(v);
      (*vpitr)->set_momentum(p);
    }
  }
}

////////////////////////////////////////////////////////////////
void G4HepMCInterface::HepMC2G4(HepMC::GenEvent* hepmcevt, 
				G4Event* g4event)
////////////////////////////////////////////////////////////////
{
  //Print HepMC Event before boost
  if(m_logLevel <=4 )
    Print(hepmcevt);

  //get G4Svc
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IG4Svc* tmpSvc;
  G4Svc* m_G4Svc;
  StatusCode sc=svcLocator->service("G4Svc", tmpSvc);
  m_G4Svc=dynamic_cast<G4Svc *>(tmpSvc); 

  //considering 22rmad beam clossing angle ,need to boost the Cms to Lab 
  // need to boost ?
  if(m_G4Svc->GetBoostLab()== true) 
    Boost(hepmcevt); 
  
  //Print HepMC Event after boost
  if(m_logLevel <=2  && m_G4Svc->GetBoostLab()== true)
    Print(hepmcevt);

  //*********************send particles to HPlist*************************//
  G4int flag = CheckType(hepmcevt);
  if(flag)
  {
    for(HepMC::GenEvent::vertex_const_iterator vitr= hepmcevt->vertices_begin();
        vitr != hepmcevt->vertices_end(); ++vitr ) 
    { // loop for vertex ...
      G4int vtxFlag=0;
      G4int pOut = (*vitr)->particles_out_size();
      HepMC::GenVertex::particle_iterator pitr = (*vitr)->particles_begin(HepMC::children);
      G4int pdgcode= (*pitr)-> pdg_id();
      if(pOut == 1 && abs(pdgcode) == 11)
        vtxFlag=1;
      G4int barcodeVtx = (*vitr)->barcode();

      for (HepMC::GenVertex::particle_iterator pitr= (*vitr)->particles_begin(HepMC::children);
           pitr != (*vitr)->particles_end(HepMC::children); ++pitr) 
      {// loop for particle from this vertex 
        G4int pdgcode = (*pitr)->pdg_id();
        if(vtxFlag==0)
        {
          if(pdgcode==-22) pdgcode=22;
          G4LorentzVector p;
          p.setX( (*pitr)-> momentum().x());
          p.setY( (*pitr)-> momentum().y());
          p.setZ( (*pitr)-> momentum().z());    
          p.setT( (*pitr)-> momentum().t());
          G4PrimaryParticle* particle = new G4PrimaryParticle(pdgcode, p.x()*GeV, p.y()*GeV, p.z()*GeV);
          //G4PrimaryParticle* particle = new G4PrimaryParticle(pdgcode);
          //particle->Set4Momentum(p.x()*GeV,p.y()*GeV,p.z()*GeV,p.t()*GeV);
          G4int ISTHEP = (*pitr)->status();
  
          G4int barcodeEndVtx = 0;
          if((*pitr)->end_vertex())
            barcodeEndVtx = (*pitr)->end_vertex()->barcode();
  
          G4HepMCParticle* hepmcParticle = new G4HepMCParticle(particle,ISTHEP,barcodeEndVtx);
          HPlist.push_back(hepmcParticle);
          
          for( size_t ii=0; ii<HPlist.size(); ii++ )
          {  
            if(barcodeVtx == HPlist[ii]->GetBarcodeEndVtx())
            {
              HPlist[ii]->GetTheParticle()->SetDaughter(particle); 
              hepmcParticle->Done();
              break; 
            }
          }
        }
      }
    }
  }
  else 
  {
    for(HepMC::GenEvent::vertex_const_iterator vitr= hepmcevt->vertices_begin();
                vitr != hepmcevt->vertices_end(); ++vitr )
    {
      for (HepMC::GenVertex::particle_iterator pitr= (*vitr)->particles_begin(HepMC::children);
                     pitr != (*vitr)->particles_end(HepMC::children); ++pitr)
      {
        G4int ISTHEP = (*pitr)->status();
        G4LorentzVector p;
        p.setX( (*pitr)-> momentum().x());
        p.setY( (*pitr)-> momentum().y());
        p.setZ( (*pitr)-> momentum().z());
        p.setT( (*pitr)-> momentum().t());

        G4int pdgcode = (*pitr)->pdg_id();
        G4int barcodeEndVtx = 0;
        if((*pitr)->end_vertex())
          barcodeEndVtx = (*pitr)->end_vertex()->barcode();
        G4PrimaryParticle* particle = new G4PrimaryParticle(pdgcode, p.x()*GeV, p.y()*GeV, p.z()*GeV);
        //G4PrimaryParticle* particle = new G4PrimaryParticle(pdgcode);
        //particle->Set4Momentum(p.x()*GeV,p.y()*GeV,p.z()*GeV,p.t()*GeV);

        G4HepMCParticle* hepmcParticle = new G4HepMCParticle(particle,ISTHEP,barcodeEndVtx);
        HPlist.push_back(hepmcParticle);
        if(ISTHEP>1)
          hepmcParticle->Done();
      }
    }
  }

  //print particles in HPlist
  if(m_logLevel<=4)
    PrintHPlist();

  //get time and position information from G4Svc
  G4double pmPosX,pmPosY,pmPosZ,pmTime;
  G4double tmpPosX,tmpPosY,tmpPosZ,tmpT;
  G4double beamPosX,beamPosY,beamPosZ,beamSizeX,beamSizeY,beamSizeZ;

  if(m_RealizationSvc->UseDBFlag() == false) {
    beamPosX = m_G4Svc->GetBeamPosX();
    beamPosY = m_G4Svc->GetBeamPosY();
    beamPosZ = m_G4Svc->GetBeamPosZ();

    beamSizeX = m_G4Svc->GetBeamSizeX();
    beamSizeY = m_G4Svc->GetBeamSizeY();
    beamSizeZ = m_G4Svc->GetBeamSizeZ()/sqrt(2);
  } 
  if(m_RealizationSvc->UseDBFlag() == true && m_RealizationSvc->ifReadBunch() == true) {
    beamPosX = m_RealizationSvc->getBunchPosX();
    beamPosY = m_RealizationSvc->getBunchPosY();
    beamPosZ = m_RealizationSvc->getBunchPosZ();

    beamSizeX = m_RealizationSvc->getBunchSizeX();
    beamSizeY = m_RealizationSvc->getBunchSizeY();
    beamSizeZ = m_RealizationSvc->getBunchSizeZ();
  }
  if(m_RealizationSvc->UseDBFlag() == true && m_RealizationSvc->ifReadBunch() == false) {
    beamPosX = m_G4Svc->GetBeamPosX();
    beamPosY = m_G4Svc->GetBeamPosY();
    beamPosZ = m_G4Svc->GetBeamPosZ();

    beamSizeX = m_G4Svc->GetBeamSizeX();
    beamSizeY = m_G4Svc->GetBeamSizeY();
    beamSizeZ = m_G4Svc->GetBeamSizeZ()/sqrt(2);
  }

  G4double gaussX = G4RandGauss::shoot();
  G4double gaussY = G4RandGauss::shoot();
  G4double gaussZ = G4RandGauss::shoot();
  G4double gaussT = G4RandGauss::shoot();

  G4double beamStartTime = m_G4Svc->GetBeamStartTime() * ns;
  G4double beamDeltaTime = m_G4Svc->GetBeamDeltaTime() * ns;
  G4double nBunch = m_G4Svc->GetNBunch();
  G4double bunchTimeSigma = m_G4Svc->GetBunchTimeSigma() * ns;
  
  G4double ran=G4UniformRand();
  G4double beamTime = bunchTimeSigma*G4RandGauss::shoot() + beamStartTime + beamDeltaTime*int(ran*nBunch); 

  tmpPosX = (beamPosX + beamSizeX*gaussX ) *mm;
  tmpPosY = (beamPosY + beamSizeY*gaussY ) *mm;
  tmpPosZ = (beamPosZ + beamSizeZ*gaussZ ) *mm;
  tmpT = (beamSizeZ * gaussT ) * mm/c_light +beamTime;

  G4LorentzVector tmpv(tmpPosX,tmpPosY,tmpPosZ,tmpT*c_light/mm);

  HepMC::GenEvent::vertex_const_iterator vitr0= hepmcevt->vertices_begin();
  G4LorentzVector xvtx0 ;
  xvtx0.setX( (*vitr0)-> position().x());
  xvtx0.setY( (*vitr0)-> position().y());
  xvtx0.setZ( (*vitr0)-> position().z());
  xvtx0.setT( (*vitr0)-> position().t());
  pmPosX = xvtx0.x()*mm + tmpPosX;
  pmPosY = xvtx0.y()*mm + tmpPosY;
  pmPosZ = xvtx0.z()*mm + tmpPosZ;
  pmTime = xvtx0.t()*mm/c_light + tmpT;

  if(m_logLevel<=4)
  {
    G4cout<<G4endl;
    G4cout<<xvtx0.x()<<" "<<xvtx0.y()<<" "<<xvtx0.z()<<" "
          <<beamSizeX*gaussX<<" "
          <<beamSizeY*gaussY<<" "
          <<beamSizeZ*gaussZ<<" "<<G4endl;
    G4cout<<xvtx0.t()* mm/c_light<<" "
          <<beamSizeZ * gaussT/sqrt(2)*mm/c_light<<" "
          <<beamTime<<G4endl;
  }
  for(HepMC::GenEvent::vertex_const_iterator vitr= hepmcevt->vertices_begin();
              vitr != hepmcevt->vertices_end(); ++vitr )
  {
    G4LorentzVector xvtx;
    xvtx.setX((*vitr)-> position().x());
    xvtx.setY((*vitr)-> position().y());
    xvtx.setZ((*vitr)-> position().z());
    xvtx.setT((*vitr)-> position().t());
    (*vitr)->set_position(xvtx+tmpv);
  }
  m_G4Svc->SetBeamTime(pmTime);

  G4PrimaryVertex* g4vtx= new G4PrimaryVertex(pmPosX,pmPosY,pmPosZ,pmTime);

  // put initial particles to the vertex
  for( size_t ii=0; ii<HPlist.size(); ii++ )
  {
    if( HPlist[ii]->GetISTHEP() > 0 ) // ISTHEP of daughters had been 
                                      // set to negative
    {
      G4PrimaryParticle* initialParticle = HPlist[ii]->GetTheParticle();
      g4vtx->SetPrimary( initialParticle );
    }
  }
  
  //clear G4HepMCInterface
  for(size_t iii=0;iii<HPlist.size();iii++)
  { delete HPlist[iii]; }
  HPlist.clear();

  g4event->AddPrimaryVertex(g4vtx);
} 


///////////////////////////////////////////////////////
HepMC::GenEvent* G4HepMCInterface::GenerateHepMCEvent()
///////////////////////////////////////////////////////
{
  HepMC::GenEvent* aevent= new HepMC::GenEvent();
  return aevent;
}

//////////////////////////////////////////////////////////////
void G4HepMCInterface::GeneratePrimaryVertex(G4Event* anEvent)
//////////////////////////////////////////////////////////////
{
  // delete previous event object
  delete hepmcEvent;

  // generate next event
  hepmcEvent= GenerateHepMCEvent();
  if(! hepmcEvent) {
    G4Exception("G4HepMCInterface","GeneratePrimaryVertex",RunMustBeAborted,
              "HepMCInterface: no generated particles. run terminated..." );
    return;
  }
  HepMC2G4(hepmcEvent, anEvent);
}

#endif
