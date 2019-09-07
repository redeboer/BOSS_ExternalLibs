// -------------------------------------------------------------
// File: CosmicGenerator/CosmicGenerator.cxx
// Description:
//
// Non-simulation package CosmicGenerator
//
// A Generator providing cosmicmuon flux at ground level (X-Z plane) 
//
// Generator based on fits to measured surface fluxes, used by ALEPH
//
// Ported From Atlas to BES
//                   by Beijiang Liu <liubj@mail.ihep.ac.cn>
//
// Modification to generate on a plane in the center of detector, then project back to Geant 4 world
//                      Beijiang Liu 18-May-2008
//
// Modification for cleanup
//                      Beijiang Liu 28-Nov-2007
// /
// Modification for  increasing efficiency of muon hitting the detector:
//            m_sphereOpt, Point to a sphere around the interaction point
//                      Beijiang Liu 28-Nov-2007
//
// Modification to project the muons to the Geant4 world
//                      Beijiang Liu 18-Nov-2007
//
// 
//
//
// -------------------------------------------------------------
// File: CosmicGenerator/CosmicGenerator.cxx
// Description:

//    The output will be stored in the transient event store so it can be
//    passed to the simulation.
// 
// AuthorList:
//         W. Seligman: Initial Code 08-Nov-2002,
//         based on work by M. Shapiro and I. Hinchliffe
//

// Modification for increasing efficiency of muon hitting the detector:
//                     H. Ma.    March 17, 2006 
//   Property: ExzCut: 	
//	if true, the method exzCut(...) will be called to apply a 
//               energy dependent position cut on the surface.
//               This rejects low energy muons at large distance. 
//   Property: RMax
//               Used by exzCut to reject non-projective muons, which are 
//               too far out on the surface


// Modifications to accomodate Pixel EndCap C Cosmic Test needs
//      Marian Zdrazil   June 7, 2006   mzdrazil@lbl.gov
//
// Modifications to accomodate replacement of Pixel EndCap C by a Pixel EndCap A
//      Marian Zdrazil   November 24, 2006  mzdrazil@lbl.gov
//
//
// Description:
// ------------
// It is easier and actually more useful to leave the EndCap A
// in the vertical position (the way it is positioned in the ATLAS detector)
// instead of rotating it clockwise by 90deg which corresponds to the
// placement during the Pixel EndCap A cosmic test in SR1 in November 2006.
// This is why we will generate cosmic muons coming from the positive Z-axis 
// direction better than rotating the whole setup in PixelGeoModel.
//
// Modifications July 3rd 2007, Rob McPherson
//     - Fix mu+/mu- bug (always present in Athena versions)
//     - Fix sign of Py (since tag CosmicGenerator-00-00-21, muons only upward-going) 

#include "CosmicGenerator/CosmicGenerator.h"
#include "CosmicGenerator/CosmicGun.h"
#include "CosmicGenerator/CosmicEventParser.h"

// Framework Related Headers:-
#include "GaudiKernel/MsgStream.h"
///#include "StoreGate/StoreGateSvc.h"

// Other classes used by this class:-
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Normal3D.h"
#include "CLHEP/Geometry/Point3D.h"
// #include "HepPDT/ParticleDataTable.hh"

////#include "CLHEP/HepPDT/ParticleData.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "HepMC/Polarization.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"


#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "GeneratorObject/McGenEvent.h"

//#include "TrackRecord/TimedTrackRecordCollection.h"
///#include "TrackRecord/TrackRecordCollection.h"
/// For the Athena-based random numbers.
///#include "CLHEP/Random/RandFlat.h"
// For BES random numnber service
#include "BesKernel/IBesRndmGenSvc.h" 

#include "CLHEP/Random/Ranlux64Engine.h"
#include "CLHEP/Random/RandFlat.h" 

#include <limits.h>

#include <cmath>
#include <vector>
#include <string>
#include <fstream>
using  namespace CLHEP;

static inline int sign(double x) { return (x>0 ? 1: -1);}

// definition of PI
float PI = 3.1415927; // <== Should use M_PI (defined in standard cmath header)
double mass =0.1055658;

//// Pointer On AtRndmGenSvc
//IAtRndmGenSvc* 	CosmicGenerator::p_AtRndmGenSvc = 0;
IBesRndmGenSvc* 	CosmicGenerator::p_BesRndmGenSvc = 0;

extern "C" float cosmicrndm_(int* /*dummy*/)
{
  //  HepRandomEngine* engine = CosmicGenerator::p_AtRndmGenSvc->GetEngine("COSMICS");
  HepRandomEngine* engine = CosmicGenerator::p_BesRndmGenSvc->GetEngine("PYTHIA");
  //  std::cout<<"seed: "<<engine->getSeed()<<", "<< RandFlat::shoot(engine);
 return RandFlat::shoot(engine);
}

//--------------------------------------------------------------------------
CosmicGenerator::CosmicGenerator(const std::string& name, 
      ISvcLocator* pSvcLocator): Algorithm(name,pSvcLocator)
//--------------------------------------------------------------------------  
{
  //
  // Migration to MeV and mm units: all conversions are done in this interface 
  // to the CosmicGun. The CosmicGun itself uses GeV units internally - to call
  // the fortran code.
  //

  m_GeV = 1000;
  m_mm  = 10;
  m_readfile = false;
  m_exzCut=false    ; 
  m_tuple=0;
  m_tuple1=0;
  declareProperty("eventfile",  m_infile = "NONE" );
  declareProperty("emin",       m_emin =0.1 );
  declareProperty("emax",       m_emax =10. );
  declareProperty("xvert_low",  m_xlow =-110.7); //EMC BSCRmax2
  declareProperty("xvert_hig",  m_xhig =110.7 );
  declareProperty("zvert_low",  m_zlow =-171.2);//EMC WorldZposition+0.5*CrystalLength
  declareProperty("zvert_hig",  m_zhig = 171.2 );
  declareProperty("yvert_val",  m_yval = 0 );
  declareProperty("tmin",       m_tmin =0. );
  declareProperty("tmax",       m_tmax =24. );
  declareProperty("tcor",       m_tcor =0. );
  declareProperty("IPx",  m_IPx =0. );
  declareProperty("IPy",  m_IPy =0. );
  declareProperty("IPz",  m_IPz =0. );
  declareProperty("Radius",  m_radius =0. );
  declareProperty("ExzCut",  m_exzCut );
  declareProperty("CubeProjection",  m_cubeProj = true );
  declareProperty("OptimizeSphere",  m_sphereOpt = false );
 

  declareProperty("SwapYZAxis", m_swapYZAxis = false); 
  declareProperty("ctcut",      m_ctcut =0.0 );// according to sin(acos(0.93)) =0.368
  // declareProperty("ReadTTR",      m_readTTR =false );
  //  declareProperty("ReadTR",      m_readTTR =false );
  declareProperty("PrintEvent", m_printEvent=10);
  declareProperty("PrintMod",   m_printMod=100);
  declareProperty("RMax",       m_rmax = 10000000. );
  declareProperty("ThetaMin", m_thetamin = 0.);
  declareProperty("ThetaMax", m_thetamax = 1.);
  declareProperty("PhiMin", m_phimin = -1*PI);
  declareProperty("PhiMax", m_phimax = PI);
  declareProperty("Xposition", m_xpos = 263.5-0.0001); //263.5*cm,263.5*cm,287.5*cm
  declareProperty("Yposition", m_ypos = 263.5-0.0001);
  declareProperty("Zposition", m_zpos = 287.5-0.0001);

  declareProperty("DumpMC", m_dumpMC = 0);
}

//--------------------------------------------------------------------------
 CosmicGenerator::~CosmicGenerator()
//--------------------------------------------------------------------------
{}

//---------------------------------------------------------------------------
StatusCode CosmicGenerator::initialize() {
//---------------------------------------------------------------------------

  // Initialize event count.

  m_events = 0;
  m_tried=0;
  m_accepted=0;
  m_rejected=0;
  if(m_emin<0.1) {m_emin=0.1;std::cout<<"WARNING: set emin to 0.1 GeV"<<std::endl;}
  m_emin =m_emin *m_GeV;
  m_emax =m_emax *m_GeV;
  m_xlow =m_xlow *m_mm;
  m_xhig =m_xhig *m_mm;
  m_zlow =m_zlow *m_mm;
  m_zhig =m_zhig *m_mm;
  m_yval =m_yval *m_mm;
  m_xpos =m_xpos *m_mm;
  m_ypos =m_ypos *m_mm;
  m_zpos =m_zpos *m_mm;
  m_radius=m_radius*m_mm;
  m_tcor=m_tcor
    +sqrt((m_xpos-m_xlow)*(m_xpos-m_xlow)+(m_zpos-m_zlow)*(m_zpos-m_zlow)+(m_ypos-m_yval)*(m_ypos-m_yval))
    /(m_emin/sqrt(m_emin*m_emin+mass*mass*m_GeV*m_GeV));

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap.h
  p_msgSvc = 0;
  StatusCode result = svcLocator->service("MessageSvc", p_msgSvc);
  
  if ( !result.isSuccess()  ||  p_msgSvc == 0 )
    {
      std::cerr << "VGenCommand(): could not initialize MessageSvc!" << std::endl;
      throw GaudiException("Could not initalize MessageSvc","CosmicGenerator",StatusCode::FAILURE);      
    }
      
  MsgStream log (p_msgSvc,"CosmicGenerator");
  if(m_dumpMC==1) {
    StatusCode status;
    NTuplePtr nt(ntupleSvc(), "FILE1/comp");
    if(nt) m_tuple = nt;
    else {
      m_tuple = ntupleSvc()->book ("FILE1/comp", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if ( m_tuple )    {
	status = m_tuple->addItem ("cosmic_e",   m_cosmicE);
	status = m_tuple->addItem ("cosmic_theta",   m_cosmicTheta);
	status = m_tuple->addItem ("cosmic_phi",   m_cosmicPhi);
	status = m_tuple->addItem ("cosmic_charge",   m_cosmicCharge);
      }
      else    { 
	log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple) << endmsg;
	return StatusCode::FAILURE;
      }
    } 
    NTuplePtr nt1(ntupleSvc(), "FILE1/compp");
    if(nt1) m_tuple1 = nt1;
    else {
      m_tuple1 = ntupleSvc()->book ("FILE1/compp", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if ( m_tuple1 )    {
	status = m_tuple1->addItem ("mc_theta",   mc_theta);
	status = m_tuple1->addItem ("mc_phi",   mc_phi);
	status = m_tuple1->addItem ("mc_px",   mc_px);
	status = m_tuple1->addItem ("mc_py",   mc_py);
	status = m_tuple1->addItem ("mc_pz",   mc_pz);		
      }
      else    { 
	log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
	return StatusCode::FAILURE;
      }
    } 
  }
  if(m_infile=="NONE")

    {
      // Initialize the BES random number services.

      static const bool CREATEIFNOTTHERE(true);
      StatusCode RndmStatus = svcLocator->service("BesRndmGenSvc", p_BesRndmGenSvc, CREATEIFNOTTHERE);
      
      if (!RndmStatus.isSuccess() || 0 == p_BesRndmGenSvc)
	{
	  log << MSG::ERROR << " Could not initialize Random Number Service" << endreq;
	  return RndmStatus;
	}
      
      CosmicGun* gun = CosmicGun::GetCosmicGun();
      
      gun->SetEnergyRange(m_emin/m_GeV,m_emax/m_GeV);  
      gun->SetCosCut(m_ctcut);
      gun->PrintLevel(m_printEvent, m_printMod);
      float flux_withCT = gun->InitializeGenerator();
      
      log << MSG::INFO << "Initialisation cosmic gun done." << endreq;
      log << MSG::INFO << "Accepted diff  flux after E and cos(theta) cuts = " <<
        flux_withCT << " /cm^2/s" << endreq;
      log << MSG::INFO << "Accepted total flux after E and cos(theta) cuts = " <<
        flux_withCT*(m_xhig-m_xlow)/m_mm*(m_zhig-m_zlow)/m_mm << " /s" << endreq;

      std::cout<< "Accepted diff  flux after E and cos(theta) cuts = " <<
        flux_withCT << " /cm^2/s" << std::endl;
      std::cout << "Accepted total flux after E and cos(theta) cuts = " <<
        flux_withCT*(m_xhig-m_xlow)/m_mm*(m_zhig-m_zlow)/m_mm << " /s" << std::endl; 
      
    }
  else
    {
      log << MSG::INFO << "Cosmics are read from file " << m_infile << endreq;
      m_ffile.open(m_infile.c_str());
      if(!m_ffile)
        {
          log << MSG::FATAL << "Could not open input file - stop! " << endreq;
          return StatusCode::FAILURE;
        }
      m_readfile = true;
    }

  m_center=Hep3Vector(m_IPx, m_IPy, m_IPz);
	  log << MSG::INFO << "Initialisation done" << endreq;
  return StatusCode::SUCCESS;

}

HepLorentzVector CosmicGenerator::generateVertex(void) {
  
  MsgStream log(messageService(), name());

  // Get the pointer to the engine of the stream named SINGLE. If the
  // stream does not exist is created automaticaly
//  HepRandomEngine* engine = CosmicGenerator::p_AtRndmGenSvc->GetEngine("COSMICS");
  HepRandomEngine* engine = CosmicGenerator::p_BesRndmGenSvc->GetEngine("PYTHIA");  
  // Generate a random number according to the distribution.
  
  float x_val = RandFlat::shoot(engine, m_xlow, m_xhig);
  float z_val = RandFlat::shoot(engine, m_zlow, m_zhig);

  // Generate a random number for time offset
  float t_val=0;
  if(m_tmin < m_tmax){
    t_val = RandFlat::shoot(engine, m_tmin, m_tmax);
  }
  else if(m_tmin == m_tmax){
    t_val = m_tmin;
  }
  else log << MSG::FATAL << " You specified m_tmin = " << m_tmin << " and m_tmax " << m_tmax << endreq;
  HepLorentzVector p(x_val,m_yval,z_val, t_val*c_light);

  return p;
  
}

//---------------------------------------------------------------------------
StatusCode CosmicGenerator::execute() {
//---------------------------------------------------------------------------
  MsgStream log(messageService(), name());
  log << MSG::INFO << "CosmicGenerator executing" << endreq;
  

  ++m_events;
  //  MsgStream log(messageService(), name());
  log << MSG::DEBUG << "Event #" << m_events << endreq;


  // clear up the vectors
  m_fourPos.clear();
  m_fourMom.clear();
  m_polarization.clear();
  m_pdgCode.clear();
  

  if(m_readfile)
    {
      if(!m_ffile.eof())
        {
          CosmicEventParser evt;
          m_ffile >> evt;
          
          std::cout << evt << std::endl;
          
          double polx = 0;
          double poly = 0;
          double polz = 0;
	  Polarization thePolarization;
          thePolarization.set_normal3d(HepNormal3D(polx,poly,polz));
          m_polarization.push_back(thePolarization);

          //
          // units are already converted to MeV's and mm.
          //
          m_fourPos.push_back(evt.Vertex());
          m_fourMom.push_back(evt.Momentum());
          m_pdgCode.push_back(evt.pdgID());
          
        }
      else
        {
          log << MSG::FATAL << "End of file reached - stop " << endreq;
          exit(1);
          return StatusCode::FAILURE;
        }
    } 

  else
    {
      
      bool accepted=false;
      bool projected=false;
      HepLorentzVector pp;
      CosmicGun* gun = CosmicGun::GetCosmicGun();
      HepLorentzVector vert;
      HepLorentzVector vert_proj;      
      while(!accepted){
	m_tried++;	
	vert = generateVertex();
	Hep3Vector vert3(vert.x(),vert.y(),vert.z());
	
	pp = gun->GenerateEvent();
	if(m_dumpMC==1) {
	  m_cosmicE=pp.e()*m_GeV;
	  m_cosmicTheta=pp.theta();
	  m_cosmicPhi=pp.phi();
	  m_cosmicCharge=gun->GetMuonCharge();
	  m_tuple->write();	
	}
	double theta1=pp.theta();
	double phi1=pp.phi();
	double mag1=pp.vect().mag();
	
	Hep3Vector pp_corr(mag1*sin(theta1)*cos(phi1),
			   -mag1*cos(theta1),
			   mag1*sin(theta1)*sin(phi1));
	Hep3Vector direction(pp_corr.x(),pp_corr.y(), pp_corr.z()); 
    Hep3Vector center_dir=m_center-vert3;
	// if optimization activated, check for the direction of the generated muon
	
	
	if (m_cubeProj) {
	  double alpha,beta;
	  if(m_sphereOpt){

	  beta=direction.angle(center_dir);
	  alpha=asin(m_radius/center_dir.mag());
      if(fabs(beta)>alpha){
 	    log<<MSG::DEBUG<<alpha<<", "<<beta<<" rejected muon due to sphere cut! "<<endreq;
	    m_rejected++;
	    continue;     	
      } 	  
          }

	    double l_fight0,l_fight1, l_fight2;
	    double coor_x0, coor_y0, coor_z0;
	    double coor_x1, coor_y1, coor_z1;
	    double coor_x2, coor_y2, coor_z2;
	    bool isIn0=false;
	    bool isIn1=false;
	    bool isIn2=false;
	    HepPoint3D vert_pro0;	    
	    HepPoint3D vert_pro1;
	    HepPoint3D vert_pro2;
	    HepPoint3D vert_org(vert3);
	    
	      coor_y0 = m_ypos; // defined in jobOpt.
	      coor_x0 = direction.x()*(coor_y0 - vert.y())/direction.y() +vert.x();
	      coor_z0 = direction.z()*(coor_y0 - vert.y())/direction.y() +vert.z();	    
	    
	    
	    
	    if( fabs(coor_x0) <= m_xpos && fabs(coor_z0) <= m_zpos){
		vert_pro0=HepPoint3D (coor_x0, coor_y0, coor_z0);
		l_fight0=vert_org.distance(vert_pro0);
		isIn0=true;	 
	    }
	    else{   

	      coor_z1 = sign(coor_z0-vert.z())*m_zpos; // defined in jobOpt.
	      coor_x1 = direction.x()*(coor_z1 - vert.z())/direction.z() +vert.x();
	      coor_y1 = direction.y()*(coor_z1 - vert.z())/direction.z() +vert.y();

		vert_pro1=HepPoint3D (coor_x1, coor_y1, coor_z1);
		l_fight1=vert_org.distance(vert_pro1);
			      
	      
	      coor_x2 = sign(coor_x0-vert.x())*m_xpos; // defined in jobOpt.
	      coor_z2 = direction.z()*(coor_x2 - vert.x())/direction.x() +vert.z();
	      coor_y2 = direction.y()*(coor_x2 - vert.x())/direction.x() +vert.y();
		vert_pro2=HepPoint3D (coor_x2, coor_y2, coor_z2);	 
		l_fight2=vert_org.distance(vert_pro2);
		if(l_fight1<l_fight2)	
		  isIn1=true;
		else
		  isIn2=true;	
	      }
	    //reset vert(x,y,z,t), after projection
	     if(isIn0){
		vert_proj=HepLorentzVector (vert_pro0.x(),vert_pro0.y(),vert_pro0.z() , l_fight0);	      
		
		projected =true;	     
		accepted = true;
		m_accepted++;
	      }
	      else if(isIn1){
		vert_proj=HepLorentzVector (vert_pro1.x(),vert_pro1.y(),vert_pro1.z() , l_fight1);	      
		
		projected =true;	     
		accepted = true;
		m_accepted++;
	      }
	      else if(isIn2){
		vert_proj=HepLorentzVector (vert_pro2.x(),vert_pro2.y(),vert_pro2.z() , l_fight2);	      
		
		projected =true;	     
		accepted = true;
		m_accepted++;
	      }  else {
		log<<MSG::DEBUG<<" rejected muon due to cube projection request! "<<endreq;
		m_rejected++;
	      }
  
	

	  
	}
	else accepted=true; // if no opt required accept the first muon
      }	
	
      //      pp.setX(pp.x()*m_GeV);
      //      pp.setY(pp.y()*m_GeV);
      //      pp.setZ(pp.z()*m_GeV);
      //      pp.setT(pp.t()*m_GeV);
     
      pp.setX(pp.x());
      pp.setY(pp.y());
      pp.setZ(pp.z());
      pp.setT(pp.t());           
      // Get the mass of the particle to be generated
      int charge = gun->GetMuonCharge();
      // m_pdgCode.push_back(charge*13);
      m_pdgCode.push_back(charge*-13);
      
      //      const ParticleData* pdtparticle = m_particleTable->particle(ParticleID(abs(m_pdgCode.back())));
      //      double mass = pdtparticle->mass().value();
      //      double mass =105.5658;

      // Compute the kinematic values.  First, the vertex 4-vector:


      // Set the polarization.  Realistically, this is going to be zero
      // for most studies, but you never know...
      double polx = 0;
      double poly = 0;
      double polz = 0;
      //m_polarization.set_normal3d(HepNormal3D(polx,poly,polz));
      Polarization thePolarization;

      // Do we need to swap Y- and Z-axis for the PixelEndCap C Cosmic test ?
      // if not...do nothing...if so, invert position of y- and z- coordinate
      //
      // well and don't forget about the direction of the incoming cosmic muon(s) either
      // that means:  y -> -y
      //    
      if(!m_swapYZAxis){
	// thePolarization.set_normal3d(HepNormal3D(polx,-poly,polz));
        thePolarization.set_normal3d(HepNormal3D(polx,poly,polz));
      }
      else
	thePolarization.set_normal3d(HepNormal3D(polx,polz,-poly));
      
      m_polarization.push_back(thePolarization);
      

      // The method of calculating e, theta, and phi depends on the user's
      // commands.  Let the KinematicManager handle it.
      double e     = pp.e();
      double theta = pp.theta();
      double phi   = pp.phi();
      
      // At this point, we have e, theta, and phi.  Put them together to
      // get the four-momentum.
      
      double p2 = e*e - mass*mass;
      if ( p2 < 0 )
	{
	  log << MSG::ERROR
	      << "Event #" << m_events
	      << " E=" << e << ", mass=" << mass
	      << " -- you have generated a tachyon! Increase energy or change particle ID." 
	      << endmsg;   
	  return StatusCode::FAILURE;
	}
      
      double p = sqrt(p2);  
      double px = p*sin(theta)*cos(phi);
      double pz = p*sin(theta)*sin(phi);
      double py = -p*cos(theta);



      // Do we need to swap Y- and Z-axis for the PixelEndCap C Cosmic test ?
      // if not...do nothing...if so, invert position of y- and z- coordinate
      //
      // well and don't forget about the direction of the incoming cosmic muon(s) either
      // that means:  y -> -y
      //
      if(!m_swapYZAxis) {
        // Line below corrupted py sign and forces muons to be upwards, not downwards.
	// m_fourMom.push_back(HepLorentzVector(px,-py,pz,pp.e()));
	m_fourMom.push_back(HepLorentzVector(px,py,pz,pp.e()));
      }
      else
	m_fourMom.push_back(HepLorentzVector(px,pz,-py,pp.e()));
	double x = vert.x();
	double y = vert.y();
	double z = vert.z();
	double t = vert.t();  
      if(projected){
	
	 x = vert_proj.x();
	 y = vert_proj.y();
	 z = vert_proj.z();
	 t = vert.t()-vert_proj.t()/HepLorentzVector(px,py,pz,pp.e()).beta()
	   +m_tcor;



      }
      //	    log << MSG::DEBUG
      //          << "proj:"<<m_cubeProj<<" ("<<x<<", "<<y<<", "<<z<<", "<<t<<")"
      //		<<endreq;
      // Do we need to swap Y- and Z-axis for the PixelEndCap A Cosmic test ?
      // if not...do nothing...if so, invert position of y- and z- coordinate
      //
      // but not only that...change also the direction of the incoming cosmic muon(s),
      // they must go towards the pixel endcap A, i.e. y -> -y 
      //


      if(!m_swapYZAxis)
	m_fourPos.push_back(HepLorentzVector(x,y,z,t));
      else
	m_fourPos.push_back(HepLorentzVector(x,z,y,t));
    
      log << MSG::DEBUG 
	  << "  (x,y,z,t) = (" 
	  << m_fourPos.back().x() << "," 
	  << m_fourPos.back().y() << "," 
	  << m_fourPos.back().z() << "," 
	  << m_fourPos.back().t() << "), (Px,Py,Pz,E) = (" 
	  << m_fourMom.back().px() << "," 
	  << m_fourMom.back().py() << "," 
	  << m_fourMom.back().pz() << "," 
	  << m_fourMom.back().e()  << ")" 
	    << endreq;
      log << MSG::DEBUG
	  << "  (theta,phi) = (" << theta << "," << phi << "), "
	  << "polarization(x,y,z) = (" 
	  << m_polarization.back().normal3d().x() << ","
	  << m_polarization.back().normal3d().y() << ","
	  << m_polarization.back().normal3d().z() << ")"
	  << endreq;
	if(m_dumpMC==1) {
// 	  m_cosmicE=pp.e()*m_GeV;
// 	  m_cosmicTheta=pp.theta();
// 	  m_cosmicPhi=pp.phi();
// 	  m_cosmicCharge=gun->GetMuonCharge();
	  mc_theta=m_fourMom.back().theta();
	  mc_phi=m_fourMom.back().phi();
	  mc_px=m_fourMom.back().px();
	  mc_py=m_fourMom.back().py();
	  mc_pz=m_fourMom.back().pz();
			   //	  	  	 m_cosmicE=pz;
	  m_tuple1->write();	
	} 
    }

// fill evt

  GenEvent* event = new GenEvent(1,1);

  if(m_fourMom.size()==m_fourPos.size()&&m_fourMom.size()==m_polarization.size()){

    for(std::size_t v=0;v<m_fourMom.size();++v){
            
      // Note: The vertex and particle are owned by the event, so the
      // event is responsible for those pointers.
      
      // Create the particle, and specify its polarization.
      
      GenParticle* particle = new GenParticle( m_fourMom[v], m_pdgCode[v], 1);
      particle->set_polarization( m_polarization[v] );
      
      // Create the vertex, and add the particle to the vertex.
      GenVertex* vertex = new GenVertex(m_fourPos[v]);
      vertex->add_particle_out( particle );
      
      // Add the vertex to the event.
      event->add_vertex( vertex );

    }

    event->set_event_number(m_events); // Set the event number
    


  } else {

    log<<MSG::ERROR<<"Wrong different number of vertexes/momenta/polaritazions!"<<endreq;
      return StatusCode::FAILURE;
  }
  // Check if the McCollection already exists
  SmartDataPtr<McGenEventCol> anMcCol(eventSvc(), "/Event/Gen");
  if (anMcCol!=0) 
  {
    // Add event to existing collection
  
    log << MSG::INFO << "Add McGenEvent to existing collection" << endreq;
    McGenEvent* mcEvent = new McGenEvent(event);
    anMcCol->push_back(mcEvent);
  }
  else 
  {
    // Create Collection and add  to the transient store
    McGenEventCol *mcColl = new McGenEventCol;
    McGenEvent* mcEvent = new McGenEvent(event);
    mcColl->push_back(mcEvent);
    StatusCode sc = eventSvc()->registerObject("/Event/Gen",mcColl);
    if (sc != StatusCode::SUCCESS) 
    {
      log << MSG::ERROR << "Could not register McGenEvent" << endreq;
      delete mcColl;
      delete event;
      delete mcEvent;
      return StatusCode::FAILURE;
    }

  }


  


  return StatusCode::SUCCESS;
  
}


//---------------------------------------------------------------------------
StatusCode CosmicGenerator::finalize() {
//---------------------------------------------------------------------------
  // Get the KinematicManager.

  if(m_cubeProj) {
    
    MsgStream log(messageService(), name());
    
    log << MSG::INFO<<"***************************************************"<<endreq;
    log << MSG::INFO <<"** you have run CosmicGenerator with some "        <<endreq;
    log << MSG::INFO <<"** filters for cosmic muon simulation"             <<endreq;
    log << MSG::INFO <<"** "<<m_tried<<" muons were tried"           <<endreq;
    log << MSG::INFO <<"** "<<m_accepted<<" muons were accepted"           <<endreq;
    log << MSG::INFO <<"** "<<m_rejected<<" muons were rejected"           <<endreq;
    log << MSG::INFO<<"***************************************************"<<endreq;
    std::cout<<"***************************************************"<<std::endl;
    std::cout <<"** you have run CosmicGenerator with some "        <<std::endl;
    std::cout <<"** filters for cosmic muon simulation"             <<std::endl;
    std::cout <<"** "<<m_tried<<" muons were tried"           <<std::endl;
    std::cout <<"** "<<m_accepted<<" muons were accepted"           <<std::endl;
    std::cout <<"** "<<m_rejected<<" muons were rejected"           <<std::endl;
    std::cout<<"***************************************************"<<std::endl;
    	
  }

  return StatusCode::SUCCESS;
}




// Energy dependent position cut on the surface. 
bool CosmicGenerator::exzCut(const Hep3Vector& pos,const HepLorentzVector& p) 
{
// p is in GeV... 

	double r =0; 
	bool cut = false; 
	if(pos.z()<0){
	  r = sqrt((pow(pos.x(),2)+pow(pos.z()+28000,2))) ;
	  double e = 0.45238*r+5000 ; 
	  cut = p.e()*m_GeV>e; 
	}	 
	else 
	{
	  r = sqrt((pow(pos.x(),2)+pow(pos.z()-20000,2))) ;
	  if(r<15000) {
	    cut = true;
	  } else
	  {
	  double e = 0.461538*(r-15000)+10000 ; 
	  cut = p.e()*m_GeV>e; 
//	  std::cout<<"z>0 r , e, p.e = "<<r <<" " <<e <<" " <<p.e()*m_GeV<<std::endl;
	  } 
	}	 


	cut = cut && r < m_rmax ; 

	return cut; 

 
}
