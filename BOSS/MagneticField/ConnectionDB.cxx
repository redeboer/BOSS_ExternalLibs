#ifndef BEAN
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"

#include "facilities/Util.h"
#else
#include "DatabaseSvc/DatabaseSvc.h"
#endif

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "MagneticField/ConnectionDB.h"

using namespace std;

namespace FieldDBUtil {

#ifndef BEAN
  ConnectionDB::ConnectionDB() {
    StatusCode sc = Gaudi::svcLocator()->service("DatabaseSvc", m_dbsvc, true);
    if (sc .isFailure() ) {
      std::cout << "ERROR: In ConnectionDB()--> Unable to find DatabaseSvc " << std::endl;
    }
  }
#endif

  ConnectionDB::eRet ConnectionDB::getReadSC_MagnetInfo( std::vector<double>& current, int runNo) {

#ifdef BEAN
    static DatabaseSvc* m_dbsvc = 0;          
                                         
    if( !m_dbsvc ) {                          
      m_dbsvc = DatabaseSvc::instance();      
    }                                         
#endif

    //Read magnetic field map                                                                                             
    char stmt1[200];
    int run_No = std::abs(runNo);                                                                                             

    sprintf(stmt1,"select Magnet_Current,SCQL,SCQR from SC_magnet where run_number = %d ",run_No);

    DatabaseRecordVector results;                                                                                         
    results.clear();                                                                                                      
    int status = m_dbsvc->query("run",stmt1,results);                                                                      

    if(status<0){                                                                                                         
      std::cout << "ERROR Read the SSM and SCQ current from the Database" << endl;                                        
      exit(1);                                                                                                            
    }                                                                                                                     
                                                                                                                          
    int RowNumber = results.size();                                                                                       
    if(RowNumber!=1){                                                                                                     
      std::cout<<"ERROR:error searching SC_Magnet Data in the database, check your selection criterions"<<std::endl;      
      return RETMySQLError;                                                                                               
    }                                                                                                                     

    DatabaseRecord& rec = *results[0];                                                                                    
    double ssm_curr = rec.GetDouble("Magnet_Current");                                                                    
    double scql_curr = rec.GetDouble("SCQL");                                                                             
    double scqr_curr = rec.GetDouble("SCQR");                                                                             

    // save results in vector                                                                                             
    current.resize(3);                                                                                                    
    current[0] = ssm_curr;                                                                                                
    current[1] = scql_curr;                                                                                               
    current[2] = scqr_curr;                                                                                               

    return RETOk;                                                                                                         
  }

  ConnectionDB::eRet ConnectionDB::getBeamEnergy( std::vector<double>& beamE, int runNo) {
    //Read magnetic field map
     char stmt1[200];
     int run_No = std::abs(runNo);

     sprintf(stmt1,"select BPR_PRB,BER_PRB from RunParams where run_number = %d ",run_No);

     DatabaseRecordVector results;
     results.clear();
     int status = m_dbsvc->query("run",stmt1,results);
     if(status<0){
       std::cout << "ERROR Read the beam energy from the Database" << endreq;
       exit(1);
     }

     int RowNumber = results.size();

     if(RowNumber == 0) {
       beamE.push_back(1.843); // for positron
       beamE.push_back(1.843); // for electron

       return RETOk;
     }

     if(RowNumber!=1){
         std::cout<<"ERROR:error searching beam energy in the database, check your selection criterions"<<std::endl;
        return RETMySQLError;
     }

     beamE.push_back(atof((*results[0])["BPR_PRB"])); // for positron
     beamE.push_back(atof((*results[0])["BER_PRB"])); // for electron

     return RETOk;
  }                                                                                                                       

}
