// ------------------------------------------
// File: SingleParticleGun/SingleParticleGun.cpp
// Description:
//   Allows the user to "shoot" Monte Carlo particles and store the result
//   in the Transient Store.
// -------------------------------------------------------------

#include "SingleParticleGun/SingleParticleGun.h"
#include "GeneratorModule/GeneratorName.h"

// Framework Related Headers:-
#include "GaudiKernel/MsgStream.h"

// Other classes used by this class:-
#include <math.h>
#include "CLHEP/Units/PhysicalConstants.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "BesKernel/IBesRndmGenSvc.h"

#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef double HepDouble;
#endif
using HepMC::GenVertex;
using HepMC::GenParticle;

// File scope declarations:-

//--------------------------------------------------------------------------
SingleParticleGun::SingleParticleGun(const std::string& name, ISvcLocator* pSvcLocator)
  : GenModule(name,pSvcLocator)
{
//--------------------------------------------------------------------------  
  declareProperty("Mode",m_Emode =  SingleEnergyMode::PtMode);
  // in PtMode user specifies Pt theta and phi
  // in EMode user specifies E, theta and phi 
  // defaults are set so that something is always generated

  declareProperty("Pt",m_requestedPt = 5.0); 
  declareProperty("E",m_requestedE = 5.0); 
  declareProperty("Phi",m_requestedPhi = 0.0);  
  declareProperty("Theta",m_requestedTheta = 3.14/4.0);  

  declareProperty("VertexX",m_requestedX = 0.0); 
  declareProperty("VertexY",m_requestedY = 0.0); 
  declareProperty("VertexZ",m_requestedZ = 0.0); 
  declareProperty("Time",m_requestedT = 0.0); 

  declareProperty("MinPt",m_minPt = 1.); 
  declareProperty("MinE",m_minE = 1.); 
  declareProperty("MinTheta",m_minTheta = 0.); 
  declareProperty("MinPhi",m_minPhi = 0.); 

  declareProperty("MaxPt",m_maxPt = 100.); 
  declareProperty("MaxE",m_maxE = 100.); 
  declareProperty("MaxTheta",m_maxTheta=CLHEP::pi); 
  declareProperty("MaxPhi",m_maxPhi = CLHEP::twopi); 

  declareProperty("SigmaPt",m_sigmaPt = 0.1); 
  declareProperty("SigmaE",m_sigmaE = 0.1); 

  declareProperty("SigmaTheta",m_sigmaTheta= 0.1); 
  declareProperty("SigmaPhi",m_sigmaPhi = 0.1); 

  declareProperty("ModePt",m_PtGenMode = SingleParticleGunGenMode::FixedMode); 
  declareProperty("ModeE",m_EGenMode = SingleParticleGunGenMode::FixedMode); 
  declareProperty("ModeTheta",m_ThetaGenMode=SingleParticleGunGenMode::FixedMode); 

  declareProperty("ModePhi",m_PhiGenMode=SingleParticleGunGenMode::FixedMode);
  // specifies type of distribution

  declareProperty("PdgCode",m_pdgCode=211);
}

//--------------------------------------------------------------------------
 SingleParticleGun::~SingleParticleGun(){
//--------------------------------------------------------------------------
//   if(m_pFlatGenerator)  delete m_pFlatGenerator;
//   if(m_pGaussGenerator) delete m_pGaussGenerator;
}

//---------------------------------------------------------------------------
StatusCode SingleParticleGun::genInitialize() {
//---------------------------------------------------------------------------

 // Create the flat and gaussian generators
 //

 MsgStream log(messageService(), name());

 static const bool CREATEIFNOTTHERE(true);
 StatusCode RndmStatus = service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);
 if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)
 {
   log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
   return RndmStatus;
 }	

 // Get the mass of the particle to be generated
 //
 const HepPDT::ParticleData* particle = m_particleTable->particle(HepPDT::ParticleID(abs(m_pdgCode)));
 m_mass = particle->mass().value();

 // 
 // Make sure the parameters are in a sensible range...
 //
 switch (m_Emode){
 case SingleEnergyMode::PtMode:
 if(!m_PtGenMode && (m_minPt > m_requestedPt || m_maxPt < m_requestedPt)
      || m_maxPt < m_minPt ) {
   log << MSG:: ERROR << " Pt min and max out of range.  \n" <<
       "     Will set Pt mode to Fixed!!!" << endreq;
   m_PtGenMode = SingleParticleGunGenMode::FixedMode;
 }
 if(!m_ThetaGenMode && (m_minTheta > m_requestedTheta || m_maxTheta < m_requestedTheta) 
      || m_maxTheta < m_minTheta ) {
   log << MSG:: ERROR << " Theta min and max out of range. \n" <<
       "     Will set Theta mode to Fixed!!!" << endreq;
   m_ThetaGenMode = SingleParticleGunGenMode::FixedMode;
 }
 if(!m_PhiGenMode && (m_minPhi > m_requestedPhi || m_maxPhi < m_requestedPhi)
      || m_maxPhi < m_minPhi ) {
   log << MSG:: ERROR << " Phi min and max out of range.  \n" <<
       "     Will set Phi mode to Fixed!!!" << endreq;
   m_PhiGenMode = SingleParticleGunGenMode::FixedMode;
 }
 break;
 case SingleEnergyMode::EMode:
 if(!m_EGenMode && (m_minE > m_requestedE || m_maxE < m_requestedE)
      || m_maxE < m_minE ) {
   log << MSG:: ERROR << " E min and max out of range.  \n" <<
       "     Will set E mode to Fixed!!!" << endreq;
   m_EGenMode = SingleParticleGunGenMode::FixedMode;
 }
 if(!m_ThetaGenMode && (m_minTheta > m_requestedTheta || m_maxTheta < m_requestedTheta) 
      || m_maxTheta < m_minTheta ) {
   log << MSG:: ERROR << " Theta min and max out of range. \n" <<
       "     Will set Theta mode to Fixed!!!" << endreq;
   m_ThetaGenMode = SingleParticleGunGenMode::FixedMode;
 }
 if(!m_PhiGenMode && (m_minPhi > m_requestedPhi || m_maxPhi < m_requestedPhi)
      || m_maxPhi < m_minPhi ) {
   log << MSG:: ERROR << " Phi min and max out of range.  \n" <<
       "     Will set Phi mode to Fixed!!!" << endreq;
   m_PhiGenMode = SingleParticleGunGenMode::FixedMode;
 }break;
 }
  m_events = 0;
 return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode SingleParticleGun::callGenerator() {
//---------------------------------------------------------------------------

  ++m_events;
  // Save the random number seeds in the event
  CLHEP::HepRandomEngine*	engine	=	p_BesRndmGenSvc->GetEngine("SINGLE");
  const long*		s	=	engine->getSeeds();
  m_seeds.clear();
  m_seeds.push_back(s[0]);
  m_seeds.push_back(s[1]);

 // Generate values for pt, theta and phi
 //
  double pt,phi,theta,E,px,py,pz,p;
  switch(m_Emode){

  case SingleEnergyMode::PtMode:
 pt = generateValue(m_PtGenMode,m_requestedPt, m_sigmaPt, 
                           m_minPt, m_maxPt);
 theta = generateValue(m_ThetaGenMode,m_requestedTheta, m_sigmaTheta, 
                           m_minTheta, m_maxTheta);
 phi = generateValue(m_PhiGenMode,m_requestedPhi, m_sigmaPhi, 
                           m_minPhi, m_maxPhi);

 // Transform to x,y,z coordinates
 //
 px = pt*cos(phi);
 py = pt*sin(phi);
 pz = pt/tan(theta);
 m_fourMom.setVectM(CLHEP::Hep3Vector(px,py,pz),m_mass);
 m_fourPos.set(m_requestedX,m_requestedY,m_requestedZ,m_requestedT);
 return StatusCode::SUCCESS;

 case SingleEnergyMode::EMode:
 E = generateValue(m_EGenMode,m_requestedE, m_sigmaE, 
                           m_minE, m_maxE);
 theta = generateValue(m_ThetaGenMode,m_requestedTheta, m_sigmaTheta, 
                           m_minTheta, m_maxTheta);
 phi = generateValue(m_PhiGenMode,m_requestedPhi, m_sigmaPhi, 
                           m_minPhi, m_maxPhi);

 // Transform to x,y,z coordinates
 //
 if(E*E-m_mass*m_mass < 0.){
 MsgStream log(messageService(), name());
 log << MSG::ERROR  << "You have Generated a Tachyon!! Increase energy or change particle ID" << endreq;   
return StatusCode::FAILURE;
 }
 p=sqrt(E*E-m_mass*m_mass);
 px = p*sin(theta)*cos(phi);
 py = p*sin(theta)*sin(phi);
 pz = p*cos(theta);

 m_fourMom.setVectM(CLHEP::Hep3Vector(px,py,pz),m_mass);
 m_fourPos.set(m_requestedX,m_requestedY,m_requestedZ,m_requestedT);
 return StatusCode::SUCCESS;
}  
 return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode SingleParticleGun::genFinalize() {
//---------------------------------------------------------------------------
 return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode SingleParticleGun::fillEvt(GenEvent* evt) {
//---------------------------------------------------------------------------
  // Note:  The vertex is owned by the event so the event is responsible
  // for deleting its memory
  evt->set_event_number(m_events); // Set the event number
  GenVertex* v1 = new GenVertex(m_fourPos);

  std::cout << " SingleParticleGun::fillEvt --> " <<std::endl;
  std::cout << " Event number:: " << m_events << std::endl; 
  std::cout << " vertex.x = " << m_fourPos.x() 
            << " vertex.y = " << m_fourPos.y() 
            << " vertex.z = " << m_fourPos.z() 
            << " vertex.t = " << m_fourPos.t() <<  std::endl;

  std::cout << " momentum.x = " << m_fourMom.x() 
            << " momentum.y = " << m_fourMom.y() 
            << " momentum.z = " << m_fourMom.z() 
            << " momentum.t = " << m_fourMom.t() <<  std::endl;
 
  evt->add_vertex( v1 );

 
  v1->add_particle_out( new GenParticle( m_fourMom, m_pdgCode,1) );

  std::cout << " particles_size = " << evt->particles_size() 
	    << " vertices_size = " << evt->vertices_size()
	    << std::endl;

  evt->set_signal_process_id(SINGLE);
  evt->set_random_states(m_seeds);
 
 return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
double SingleParticleGun::generateValue(int mode, double val, double sigma,
                 double min, double max) {
//---------------------------------------------------------------------------
  double tmp;
  int i = 0;
  CLHEP::HepRandomEngine* engine = p_BesRndmGenSvc->GetEngine("SINGLE");
  const int maxtries = 100;
  switch (mode) {
  case SingleParticleGunGenMode::FixedMode:
    return val;
  case SingleParticleGunGenMode::GaussMode:
    tmp = max+1.0;
    i = 0;
    do{
//        tmp = m_pGaussGenerator->fire((HepDouble) val,(HepDouble) sigma);
       tmp = CLHEP::RandGauss::shoot(engine, (HepDouble) val,(HepDouble) sigma);
       i++;
    } while ( (tmp<min) || (tmp > max) && (i < maxtries));
    if(i>maxtries) {
       MsgStream log(messageService(), name());
       log << MSG::ERROR << "Cant generate value in range (min, max) "
 	   << val << "\t" << min << "\t" << max << endreq;
    }
    return tmp;
  case SingleParticleGunGenMode::FlatMode:
//     tmp = m_pFlatGenerator->fire((HepDouble) min,(HepDouble) max);
    tmp = CLHEP::RandFlat::shoot(engine, (HepDouble) min,(HepDouble) max);
    return tmp;
  default:
    MsgStream log(messageService(), name());
    log << MSG::ERROR  << "Unknown Generation Mode" << endreq;
    return 0.;
  }
}
