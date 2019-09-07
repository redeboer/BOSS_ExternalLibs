// Include files
#ifndef BEAN
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "MagneticField/IMagneticFieldSvc.h"
#endif

#include "MagneticField/MagneticFieldSvc.h"
#include "MagneticField/MucMagneticField.h"

#include "CLHEP/Units/PhysicalConstants.h"

#ifndef BEAN
#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"
#endif

#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;
using namespace CLHEP;

/** @file MagneticFieldSvc.cxx
 *  Implementation of MagneticFieldSvc class
 *
 */

#ifndef BEAN
// Instantiation of a static factory class used by clients to create
// instances of this service
//static SvcFactory<MagneticFieldSvc> s_factory;
//const ISvcFactory& MagneticFieldSvcFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MagneticFieldSvc::MagneticFieldSvc( const std::string& name, 
            ISvcLocator* svc ) : Service( name, svc )
{
  declareProperty( "TurnOffField", m_turnOffField = false);
  declareProperty( "FieldMapFile", m_filename );
  declareProperty( "GridDistance", m_gridDistance = 5); 
  declareProperty( "RunMode", m_runmode = 2);
  declareProperty( "IfRealField", m_ifRealField = true);
  declareProperty( "OutLevel", m_outlevel = 1);
  declareProperty( "Scale", m_scale = 1.0);
  declareProperty( "UniField", m_uniField = false);

  declareProperty( "Cur_SCQ1_55", m_Cur_SCQ1_55 = 349.4);
  declareProperty( "Cur_SCQ1_89", m_Cur_SCQ1_89 = 426.2);
  declareProperty( "Cur_SCQ2_10", m_Cur_SCQ2_10 = 474.2);

  declareProperty( "UseDBFlag", m_useDB = true);

  m_Mucfield = new MucMagneticField();
  if(!m_Mucfield) cout<<"error: can not get MucMagneticField pointer"<<endl;

  m_zfield = -1.0*tesla;
  
  if(getenv("MAGNETICFIELDROOT") != NULL) {
    path = std::string(getenv( "MAGNETICFIELDROOT" ));
  } else {
    std::cerr<<"Couldn't find MAGNETICFIELDROOT"<<std::endl;
  }

}

#else
// -------------------------- BEAN ------------------------------------
MagneticFieldSvc* MagneticFieldSvc::m_field = 0;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MagneticFieldSvc::MagneticFieldSvc()
{
  // use functions instead of "declareProperty"
  m_gridDistance = 5; 
  m_runmode = 2;
  m_ifRealField = true;
  m_outlevel = 1;

  m_Cur_SCQ1_55 = 349.4;
  m_Cur_SCQ1_89 = 426.2;
  m_Cur_SCQ2_10 = 474.2;

  m_useDB = true;

  path = "";
  m_Mucfield = 0;

  m_zfield = -1.0*tesla;
}
#endif

//=============================================================================
// Standard destructor
//=============================================================================
MagneticFieldSvc::~MagneticFieldSvc()
{
  if(m_Mucfield) delete m_Mucfield;
}

//=============================================================================
// Initialize
//=============================================================================
#ifdef BEAN
bool MagneticFieldSvc::init_mucMagneticField()
{
  // reinstall MucMagneticField probably with a new path
  if( m_Mucfield ) {
    if( m_Mucfield->getPath() == path ) return true;
    delete m_Mucfield;
  }
    
  m_Mucfield = new(std::nothrow) MucMagneticField(path);
  if( !m_Mucfield ) {
    cout<<"error: can not get MucMagneticField pointer"<<endl;
    return false;
  }
  
  return true;
}
#endif

StatusCode MagneticFieldSvc::initialize()
{
#ifndef BEAN
  MsgStream log(msgSvc(), name());
  StatusCode status = Service::initialize();
  // Get the references to the services that are needed by the ApplicationMgr itself
  IIncidentSvc* incsvc;
  status = service("IncidentSvc", incsvc);
  int priority = 100;
  if( status.isSuccess() ){
    incsvc -> addListener(this, "NewRun", priority);
  }

  status = serviceLocator()->service("EventDataSvc", m_eventSvc, true);
  if (status.isFailure() ) {
    log << MSG::ERROR << "Unable to find EventDataSvc " << endreq;
    return status;
  }
#else
  if( !init_mucMagneticField() ) return false;
  StatusCode status = true;
#endif

  if(m_useDB == false) {
    if(m_gridDistance == 5) {
      m_Q.reserve(201250);
      m_P.reserve(201250);
      m_Q_TE.reserve(176608);
      m_P_TE.reserve(176608);
    }
    if(m_gridDistance == 10) {
      m_Q.reserve(27082);
      m_P.reserve(27082);
      m_Q_TE.reserve(23833);
      m_P_TE.reserve(23833);
    }
 
      m_filename = path;
      m_filename_TE = path;
      if(m_gridDistance == 10) {
        m_filename_TE += std::string("/dat/TEMap10cm.dat");
        if(m_runmode == 2)
          m_filename += std::string( "/dat/2008-04-03BESIII-field-Mode2.dat");
        else if(m_runmode == 4)
          m_filename += std::string( "/dat/2008-04-03BESIII-field-Mode4.dat");
        else if(m_runmode == 6)
          m_filename += std::string( "/dat/2008-04-03BESIII-field-Mode6.dat");
        else if(m_runmode == 7)
          m_filename += std::string( "/dat/2008-04-03BESIII-field-Mode7.dat");
        else {
          m_filename += std::string( "/dat/2007-08-28BESIII-field.dat");
          std::cout<<"WARNING: YOU ARE USING OLD FIELD MAP!!!!"<<std::endl;
        }
      }
      if(m_gridDistance == 5) {
        m_filename_TE += std::string("/dat/TEMap5cm.dat");
        if(m_runmode == 1)
          m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode1.dat");
        else if(m_runmode == 2)
          m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode2.dat");
        else if(m_runmode == 3)
          m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode3.dat");
        else if(m_runmode == 4)
          m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode4.dat");
        else if(m_runmode == 5)
          m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode5.dat");
        else if(m_runmode == 6)
          m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode6.dat");
        else if(m_runmode == 7)
          m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode7.dat");
        else if(m_runmode == 8)
          m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode8.dat");
        else {
          m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode2.dat");
          std::cout<<"WARNING: NO RUN MODE, YOU ARE USING DEFAULT FIELD MAP!!!!"<<std::endl;
        }
      }
      cout<<"*** Read field map: "<<m_filename<<endl;
      cout<<"*** Read field map: "<<m_filename_TE<<endl;

    status = parseFile();
    status = parseFile_TE();

#ifndef BEAN
    if ( status.isSuccess() ) {
        log << MSG::DEBUG << "Magnetic field parsed successfully" << endreq;
#else
    if ( status ) {
        cout << "Magnetic field parsed successfully" << endl;
#endif

      for (int iC = 0; iC< 2; ++iC ){
        m_min_FL[iC] = -90.0*cm;
        m_max_FL[iC] = m_min_FL[iC]+( m_Nxyz[iC]-1 )* m_Dxyz[iC];
        //for tof and emc
        m_min_FL_TE[iC] = 0.0*cm;
        m_max_FL_TE[iC] = m_min_FL_TE[iC]+( m_Nxyz_TE[iC]-1 )* m_Dxyz_TE[iC];
      } // iC
      m_min_FL[2] = -120.0*cm;
      m_max_FL[2] = m_min_FL[2]+( m_Nxyz[2]-1 )* m_Dxyz[2];
      //for tof and emc
      m_min_FL_TE[2] = 0.0*cm;
      m_max_FL_TE[2] = m_min_FL_TE[2]+( m_Nxyz_TE[2]-1 )* m_Dxyz_TE[2];
    }
    else {
#ifndef BEAN
      log << MSG::DEBUG << "Magnetic field parse failled" << endreq;
#else
      cout << "Magnetic field parse failled" << endl;
#endif
    }
  }
  else {
    m_connect_run = new FieldDBUtil::ConnectionDB();
#ifndef BEAN
    if (!m_connect_run) {
      log << MSG::ERROR << "Could not open connection to database" << endreq;
    }
#endif
  }

  return status;
}

#ifndef BEAN
void MagneticFieldSvc::init_params()
{
  MsgStream log(msgSvc(), name()); 
#else
void MagneticFieldSvc::init_params(int run)
{
  if( !init_mucMagneticField() ) {
    cerr << " STOP! " << endl;
    exit(1);
  }
#endif

  m_Q.clear();
  m_P.clear();
  m_Q_1.clear();
  m_P_1.clear();
  m_Q_2.clear();
  m_P_2.clear();
  m_P_TE.clear();
  m_Q_TE.clear();
 
  if(m_gridDistance == 5) {
     m_Q.reserve(201250);
     m_P.reserve(201250);
     m_Q_1.reserve(201250);
     m_P_1.reserve(201250);
     m_Q_2.reserve(201250);
     m_P_2.reserve(201250);
     m_Q_TE.reserve(176608);
     m_P_TE.reserve(176608);
   }
   if(m_gridDistance == 10) {
     m_Q.reserve(27082);
     m_P.reserve(27082);
     m_Q_1.reserve(27082);
     m_P_1.reserve(27082);
     m_Q_2.reserve(27082); 
     m_P_2.reserve(27082);
     m_Q_TE.reserve(23833);
     m_P_TE.reserve(23833);
   }
   
#ifndef BEAN
   int runNo;
   SmartDataPtr<Event::EventHeader> evt(m_eventSvc,"/Event/EventHeader");
   if( evt ){
     runNo = evt -> runNumber();
     log << MSG::INFO <<"The runNumber of current event is  "<<runNo<<endreq;
   }
   else {
     log << MSG::ERROR <<"Can not get EventHeader!"<<endreq;
   }
#else
   int runNo = run;
#endif

   using FieldDBUtil::ConnectionDB;
   std::vector<double> current(3,0.);
   ConnectionDB::eRet e = m_connect_run->getReadSC_MagnetInfo(current,std::abs(runNo));   

   std::vector<double> beamEnergy;
   beamEnergy.clear();
   e = m_connect_run->getBeamEnergy(beamEnergy, std::abs(runNo));
   char BPR_PRB[200];
   char BER_PRB[200];

   sprintf(BPR_PRB,"%f ",beamEnergy[0]);
   sprintf(BER_PRB,"%f ",beamEnergy[1]);

   setenv("BEPCII_INFO.BPR_PRB", BPR_PRB, 1);
   setenv("BEPCII_INFO.BER_PRB", BER_PRB, 1);

   int ssm_curr = int (current[0]);
   double scql_curr = current[1];
   double scqr_curr = current[2];

#ifndef BEAN
   log << MSG::INFO << "SSM current: " << current[0] << " SCQL current: " << current[1] << " SCQR current: " << current[2] << " in Run " << runNo << endreq;
#else
   cout << "SSM current: " << current[0] << " SCQL current: " << current[1] 
        << " SCQR current: " << current[2] << " in Run " << runNo << endl;
#endif
   
   //int ssm_curr = 3369;
   //double scql_curr = 426.2;
   //double scqr_curr = 426.2;
   //for the energy less than 1.89GeV
   if((ssm_curr >= 3367) && (ssm_curr <= 3370) && ((scql_curr+scqr_curr)/2 < m_Cur_SCQ1_89)) {
     m_zfield = -1.0*tesla;
       m_filename = path;
       m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode2.dat");
#ifndef BEAN
     log << MSG::INFO << "Select field map: " << m_filename << endreq;
#else
     cout << "Select field map: " << m_filename << endl;
#endif

     StatusCode status = parseFile();

#ifndef BEAN
     if ( !status.isSuccess() ) {
       log << MSG::DEBUG << "Magnetic field parse failled" << endreq;
     }
#else
     if ( !status ) {
       cout << "Magnetic field parse failled" << endl;
     }
#endif
     m_Q_1 = m_Q;
     m_P_1 = m_P;

     m_Q.clear();
     m_P.clear();
     if(m_gridDistance == 5) {
       m_Q.reserve(201250);
       m_P.reserve(201250);
     }
     if(m_gridDistance == 10) {
       m_Q.reserve(27082);
       m_P.reserve(27082);
     }

       m_filename = path;
       m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode3.dat");
#ifndef BEAN
     log << MSG::INFO << "Select field map: " << m_filename << endreq;
#else
     cout << "Select field map: " << m_filename << endl;
#endif

     status = parseFile();

#ifndef BEAN
     if ( !status.isSuccess() ) {
       log << MSG::DEBUG << "Magnetic field parse failled" << endreq;
     } 
#else
     if ( !status ) {
       cout << "Magnetic field parse failled" << endl;
     } 
#endif
     m_Q_2 = m_Q;
     m_P_2 = m_P;

     m_Q.clear();
     m_P.clear();
     if(m_gridDistance == 5) {
       m_Q.reserve(201250);
       m_P.reserve(201250);
     }
     if(m_gridDistance == 10) {
       m_Q.reserve(27082);
       m_P.reserve(27082);
     } 
     //check
     if(m_Q_2.size() != m_Q_1.size()) {
#ifndef BEAN
       log << MSG::FATAL << "The two field maps used in this run are wrong!!!" << endreq;
#else
       cout << "The two field maps used in this run are wrong!!!" << endl;
#endif
       exit(1);
     }

     for(unsigned int iQ = 0; iQ < m_Q_1.size(); iQ++) {
       double fieldvalue = (m_Q_1[iQ] - m_Q_2[iQ])/(m_Cur_SCQ1_55 - m_Cur_SCQ1_89)*((scql_curr+scqr_curr)/2 - m_Cur_SCQ1_89) + m_Q_2[iQ];
       m_Q.push_back(fieldvalue);
       m_P.push_back(m_P_2[iQ]);
     }
   }
   //for the energy greater than 1.89GeV
   if((ssm_curr >= 3367) && (ssm_curr <= 3370) && ((scql_curr+scqr_curr)/2 >= m_Cur_SCQ1_89)) {
     m_zfield = -1.0*tesla;
       m_filename = path;
       m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode3.dat");
#ifndef BEAN
     log << MSG::INFO << "Select field map: " << m_filename << endreq;
#else
     cout << "Select field map: " << m_filename << endl;
#endif

     StatusCode status = parseFile();

#ifndef BEAN
     if ( !status.isSuccess() ) {
       log << MSG::DEBUG << "Magnetic field parse failled" << endreq;
     }
#else
     if ( !status ) {
       cout << "Magnetic field parse failled" << endl;
     }
#endif
     m_Q_1 = m_Q;
     m_P_1 = m_P;

     m_Q.clear();
     m_P.clear();
     if(m_gridDistance == 5) {
       m_Q.reserve(201250);
       m_P.reserve(201250);
     }
     if(m_gridDistance == 10) {
       m_Q.reserve(27082);
       m_P.reserve(27082);
     }

       m_filename = path;
       m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode4.dat");
#ifndef BEAN
     log << MSG::INFO << "Select field map: " << m_filename << endreq;
#else
     cout << "Select field map: " << m_filename << endl;
#endif

     status = parseFile();

#ifndef BEAN
     if ( !status.isSuccess() ) {
       log << MSG::DEBUG << "Magnetic field parse failled" << endreq;
     } 
#else
     if ( !status ) {
       cout << "Magnetic field parse failled" << endl;
     } 
#endif
     m_Q_2 = m_Q;
     m_P_2 = m_P;                           

     m_Q.clear();
     m_P.clear();
     if(m_gridDistance == 5) {
       m_Q.reserve(201250);
       m_P.reserve(201250);
     }
     if(m_gridDistance == 10) {
       m_Q.reserve(27082);
       m_P.reserve(27082);
     }
     //check
     if(m_Q_2.size() != m_Q_1.size()) {
#ifndef BEAN
       log << MSG::FATAL << "The two field maps used in this run are wrong!!!" << endreq;
#else
       cout << "The two field maps used in this run are wrong!!!" << endl;
#endif
       exit(1);
     }

     for(unsigned int iQ = 0; iQ < m_Q_1.size(); iQ++) {
       double fieldvalue = (m_Q_1[iQ] - m_Q_2[iQ])/(m_Cur_SCQ1_89 - m_Cur_SCQ2_10)*((scql_curr+scqr_curr)/2 - m_Cur_SCQ2_10) + m_Q_2[iQ];
       m_Q.push_back(fieldvalue);
       m_P.push_back(m_P_2[iQ]);
     } 
   }
   if((ssm_curr >= 3033) && (ssm_curr <= 3035)) {
     m_zfield = -0.9*tesla;
       m_filename = path;
       m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode7.dat");
#ifndef BEAN
     log << MSG::INFO << "Select field map: " << m_filename << endreq;
#else
     cout << "Select field map: " << m_filename << endl;
#endif

     StatusCode status = parseFile();

#ifndef BEAN
     if ( !status.isSuccess() ) {
       log << MSG::DEBUG << "Magnetic field parse failled" << endreq;
     }
#else
     if ( !status ) {
       cout << "Magnetic field parse failled" << endl;
     }
#endif
     m_Q_1 = m_Q;
     m_P_1 = m_P;
     
     m_Q.clear();
     m_P.clear();
     if(m_gridDistance == 5) {
       m_Q.reserve(201250);
       m_P.reserve(201250);
     }
     if(m_gridDistance == 10) {
       m_Q.reserve(27082);
       m_P.reserve(27082);
     }

       m_filename = path;
       m_filename += std::string( "/dat/2008-05-27BESIII-field-Mode8.dat");
#ifndef BEAN
     log << MSG::INFO << "Select field map: " << m_filename << endreq; 
#else
     cout << "Select field map: " << m_filename << endl; 
#endif

     status = parseFile();

#ifndef BEAN
     if ( !status.isSuccess() ) {
       log << MSG::DEBUG << "Magnetic field parse failled" << endreq;
     }
#else
     if ( !status ) {
       cout << "Magnetic field parse failled" << endl;
     }
#endif
     m_Q_2 = m_Q;
     m_P_2 = m_P;

     m_Q.clear();
     m_P.clear();
     if(m_gridDistance == 5) {
       m_Q.reserve(201250);
       m_P.reserve(201250);
     }
     if(m_gridDistance == 10) {
       m_Q.reserve(27082);
       m_P.reserve(27082);
     }
     //check
     if(m_Q_2.size() != m_Q_1.size()) {
#ifndef BEAN
       log << MSG::FATAL << "The two field maps used in this run are wrong!!!" << endreq;
#else
       cout << "The two field maps used in this run are wrong!!!" << endl;
#endif
       exit(1);
     }

     for(unsigned int iQ = 0; iQ < m_Q_1.size(); iQ++) {
       double fieldvalue = (m_Q_1[iQ] - m_Q_2[iQ])/(m_Cur_SCQ1_55 - m_Cur_SCQ1_89)*((scql_curr+scqr_curr)/2 - m_Cur_SCQ1_89) + m_Q_2[iQ];
       m_Q.push_back(fieldvalue);
       m_P.push_back(m_P_2[iQ]);
     }
   }

   if((!((ssm_curr >= 3367) && (ssm_curr <= 3370)) && !((ssm_curr >= 3033) && (ssm_curr <= 3035))) || scql_curr == 0 || scqr_curr == 0) {
#ifndef BEAN
     log << MSG::FATAL << "The current of run " << runNo << " is abnormal in database, please check it! " << endreq;
     log << MSG::FATAL << "The current of SSM is " << ssm_curr << ", SCQR is " << scqr_curr << ", SCQL is " << scql_curr << endreq;
#else
     cout << "The current of run " << runNo 
          << " is abnormal in database, please check it! " << endl;
     cout << "The current of SSM is " << ssm_curr 
          << ", SCQR is " << scqr_curr << ", SCQL is " << scql_curr << endl;
#endif
     exit(1);
   } 

   if(m_Q.size() == 0) {
#ifndef BEAN
     log << MSG::FATAL << "This size of field map vector is ZERO, please check the current of run " << runNo << " in database!" << endreq;
#else
     cout << "This size of field map vector is ZERO,"
          << " please check the current of run " << runNo 
          << " in database!" << endl;
#endif
     exit(1); 
   } 

   m_filename_TE = path;
   if(m_gridDistance == 10)  m_filename_TE += std::string( "/dat/TEMap10cm.dat");
   if(m_gridDistance == 5)  m_filename_TE += std::string( "/dat/TEMap5cm.dat");
#ifndef BEAN
   log << MSG::INFO << "Select field map: " << m_filename_TE << endreq;
#else
   cout << "Select field map: " << m_filename_TE << endl;
#endif

   StatusCode status = parseFile_TE();

#ifndef BEAN
   if ( !status.isSuccess() ) {
     log << MSG::DEBUG << "Magnetic field parse failled" << endreq;
   }
#else
   if ( !status ) {
     cout << "Magnetic field parse failled" << endl;
   }
#endif

   for (int iC = 0; iC< 2; ++iC ){
     m_min_FL[iC] = -90.0*cm;
     m_max_FL[iC] = m_min_FL[iC]+( m_Nxyz[iC]-1 )* m_Dxyz[iC];
     //for tof and emc
     m_min_FL_TE[iC] = 0.0*cm;
     m_max_FL_TE[iC] = m_min_FL_TE[iC]+( m_Nxyz_TE[iC]-1 )* m_Dxyz_TE[iC];
   } // iC
   m_min_FL[2] = -120.0*cm;
   m_max_FL[2] = m_min_FL[2]+( m_Nxyz[2]-1 )* m_Dxyz[2];
   //for tof and emc
   m_min_FL_TE[2] = 0.0*cm;
   m_max_FL_TE[2] = m_min_FL_TE[2]+( m_Nxyz_TE[2]-1 )* m_Dxyz_TE[2];
}

#ifndef BEAN
//=============================================================================
// Finalize
//=============================================================================
StatusCode MagneticFieldSvc::finalize()
{
  MsgStream log( msgSvc(), name() );
  //if(m_connect_run) delete m_connect_run;
  StatusCode status = Service::finalize();
  
  if ( status.isSuccess() )
    log << MSG::INFO << "Service finalized successfully" << endreq;
  return status;
}

//=============================================================================
// QueryInterface
//=============================================================================
StatusCode MagneticFieldSvc::queryInterface( const InterfaceID& riid, 
                                             void** ppvInterface      ) 
{
  StatusCode sc = StatusCode::FAILURE;
  if ( ppvInterface ) {
    *ppvInterface = 0;
    
    if ( riid == IID_IMagneticFieldSvc ) {
      *ppvInterface = static_cast<IMagneticFieldSvc*>(this);
      sc = StatusCode::SUCCESS;
      addRef();
    }
    else
      sc = Service::queryInterface( riid, ppvInterface );    
  }
  return sc;
}

void MagneticFieldSvc::handle(const Incident& inc) {
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;
  if ( inc.type() != "NewRun" ){
    return;
  }
  log << MSG::DEBUG << "Begin New Run" << endreq;
  if(m_useDB == true) {
    init_params();
  }
}

#else
void MagneticFieldSvc::handle(int new_run) {
  static int save_run = 0;
  if( new_run == save_run ) return;

  cout << "Begin New Run " << new_run << endl;
  save_run = new_run;
  if(m_useDB == true) {
    init_params(new_run);
  }
}
#endif

// ---------------------------------------------------------------------------
// Routine: parseFile
// Purpose: Parses the file and fill a magnetic field vector
// ---------------------------------------------------------------------------
StatusCode MagneticFieldSvc::parseFile() {
#ifndef BEAN
  StatusCode sc = StatusCode::FAILURE;
  
  MsgStream log( msgSvc(), name() );
#else
  StatusCode sc = false;
#endif
 
  char line[ 255 ];
  std::ifstream infile( m_filename.c_str() );
  
  if ( infile ) {
#ifndef BEAN
	  sc = StatusCode::SUCCESS;
#else
	  sc = true;
#endif
    
    // Skip the header till PARAMETER
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'P' );
    
    // Get the PARAMETER
    std::string sPar[2];
    char* token = strtok( line, " " );
    int ip = 0;
    do{
      if ( token ) { sPar[ip] = token; token = strtok( NULL, " " );} 
      else continue;
      ip++;
    } while ( token != NULL );
    long int npar = atoi( sPar[1].c_str() );

    // Skip the header till GEOMETRY
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != 'G' );
    
    // Skip any comment before GEOMETRY 
    do{
	    infile.getline( line, 255 );
	  } while( line[0] != '#' );
    
    // Get the GEOMETRY
    infile.getline( line, 255 );
    std::string sGeom[7];
    token = strtok( line, " " );
    int ig = 0;
    do{
      if ( token ) { sGeom[ig] = token; token = strtok( NULL, " " );} 
      else continue; 
      ig++; 
    } while (token != NULL);

    // Grid dimensions are given in cm in CDF file. Convert to CLHEP units
    m_Dxyz[0] = atof( sGeom[0].c_str() ) * cm;
    m_Dxyz[1] = atof( sGeom[1].c_str() ) * cm;
    m_Dxyz[2] = atof( sGeom[2].c_str() ) * cm;
    m_Nxyz[0] = atoi( sGeom[3].c_str() );
    m_Nxyz[1] = atoi( sGeom[4].c_str() );
    m_Nxyz[2] = atoi( sGeom[5].c_str() );
    m_zOffSet = atof( sGeom[6].c_str() ) * cm;
    // Number of lines with data to be read
    long int nlines = ( npar - 7 ) / 3;
    
    // Check number of lines with data read in the loop
    long int ncheck = 0;
    
    // Skip comments and fill a vector of magnetic components for the
    // x, y and z positions given in GEOMETRY
    
   	while( infile ) {
      // parse each line of the file, 
      // comment lines begin with '#' in the cdf file
	    infile.getline( line, 255 );
	    if ( line[0] == '#' ) continue;
	    std::string gridx, gridy, gridz, sFx, sFy, sFz; 
	    char* token = strtok( line, " " );
      if ( token ) { gridx = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { gridy = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { gridz = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { sFx = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { sFy = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { sFz = token; token = strtok( NULL, " " );} else continue;
      if ( token != NULL ) continue;
      //Grid position
      double gx = atof( gridx.c_str() ) * m;
      double gy = atof( gridy.c_str() ) * m;
      double gz = atof( gridz.c_str() ) * m;
      // Field values are given in tesla in CDF file. Convert to CLHEP units
      double fx = atof( sFx.c_str() ) * tesla;
      double fy = atof( sFy.c_str() ) * tesla;
      double fz = atof( sFz.c_str() ) * tesla;
      //for debug
      if(m_outlevel == 0) {
#ifndef BEAN
        log << MSG::DEBUG << "grid x, y, z is "<< gx << ", " << gy << ", " << gz << endreq;
        log << MSG::DEBUG << "field x, y, z is "<< fx << ", " << fy << ", " << fz << endreq;
#else
        cout << "grid x, y, z is "<< gx << ", " << gy << ", " << gz << endl;
        cout << "field x, y, z is "<< fx << ", " << fy << ", " << fz << endl;
#endif
      }
      //Fill the postion to the vector
      m_P.push_back( gx );
      m_P.push_back( gy );
      m_P.push_back( gz );
      // Add the magnetic field components of each point to 
      // sequentialy in a vector 
      m_Q.push_back( fx );
      m_Q.push_back( fy );
      m_Q.push_back( fz );
      // counts after reading and filling to match the number of lines
      ncheck++; 
	  }
    infile.close();
    if ( nlines != ncheck) {
#ifndef BEAN
      log << MSG::ERROR << "nline is " << nlines << "; ncheck is " << ncheck << " Number of points in field map does not match!" 
          << endreq;
      return StatusCode::FAILURE;
#else
      cout << "nline is " << nlines << "; ncheck is " << ncheck << " Number of points in field map does not match!" 
          << endl;
      return false;
#endif
    }
  }
  else {
#ifndef BEAN
  	log << MSG::ERROR << "Unable to open magnetic field file : " 
        << m_filename << endreq;
#else
  	cout << "Unable to open magnetic field file : " 
        << m_filename << endl;
#endif
  }
  
  return sc;
}


// ---------------------------------------------------------------------------
// Routine: parseFile_TE
// Purpose: Parses the file and fill a magnetic field vector for tof and emc
// ---------------------------------------------------------------------------
StatusCode MagneticFieldSvc::parseFile_TE() {
#ifndef BEAN
  StatusCode sc = StatusCode::FAILURE;
    
  MsgStream log( msgSvc(), name() );
#else
  StatusCode sc = false;
#endif
    
  char line[ 255 ];
  std::ifstream infile( m_filename_TE.c_str() );
    
  if ( infile ) {
#ifndef BEAN
          sc = StatusCode::SUCCESS;
#else
          sc = true;
#endif
    
    // Skip the header till PARAMETER
    do{
            infile.getline( line, 255 );
          } while( line[0] != 'P' );
    
    // Get the PARAMETER
    std::string sPar[2];
    char* token = strtok( line, " " );
    int ip = 0;
    do{ 
      if ( token ) { sPar[ip] = token; token = strtok( NULL, " " );}
      else continue;
      ip++; 
    } while ( token != NULL );
    long int npar = atoi( sPar[1].c_str() ); 
            
    // Skip the header till GEOMETRY
    do{
            infile.getline( line, 255 );
          } while( line[0] != 'G' );
      
    // Skip any comment before GEOMETRY 
    do{
            infile.getline( line, 255 );
          } while( line[0] != '#' );

    // Get the GEOMETRY
    infile.getline( line, 255 );
    std::string sGeom[7];
    token = strtok( line, " " );
    int ig = 0;
    do{
      if ( token ) { sGeom[ig] = token; token = strtok( NULL, " " );}
      else continue;
      ig++;
    } while (token != NULL);

    // Grid dimensions are given in cm in CDF file. Convert to CLHEP units
    m_Dxyz_TE[0] = atof( sGeom[0].c_str() ) * cm;
    m_Dxyz_TE[1] = atof( sGeom[1].c_str() ) * cm;
    m_Dxyz_TE[2] = atof( sGeom[2].c_str() ) * cm;
    m_Nxyz_TE[0] = atoi( sGeom[3].c_str() );
    m_Nxyz_TE[1] = atoi( sGeom[4].c_str() );
    m_Nxyz_TE[2] = atoi( sGeom[5].c_str() );
    m_zOffSet_TE = atof( sGeom[6].c_str() ) * cm;
    // Number of lines with data to be read
    long int nlines = ( npar - 7 ) / 3;

    // Check number of lines with data read in the loop
    long int ncheck = 0;

    // Skip comments and fill a vector of magnetic components for the
    // x, y and z positions given in GEOMETRY

        while( infile ) {
      // parse each line of the file, 
      // comment lines begin with '#' in the cdf file
            infile.getline( line, 255 );
            if ( line[0] == '#' ) continue;
            std::string gridx, gridy, gridz, sFx, sFy, sFz;
            char* token = strtok( line, " " );
      if ( token ) { gridx = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { gridy = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { gridz = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { sFx = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { sFy = token; token = strtok( NULL, " " );} else continue;
      if ( token ) { sFz = token; token = strtok( NULL, " " );} else continue;
      if ( token != NULL ) continue;
      //Grid position
      double gx = atof( gridx.c_str() ) * m;
      double gy = atof( gridy.c_str() ) * m;
      double gz = atof( gridz.c_str() ) * m;
      // Field values are given in tesla in CDF file. Convert to CLHEP units
      double fx = atof( sFx.c_str() ) * tesla;
      double fy = atof( sFy.c_str() ) * tesla;
      double fz = atof( sFz.c_str() ) * tesla;
      //for debug
      if(m_outlevel == 0) {
#ifndef BEAN
        log << MSG::DEBUG << "grid x, y, z is "<< gx << ", " << gy << ", " << gz << endreq;
        log << MSG::DEBUG << "field x, y, z is "<< fx << ", " << fy << ", " << fz << endreq;
#else
        cout << "grid x, y, z is "<< gx << ", " << gy << ", " << gz << endl;
        cout << "field x, y, z is "<< fx << ", " << fy << ", " << fz << endl;
#endif
      }
      //Fill the postion to the vector
      m_P_TE.push_back( gx );
      m_P_TE.push_back( gy );
      m_P_TE.push_back( gz );
      // Add the magnetic field components of each point to 
      // sequentialy in a vector 
      m_Q_TE.push_back( fx );
      m_Q_TE.push_back( fy );
      m_Q_TE.push_back( fz );
      // counts after reading and filling to match the number of lines
      ncheck++;
          }
    infile.close();
    if ( nlines != ncheck) {
#ifndef BEAN
      log << MSG::ERROR << "nline is " << nlines << "; ncheck is " << ncheck << " Number of points in field map does not match!"
          << endreq;
      return StatusCode::FAILURE;
#else
      cout << "nline is " << nlines << "; ncheck is " << ncheck << " Number of points in field map does not match!"
          << endl;
      return false;
#endif
    }
  }
  else {
#ifndef BEAN
        log << MSG::ERROR << "Unable to open magnetic field file : "
        << m_filename_TE << endreq;
#else
        cout << "Unable to open magnetic field file : "
        << m_filename_TE << endl;
#endif
  }

  return sc;
}
//=============================================================================
// FieldVector: find the magnetic field value at a given point in space
//=============================================================================
StatusCode MagneticFieldSvc::fieldVector(const HepPoint3D& newr, 
                                         HepVector3D& newb) const
{

 if(m_turnOffField == true) {
   newb[0] = 0.;
   newb[1] = 0.;
   newb[2] = 0.;
#ifndef BEAN
   return StatusCode::SUCCESS;
#else
   return true;
#endif
 }

 // wll added 2012-08-27
 if(m_uniField) {
	 uniFieldVector(newr,newb);
#ifndef BEAN
	 return StatusCode::SUCCESS;
#else
	 return true;
#endif
 }


 //reference frames defined by simulation and SCM are different. In simulation: x--south to north, y--down to up, but in SCM: x--north to south, y--down to up
  double new_x = -newr.x();
  double new_y = newr.y();
  double new_z = -newr.z();
  HepPoint3D r(new_x,new_y,new_z);
  HepVector3D b;
  b[0]=0.0*tesla;
  b[1]=0.0*tesla;
  b[2]=0.0*tesla;
  // This routine is now dummy. Was previously converting to/from CLHEP units
  if(-2.1*m<r.z() && r.z()<2.1*m && -1.8*m<r.x() && r.x()<1.8*m && -1.8*m<r.y() && r.y()<1.8*m)
  {
    if(-1.2*m<r.z()&&r.z()<1.2*m&&0*m<=std::sqrt(r.x()*r.x()+r.y()*r.y())&&std::sqrt(r.x()*r.x()+r.y()*r.y())<0.9*m)
    //if(std::abs(r.z())<1.2*m&&std::abs(r.x())<=0.9*m&&std::abs(r.y())<=0.9*m)
    {
      this->fieldGrid( r, b );
    }
    else
    {
      this->fieldGrid_TE( r, b );
    }
  }
  if((fabs(r.z())<=1970*mm && sqrt(r.x()*r.x()+r.y()*r.y()) >= 1740*mm) || (fabs(r.z())>=2050*mm))
  {
    HepPoint3D mr;
    HepVector3D tb;
    int part = 0, layer = 0, mat = 0;
    double theta;
    bool ifbar = false;
    bool ifend = false;
    if(r.x()!=0.){
      theta = atan2(fabs(r.y()),fabs(r.x()));
      if(0<=theta&&theta<pi/8) {
        mr[0] = fabs(r.x()); mr[1] = -fabs(r.y()); mr[2] = fabs(r.z()); 
        if(mr[2]<=1970*mm&&1740*mm<=mr[0]&&mr[0]<=2620*mm){
          part = 1;
          if(1740*mm<=mr[0]&&mr[0]<1770*mm) { layer = 0; mat = 0; }
          if(1770*mm<=mr[0]&&mr[0]<1810*mm) { layer = 0; mat = 1; }
          if(1810*mm<=mr[0]&&mr[0]<1840*mm) { layer = 1; mat = 0; }
          if(1840*mm<=mr[0]&&mr[0]<1880*mm) { layer = 1; mat = 1; }
          if(1880*mm<=mr[0]&&mr[0]<1910*mm) { layer = 2; mat = 0; }
          if(1910*mm<=mr[0]&&mr[0]<1950*mm) { layer = 2; mat = 1; }
          if(1950*mm<=mr[0]&&mr[0]<1990*mm) { layer = 3; mat = 0; }
          if(1990*mm<=mr[0]&&mr[0]<2030*mm) { layer = 3; mat = 1; }
          if(2030*mm<=mr[0]&&mr[0]<2070*mm) { layer = 4; mat = 0; }
          if(2070*mm<=mr[0]&&mr[0]<2110*mm) { layer = 4; mat = 1; }
          if(2110*mm<=mr[0]&&mr[0]<2190*mm) { layer = 5; mat = 0; }
          if(2190*mm<=mr[0]&&mr[0]<2230*mm) { layer = 5; mat = 1; }
          if(2230*mm<=mr[0]&&mr[0]<2310*mm) { layer = 6; mat = 0; }
          if(2310*mm<=mr[0]&&mr[0]<2350*mm) { layer = 6; mat = 1; }
          if(2350*mm<=mr[0]&&mr[0]<2430*mm) { layer = 7; mat = 0; }
          if(2430*mm<=mr[0]&&mr[0]<2470*mm) { layer = 7; mat = 1; }
          if(2470*mm<=mr[0]&&mr[0]<=2620*mm) { layer = 8; mat = 0; }
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1]; 
          b[2] = tb[2];
          ifbar = true;
        }
        if(2050*mm<=mr[2]&&mr[2]<2090*mm&&1034*mm<=mr[0]&&mr[0]<=2500*mm){
          part = 0; layer = 0; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1]; 
          b[2] = tb[2];
          ifend = true;
        }
        if(2090*mm<=mr[2]&&mr[2]<2130*mm&&1067*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 0; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2130*mm<=mr[2]&&mr[2]<2170*mm&&1067*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 1; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2170*mm<=mr[2]&&mr[2]<2210*mm&&1100*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 1; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2210*mm<=mr[2]&&mr[2]<2240*mm&&1100*mm<mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 2; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2240*mm<=mr[2]&&mr[2]<2280*mm&&1133*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 2; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2280*mm<=mr[2]&&mr[2]<2310*mm&&1133*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 3; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true; 
        }
        if(2310*mm<=mr[2]&&mr[2]<2350*mm&&1167*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 3; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2350*mm<=mr[2]&&mr[2]<2380*mm&&1167*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 4; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;  
        }
        if(2380*mm<=mr[2]&&mr[2]<2420*mm&&1203*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 4; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2420*mm<=mr[2]&&mr[2]<2470*mm&&1203*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 5; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2470*mm<=mr[2]&&mr[2]<2510*mm&&1241*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 5; mat =1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2510*mm<=mr[2]&&mr[2]<2590*mm&&1241*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 6; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true; 
        }
        if(2590*mm<=mr[2]&&mr[2]<2630*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 6; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2630*mm<=mr[2]&&mr[2]<2710*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 7; mat = 0; 
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2710*mm<=mr[2]&&mr[2]<2750*mm&&1362*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 7; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
        if(2750*mm<=mr[2]&&mr[2]<=2800*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 8; mat = 0; 
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0];
          b[1] = -tb[1];
          b[2] = tb[2];
          ifend = true;
        }
      }
      if(pi/8<=theta&&theta<pi/4) {
        mr[0] = fabs(r.x())*cos(pi/4)+fabs(r.y())*sin(pi/4); 
        mr[1] = -fabs(r.x())*sin(pi/4)+fabs(r.y())*cos(pi/4); 
        mr[2] = fabs(r.z());
        if(mr[2]<=1970*mm&&1740*mm<=mr[0]&&mr[0]<=2620*mm) {
          part = 1;
          if(1740*mm<=mr[0]&&mr[0]<1770*mm) { layer = 0; mat = 0; }
          if(1770*mm<=mr[0]&&mr[0]<1810*mm) { layer = 0; mat = 1; }
          if(1810*mm<=mr[0]&&mr[0]<1840*mm) { layer = 1; mat = 0; }
          if(1840*mm<=mr[0]&&mr[0]<1880*mm) { layer = 1; mat = 1; }
          if(1880*mm<=mr[0]&&mr[0]<1910*mm) { layer = 2; mat = 0; }
          if(1910*mm<=mr[0]&&mr[0]<1950*mm) { layer = 2; mat = 1; }
          if(1950*mm<=mr[0]&&mr[0]<1990*mm) { layer = 3; mat = 0; }
          if(1990*mm<=mr[0]&&mr[0]<2030*mm) { layer = 3; mat = 1; }
          if(2030*mm<=mr[0]&&mr[0]<2070*mm) { layer = 4; mat = 0; }
          if(2070*mm<=mr[0]&&mr[0]<2110*mm) { layer = 4; mat = 1; }
          if(2110*mm<=mr[0]&&mr[0]<2190*mm) { layer = 5; mat = 0; }
          if(2190*mm<=mr[0]&&mr[0]<2230*mm) { layer = 5; mat = 1; }
          if(2230*mm<=mr[0]&&mr[0]<2310*mm) { layer = 6; mat = 0; }
          if(2310*mm<=mr[0]&&mr[0]<2350*mm) { layer = 6; mat = 1; }
          if(2350*mm<=mr[0]&&mr[0]<2430*mm) { layer = 7; mat = 0; }
          if(2430*mm<=mr[0]&&mr[0]<2470*mm) { layer = 7; mat = 1; }
          if(2470*mm<=mr[0]&&mr[0]<=2620*mm) { layer = 8; mat = 0; }
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifbar = true;
        }
        if(2050*mm<=mr[2]&&mr[2]<2090*mm&&1034*mm<=mr[0]&&mr[0]<=2500*mm){
          part = 0; layer = 0; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2090*mm<=mr[2]&&mr[2]<2130*mm&&1067*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 0; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2130*mm<=mr[2]&&mr[2]<2170*mm&&1067*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 1; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true; 
        }
        if(2170*mm<=mr[2]&&mr[2]<2210*mm&&1100*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 1; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2210*mm<=mr[2]&&mr[2]<2240*mm&&1100*mm<mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 2; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2240*mm<=mr[2]&&mr[2]<2280*mm&&1133*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 2; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2280*mm<=mr[2]&&mr[2]<2310*mm&&1133*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 3; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true; 
        }
        if(2310*mm<=mr[2]&&mr[2]<2350*mm&&1167*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 3; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;  
        }
        if(2350*mm<=mr[2]&&mr[2]<2380*mm&&1167*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 4; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2380*mm<=mr[2]&&mr[2]<2420*mm&&1203*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 4; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2420*mm<=mr[2]&&mr[2]<2470*mm&&1203*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 5; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2470*mm<=mr[2]&&mr[2]<2510*mm&&1241*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 5; mat =1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2510*mm<=mr[2]&&mr[2]<2590*mm&&1241*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 6; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2590*mm<=mr[2]&&mr[2]<2630*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 6; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2630*mm<=mr[2]&&mr[2]<2710*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 7; mat = 0; 
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2710*mm<=mr[2]&&mr[2]<2750*mm&&1362*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 7; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2750*mm<=mr[2]&&mr[2]<=2800*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 8; mat = 0; 
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)-tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)+tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
      }
      if(pi/4<=theta&&theta<3*pi/8) {
        mr[0] = fabs(r.x())*cos(pi/4)+fabs(r.y())*sin(pi/4);
        mr[1] = fabs(r.x())*sin(pi/4)-fabs(r.y())*cos(pi/4);
        mr[2] = fabs(r.z());
        if(mr[2]<=1970*mm&&1740*mm<=mr[0]&&mr[0]<=2620*mm) {
          part = 1;
          if(1740*mm<=mr[0]&&mr[0]<1770*mm) { layer = 0; mat = 0; }
          if(1770*mm<=mr[0]&&mr[0]<1810*mm) { layer = 0; mat = 1; }
          if(1810*mm<=mr[0]&&mr[0]<1840*mm) { layer = 1; mat = 0; }
          if(1840*mm<=mr[0]&&mr[0]<1880*mm) { layer = 1; mat = 1; }
          if(1880*mm<=mr[0]&&mr[0]<1910*mm) { layer = 2; mat = 0; }
          if(1910*mm<=mr[0]&&mr[0]<1950*mm) { layer = 2; mat = 1; }
          if(1950*mm<=mr[0]&&mr[0]<1990*mm) { layer = 3; mat = 0; }
          if(1990*mm<=mr[0]&&mr[0]<2030*mm) { layer = 3; mat = 1; }
          if(2030*mm<=mr[0]&&mr[0]<2070*mm) { layer = 4; mat = 0; }
          if(2070*mm<=mr[0]&&mr[0]<2110*mm) { layer = 4; mat = 1; }
          if(2110*mm<=mr[0]&&mr[0]<2190*mm) { layer = 5; mat = 0; }
          if(2190*mm<=mr[0]&&mr[0]<2230*mm) { layer = 5; mat = 1; }
          if(2230*mm<=mr[0]&&mr[0]<2310*mm) { layer = 6; mat = 0; }
          if(2310*mm<=mr[0]&&mr[0]<2350*mm) { layer = 6; mat = 1; }
          if(2350*mm<=mr[0]&&mr[0]<2430*mm) { layer = 7; mat = 0; }
          if(2430*mm<=mr[0]&&mr[0]<2470*mm) { layer = 7; mat = 1; }
          if(2470*mm<=mr[0]&&mr[0]<=2620*mm) { layer = 8; mat = 0; }
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifbar = true;
        }
        if(2050*mm<=mr[2]&&mr[2]<2090*mm&&1034*mm<=mr[0]&&mr[0]<=2500*mm){
          part = 0; layer = 0; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2090*mm<=mr[2]&&mr[2]<2130*mm&&1067*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 0; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2130*mm<=mr[2]&&mr[2]<2170*mm&&1067*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 1; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2170*mm<=mr[2]&&mr[2]<2210*mm&&1100*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 1; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2210*mm<=mr[2]&&mr[2]<2240*mm&&1100*mm<mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 2; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true; 
        }
        if(2240*mm<=mr[2]&&mr[2]<2280*mm&&1133*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 2; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2280*mm<=mr[2]&&mr[2]<2310*mm&&1133*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 3; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2310*mm<=mr[2]&&mr[2]<2350*mm&&1167*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 3; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2350*mm<=mr[2]&&mr[2]<2380*mm&&1167*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 4; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true; 
        }
        if(2380*mm<=mr[2]&&mr[2]<2420*mm&&1203*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 4; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2420*mm<=mr[2]&&mr[2]<2470*mm&&1203*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 5; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2470*mm<=mr[2]&&mr[2]<2510*mm&&1241*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 5; mat =1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2510*mm<=mr[2]&&mr[2]<2590*mm&&1241*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 6; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2590*mm<=mr[2]&&mr[2]<2630*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 6; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2630*mm<=mr[2]&&mr[2]<2710*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 7; mat = 0; 
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2710*mm<=mr[2]&&mr[2]<2750*mm&&1362*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 7; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
        if(2750*mm<=mr[2]&&mr[2]<=2800*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 8; mat = 0; 
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = tb[0]*cos(pi/4)+tb[1]*sin(pi/4);
          b[1] = tb[0]*sin(pi/4)-tb[1]*cos(pi/4);
          b[2] = tb[2];
          ifend = true;
        }
      }
      if(3*pi/8<=theta&&theta<pi/2) {
        mr[0] = fabs(r.y()); mr[1] = -fabs(r.x()); mr[2] = fabs(r.z());
        if(mr[2]<=1970*mm&&1740*mm<=mr[0]&&mr[0]<=2620*mm) {
          part = 1;
          if(1740*mm<=mr[0]&&mr[0]<1770*mm) { layer = 0; mat = 0; }
          if(1770*mm<=mr[0]&&mr[0]<1810*mm) { layer = 0; mat = 1; }
          if(1810*mm<=mr[0]&&mr[0]<1840*mm) { layer = 1; mat = 0; }
          if(1840*mm<=mr[0]&&mr[0]<1880*mm) { layer = 1; mat = 1; }
          if(1880*mm<=mr[0]&&mr[0]<1910*mm) { layer = 2; mat = 0; }
          if(1910*mm<=mr[0]&&mr[0]<1950*mm) { layer = 2; mat = 1; }
          if(1950*mm<=mr[0]&&mr[0]<1990*mm) { layer = 3; mat = 0; }
          if(1990*mm<=mr[0]&&mr[0]<2030*mm) { layer = 3; mat = 1; }
          if(2030*mm<=mr[0]&&mr[0]<2070*mm) { layer = 4; mat = 0; }
          if(2070*mm<=mr[0]&&mr[0]<2110*mm) { layer = 4; mat = 1; }
          if(2110*mm<=mr[0]&&mr[0]<2190*mm) { layer = 5; mat = 0; }
          if(2190*mm<=mr[0]&&mr[0]<2230*mm) { layer = 5; mat = 1; }
          if(2230*mm<=mr[0]&&mr[0]<2310*mm) { layer = 6; mat = 0; }
          if(2310*mm<=mr[0]&&mr[0]<2350*mm) { layer = 6; mat = 1; }
          if(2350*mm<=mr[0]&&mr[0]<2430*mm) { layer = 7; mat = 0; }
          if(2430*mm<=mr[0]&&mr[0]<2470*mm) { layer = 7; mat = 1; }
          if(2470*mm<=mr[0]&&mr[0]<=2620*mm) { layer = 8; mat = 0; }
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifbar = true;
        }
        if(2050*mm<=mr[2]&&mr[2]<2090*mm&&1034*mm<=mr[0]&&mr[0]<=2500*mm){
          part = 0; layer = 0; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0]; 
          b[2] = tb[2];
          ifend = true;
        }
        if(2090*mm<=mr[2]&&mr[2]<2130*mm&&1067*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 0; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2130*mm<=mr[2]&&mr[2]<2170*mm&&1067*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 1; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2170*mm<=mr[2]&&mr[2]<2210*mm&&1100*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 1; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true; 
        }
        if(2210*mm<=mr[2]&&mr[2]<2240*mm&&1100*mm<mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 2; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2240*mm<=mr[2]&&mr[2]<2280*mm&&1133*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 2; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2280*mm<=mr[2]&&mr[2]<2310*mm&&1133*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 3; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2310*mm<=mr[2]&&mr[2]<2350*mm&&1167*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 3; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2350*mm<=mr[2]&&mr[2]<2380*mm&&1167*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 4; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2]; 
          ifend = true;
        }
        if(2380*mm<=mr[2]&&mr[2]<2420*mm&&1203*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 4; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2420*mm<=mr[2]&&mr[2]<2470*mm&&1203*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 5; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2470*mm<=mr[2]&&mr[2]<2510*mm&&1241*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 5; mat =1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true; 
        }
        if(2510*mm<=mr[2]&&mr[2]<2590*mm&&1241*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 6; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2590*mm<=mr[2]&&mr[2]<2630*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 6; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2630*mm<=mr[2]&&mr[2]<2710*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 7; mat = 0; 
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2710*mm<=mr[2]&&mr[2]<2750*mm&&1362*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 7; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2750*mm<=mr[2]&&mr[2]<=2800*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 8; mat = 0; 
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
      }
    }
    if(r.x()==0.) {
        mr[0] = fabs(r.y()); mr[1] = -fabs(r.x()); mr[2] = fabs(r.z());
        if(mr[2]<=1970*mm&&1740*mm<=mr[0]&&mr[0]<=2620*mm) {
          part = 1;
          if(1740*mm<=mr[0]&&mr[0]<1770*mm) { layer = 0; mat = 0; }
          if(1770*mm<=mr[0]&&mr[0]<1810*mm) { layer = 0; mat = 1; }
          if(1810*mm<=mr[0]&&mr[0]<1840*mm) { layer = 1; mat = 0; }
          if(1840*mm<=mr[0]&&mr[0]<1880*mm) { layer = 1; mat = 1; }
          if(1880*mm<=mr[0]&&mr[0]<1910*mm) { layer = 2; mat = 0; }
          if(1910*mm<=mr[0]&&mr[0]<1950*mm) { layer = 2; mat = 1; }
          if(1950*mm<=mr[0]&&mr[0]<1990*mm) { layer = 3; mat = 0; }
          if(1990*mm<=mr[0]&&mr[0]<2030*mm) { layer = 3; mat = 1; }
          if(2030*mm<=mr[0]&&mr[0]<2070*mm) { layer = 4; mat = 0; }
          if(2070*mm<=mr[0]&&mr[0]<2110*mm) { layer = 4; mat = 1; }
          if(2110*mm<=mr[0]&&mr[0]<2190*mm) { layer = 5; mat = 0; }
          if(2190*mm<=mr[0]&&mr[0]<2230*mm) { layer = 5; mat = 1; }
          if(2230*mm<=mr[0]&&mr[0]<2310*mm) { layer = 6; mat = 0; }
          if(2310*mm<=mr[0]&&mr[0]<2350*mm) { layer = 6; mat = 1; }
          if(2350*mm<=mr[0]&&mr[0]<2430*mm) { layer = 7; mat = 0; }
          if(2430*mm<=mr[0]&&mr[0]<2470*mm) { layer = 7; mat = 1; }
          if(2470*mm<=mr[0]&&mr[0]<=2620*mm) { layer = 8; mat = 0; }
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifbar = true;
        }
        if(2050*mm<=mr[2]&&mr[2]<2090*mm&&1034*mm<=mr[0]&&mr[0]<=2500*mm){
          part = 0; layer = 0; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0]; 
          b[2] = tb[2];
          ifend = true;
        }
        if(2090*mm<=mr[2]&&mr[2]<2130*mm&&1067*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 0; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2130*mm<=mr[2]&&mr[2]<2170*mm&&1067*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 1; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2170*mm<=mr[2]&&mr[2]<2210*mm&&1100*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 1; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2210*mm<=mr[2]&&mr[2]<2240*mm&&1100*mm<mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 2; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2240*mm<=mr[2]&&mr[2]<2280*mm&&1133*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 2; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2280*mm<=mr[2]&&mr[2]<2310*mm&&1133*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 3; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2310*mm<=mr[2]&&mr[2]<2350*mm&&1167*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 3; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;  
        }
        if(2350*mm<=mr[2]&&mr[2]<2380*mm&&1167*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 4; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2380*mm<=mr[2]&&mr[2]<2420*mm&&1203*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 4; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true; 
        }
        if(2420*mm<=mr[2]&&mr[2]<2470*mm&&1203*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 5; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2470*mm<=mr[2]&&mr[2]<2510*mm&&1241*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 5; mat =1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2510*mm<=mr[2]&&mr[2]<2590*mm&&1241*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 6; mat = 0;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2590*mm<=mr[2]&&mr[2]<2630*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 6; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2630*mm<=mr[2]&&mr[2]<2710*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 7; mat = 0; 
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
        if(2710*mm<=mr[2]&&mr[2]<2750*mm&&1362*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 7; mat = 1;
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true; 
        }
        if(2750*mm<=mr[2]&&mr[2]<=2800*mm&&1302*mm<=mr[0]&&mr[0]<=2500*mm) {
          part = 0; layer = 8; mat = 0; 
          m_Mucfield->getMucField(part,layer,mat,mr,tb);
          b[0] = -tb[1];
          b[1] = tb[0];
          b[2] = tb[2];
          ifend = true;
        }
    }
    if(ifbar==true||ifend==true) {
      if( r.x() < 0. && r.y() >= 0. && r.z() > 0. ){
        b[0] = -b[0];
      }
      else if(  r.x() <= 0. &&  r.y()  < 0. && r.z() > 0. ){
        b[0] = -b[0];
        b[1] = -b[1];
      }
      else if(  r.x() > 0. &&  r.y()  < 0. && r.z() > 0. ){
        b[1] = -b[1];
      }
      else if(  r.x() >= 0. &&  r.y()  > 0. && r.z() < 0. ){
        b[0] = -b[0];
        b[1] = -b[1];
      }
      else if(  r.x() < 0. &&  r.y()  >= 0. && r.z() < 0. ){
        b[1] = -b[1];
      }
      else if(  r.x() <= 0. &&  r.y()  < 0. && r.z() < 0. ){
        b[0] = b[0];
        b[1] = b[1];
      }
      else if(  r.x() > 0. &&  r.y()  <= 0. && r.z() < 0. ){
        b[0] = -b[0];
      }
    }
  }

  //reference frames defined by simulation and SCM are different.
  newb[0] = -b[0] * m_scale;
  newb[1] = b[1] * m_scale;
  newb[2] = -b[2] * m_scale;
/*  if(-1.2*m<r.z()&&r.z()<1.2*m&&0*m<=std::sqrt(r.x()*r.x()+r.y()*r.y())&&std::sqrt(r.x()*r.x()+r.y()*r.y())<0.9*m) {
     return StatusCode::SUCCESS;
  }
  else {
    newb[0] = newb[0] - 0.10*tesla;
    newb[1] = newb[1] + 0.10*tesla;
    newb[2] = newb[2] - 0.10*tesla;
  }*/

  //newb[0] = b[0];
  //newb[1] = b[1];
  //newb[2] = b[2];

#ifndef BEAN
 return StatusCode::SUCCESS;
#else
  return true;
#endif
}

StatusCode MagneticFieldSvc::uniFieldVector(const HepPoint3D& r,
                                         HepVector3D& b) const
{
  if(m_runmode == 8 || m_runmode == 7) {
    if(-1.5*m<=r.z()&&r.z()<=1.5*m&&0*m<=std::sqrt(r.x()*r.x()+r.y()*r.y())&&std::sqrt(r.x()*r.x()+r.y()*r.y())<=1.5*m)
    {
      b[0]=0.0;
      b[1]=0.0;
      b[2]=-0.9*tesla;
    }
    else 
    {
      b[0]=0.0;
      b[1]=0.0;
      b[2]=0.0;
    }
  }
  else {
    if(-1.5*m<=r.z()&&r.z()<=1.5*m&&0*m<=std::sqrt(r.x()*r.x()+r.y()*r.y())&&std::sqrt(r.x()*r.x()+r.y()*r.y())<=1.5*m)
    {
      b[0]=0.0;
      b[1]=0.0;
      b[2]=-1.0*tesla;
    }
    else
    {
      b[0]=0.0;
      b[1]=0.0;
      b[2]=0.0;
    }
  }

 if(m_turnOffField == true) {
   b[0] = 0.;
   b[1] = 0.;
   b[2] = 0.;
 }
 //yzhang add 2012-04-25 
 b[0] *= m_scale;
 b[1] *= m_scale;
 b[2] *= m_scale;
#ifndef BEAN
  return StatusCode::SUCCESS;
#else
  return true;
#endif
}

double MagneticFieldSvc::getReferField()
{
  if(m_useDB == false) {
    if(m_runmode == 8 || m_runmode == 7) m_zfield = -0.9*tesla;
    else m_zfield = -1.0*tesla;
  }

 if(m_turnOffField == true) {
   m_zfield = 0.;
 }
  return m_zfield * m_scale;
}

bool  MagneticFieldSvc::ifRealField() const { 
  return m_ifRealField; 
}

//=============================================================================
// routine to fill the field vector
//=============================================================================
void MagneticFieldSvc::fieldGrid (const HepPoint3D& r, 
                                   HepVector3D& bf ) const {
    
  bf[0] = 0.0;
  bf[1] = 0.0;
  bf[2] = 0.0;

  ///  Linear interpolated field
  double z =  r.z() - m_zOffSet;
  if( z < m_min_FL[2] || z > m_max_FL[2] )  return;
  double x =  r.x();  
  if( x < m_min_FL[0] || x > m_max_FL[0] )  return;
  double y =  r.y();
  if( y < m_min_FL[1] || y > m_max_FL[1] )  return;
  int i = int( (x-m_min_FL[0])/m_Dxyz[0]);
  int j = int( (y-m_min_FL[1])/m_Dxyz[1] );
  int k = int( (z-m_min_FL[2])/m_Dxyz[2] );
  
  int ijk000 = 3*( m_Nxyz[0]*( m_Nxyz[1]*k     + j )     + i );
  int ijk001 = 3*( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j )     + i );
  int ijk010 = 3*( m_Nxyz[0]*( m_Nxyz[1]*k     + j + 1 ) + i );
  int ijk011 = 3*( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j + 1)  + i );
  int ijk100 = 3*( m_Nxyz[0]*( m_Nxyz[1]*k     + j)      + i + 1 );
  int ijk101 = 3*( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j)      + i + 1 );
  int ijk110 = 3*( m_Nxyz[0]*( m_Nxyz[1]*k     + j + 1)  + i + 1 );
  int ijk111 = 3*( m_Nxyz[0]*( m_Nxyz[1]*(k+1) + j + 1 ) + i + 1 );

  // auxiliary variables defined at the vertices of the cube that
  // contains the (x, y, z) point where the field is interpolated
/*  std::cout<<"x,y,z: "<<x<<","<<y<<","<<z<<std::endl;
  std::cout<<"point1(x,y,z): "<<m_P[ijk000]<<","<<m_P[ijk000+1]<<","<<m_P[ijk000+2]<<std::endl;
  std::cout<<"point2(x,y,z): "<<m_P[ijk001]<<","<<m_P[ijk001+1]<<","<<m_P[ijk001+2]<<std::endl;
  std::cout<<"point3(x,y,z): "<<m_P[ijk010]<<","<<m_P[ijk010+1]<<","<<m_P[ijk010+2]<<std::endl;
  std::cout<<"point4(x,y,z): "<<m_P[ijk011]<<","<<m_P[ijk011+1]<<","<<m_P[ijk011+2]<<std::endl;
  std::cout<<"point5(x,y,z): "<<m_P[ijk100]<<","<<m_P[ijk100+1]<<","<<m_P[ijk100+2]<<std::endl;
  std::cout<<"point6(x,y,z): "<<m_P[ijk101]<<","<<m_P[ijk101+1]<<","<<m_P[ijk101+2]<<std::endl;
  std::cout<<"point7(x,y,z): "<<m_P[ijk110]<<","<<m_P[ijk110+1]<<","<<m_P[ijk110+2]<<std::endl;
  std::cout<<"point8(x,y,z): "<<m_P[ijk111]<<","<<m_P[ijk111+1]<<","<<m_P[ijk111+2]<<std::endl;
  
  if(std::fabs(m_P[ijk000]-x)>m_Dxyz[0]||std::fabs(m_P[ijk001]-x)>m_Dxyz[0]||std::fabs(m_P[ijk010]-x)>m_Dxyz[0]||std::fabs(m_P[ijk011]-x)>m_Dxyz[0]||std::fabs(m_P[ijk100]-x)>m_Dxyz[0]||std::fabs(m_P[ijk101]-x)>m_Dxyz[0]||std::fabs(m_P[ijk110]-x)>m_Dxyz[0]||std::fabs(m_P[ijk111]-x)>m_Dxyz[0]) 
	  std::cout<<"FATALERRORX****************************"<<std::endl;
  if(std::fabs(m_P[ijk000+1]-y)>m_Dxyz[1]||std::fabs(m_P[ijk001+1]-y)>m_Dxyz[1]||std::fabs(m_P[ijk010+1]-y)>m_Dxyz[1]||std::fabs(m_P[ijk011+1]-y)>m_Dxyz[1]||std::fabs(m_P[ijk100+1]-y)>m_Dxyz[1]||std::fabs(m_P[ijk101+1]-y)>m_Dxyz[1]||std::fabs(m_P[ijk110+1]-y)>m_Dxyz[1]||std::fabs(m_P[ijk111+1]-y)>m_Dxyz[1])
	            std::cout<<"FATALERRORY***************************"<<std::endl;
  if(std::fabs(m_P[ijk000+2]-z)>m_Dxyz[2]||std::fabs(m_P[ijk001+2]-z)>m_Dxyz[2]||std::fabs(m_P[ijk010+2]-z)>m_Dxyz[2]||std::fabs(m_P[ijk011+2]-z)>m_Dxyz[2]||std::fabs(m_P[ijk100+2]-z)>m_Dxyz[2]||std::fabs(m_P[ijk101+2]-z)>m_Dxyz[2]||std::fabs(m_P[ijk110+2]-z)>m_Dxyz[2]||std::fabs(m_P[ijk111+2]-z)>m_Dxyz[2])
	                      std::cout<<"FATALERRORZ****************************"<<std::endl; */
  double cx000 = m_Q[ ijk000 ];
  double cx001 = m_Q[ ijk001 ];
  double cx010 = m_Q[ ijk010 ];
  double cx011 = m_Q[ ijk011 ];
  double cx100 = m_Q[ ijk100 ];
  double cx101 = m_Q[ ijk101 ];
  double cx110 = m_Q[ ijk110 ];
  double cx111 = m_Q[ ijk111 ];
  double cy000 = m_Q[ ijk000+1 ];
  double cy001 = m_Q[ ijk001+1 ];
  double cy010 = m_Q[ ijk010+1 ];
  double cy011 = m_Q[ ijk011+1 ];
  double cy100 = m_Q[ ijk100+1 ];
  double cy101 = m_Q[ ijk101+1 ];
  double cy110 = m_Q[ ijk110+1 ];
  double cy111 = m_Q[ ijk111+1 ];
  double cz000 = m_Q[ ijk000+2 ];
  double cz001 = m_Q[ ijk001+2 ];
  double cz010 = m_Q[ ijk010+2 ];
  double cz011 = m_Q[ ijk011+2 ];
  double cz100 = m_Q[ ijk100+2 ];
  double cz101 = m_Q[ ijk101+2 ];
  double cz110 = m_Q[ ijk110+2 ];
  double cz111 = m_Q[ ijk111+2 ];
  double hx1 = ( x-m_min_FL[0]-i*m_Dxyz[0] )/m_Dxyz[0];
  double hy1 = ( y-m_min_FL[1]-j*m_Dxyz[1] )/m_Dxyz[1];
  double hz1 = ( z-m_min_FL[2]-k*m_Dxyz[2] )/m_Dxyz[2];
  double hx0 = 1.0-hx1;
  double hy0 = 1.0-hy1;
  double hz0 = 1.0-hz1;
  double h000 = hx0*hy0*hz0;
  if( fabs(h000) > 0.0 &&
      cx000 > 9.0e5 && cy000 > 9.0e5 && cz000 > 9.0e5) return;
 
  double h001 = hx0*hy0*hz1;
  if( fabs(h001) > 0.0 && 
      cx001 > 9.0e5 && cy001 > 9.0e5 && cz001 > 9.0e5) return;

  double h010 = hx0*hy1*hz0;
  if( fabs(h010) > 0.0 && 
      cx010 > 9.0e5 && cy010 > 9.0e5 && cz010 > 9.0e5) return;

  double h011 = hx0*hy1*hz1;
  if( fabs(h011) > 0.0 && 
      cx011 > 9.0e5 && cy011 > 9.0e5 && cz011 > 9.0e5) return;

  double h100 = hx1*hy0*hz0;
  if( fabs(h100) > 0.0 && 
      cx100 > 9.0e5 && cy100 > 9.0e5 && cz100 > 9.0e5) return;
 
  double h101 = hx1*hy0*hz1;
  if( fabs(h101) > 0.0 && 
      cx101 > 9.0e5 && cy101 > 9.0e5 && cz101 > 9.0e5) return;
 
  double h110 = hx1*hy1*hz0;
  if( fabs(h110) > 0.0 && 
      cx110 > 9.0e5 && cy110 > 9.0e5 && cz110 > 9.0e5) return;

  double h111 = hx1*hy1*hz1;
  if( fabs(h111) > 0.0 && 
      cx111 > 9.0e5 && cy111 > 9.0e5 && cz111 > 9.0e5) return;

  bf(0) = ( cx000*h000 + cx001*h001 + cx010*h010 + cx011*h011 +
            cx100*h100 + cx101*h101 + cx110*h110 + cx111*h111);
  bf(1) = ( cy000*h000 + cy001*h001 + cy010*h010 + cy011*h011 +
            cy100*h100 + cy101*h101 + cy110*h110 + cy111*h111 );
  bf(2) = ( cz000*h000 + cz001*h001 + cz010*h010 + cz011*h011 +
            cz100*h100 + cz101*h101 + cz110*h110 + cz111*h111 );
  return;      
}

//=============================================================================
// routine to fill the field vector
//=============================================================================
void MagneticFieldSvc::fieldGrid_TE (const HepPoint3D& r,
                                   HepVector3D& bf ) const {

  bf[0] = 0.0;
  bf[1] = 0.0;
  bf[2] = 0.0;

  ///  Linear interpolated field
  double z =  r.z() - m_zOffSet_TE;
  if( fabs(z) < m_min_FL_TE[2] || fabs(z) > m_max_FL_TE[2] )  return;
  double x =  r.x();
  if( fabs(x) < m_min_FL_TE[0] || fabs(x) > m_max_FL_TE[0] )  return;
  double y =  r.y();
  if( fabs(y) < m_min_FL_TE[1] || fabs(y) > m_max_FL_TE[1] )  return;
  int i = int( (fabs(x)-m_min_FL_TE[0])/m_Dxyz_TE[0]);
  int j = int( (fabs(y)-m_min_FL_TE[1])/m_Dxyz_TE[1] );
  int k = int( (fabs(z)-m_min_FL_TE[2])/m_Dxyz_TE[2] );

  int ijk000 = 3*( m_Nxyz_TE[0]*( m_Nxyz_TE[1]*k     + j )     + i );
  int ijk001 = 3*( m_Nxyz_TE[0]*( m_Nxyz_TE[1]*(k+1) + j )     + i );
  int ijk010 = 3*( m_Nxyz_TE[0]*( m_Nxyz_TE[1]*k     + j + 1 ) + i );
  int ijk011 = 3*( m_Nxyz_TE[0]*( m_Nxyz_TE[1]*(k+1) + j + 1)  + i );
  int ijk100 = 3*( m_Nxyz_TE[0]*( m_Nxyz_TE[1]*k     + j)      + i + 1 );
  int ijk101 = 3*( m_Nxyz_TE[0]*( m_Nxyz_TE[1]*(k+1) + j)      + i + 1 );
  int ijk110 = 3*( m_Nxyz_TE[0]*( m_Nxyz_TE[1]*k     + j + 1)  + i + 1 );
  int ijk111 = 3*( m_Nxyz_TE[0]*( m_Nxyz_TE[1]*(k+1) + j + 1 ) + i + 1 );

  // auxiliary variables defined at the vertices of the cube that
  // contains the (x, y, z) point where the field is interpolated
/*  std::cout<<"x,y,z: "<<x<<","<<y<<","<<z<<std::endl;
  std::cout<<"point1(x,y,z): "<<m_P[ijk000]<<","<<m_P[ijk000+1]<<","<<m_P[ijk000+2]<<std::endl;
  std::cout<<"point2(x,y,z): "<<m_P[ijk001]<<","<<m_P[ijk001+1]<<","<<m_P[ijk001+2]<<std::endl;
  std::cout<<"point3(x,y,z): "<<m_P[ijk010]<<","<<m_P[ijk010+1]<<","<<m_P[ijk010+2]<<std::endl;
  std::cout<<"point4(x,y,z): "<<m_P[ijk011]<<","<<m_P[ijk011+1]<<","<<m_P[ijk011+2]<<std::endl;
  std::cout<<"point5(x,y,z): "<<m_P[ijk100]<<","<<m_P[ijk100+1]<<","<<m_P[ijk100+2]<<std::endl;
  std::cout<<"point6(x,y,z): "<<m_P[ijk101]<<","<<m_P[ijk101+1]<<","<<m_P[ijk101+2]<<std::endl;
  std::cout<<"point7(x,y,z): "<<m_P[ijk110]<<","<<m_P[ijk110+1]<<","<<m_P[ijk110+2]<<std::endl;
  std::cout<<"point8(x,y,z): "<<m_P[ijk111]<<","<<m_P[ijk111+1]<<","<<m_P[ijk111+2]<<std::endl;
  */
  double cx000 = m_Q_TE[ ijk000 ];
  double cx001 = m_Q_TE[ ijk001 ];
  double cx010 = m_Q_TE[ ijk010 ];
  double cx011 = m_Q_TE[ ijk011 ];
  double cx100 = m_Q_TE[ ijk100 ];
  double cx101 = m_Q_TE[ ijk101 ];
  double cx110 = m_Q_TE[ ijk110 ];
  double cx111 = m_Q_TE[ ijk111 ];
  double cy000 = m_Q_TE[ ijk000+1 ];
  double cy001 = m_Q_TE[ ijk001+1 ];
  double cy010 = m_Q_TE[ ijk010+1 ];
  double cy011 = m_Q_TE[ ijk011+1 ];
  double cy100 = m_Q_TE[ ijk100+1 ];
  double cy101 = m_Q_TE[ ijk101+1 ];
  double cy110 = m_Q_TE[ ijk110+1 ];
  double cy111 = m_Q_TE[ ijk111+1 ];
  double cz000 = m_Q_TE[ ijk000+2 ];
  double cz001 = m_Q_TE[ ijk001+2 ];
  double cz010 = m_Q_TE[ ijk010+2 ];
  double cz011 = m_Q_TE[ ijk011+2 ];
  double cz100 = m_Q_TE[ ijk100+2 ];
  double cz101 = m_Q_TE[ ijk101+2 ];
  double cz110 = m_Q_TE[ ijk110+2 ];
  double cz111 = m_Q_TE[ ijk111+2 ];
  double hx1 = ( fabs(x)-m_min_FL_TE[0]-i*m_Dxyz_TE[0] )/m_Dxyz_TE[0];
  double hy1 = ( fabs(y)-m_min_FL_TE[1]-j*m_Dxyz_TE[1] )/m_Dxyz_TE[1];
  double hz1 = ( fabs(z)-m_min_FL_TE[2]-k*m_Dxyz_TE[2] )/m_Dxyz_TE[2];
  double hx0 = 1.0-hx1;
  double hy0 = 1.0-hy1;
  double hz0 = 1.0-hz1;
  double h000 = hx0*hy0*hz0;
  if( fabs(h000) > 0.0 &&
      cx000 > 9.0e5 && cy000 > 9.0e5 && cz000 > 9.0e5) return;

  double h001 = hx0*hy0*hz1;
  if( fabs(h001) > 0.0 &&
      cx001 > 9.0e5 && cy001 > 9.0e5 && cz001 > 9.0e5) return;

  double h010 = hx0*hy1*hz0;
  if( fabs(h010) > 0.0 &&
      cx010 > 9.0e5 && cy010 > 9.0e5 && cz010 > 9.0e5) return;

  double h011 = hx0*hy1*hz1;
  if( fabs(h011) > 0.0 &&
      cx011 > 9.0e5 && cy011 > 9.0e5 && cz011 > 9.0e5) return;

  double h100 = hx1*hy0*hz0;
  if( fabs(h100) > 0.0 &&
      cx100 > 9.0e5 && cy100 > 9.0e5 && cz100 > 9.0e5) return;

  double h101 = hx1*hy0*hz1;
  if( fabs(h101) > 0.0 &&
      cx101 > 9.0e5 && cy101 > 9.0e5 && cz101 > 9.0e5) return;

  double h110 = hx1*hy1*hz0;
  if( fabs(h110) > 0.0 &&
      cx110 > 9.0e5 && cy110 > 9.0e5 && cz110 > 9.0e5) return;

  double h111 = hx1*hy1*hz1;
  if( fabs(h111) > 0.0 &&
      cx111 > 9.0e5 && cy111 > 9.0e5 && cz111 > 9.0e5) return;

  bf(0) = ( cx000*h000 + cx001*h001 + cx010*h010 + cx011*h011 +
            cx100*h100 + cx101*h101 + cx110*h110 + cx111*h111);
  bf(1) = ( cy000*h000 + cy001*h001 + cy010*h010 + cy011*h011 +
            cy100*h100 + cy101*h101 + cy110*h110 + cy111*h111 );
  bf(2) = ( cz000*h000 + cz001*h001 + cz010*h010 + cz011*h011 +
            cz100*h100 + cz101*h101 + cz110*h110 + cz111*h111 );

            
  if( r.x() < 0. && r.y() >= 0. && r.z() > 0. ){
    bf(0) = -bf(0);
  }
  else if(  r.x() <= 0. &&  r.y()  < 0. && r.z() > 0. ){
    bf(0) = -bf(0);
    bf(1) = -bf(1);
  }
  else if(  r.x() > 0. &&  r.y()  < 0. && r.z() > 0. ){
    bf(1) = -bf(1);
  }

  else if(  r.x() >= 0. &&  r.y()  > 0. && r.z() < 0. ){
    bf(0) = -bf(0);
    bf(1) = -bf(1);
  }
  else if(  r.x() < 0. &&  r.y()  >= 0. && r.z() < 0. ){
    bf(1) = -bf(1);
  }
  else if(  r.x() <= 0. &&  r.y()  < 0. && r.z() < 0. ){
    bf(0) = bf(0);
    bf(1) = bf(1);
  }
  else if(  r.x() > 0. &&  r.y()  <= 0. && r.z() < 0. ){
    bf(0) = -bf(0);
  }

  return;
}
