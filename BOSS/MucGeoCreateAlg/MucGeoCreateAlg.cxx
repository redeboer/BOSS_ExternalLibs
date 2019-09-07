//------------------------------------------------------------------------------|
//      [File  ]:                       MucGeoCreateAlg.cxx                     |
//      [Brief ]:       Muon detector geometry creating algrithom               |
//			Both designed and aligned geometry can be generated     |
//			Geometry entries included:				|
//				yoke, gas gap, RPC, strip,			|		
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Jul 2, 2006                                             |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|
#include<iostream>

#include "GaudiKernel/MsgStream.h"

#include "MucGeoCreateAlg/MucGeoCreateAlg.h"

using namespace std;

// -----------------------------------Declaration----------------------------------------
MucGeoCreateAlg::MucGeoCreateAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), m_bAlignFlag(true)
{
  // Declare the properties
	declareProperty("CreateFlag", m_sCreateFlag);
  declareProperty("AlignFlag",  m_bAlignFlag);
  declareProperty("AlignFile",  m_sAlignFile = "MucGeoAlignConst.root");
}

// ----------------------------------Initialize----------------------------------------- 
StatusCode MucGeoCreateAlg::initialize()
{
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << endreq << "In initialize() " << endreq;
	log << MSG::INFO << "Add alignment:\t"  << ((m_bAlignFlag)?"YES":"NO") << endreq << endreq;

  if( m_bAlignFlag )
	log << MSG::INFO << "Alignment file:\t" << m_sAlignFile << endreq;	

	m_pMucGeoMgr = new MucGeoMgr( m_sCreateFlag, m_bAlignFlag, m_sAlignFile );
	
  return StatusCode::SUCCESS;
}

// ----------------------------------Execute--------------------------------------------
StatusCode MucGeoCreateAlg::execute()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << endreq << "In execute()" << endreq;

	return StatusCode::SUCCESS;
}
// ----------------------------------Finalize------------------------------------------
StatusCode MucGeoCreateAlg::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << endreq << "In finalize()" << endreq << endreq;
	
	m_pMucGeoMgr->CreateEntities();	
	m_pMucGeoMgr->CreateRootGeo();
	m_pMucGeoMgr->CreateOnlineStripGeo();

  return StatusCode::SUCCESS;
}

//--------------------------------------------END-----------------------------------------

