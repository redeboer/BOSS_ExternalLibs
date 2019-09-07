#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "RawDataCnv/EventManagement/Builder.h"

string Builder::m_confFile;

Builder::Builder() 
{
  // default 128k bytes buffer size, it's dangerous!!!
  // a smarter method is expected !!!
  m_buf = new uint32_t[32768];

  if (!m_confFile.empty()) return;
  m_propMgr.declareProperty("RawDataCnvConfFile", m_confFile);
  m_propMgr.declareProperty("Status", m_status=0xFFFFFFFF);                    //fucd

  // Get a pointer to the Job Options Service
  IJobOptionsSvc* jobSvc;
  Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);
  jobSvc->setMyProperties("Builder", &m_propMgr);
}
  
Builder::~Builder() {
   delete m_buf;
}

bool Builder::expect( ifstream &f, string msg, string fname )
{
  string str;
  if( f.bad() ) 
  {
      cerr << "Error: file " << fname << " is bad" << endl;
      return false;
  }
  f >> str;
  if( str == msg )
    return true;

  cerr << "Error: expected `" << msg << "|" << str <<"` in " << fname << endl;
  return false;
}

bool Builder::expectInt( ifstream &f, string msg, string fname, uint32_t &val1, uint32_t &val2 )
{
  if( !expect( f, msg, fname ) || f.bad() )
    return false;
  f >> val1 >> val2;
  return true;
}

bool Builder::expectLong(ifstream &f, string msg, string fname, uint64_t &val)
{
  if ( !expect(f, msg, fname) || f.bad() )
    return false;
  f >> val;
  return true;
}

bool Builder::find( ifstream &f, string msg, string fname )
{
  if ( f.bad() ) {
    cerr << "Error: file " << fname << " bad checked" << endl;
    return false;
  }

  string str;
  while ( !f.eof() ) {
    f >> str;
    if ( str == msg ) return true;
  }

  return false;
}

void Builder::append2event(WriteRawEvent*& re, uint32_t source_id, uint32_t size, uint32_t shift)
{
  uint32_t run_no    = re->run_no();
  uint32_t lvl1_id   = re->lvl1_id();
  uint32_t bc_id     = 0;   // ?
  // ROB
  eformat::write::ROBFragment* nrob = new eformat::write::ROBFragment(
      source_id,      // source_id
      run_no,         // run_no
      lvl1_id,        // lvl1_id
      bc_id,          // bc_id
      0,              // lvl1_type
      0,              // detev_type
      size,           // ndata
      m_buf+shift,    // data
      0               // status_position
      );
  if(m_status==0xFFFFFFFF){
    nrob->status(0, NULL);
    nrob->rod_status(0, NULL);
  }
  else{                     //fucd
    nrob->status(1, &m_status);
    nrob->rod_status(1, &m_status);
  }
  // ROS
  eformat::write::ROSFragment* nros = new eformat::write::ROSFragment(
      source_id,      // source_id
      run_no,         // run_no
      lvl1_id,        // lvl1_id
      bc_id           // bc_id
      );
  if(m_status==0xFFFFFFFF) nros->status(0, NULL);
  else        nros->status(1, &m_status);    //fucd
  // SubDetector
  eformat::write::SubDetectorFragment* nsd = new eformat::write::SubDetectorFragment(source_id);
  if(m_status==0xFFFFFFFF) nsd->status(0, NULL);
  else        nsd->status(1, &m_status);     //fucd
  // append to FullEventFragment
  nros->append(nrob);
  nsd->append(nros);
  re->append(nsd);
}
