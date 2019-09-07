#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"

#include "EmcCalibConstSvc/EmcCalibConstSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "RealizationSvc/ConnectionProvider.h"
#include "facilities/Util.h"
#include <iostream>
#include <cstdio>
#include <boost/lexical_cast.hpp>
#include <cstdlib>

using namespace std;

namespace RealDBUtil {

  #define _T(str) ("\'"+str+"\'")
  typedef basic_string<char>::size_type S_T;
  static const  S_T npos = -1;

  std::vector<std::string> ConnectionProvider::split(const std::string& src, std::string delimit, std::string null_subst)
  {
    if( src.empty() || delimit.empty() ) throw "split:  empty string\0";
    vector<string> v;
    S_T deli_len = delimit.size();
    long index = npos, last_search_position = 0;
    while( (index=src.find(delimit, last_search_position))!=npos )
    {
      if(index==last_search_position)
        v.push_back(null_subst);
      else
        v.push_back( src.substr(last_search_position, index-
              last_search_position) );
      last_search_position = index + deli_len;
    }
    string last_one = src.substr(last_search_position);
    v.push_back( last_one.empty()? null_subst:last_one );
    return v;
  }

  bool ConnectionProvider::getcal(int runNo, std::string ids){

    //Read Luminosity information
    char stmt1[1024];
    int run_No =std::abs(runNo);
    sprintf(stmt1,"select EQM_fileid,EMCGain from RunParams where run_number = %d",run_No);

    DatabaseRecordVector results;
    results.clear(); 
    int rowNumber = m_dbsvc->query("run",stmt1,results);
    if(rowNumber <= 0){
      std::cout << "ERROR Read EQM_fileid from the Database, exit." << endreq;
      exit(1);
    } 

    int fileid = std::atoi((*results[0])["EQM_fileid"]);
    
    string gainnum((*results[0])["EMCGain"]);
   
    sprintf(stmt1, "select OID,Position,Channel1,Channel2,Channel3,Channel4,Channel5,Channel6,Channel7,Channel8,Channel9,Channel10,Channel11,Channel12,Channel13,Channel14,Channel15,Channel16,Channel17,Channel18,Channel19,Channel20,Channel21,Channel22,Channel23,Channel24,Channel25,Channel26,Channel27,Channel28,Channel29,Channel30,Channel31,Channel32 from EQM where FileId=%d", fileid);
  
    results.clear();
    rowNumber = m_dbsvc->query("run",stmt1,results);
    if(rowNumber <= 0){
      std::cout << "ERROR Read EQM from the Database, exit." << endreq;
      exit(1);
    }

    std::map<std::string, std::vector<std::string> > map_EQM;
    map_EQM.clear();
    for(int i=0;i<rowNumber;i++)
    {  for(int j=2;j<34;j++)
      {
        std::vector<std::string> vec_tmp;
        vec_tmp.clear();
        vec_tmp.push_back(toString<int>(j-1));
        vec_tmp.push_back((*results[i])["OID"]);
        vec_tmp.push_back((*results[i])["Position"]);
        typedef pair<std::string, std::vector<std::string> > vpair;
        map_EQM.insert(vpair((*results[i])["Channel"+toString<int>(j-1)], vec_tmp));
  
      }
    }
  
    vector<string> id = split(ids, ",", "<null>");
    id_num=id.size();

    for(unsigned int m=0;m<id.size();m++){
      if(id[m] == "0") {  std::cout << " error: No." << m+1 << " id=0,please delete it! " << std::endl; break; }
      if(id[m] == "NULL") {  std::cout << " end of loop! " << std::endl; break; }
      string channelnum,oid,modu;
      bool if_found = false;
      for(std::map<std::string,std::vector<std::string> >::iterator iter = map_EQM.begin(); iter != map_EQM.end(); iter++)
      {
        if(iter->first == id[m]) {
          if_found = true;
          channelnum = iter->second[0];
          oid = iter->second[1];
          modu = iter->second[2];
        }
      }

      if(!if_found) { std::cout << "can not find id " << id[m] << " in EQM, exit." << std::endl;  exit(1); }

      string b="AAAA";
      string trgcratenum;
      for(int i=0;i<4;i++){
        b[i] =oid[i+4];
      }
     
      //change oid to cratenum
      string a[16][2]={{"E3_1","96"},{"E2_0","97"},{"E2_1","98"},{"E1_0","99"},{"E1_1","100"},{"E3_0","101"},{"E2_2","102"},{"E3_2","103"},{"W3_1","112"},{"W2_0","113"},{"W2_1","114"},{"W1_0","115"},{"W1_1","116"},{"W3_0","117"},{"W2_2","118"},{"W3_2","119"}};
      string trgCrate[16]={"0x60","0x61","0x62","0x63","0x64","0x65","0x66","0x67","0x70","0x71","0x72","0x73","0x74","0x75","0x76","0x77"};
  
      for(int i=0;i<16;i++){
        if(b.compare(a[i][0])) continue;
        else {
          trgcratenum = trgCrate[i];
        }
      }
  
      std::string query_name = "select gain from EMC_Gain where run_id="+gainnum+" and ppc_id=\""+trgcratenum+"\" and Qmodule_no="+modu+" and Qchannel_no="+channelnum; 
      results.clear();
      rowNumber = m_dbsvc->query("run",query_name.c_str(),results);
      if(rowNumber <= 0){
        std::cout << "ERROR Read gain from EMC_Gain Database, exit." << endreq;
        exit(1);
      }

      //std::cout << "run_id: " << gainnum << " ppc_id: " << trgcratenum << " Qmodule_no: " << modu << " Qchannel_no: " << channelnum << " m: " << m << std::endl;
  
      trgGain[m] = new std::string((*results[0])["gain"]);
      //std::cout << "gain: " << *trgGain[m] << std::endl;
    }
    return true;
  }

  ConnectionProvider::ConnectionProvider() {
    StatusCode sc = Gaudi::svcLocator()->service("DatabaseSvc", m_dbsvc, true);
    if (sc .isFailure() ) {
      std::cout << "ERROR: In ConnectionProvider()--> Unable to find DatabaseSvc " << std::endl;
    }
    m_trgTpye = -1;
    m_tfee_fileid = -1;
  }

  ConnectionProvider::~ConnectionProvider() {
  }

ConnectionProvider::eRet ConnectionProvider::getReadLumInfo(std::string& Lum, int runNo, std::string SftVer, std::string ParVer, std::string BossRelease) {
    //Read Luminosity information
   char stmt1[200];
   int run_No =std::abs(runNo);


   if(SftVer == "default") {
     if(BossRelease == "default") {
       std::cout << "ERROR BossRelease must be set! Current value is " << BossRelease << "." << endreq;
       exit(1);
     }
     else {
       sprintf(stmt1, "select SftVer, ParVer from CalVtxLumVer where BossRelease = '%s' and RunFrom <= %d and RunTo >= %d and DataType = 'LumVtx'", BossRelease.c_str(), run_No, run_No);

       DatabaseRecordVector records;
       int rowNo = m_dbsvc->query("offlinedb",stmt1,records);
       if(rowNo == 0) {
         std::cout << "ERROR: can not find records for run = " << run_No << " and BossRelease = " << BossRelease << endreq;
         exit(1);
       }
       DatabaseRecord* recordst = records[0];
       SftVer = recordst->GetString("SftVer");
       ParVer = recordst->GetString("ParVer");
       std::cout << "Using the new SftVer and ParVer (" << SftVer << ", " << ParVer << ") for run " << run_No << ". " << endreq;
     }
   }


   sprintf(stmt1,"select OfflineTwoGam,SftVer,ParVer from OfflineLum where RunNo = %d",run_No);
/*
  if(std::atoi(ParVer.c_str()) == 0) {
    sprintf(stmt1,"select OfflineTwoGam,ParVer from OfflineLum where RunNo = %d && SftVer = \"%s\"",run_No,SftVer.c_str());
  }
  else {
    sprintf(stmt1,"select OfflineTwoGam from OfflineLum where RunNo = %d && SftVer = \"%s\" && ParVer = \"%s\"",run_No,SftVer.c_str(),ParVer.c_str());
  }
*/  
  DatabaseRecordVector results;
  results.clear();
  int status = m_dbsvc->query("offlinedb",stmt1,results);
  if(status<0){
    std::cout << "ERROR Read the luminosity from the Database" << endreq;
    exit(1);
  }

  int RowNumber = results.size();
  if(RowNumber == 0){
    std::cout<<"WARNING: No Luminosity Data in this run, return 0."<<std::endl;
    Lum = "0";
    return RETOk;
  }

   int ParVerID = 0;
   int RecordNo = 0;
   if(std::atoi(ParVer.c_str()) == 0) {
     for(int i = 0; i < RowNumber; i++) {
       if((std::atoi((*results[i])["ParVer"]) > ParVerID)&&((*results[i])["SftVer"]==SftVer)) { Lum = (*results[i])["OfflineTwoGam"]; ParVerID = std::atoi((*results[i])["ParVer"]); RecordNo++;}
     }
   }
   else {
     RecordNo = 0;
     for(int i = 0; i < RowNumber; i++) {
       if((std::atoi((*results[i])["ParVer"]) == std::atoi(ParVer.c_str()))&&((*results[i])["SftVer"]==SftVer)) { Lum = (*results[i])["OfflineTwoGam"]; ParVerID = std::atoi((*results[i])["ParVer"]); RecordNo++;}
     }
   }

   if(RecordNo == 0) {
     //std::cout<<"ERROR: No Luminosity infor. found by your selection criteria, please check it (SftVer, ParVer)"<<std::endl;
     //std::abort();
     std::cout<<"WARNING: No Luminosity infor. found by your selection criteria, please check it (SftVer, ParVer)"<<std::endl;
     Lum = "0";
     return RETOk;
                     
   }

   if(RecordNo >= 2 && std::atoi(ParVer.c_str()) != 0) {
     std::cout<<"ERROR: More than two Lum. records found by your selection criteria, please check it (SftVer, ParVer)"<<std::endl;
     exit(1);
   }

   return RETOk;
}

  ConnectionProvider::eRet ConnectionProvider::getLumCurvePar(std::string& runTotalTime, std::string& tau_value, int runNo, std::string SftVer, std::string ParVer, std::string BossRelease) {
    //Read Luminosity curve parameters 
    char stmt1[200];
    int run_No =std::abs(runNo);
 
    if(SftVer == "default") {
     if(BossRelease == "default") {
       std::cout << "ERROR BossRelease must be set! Current value is " << BossRelease << "." << endreq;
       exit(1);
     }
     else {
       sprintf(stmt1, "select SftVer, ParVer from CalVtxLumVer where BossRelease = '%s' and RunFrom <= %d and RunTo >= %d and DataType = 'LumVtx'", BossRelease.c_str(), run_No, run_No);

       DatabaseRecordVector records;
       int rowNo = m_dbsvc->query("offlinedb",stmt1,records);
       if(rowNo == 0) {
         std::cout << "ERROR: can not find records for run = " << run_No << " and BossRelease = " << BossRelease << endreq;
         exit(1);
       }
       DatabaseRecord* recordst = records[0];
       SftVer = recordst->GetString("SftVer");
       ParVer = recordst->GetString("ParVer");
       std::cout << "Using the new SftVer and ParVer (" << SftVer << ", " << ParVer << ") for run " << run_No << ". " << endreq;
     }
   }

    sprintf(stmt1,"select runTime,Tau_Value,SftVer,ParVer from OfflineLum where RunNo = %d",run_No);
  
    DatabaseRecordVector results;
    results.clear();
    int status = m_dbsvc->query("offlinedb",stmt1,results);
    if(status<0){
      std::cout << "ERROR Read the luminosity curve parameters from the Database" << endreq;
      exit(1);
    }

    int RowNumber = results.size();
    if(RowNumber == 0){
      std::cout<<"ERROR: No Luminosity curve parameters in this run, aborted."<<std::endl;
      exit(1); 
    }

    int ParVerID = 0;
    int RecordNo = 0;
    if(std::atoi(ParVer.c_str()) == 0) {
      for(int i = 0; i < RowNumber; i++) {
        if((std::atoi((*results[i])["ParVer"]) > ParVerID)&&((*results[i])["SftVer"]==SftVer)) {
          runTotalTime = (*results[i])["runTime"]; 
          tau_value = (*results[i])["Tau_Value"]; 
          ParVerID = std::atoi((*results[i])["ParVer"]);
          RecordNo++;
        }
      }
    }
    else {
      RecordNo = 0;
      for(int i = 0; i < RowNumber; i++) {
        if((std::atoi((*results[i])["ParVer"]) == std::atoi(ParVer.c_str()))&&((*results[i])["SftVer"]==SftVer)) { 
          runTotalTime = (*results[i])["runTime"]; 
          tau_value = (*results[i])["Tau_Value"]; 
          ParVerID = std::atoi((*results[i])["ParVer"]); 
          RecordNo++;
        }
      }
    }
    if(RecordNo == 0) {
       std::cout<<"ERROR: No Luminosity curve parameters found by your selection criteria, please check it (SftVer, ParVer)"<<std::endl;
       exit(1);
     }
   
     if(RecordNo >= 2 && std::atoi(ParVer.c_str()) != 0) {
       std::cout<<"ERROR: More than two Lum. curve parameters records found by your selection criteria, please check it (SftVer, ParVer)"<<std::endl;
       exit(1);
     } 
   
     return RETOk;
  }

  ConnectionProvider::eRet ConnectionProvider::getReadBunchInfo(std::vector<std::string>& bunch, int runNo, std::string SftVer, std::string ParVer, std::string BossRelease) {
    //Read bunch size information
   char stmt1[200]; 
   int run_No =std::abs(runNo);

   if(SftVer == "default") {
     if(BossRelease == "default") {
       std::cout << "ERROR BossRelease must be set! Current value is " << BossRelease << "." << endreq;
       exit(1);
     }
     else {
       sprintf(stmt1, "select SftVer, ParVer from CalVtxLumVer where BossRelease = '%s' and RunFrom <= %d and RunTo >= %d and DataType = 'LumVtx'", BossRelease.c_str(), run_No, run_No);

       DatabaseRecordVector records;
       int rowNo = m_dbsvc->query("offlinedb",stmt1,records);
       if(rowNo == 0) {
         std::cout << "ERROR: can not find records for run = " << run_No << " and BossRelease = " << BossRelease << endreq;
         exit(1);
       }
       DatabaseRecord* recordst = records[0];
       SftVer = recordst->GetString("SftVer");
       ParVer = recordst->GetString("ParVer");
       std::cout << "Using the new SftVer and ParVer (" << SftVer << ", " << ParVer << ") for run " << run_No << ". " << endreq;
     }
   }

   sprintf(stmt1,"select Vx,Vy,Vz,AcSigmaVx,AcSigmaVy,SigmaVz,SftVer,ParVer from BeamPar where runNo = %d",run_No);
/*
  if(std::atoi(ParVer.c_str()) == 0) {
    sprintf(stmt1,"select Vx,Vy,Vz,AcSigmaVx,AcSigmaVy,SigmaVz,ParVer from BeamPar where runNo = %d && SftVer = \"%s\"",run_No,SftVer.c_str());
  }
  else {
    sprintf(stmt1,"select Vx,Vy,Vz,AcSigmaVx,AcSigmaVy,SigmaVz from BeamPar where runNo = %d && SftVer = \"%s\" && ParVer = \"%s\"",run_No,SftVer.c_str(),ParVer.c_str());
  }
*/    

  DatabaseRecordVector results;
  results.clear();
  int status = m_dbsvc->query("offlinedb",stmt1,results);
  if(status<0){
    std::cout << "ERROR Read the bunch from the Database" << endreq;
    exit(1);
  }

     int RowNumber = results.size();
     if(RowNumber == 0){
         std::cout<<"WARNING: No bunch Data in this run! Please check your selection criteria (runNo, SftVer, ParVer)"<<std::endl;
        return RETNoSchemaMatch;
     }

     int ParVerID = 0; 
     int RecordNo = 0;
     if(std::atoi(ParVer.c_str()) == 0) {
       for(int i = 0; i < RowNumber; i++) {
         if((std::atoi((*results[i])["ParVer"]) > ParVerID)&&((*results[i])["SftVer"]==SftVer)) { 
           bunch.clear();
           bunch.push_back((*results[i])["Vx"]);
           bunch.push_back((*results[i])["Vy"]);
           bunch.push_back((*results[i])["Vz"]);
           bunch.push_back((*results[i])["AcSigmaVx"]);
           bunch.push_back((*results[i])["AcSigmaVy"]);
           bunch.push_back((*results[i])["SigmaVz"]);
           ParVerID = std::atoi((*results[i])["ParVer"]);
           RecordNo++;
         }
       }
     }
     else {
       RecordNo = 0;
       for(int i = 0; i < RowNumber; i++) {
         if((std::atoi((*results[i])["ParVer"]) == std::atoi(ParVer.c_str()))&&((*results[i])["SftVer"]==SftVer)) { 
           bunch.clear();
           bunch.push_back((*results[i])["Vx"]);
           bunch.push_back((*results[i])["Vy"]);
           bunch.push_back((*results[i])["Vz"]);
           bunch.push_back((*results[i])["AcSigmaVx"]);
           bunch.push_back((*results[i])["AcSigmaVy"]);
           bunch.push_back((*results[i])["SigmaVz"]);
           RecordNo++;
         }
       }
     }

     if(RecordNo == 0) {
       std::cout<<"WARNING: No bunch infor. found in this run, it will use other run's"<<std::endl;
       return RETNoSchemaMatch;
     }

     if(RecordNo >= 2 && std::atoi(ParVer.c_str()) != 0) {
       std::cout<<"ERROR: More than two bunch records found by your selection criteria, please check it (SftVer, ParVer)"<<std::endl;
       exit(1);
     }

     return RETOk;
  }

  ConnectionProvider::eRet ConnectionProvider::getReadTrgTableInfo( std::vector<std::string>& trgTable, int runNo) {
    //Read trigger table used in trigger simulation
   char stmt1[200];
   int run_No =std::abs(runNo);
   
  sprintf(stmt1,"select trgtable_id from RunParams where run_number = %d ",run_No);

  DatabaseRecordVector results;
  results.clear();
  int status = m_dbsvc->query("run",stmt1,results);
  if(status<0){
    std::cout << "ERROR Read the trigger table from the Database" << endreq;
    exit(1);
  }

     int RowNumber = results.size();
     if(RowNumber!=1){
         std::cout<<"ERROR:error searching Trigger Type Data in the database, check your selection criteria"<<std::endl;
        return RETMySQLError;
     }
     int i_trgTpye = std::atoi((*results[0])["trgtable_id"]);
     std::cout<<"Obj_id is "<<i_trgTpye<<std::endl;
     results.clear();

    if(i_trgTpye == m_trgTpye) {
      std::cout<<"Don't need to update trigger table, it is same with the last run!"<<std::endl;
    }
    else {
      m_trgTpye = i_trgTpye;
      m_trgTable.clear();
      //retrieve EventChannelEnableRegister from database
      sprintf(stmt1,"select EventChannelEnableRegister from TrgTable where Obj_id = %d ", i_trgTpye);
      int status1 = m_dbsvc->query("run",stmt1,results);
      if(status1<0){
        std::cout << "ERROR Read the trigger table from the Database 1" << endreq;
        exit(1);
      }
       RowNumber = results.size();
       if(RowNumber!=1){
           std::cout<<"ERROR:error searching Trigger Type Data in the database, check your selection criterions"<<std::endl;
          return RETMySQLError;
       }  
       m_trgTable.push_back((*results[0])["EventChannelEnableRegister"]);
       results.clear(); 

      //retrieve TrgTable_Channel from database
      for(int i = 1; i < 17; i++)
        for(int j = 1; j < 5; j++) {
          if(i < 10) sprintf(stmt1,"select TrgTable_CH0%d_%d from TrgTable where Obj_id = %d ", i, j, i_trgTpye);
          else  sprintf(stmt1,"select TrgTable_CH%d_%d from TrgTable where Obj_id = %d ", i, j, i_trgTpye);
          int status2 = m_dbsvc->query("run",stmt1,results);
         if(status2<0){
            std::cout << "ERROR Read the trigger table from the Database 2" << endreq;
            exit(1);
          }
           RowNumber = results.size();
           if(RowNumber!=1){
               std::cout<<"ERROR:error searching TrgTable Data in the database, check your selection criterions"<<std::endl;
              return RETMySQLError;
           }
          if(i<10) sprintf(stmt1,"TrgTable_CH0%d_%d", i, j);
          else sprintf(stmt1,"TrgTable_CH%d_%d", i, j);
          m_trgTable.push_back((*results[0])[stmt1]);
          results.clear();
      }
    }
    trgTable = m_trgTable;

    return RETOk;
  }

  ConnectionProvider::eRet ConnectionProvider::getEmcGain( std::vector<double>& emcGain, int runNo) {
    // Get EmcCalibConstSvc.
    IEmcCalibConstSvc *emcCalibConstSvc;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StatusCode sc = svcLocator->service("EmcCalibConstSvc",emcCalibConstSvc);
    if(sc!=StatusCode::SUCCESS) {
      cout << "ConnectionProvider Error: Can't get EmcCalibConstSvc." << endl;
    }
    
    string ids="128005";
    
    ostringstream str;
    int idBegin=0;
    int idEnd=6240;

    for(int i=idBegin;i<idEnd;i++) {
      unsigned int npart = emcCalibConstSvc->getPartID(i);
      unsigned int ntheta = emcCalibConstSvc->getThetaIndex(i);
      unsigned int nphi = emcCalibConstSvc->getPhiIndex(i);
      
      int triPart,triTheta,triPhi;
      if(npart==0) {
        triPart=0; 
        triTheta=23+ntheta;
      } else if(npart==1&&ntheta<22) {
        triPart=0;
        triTheta=22-ntheta;
      } else if(npart==1&&ntheta>=22) {
        triPart=1;
        triTheta=ntheta-21;
      } else {  //npart==2
        triPart=1;
        triTheta=23+ntheta;
      } 
      triPhi=nphi+1;
      
      ostringstream ssTheta;
      if(triPart==1&&triTheta<10) {
        ssTheta<<"0"<<triTheta;
      } else {
        ssTheta<<triTheta;
      } 
      
      ostringstream ssPhi;
      if(triPhi<10) {
        ssPhi<<"00"<<triPhi;
      } else if(triPhi<100) {
        ssPhi<<"0"<<triPhi; 
      } else {
        ssPhi<<triPhi;
      }

      if(triPart==0) {
        str<<ssTheta.str()<<ssPhi.str();
      } else {
        str<<triPart<<ssTheta.str()<<ssPhi.str();
      }
      if(i!=idEnd-1) {
        str<<",";
      }

      //cout<<npart<<"\t"<<ntheta<<"\t"<<nphi<<endl;

    }

    ids=str.str();
    //cout<<ids<<endl;

    getcal( runNo, ids);

    emcGain.clear();
    for(int i=0;i<id_num;i++) {
      istringstream isin(*trgGain[i]);
      double value;
      isin>>value;
      value = ((50./255.)*(value)/60.)*5*0.5;
      emcGain.push_back(value);
      delete trgGain[i];
    }

    return RETOk;
  }


  ConnectionProvider::eRet ConnectionProvider::getTrgConfigInfo(int runNo) {
    //Read trigger configure information
    char stmt1[1024];
    int run_No =std::abs(runNo);

    sprintf(stmt1,"select Trg_fileid from RunParams where run_number = %d ",run_No);

    DatabaseRecordVector results;
    results.clear();
    int rowNumber = m_dbsvc->query("run",stmt1,results);
    if( rowNumber <= 0){
      std::cout << "ERROR: Read the Trg_fileid from RunParams, exit." << endl;
      exit(1);
    }

    if(rowNumber > 1){
      std::cout<<"WARNNING: there are more than one record for Trg_fileid in run " << runNo << ", the first one will be selected."<<std::endl;
    }
    int trg_fileid = std::atoi((*results[0])["Trg_fileid"]);

    sprintf(stmt1,"select EtotDataSteps,VthBEtotH,VthEEtotH,VthEtotL,VthEtotM,VthBLZ,VthDiffB,VthDiffE,VthBalBLK,VthBalEEMC,VthDiffMin from Trg_EACC where FileId = %d ", trg_fileid);

    results.clear();
    rowNumber = m_dbsvc->query("run",stmt1,results);
    if( rowNumber <= 0){
      std::cout << "ERROR: Read the EACC config infor. from Trg_EACC, exit." << endl;
      exit(1);
    }

    if(rowNumber > 1){
      std::cout<<"WARNNING: there are more than one record for EACC in run " << runNo << ", the first one will be selected."<<std::endl;
    }

    m_EtotDataSteps = std::atoi((*results[0])["EtotDataSteps"]);
    m_VthBEtotH = std::atoi((*results[0])["VthBEtotH"]);
    m_VthEEtotH = std::atoi((*results[0])["VthEEtotH"]);
    m_VthEtotL = std::atoi((*results[0])["VthEtotL"]);
    m_VthEtotM = std::atoi((*results[0])["VthEtotM"]);
    m_VthBLZ = std::atoi((*results[0])["VthBLZ"]);
    m_VthDiffB = std::atoi((*results[0])["VthDiffB"]);
    m_VthDiffE = std::atoi((*results[0])["VthDiffE"]);
    m_VthBalBLK = std::atoi((*results[0])["VthBalBLK"]);
    m_VthBalEEMC = std::atoi((*results[0])["VthBalEEMC"]);
    m_VthDiffMin = std::atoi((*results[0])["VthDiffMin"]);
    
    return RETOk;
  }


  //ConnectionProvider::eRet ConnectionProvider::getReadBackgroundInfo(std::vector<std::string>& fileInfor, std::vector<int>& vRanTrgEvtNum, int runNo) {
  ConnectionProvider::eRet ConnectionProvider::getReadBackgroundInfo(std::vector<std::string>& fileInfor, int runNo) {
    //Read background file
   char stmt1[200];
   int run_No =std::abs(runNo);

  //sprintf(stmt1,"select FilePath,FileName,NumEvent from RanTrgData where RunNo = %d ",run_No);
  sprintf(stmt1,"select FilePath,FileName from RanTrgData where RunNo = %d ",run_No);
  
  DatabaseRecordVector results;
  results.clear();
  int status = m_dbsvc->query("offlinedb",stmt1,results);
  if(status<0){
    std::cout << "ERROR Read the background file from the Database" << endreq;
    exit(1);
  }

     int RowNumber = results.size();
     if(RowNumber == 0){
         std::cout<<"WARNING: No Bg infor. in this run!"<<std::endl;
        return RETNoSchemaMatch;
     }
     //vRanTrgEvtNum.clear();
     fileInfor.clear();
     for(int i = 0; i < RowNumber; i++) {
       //int RanTrgEvtNum = 0;
       //RanTrgEvtNum = std::atoi((*results[i])["NumEvent"]);
       //if(RanTrgEvtNum != 0) vRanTrgEvtNum.push_back(RanTrgEvtNum); 
       //if(RanTrgEvtNum != 0) {
         fileInfor.push_back((*results[i])["FilePath"]);
         fileInfor.push_back((*results[i])["FileName"]);
       //}
     }
   
    if(fileInfor.size() == 0) {
      std::cout<<"WARNING: the total bg event number in run " << runNo << " is zero, it will use other run's bg" << std::endl;
      return RETNoSchemaMatch;
    }

    return RETOk;
  }

  //ConnectionProvider::eRet ConnectionProvider::getReadBackgroundInfo( std::vector<std::string>& fileInfor, std::vector<int>& vRanTrgEvtNum, std::string stmt1) {
  ConnectionProvider::eRet ConnectionProvider::getReadBackgroundInfo( std::vector<std::string>& fileInfor, std::string stmt1) {
    //Read background file
   //char stmt1[200];
   //int run_No =runNo;

  //sprintf(stmt1,"select FilePath,FileName from RanTrgData where RunNo = %d ",run_No);

  DatabaseRecordVector results;
  results.clear();
  int status = m_dbsvc->query("offlinedb",stmt1,results);
  if(status<0){
    std::cout << "ERROR Read the background file from the Database" << endreq;
    exit(1);
  }
     int RowNumber = results.size();
     if(RowNumber == 0){
         std::cout<<"ERROR:error searching background Data in the database, check your selection criterions"<<std::endl;
        return RETMySQLError;
     }
     //vRanTrgEvtNum.clear();
     fileInfor.clear();
     for(int i = 0; i < RowNumber; i++) {
       //int RanTrgEvtNum = 0;
       //RanTrgEvtNum = std::atoi((*results[i])["NumEvent"]);
       //if(RanTrgEvtNum != 0) vRanTrgEvtNum.push_back(RanTrgEvtNum);
       //if(RanTrgEvtNum != 0) {
         fileInfor.push_back((*results[i])["FilePath"]);
         fileInfor.push_back((*results[i])["FileName"]);
       //}
     }

    if(fileInfor.size() == 0) {
      std::cout<<"WARNING: the total bg event number in your selected bg files is zero, it will abort after error message" << std::endl;
      return RETNoSchemaMatch;
    }

    return RETOk;
  }

  ConnectionProvider::eRet ConnectionProvider::getReadTofThreshInfo( std::vector<std::string>& tofThresh, int runNo) {
    //Read tof FEE threshold 
   char stmt1[200];
   int run_No =std::abs(runNo);
   
  sprintf(stmt1,"select TFEE_fileid from RunParams where run_number = %d ",run_No);
  
  DatabaseRecordVector results;
  results.clear();
  int status = m_dbsvc->query("run",stmt1,results);
  if(status<0){
    std::cout << "ERROR Read the Tof threshold from the Database" << endreq;
    exit(1);
  }
     int RowNumber = results.size();
     if(RowNumber!=1){
         std::cout<<"ERROR:error searching TFEE_fileid Data in the database, check your selection criterions"<<std::endl;
        return RETMySQLError;
     }
     int i_tfee_fileid = std::atoi((*results[0])["TFEE_fileid"]);
     std::cout<<"TFEE file id is "<<i_tfee_fileid<<std::endl;
     results.clear();

     if(i_tfee_fileid == m_tfee_fileid) {
      std::cout<<"Don't need to update TOF FEE threshold, it is same with the last run!"<<std::endl;
    }
    else {
      m_tfee_fileid = i_tfee_fileid;
      m_tofThresh.clear();

      sprintf(stmt1,"select PhysAddress,BarrelEnd,HighThreshold,LowThreshold from TFEE where FileId = %d ",i_tfee_fileid);
      int status1 = m_dbsvc->query("run",stmt1,results);
      if(status1<0){
        std::cout << "ERROR Read the Tof threshold from the Database 1" << endreq;
        exit(1);
     }
     int RowNumber = results.size();
     if(RowNumber == 0){
         std::cout<<"WARNING: No TFEE infor. in this run!"<<std::endl;
        return RETNoSchemaMatch;
     }  
     for(int i = 0; i < RowNumber; i++) {
       m_tofThresh.push_back((*results[i])["PhysAddress"]); 
       m_tofThresh.push_back((*results[i])["BarrelEnd"]); 
       m_tofThresh.push_back((*results[i])["HighThreshold"]); 
       m_tofThresh.push_back((*results[i])["LowThreshold"]); 
     }
     results.clear();
   }
 
   tofThresh = m_tofThresh;

    return RETOk;
  }

  ConnectionProvider::eRet ConnectionProvider::getRunInfo(std::vector<std::string>& runInfo, int runNo) {
    //Read run information
   char stmt1[500];
   int run_No =std::abs(runNo);
   
  sprintf(stmt1,"select R_TM_BPR_PatNum,R_TM_BER_PatNum,S_R3O_BI_DCCT_current,E_R3O_BI_DCCT_current,AV_R3O_BI_DCCT_current,S_R4O_BI_DCCT_current,E_R4O_BI_DCCT_current,AV_R4O_BI_DCCT_current,S_BPR_LIFE,E_BPR_LIFE,AV_BPR_LIFE,S_BER_LIFE,E_BER_LIFE,AV_BER_LIFE from RunParams where run_number = %d ",run_No);
    
  DatabaseRecordVector results;
  results.clear();
  int status = m_dbsvc->query("run",stmt1,results);
  if(status<0){
    std::cout << "ERROR Read the run information from the Database" << endreq;
    exit(1);
  } 
     int RowNumber = results.size();
     if(RowNumber == 0){
         std::cout<<"WARNING: No run information in this run!"<<std::endl;
        return RETNoSchemaMatch;
     }  
    
     for(int i = 0; i < RowNumber; i++) {
       for(std::map<std::string,char*>::iterator iter = (*results[i]).begin(); iter != (*results[i]).end(); iter++) {
         runInfo.push_back(iter->second);
       }
     }

     return RETOk; 
  }

}
