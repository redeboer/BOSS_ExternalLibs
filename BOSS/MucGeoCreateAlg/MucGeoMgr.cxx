//------------------------------------------------------------------------------|
//      [File  ]:                       MucGeoMgr.cxx      	                    |
//      [Brief ]:       MUC geometry created manager class   			              |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Mar 28, 2006                                            |
//------------------------------------------------------------------------------|

#include<iostream>
#include<string>
#include<fstream>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

#include "MucGeoCreateAlg/MucGeoMgr.h"
#include "MucGeoCreateAlg/MucIdTransform.h"

using namespace std;

// Constructor
MucGeoMgr::MucGeoMgr( const std::string createFlag, bool alignFlag, const std::string alignFile )
{
	m_CreateFlag 	= createFlag;
	m_AlignFlag 	= alignFlag;
	m_AlignFile 	= alignFile;
	
  Gaudi::svcLocator() -> service("MessageSvc", msgSvc);

	InitOffset();	
}

// Destructor
MucGeoMgr::~MucGeoMgr()
{
	delete []m_BoxOffset;
	delete []m_StripPlaneOffset;

	delete m_MucAbsorber;
	delete m_MucGap;
	delete m_MucBox;
	delete m_MucStripPlane;
	delete m_MucStrip;
	delete m_MucRpc;
	delete m_MucGas;
	delete m_MucBakelite;
	delete m_MucBoxCover;
}

//========================================= Alignment initialization==================================================
// Offset init
StatusCode MucGeoMgr::InitOffset()
{
 	MsgStream log(msgSvc, "MucGeoMgr");

  m_IdTr = new MucIdTransform();

	if( m_AlignFlag == true )
	{
 		log << MSG::INFO << "MucGeoMgr::initOffset()" << endreq;

    TFile* froot = new TFile(m_AlignFile.c_str(), "read");
    if( froot->IsZombie() ) 
    {
      log << MSG:: ERROR << "Open alignment data error!" << endreq;
      return StatusCode::FAILURE;
    }

    const char OFFSET_NAME[3][5] = {"dx", "dy", "dz"};
    double box_offset[3];
    double strpln_offset[3];
   
    TTree* tr_Offset;
    
    tr_Offset = (TTree*)froot->Get("Offset");
    tr_Offset->SetBranchAddress("box_dx", &box_offset[0]);
    tr_Offset->SetBranchAddress("box_dy", &box_offset[1]);
    tr_Offset->SetBranchAddress("box_dz", &box_offset[2]);
    tr_Offset->SetBranchAddress("strpln_dx", &strpln_offset[0]);
    tr_Offset->SetBranchAddress("strpln_dy", &strpln_offset[1]);
    tr_Offset->SetBranchAddress("strpln_dz", &strpln_offset[2]);

    int part, segment, layer;
        part = segment = layer = 0;
    
		log << MSG::INFO << "------------------------- offset data--------------------------" << endreq;
    log << MSG::INFO << "Part\tSegment\tLayer\tdx0\tdy0\tdz0\tdx1\tdy1\tdz1" << endreq;
    for( int i=0; i<BOX_MAX; i++)
    {
      m_IdTr->SetBoxPos(i, &part, &segment, &layer);
      tr_Offset->GetEntry(i);
            
      log << MSG::INFO << part << "\t" << segment << "\t" << layer << "\t";
      for( int j=0; j<3; j++ )
      {
        log << MSG::INFO << box_offset[j] << "\t";

        if( !CheckBoxOffset(part, segment, layer, j, box_offset[j]) )
        {
          log << MSG::INFO << endreq << "Box offset P" << part << "S" << segment << "L" << layer
              << "_" << OFFSET_NAME[j] << "\tout range!" << endreq;
          box_offset[j] = B_X_MAX[j];
        }
      }

      for( int j=0; j<3; j++ )
      {
        log << MSG::INFO << strpln_offset[j] << "\t";

        if( !CheckStripPlaneOffset(part, segment, layer, j, strpln_offset[j]) )
        {
          log << MSG::INFO << endreq << "Strip plane offset P" << part << "S" << segment << "L" << layer
              << "_" << OFFSET_NAME[j] << "\tout range!" << endreq;
          strpln_offset[j] = STR_OFFSET_MAX[j];
        }
      }  
    
      log << MSG::INFO << endreq;   
    } // end box    

    froot->Close(); 
    log << MSG::INFO << "---------------------------------------------------------------" << endreq;
	} // end alignflag
	else
	{
		for(int i=0; i<PART_MAX; i++)
		  for(int j=0; j<B_SEG_NUM; j++)
		    for(int k=0; k<B_LAY_NUM; k++)
		      for(int m=0; m<3; m++)
          {
            m_BoxOffset[i][j][k][m] = 0.0;
			      m_StripPlaneOffset[i][j][k][m] = 0.0;
          }							
	} 

	return StatusCode::SUCCESS;
}

bool MucGeoMgr::CheckBoxOffset( int part, int segment, int layer, int axis, double offset )
{
	int outRangeFlag = 0;

	if( part == BRID )
	{
		switch( axis )
		{
		 case 0:	// x
			int layerFlag;

			if( layer == 0 ) 	 layerFlag = 0; 
			else if( layer%2 == 1 )  layerFlag = 1; 
			else 			 layerFlag = 2; 

			if( B_X_MAX[layerFlag] - fabs(offset) >= 0.0 ) // |offset|<=B_X_MAX
				m_BoxOffset[part][segment][layer][axis] = offset;			
			else
			{	
				outRangeFlag ++;	
				m_BoxOffset[part][segment][layer][axis] = B_X_MAX[layerFlag] * MAX_FRACTION;
			}
			break;
		 case 1: 	// y
                        if( B_Y_MAX - fabs(offset) >= 0.0 ) // |offset|<=B_Y_MAX
                                m_BoxOffset[part][segment][layer][axis] = offset;                          
                        else
                        {
				outRangeFlag ++;
			        m_BoxOffset[part][segment][layer][axis] = B_Y_MAX * MAX_FRACTION;			
			}
			break;
		 case 2: 	// z
                        if( B_Z_MAX - fabs(offset) >= 0.0 ) // |offset|<=B_Y_MAX
                                m_BoxOffset[part][segment][layer][axis] = offset;
                        else
                        {
				outRangeFlag ++;
			        m_BoxOffset[part][segment][layer][axis] = B_Z_MAX * MAX_FRACTION;
			}
                        break;
		 default: ;
		}
	}	
	else
	{
                if( E_OFFSET_MAX[axis] - fabs(offset) >= 0.0 ) // |offset|<=B_Y_MAX
                	m_BoxOffset[part][segment][layer][axis] = offset;
                else
                {
			outRangeFlag ++;
		        m_BoxOffset[part][segment][layer][axis] = E_OFFSET_MAX[axis] * MAX_FRACTION;
		}
	}
	
	if( outRangeFlag > 0 ) 	return false;
	else 			return true;	
}

bool MucGeoMgr::CheckStripPlaneOffset( int part, int segment, int layer, int axis, double offset )
{
        int outRangeFlag = 0;

        if( STR_OFFSET_MAX[axis] - fabs(offset) >= 0.0 ) // |offset|<=STR_OFFSET_MAX
	        m_StripPlaneOffset[part][segment][layer][axis] = offset;
        else
        {
                outRangeFlag ++;
                m_StripPlaneOffset[part][segment][layer][axis] = STR_OFFSET_MAX[axis] * MAX_FRACTION;
        }

        if( outRangeFlag > 0 )  return false;
        else                    return true;
}


//====================================== Geometry entities creating methods=============================================
//
//------------------------------------------Total data of all entities -------------------------------------------------
StatusCode MucGeoMgr::CreateEntities()
{
  MsgStream log(msgSvc, "MucGeoMgr");

	StatusCode sc;

	if( m_CreateFlag.size() < ENTITY_NUM )
	{
		for( unsigned int i=m_CreateFlag.size(); i<ENTITY_NUM; i++ )
			m_CreateFlag += '0';
	}

	int entity = 0;
	for( unsigned int i=0; i< ENTITY_NUM; i++ )
		if( m_CreateFlag[i] == '1' ) entity++;
	log << MSG::INFO << entity << "\tentities should be created." << endreq << endreq;

	if( m_CreateFlag[0] == '1' )
	{
		sc = CreateAbsorber();
		if( sc == StatusCode::SUCCESS )
			log << MSG::INFO << "Create absorber successfully!" << endreq << endreq;
		else
			log << MSG::INFO << "Create absorber failure!" << endreq << endreq;			
	}

        if( m_CreateFlag[1] == '1' )
        {
                sc = CreateGap();
                if( sc == StatusCode::SUCCESS )
                        log << MSG::INFO << "Create gap successfully!" << endreq << endreq;
                else
                        log << MSG::INFO << "Create gap failure!" << endreq << endreq;       
        }

        if( m_CreateFlag[2] == '1' )
        {
                sc = CreateBox();
                if( sc == StatusCode::SUCCESS )
                        log << MSG::INFO << "Create box successfully!" << endreq << endreq;
                else
                        log << MSG::INFO << "Create box failure!" << endreq << endreq;       
        }

        if( m_CreateFlag[3] == '1' )
        {
                sc = CreateStripPlane();
                if( sc == StatusCode::SUCCESS )
                        log << MSG::INFO << "Create strip_plane successfully!" << endreq << endreq;
                else
                        log << MSG::INFO << "Create strip_plane failure!" << endreq << endreq;       
        }

        if( m_CreateFlag[4] == '1' )
        {
                sc = CreateStrip();
                if( sc == StatusCode::SUCCESS )
                        log << MSG::INFO << "Create strip successfully!" << endreq << endreq;
                else
                        log << MSG::INFO << "Create strip failure!" << endreq << endreq;       
        }

        if( m_CreateFlag[5] == '1' )
        {
                sc = CreateRpc();
                if( sc == StatusCode::SUCCESS )
                        log << MSG::INFO << "Create RPC successfully!" << endreq << endreq;
                else
                        log << MSG::INFO << "Create RPC failure!" << endreq << endreq;       
        }

        if( m_CreateFlag[6] == '1' )
        {
                sc = CreateGas();
                if( sc == StatusCode::SUCCESS )
                        log << MSG::INFO << "Create gas mixture successfully!" << endreq << endreq;
                else
                        log << MSG::INFO << "Create gas mixture failure!" << endreq << endreq;       
        }

        if( m_CreateFlag[7] == '1' )
        {
                sc = CreateBakelite();
                if( sc == StatusCode::SUCCESS )
                        log << MSG::INFO << "Create bakelite successfully!" << endreq << endreq;
                else
                        log << MSG::INFO << "Create bakelite failure!" << endreq << endreq;       
        }

        if( m_CreateFlag[8] == '1' )
        {
                sc = CreateBoxCover();
                if( sc == StatusCode::SUCCESS )
                        log << MSG::INFO << "Create box cover successfully!" << endreq << endreq;
                else
                        log << MSG::INFO << "Create box cover failure!" << endreq << endreq;       
        }

	
	return StatusCode::SUCCESS;
}

//------------------------------------ROOT geometry-----------------------------------------------
StatusCode MucGeoMgr::CreateRootGeo()
{
   MsgStream log(msgSvc, "MucGeoMgr");
	//StatusCode sc;


	return StatusCode::SUCCESS;
}	


//------------------------------------Strip geometry for online display----------------------------
StatusCode MucGeoMgr::CreateOnlineStripGeo()
{
	MsgStream log(msgSvc, "MucGeoMgr");
	//StatusCode sc;

  //-------------------------- ideal geometry----------------------
  ofstream fEast("EastEndStripGeo.dat", ios::out);
  ofstream fBarrel("BarrelStripGeo.dat", ios::out);	
  ofstream fWest("WestEndStripGeo.dat", ios::out);	

  if( fEast.bad() || fBarrel.bad() || fWest.bad() )
  {
  	log << MSG::INFO << "Strip: create ouput file error!" << endl;
		return StatusCode::FAILURE;
  }
  
	for( int i=0; i<PART_MAX; i++ )
  {
  	if( i == BRID )
    {
    	for( int j=0; j<B_SEG_NUM; j++ )
      {
      	for( int k=0; k<B_LAY_NUM; k++ )
        {
			  	// Set maximum strip 
          int maxStrip;
          if( ( k+1 )%2 == 1 )
          	maxStrip = B_ZSTR_NUM;          // odd layer
          else if( j != B_TOP )
          	maxStrip = B_PHISTR_NUM;        // even layer not top segment
          else
          	maxStrip = B_TOPSTR_NUM;        // even layer top segment 

          for( int n=0; n<maxStrip; n++ )
          {
          	MucStrip *aMucStrip = new MucStrip( i, j, k, n );
						
						fBarrel	<< i << "\t" << j << "\t" << k << "\t" << n << "\t"
						      	<< aMucStrip->GetType()-2		  << "\t"
                    << aMucStrip->GetL() 		  <<"\t"
						      	<< aMucStrip->GetW() 		  << "\t"
						      	<< aMucStrip->GetH() 		  << "\t"
                    << aMucStrip->GetObjOrgInBes(1)   <<"\t"
                    << aMucStrip->GetObjOrgInBes(2)   <<"\t"
                    << aMucStrip->GetObjOrgInBes(3)   <<"\t"
                    << endl;
						// delete aMucStrip;
          } // for
        } // layer
      } // segment
    } // barrel
    else if( i == EEID )
    {
    	for( int j=0; j<E_SEG_NUM; j++ )
      {
      	for( int k=0; k<E_LAY_NUM; k++ )
        {
        	for( int n=0; n<E_STR_NUM; n++ )
          {
          	MucStrip *aMucStrip = new MucStrip( i, j, k, n );

	          fEast << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                  << aMucStrip->GetType()   	  << "\t"
                  << aMucStrip->GetL() 		  <<"\t"
                  << aMucStrip->GetW() 		  <<"\t"
                  << aMucStrip->GetH() 	 	  <<"\t"
                  << aMucStrip->GetObjOrgInBes(1)   <<"\t"
                  << aMucStrip->GetObjOrgInBes(2)   <<"\t"
                  << aMucStrip->GetObjOrgInBes(3)   <<"\t"
                  << endl;
						// delete aMucStrip;
          } // strip
        } // layer
      } // segment
    } // east endcap
		else
		{
   		for( int j=0; j<E_SEG_NUM; j++ )
      {
      	for( int k=0; k<E_LAY_NUM; k++ )
        {
        	for( int n=0; n<E_STR_NUM; n++ )
          {
          	MucStrip *aMucStrip = new MucStrip( i, j, k, n );

	          fWest << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                  << aMucStrip->GetType()   	  << "\t"
                  << aMucStrip->GetL() 		  <<"\t"
                  << aMucStrip->GetW() 		  <<"\t"
                  << aMucStrip->GetH() 	 	  <<"\t"
                  << aMucStrip->GetObjOrgInBes(1)   <<"\t"
                  << aMucStrip->GetObjOrgInBes(2)   <<"\t"
                  << aMucStrip->GetObjOrgInBes(3)   <<"\t"
                  << endl;
						// delete aMucStrip;
          } // strip
        } // layer
      } // segment
		} // west endcap

  } // part

	fEast.close();
	fBarrel.close();	
	fWest.close();

	log << MSG::INFO << "Online display strips created." << endreq;

	return StatusCode::SUCCESS;
}	


///////////////////////////////////////////////////////////////////////////////////////////////
//                                   Sub funtions                                            //
///////////////////////////////////////////////////////////////////////////////////////////////                                   
//------------------MucAbsorber-------------
// No alignment
StatusCode MucGeoMgr::CreateAbsorber()
{
  MsgStream log(msgSvc, "MucGeoMgr");

	ofstream fOrigin("MucAbsorberOrigin.dat", ios::out);
	ofstream fPanel("MucAbsorberPanel.dat", ios::out);
	ofstream fPos("MucAbsorberPanelPos.dat", ios::out);
	
	if( fOrigin.bad() || fPanel.bad() || fPos.bad() )
	{
		log << MSG::INFO << "Absorber: create ouput file error!" << endreq;
		return StatusCode::FAILURE;
	}
	fOrigin << "part\tsegment\tlayer\tW\tH\tL\tBes_x\tBes_y\tBes_z\tRot_x\tRot_y\tRot_z" << endl;
	fPanel  << "part\tsegment\tlayer\tpanel\tWu\tWd\tH\tL\tLoc_x\tLoc_y\tLoc_z" << endl;	
	fPos    << "part\tsegment\tlayer\tpanel\tLBes_x\tLBes_y\tLBes_z\tOBes_x\tOBes_y\tOBes_z\tLoc_x\tLoc_y\tLoc_z" << endl;

	int totalObject = 0;

	for( int i=0; i<PART_MAX; i++ )
	{
		if( i == BRID )
		{
			for( int j=0; j<B_SEG_NUM; j++ )
			{
				for( int k=0; k<B_LAY_NUM; k++ )
				{
					for( int n=0; n<B_AS_NUM; n++ )
					{
						MucAbsorber *aMucAbsorber = new MucAbsorber( i, j, k, n );
						fOrigin << i << "\t" << j << "\t" << k << "\t" 
                                                        << aMucAbsorber->GetW() <<"\t"
                                                        << aMucAbsorber->GetH() <<"\t"
                                                        << aMucAbsorber->GetL() <<"\t"				
					       		<< aMucAbsorber->GetLocOrgInBes(1)   <<"\t"
					       		<< aMucAbsorber->GetLocOrgInBes(2)   <<"\t"
					       		<< aMucAbsorber->GetLocOrgInBes(3)   <<"\t"
							<< aMucAbsorber->GetObjRotToMot(1)   <<"\t"
                                                        << aMucAbsorber->GetObjRotToMot(2)   <<"\t"
                                                        << aMucAbsorber->GetObjRotToMot(3)   <<"\t"
					       		<< endl;
						fPanel  <<  i << "\t" << j << "\t" << k << "\t" << n << "\t"
					       		<< aMucAbsorber->GetWu() <<"\t"
							<< aMucAbsorber->GetWd() <<"\t"
					       		<< aMucAbsorber->GetH()  <<"\t"
					       		<< aMucAbsorber->GetL()  <<"\t"	
							<< aMucAbsorber->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInLoc(3)   <<"\t"
							<< endl;
						fPos    << i << "\t" << j << "\t" << k << "\t" << n << "\t"
							<< aMucAbsorber->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucAbsorber->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucAbsorber->GetLocOrgInBes(3)   <<"\t"
							<< aMucAbsorber->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInBes(3)   <<"\t"
							<< aMucAbsorber->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInLoc(3)   <<"\t"
							<< endl;

						totalObject++;
					//	delete aMucAbsorber;
					}
				} // layer
			} // segment
		} // barrel
		else
		{
			for( int j=0; j<E_SEG_NUM; j++ )
			{
				for( int k=0; k<E_ASLAY_NUM; k++ )
				{
					for( int n=-1; n<E_PANEL_NUM; n++ )
					{
						MucAbsorber *aMucAbsorber = new MucAbsorber( i, j, k, n );
						if( n == -1 )
						{
						 fOrigin<< i << "\t" << j << "\t" << k << "\t"
                                                        << aMucAbsorber->GetW() <<"\t"
                                                        << aMucAbsorber->GetH() <<"\t"
                                                        << aMucAbsorber->GetL() <<"\t"
                                                        << aMucAbsorber->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucAbsorber->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucAbsorber->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucAbsorber->GetObjRotToMot(1)   <<"\t"
                                                        << aMucAbsorber->GetObjRotToMot(2)   <<"\t"
                                                        << aMucAbsorber->GetObjRotToMot(3)   <<"\t"
                                                        << endl;

							totalObject ++;
							// delete aMucAbsorber;
						}
						else
						{
						  fPanel<< i << "\t" << j << "\t" << k << "\t" << n << "\t"
							<< aMucAbsorber->GetWu() <<"\t"
                                                        << aMucAbsorber->GetWd() <<"\t"
                                                        << aMucAbsorber->GetH()  <<"\t"
                                                        << aMucAbsorber->GetL()  <<"\t"
                                                        << aMucAbsorber->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;

						fPos    << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucAbsorber->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucAbsorber->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucAbsorber->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucAbsorber->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
							// delete aMucAbsorber;
						}
					}
				} // layer
			} // segment
		} // endcap
	} // for
	
	fOrigin.close();
	fPanel.close();
	fPos.close();
	
	log << MSG::INFO << totalObject << "\tabsorbers created." << endreq;
	
	return StatusCode::SUCCESS;
} // MucAbsorber

//------------------MucGap-------------
// No alignment
StatusCode MucGeoMgr::CreateGap()
{
        MsgStream log(msgSvc, "MucGeoMgr");

        ofstream fOrigin("MucGapOrigin.dat", ios::out);
        ofstream fPanel("MucGapPanel.dat", ios::out);
        ofstream fPos("MucGapPanelPos.dat", ios::out);

        if( fOrigin.bad() || fPanel.bad() || fPos.bad() )
        {
                log << MSG::INFO << "Gap: create ouput file error!" << endreq;
                return StatusCode::FAILURE;
        }
        fOrigin << "part\tsegment\tlayer\tW\tH\tL\tBes_x\tBes_y\tBes_z\tRot_x\tRot_y\tRot_z" << endl;
        fPanel  << "part\tsegment\tlayer\tpanel\tWu\tWd\tH\tL\tLoc_x\tLoc_y\tLoc_z" << endl;
        fPos    << "part\tsegment\tlayer\tpanel\tLBes_x\tLBes_y\tLBes_z\tOBes_x\tOBes_y\tOBes_z\tLoc_x\tLoc_y\tLoc_z" << endl;

	int totalObject = 0;

        for( int i=0; i<PART_MAX; i++ )
        {
                if( i == BRID )
                {
                        for( int j=0; j<B_SEG_NUM; j++ )
                        {
				// set panel number
				int idMin, idMax;
				if( j != B_TOP ) { idMin = 0; idMax = B_GP_NUM;	}
				else		 { idMin = -1; idMax = 3;    	}

                                for( int k=0; k<B_LAY_NUM; k++ )
                                {
                                        for( int n=idMin; n<idMax; n++ )
                                        {
                                                MucGap *aMucGap = new MucGap( i, j, k, n );

						if( j == B_TOP && n != -1 ) // barrel top segment panels
						{
                                                  fPanel<< i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucGap->GetWu() <<"\t"
                                                        << aMucGap->GetWd() <<"\t"
                                                        << aMucGap->GetH()  <<"\t"
                                                        << aMucGap->GetL()  <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						}

                                                if( j !=B_TOP || n == -1 )
                                                {
                                                 fOrigin<< i << "\t" << j << "\t" << k << "\t"
                                                        << aMucGap->GetW() <<"\t"
                                                        << aMucGap->GetH() <<"\t"
                                                        << aMucGap->GetL() <<"\t"	
                                                        << aMucGap->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucGap->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucGap->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucGap->GetObjRotToMot(1)   <<"\t"
                                                        << aMucGap->GetObjRotToMot(2)   <<"\t"
                                                        << aMucGap->GetObjRotToMot(3)   <<"\t"
                                                        << endl;

						 totalObject++;
						 // delete aMucGap;
                                                }						

                                                fPos    << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucGap->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucGap->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucGap->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucGap->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucGap->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucGap->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
                                        }
                                } // layer
                        } // segment
                } // barrel
                else
		{
                        for( int j=0; j<E_SEG_NUM; j++ )
                        {
                                for( int k=0; k<E_LAY_NUM; k++ )
                                {
                                        for( int n=-1; n<E_PANEL_NUM; n++ )
                                        {
                                                MucGap *aMucGap = new MucGap( i, j, k, n );

                                                if( n == -1 )
                                                {
                                                 fOrigin<< i << "\t" << j << "\t" << k << "\t"
                                                        << aMucGap->GetW() <<"\t"
                                                        << aMucGap->GetH() <<"\t"
                                                        << aMucGap->GetL() <<"\t"	
                                                        << aMucGap->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucGap->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucGap->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucGap->GetObjRotToMot(1)   <<"\t"
                                                        << aMucGap->GetObjRotToMot(2)   <<"\t"
                                                        << aMucGap->GetObjRotToMot(3)   <<"\t"
                                                        << endl;

						 totalObject++;
                                                }
                                                else
                                                {
                                                  fPanel<< i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucGap->GetWu() <<"\t"
                                                        << aMucGap->GetWd() <<"\t"
                                                        << aMucGap->GetH()  <<"\t"
                                                        << aMucGap->GetL()  <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;

                                                  fPos  << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucGap->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucGap->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucGap->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucGap->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucGap->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucGap->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucGap->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						  // delete aMucGap;
                                                }
                                        }
                                } // layer
                        } // segment
                } // endcap
        } // for

      	fOrigin.close();
        fPanel.close();
        fPos.close();

        log << MSG::INFO << totalObject << "\tgaps created." << endreq;

        return StatusCode::SUCCESS;
} // MucGap

//------------------MucBox-------------
// Alignment
StatusCode MucGeoMgr::CreateBox()
{
        MsgStream log(msgSvc, "MucGeoMgr");

	//-------------------------- ideal geometry----------------------
        ofstream fOrigin("MucBoxOrigin.dat", ios::out);
        ofstream fPanel("MucBoxPanel.dat", ios::out);
        ofstream fPos("MucBoxPanelPos.dat", ios::out);

        if( fOrigin.bad() || fPanel.bad() || fPos.bad() )
        {
                log << MSG::INFO << "Box: create ouput file error!" << endl;
		return StatusCode::FAILURE;
        }
        fOrigin << "part\tsegment\tlayer\tW\tH\tL\tLoc_x\tLoc_y\tLoc_z" << endl;
        fPanel  << "part\tsegment\tlayer\tpanel\tWu\tWd\tH\tL\tLoc_x\tLoc_y\tLoc_z" << endl;
        fPos    << "part\tsegment\tlayer\tpanel\tLBes_x\tLBes_y\tLBes_z\tOBes_x\tOBes_y\tOBes_z\tLoc_x\tLoc_y\tLoc_z" << endl;

	int totalObject = 0;
        for( int i=0; i<PART_MAX; i++ )
        {
                if( i == BRID )
                {
                        for( int j=0; j<B_SEG_NUM; j++ )
                        {	
				// set panel number
				int idMin, idMax;
				if( j != B_TOP ) { idMin = 0; idMax = B_GP_NUM;	}
				else		 { idMin = -1; idMax = 3;    	}

                                for( int k=0; k<B_LAY_NUM; k++ )
                                {
                                        for( int n=idMin; n<idMax; n++ )
                                        {
                                                MucBox *aMucBox = new MucBox( i, j, k, n );

						if( j == B_TOP && n != -1 ) // barrel top segment panels
						{
                                                  fPanel<< i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucBox->GetWu() <<"\t"
                                                        << aMucBox->GetWd() <<"\t"
                                                        << aMucBox->GetH()  <<"\t"
                                                        << aMucBox->GetL()  <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						}
						
						if( j !=B_TOP || n == -1 ) // box
						{
                                                  fOrigin << i << "\t" << j << "\t" << k << "\t"
                                                        << aMucBox->GetW() <<"\t"
                                                        << aMucBox->GetH() <<"\t"
                                                        << aMucBox->GetL() <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;

						  totalObject++;
						  // delete aMucBox;
						}

                                                fPos    << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucBox->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucBox->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucBox->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucBox->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucBox->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucBox->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						// delete aMucBox;
                                        } // panel
                                } // layer
                        } // segment
                } // barrel
                else
                {
                        for( int j=0; j<E_SEG_NUM; j++ )
                        {
                                for( int k=0; k<E_LAY_NUM; k++ )
                                {
                                        for( int n=-1; n<E_PANEL_NUM; n++ )
                                        {
                                                MucBox *aMucBox = new MucBox( i, j, k, n );
                                                if( n == -1 )
                                                {
                                                 fOrigin<< i << "\t" << j << "\t" << k << "\t"
                                                        << aMucBox->GetW() <<"\t"
                                                        << aMucBox->GetH() <<"\t"
                                                        << aMucBox->GetL() <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;

						 totalObject++;
						 // delete aMucBox;
                                                }
                                                else
                                                {
                                                  fPanel<< i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucBox->GetWu() <<"\t"
                                                        << aMucBox->GetWd() <<"\t"
                                                        << aMucBox->GetH()  <<"\t"
                                                        << aMucBox->GetL()  <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						  // delete aMucBox;
						}

                                                  fPos  << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucBox->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucBox->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucBox->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucBox->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucBox->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucBox->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						  // delete aMucBox;
                                        }
                                } // layer
                        } // segment
                } // endcap
        } // for

        fOrigin.close();
        fPanel.close();
        fPos.close();

	//-----------------------------real geometry-------------------------

	if( m_AlignFlag )
	{
		ofstream fOrgAlign("MucBoxOriginAligned.dat", ios::out);
		fOrgAlign  << "part\tsegment\tlayer\tW\tH\tL\tLoc_x\tLoc_y\tLoc_z" << endl;
		double offset[3];
		for( int i=0; i<PART_MAX; i++ )
        	{
                	if( i == BRID )
                	{
                        	for( int j=0; j<B_SEG_NUM; j++ )
                        	{
                                	for( int k=0; k<B_LAY_NUM; k++ )
                                	{
                                                MucBox *aMucBox = new MucBox( i, j, k, ((j==B_TOP)?-1:0) );
						offset[0] =m_BoxOffset[i][j][k][0];
					        offset[1] =m_BoxOffset[i][j][k][1];
						offset[2] =m_BoxOffset[i][j][k][2];
						aMucBox->SetAlignment( offset[0], offset[1], offset[2] );

                                                fOrgAlign << i << "\t" << j << "\t" << k << "\t"
                                                        << aMucBox->GetW() <<"\t"
                                                        << aMucBox->GetH() <<"\t"
                                                        << aMucBox->GetL() <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						// delete aMucBox;
                                	} // layer
                        	} // segment
                	} // barrel
			else
                	{
                        	for( int j=0; j<E_SEG_NUM; j++ )
                        	{
                                	for( int k=0; k<E_LAY_NUM; k++ )
                                	{
                                                MucBox *aMucBox = new MucBox( i, j, k, -1 );
						offset[0] =m_BoxOffset[i][j][k][0];
					        offset[1] =m_BoxOffset[i][j][k][1];
						offset[2] =m_BoxOffset[i][j][k][2];
						aMucBox->SetAlignment( offset[0], offset[1], offset[2] );
						
                                                 fOrgAlign<< i << "\t" << j << "\t" << k << "\t"
                                                        << aMucBox->GetW() <<"\t"
                                                        << aMucBox->GetH() <<"\t"
                                                        << aMucBox->GetL() <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucBox->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						// delete aMucBox;
                                	} // layer
                        	} // segment
                	} // endcap
		} // for
		
		fOrgAlign.close();
	} // if


        log << MSG::INFO << totalObject << "\tboxes created." << endreq;

        return StatusCode::SUCCESS;
} // MucBox


//------------------MucStripPlane-------------
// Alignment
StatusCode MucGeoMgr::CreateStripPlane()
{
	MsgStream log(msgSvc, "MucGeoMgr");

        //-------------------------- ideal geometry----------------------
        ofstream fOrigin("MucStripPlaneOrigin.dat", ios::out);
        ofstream fPanel("MucStripPlanePanel.dat", ios::out);
        ofstream fPos("MucStripPlanePanelPos.dat", ios::out);

        if( fOrigin.bad() || fPanel.bad() || fPos.bad() )
        {
                log << MSG::INFO << "StripPlane: create ouput file error!" << endl;
	 	return StatusCode::FAILURE;
        }
        fOrigin << "part\tsegment\tlayer\tW\tH\tL\tLoc_x\tLoc_y\tLoc_z" << endl;
        fPanel  << "part\tsegment\tlayer\tpanel\tWu\tWd\tH\tL\tLoc_x\tLoc_y\tLoc_z" << endl;
        fPos    << "part\tsegment\tlayer\tpanel\tLBes_x\tLBes_y\tLBes_z\tOBes_x\tOBes_y\tOBes_z\tLoc_x\tLoc_y\tLoc_z" << endl;
	
	int totalObject = 0;

        for( int i=0; i<PART_MAX; i++ )
        {
                if( i == BRID )
                {
                        for( int j=0; j<B_SEG_NUM; j++ )
                        {
                                for( int k=0; k<B_LAY_NUM; k++ )
                                {	
					if( j==B_TOP )
					{
                                           for( int n=-1; n<B_STR_PANEL_NUM; n++ )
					   {	   
						MucStripPlane *aMucStripPlane = new MucStripPlane( i, j, k, n );
						if( n == -1 )
						{
                                                  fOrigin << i << "\t" << j << "\t" << k << "\t"
                                                        << aMucStripPlane->GetW() <<"\t"
                                                        << aMucStripPlane->GetH() <<"\t"
                                                        << aMucStripPlane->GetL() <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
                                                  fPos  << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucStripPlane->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucStripPlane->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucStripPlane->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						  totalObject++;
						  // delete aMucStripPlane;
						}
						else
						{	
					    	  fPanel<< i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucStripPlane->GetWu() <<"\t"
                                                        << aMucStripPlane->GetWd() <<"\t"
                                                        << aMucStripPlane->GetH()  <<"\t"
                                                        << aMucStripPlane->GetL()  <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;

                                                  fPos  << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucStripPlane->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucStripPlane->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucStripPlane->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						  // delete aMucStripPlane;	
						}
					   }// for	
					}// B_TOP
					else	
                                        {
                                                MucStripPlane *aMucStripPlane = new MucStripPlane( i, j, k, 0 );
                                                fOrigin << i << "\t" << j << "\t" << k << "\t"
                                                        << aMucStripPlane->GetW() <<"\t"
                                                        << aMucStripPlane->GetH() <<"\t"
                                                        << aMucStripPlane->GetL() <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
                                                fPos    << i << "\t" << j << "\t" << k << "\t" << "\t"
                                                        << aMucStripPlane->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucStripPlane->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucStripPlane->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						totalObject++;
						// delete aMucStripPlane;
                                        }
                                } // layer
                        } // segment
                } // barrel
                else
                {
                        for( int j=0; j<E_SEG_NUM; j++ )
                        {
                                for( int k=0; k<E_LAY_NUM; k++ )
                                {
                                        for( int n=-1; n<E_PANEL_NUM; n++ )
                                        {
                                                MucStripPlane *aMucStripPlane = new MucStripPlane( i, j, k, n );
                                                if( n == -1 )
                                                {
                                                 fOrigin<< i << "\t" << j << "\t" << k << "\t"
                                                        << aMucStripPlane->GetW() <<"\t"
                                                        << aMucStripPlane->GetH() <<"\t"
                                                        << aMucStripPlane->GetL() <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;

						  fPos  << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucStripPlane->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucStripPlane->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucStripPlane->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						 totalObject++;
						 // delete aMucStripPlane;
                                                }
                                                else
                                                {
                                                  fPanel<< i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucStripPlane->GetWu() <<"\t"
                                                        << aMucStripPlane->GetWd() <<"\t"
                                                        << aMucStripPlane->GetH()  <<"\t"
                                                        << aMucStripPlane->GetL()  <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;

                                                  fPos  << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucStripPlane->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucStripPlane->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucStripPlane->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						  // delete aMucStripPlane;
                                                }
                                        }
                                } // layer
                        } // segment
                } // endcap
	} // for

        fOrigin.close();
        fPanel.close();
        fPos.close();

        //-----------------------------real geometry-------------------------

        if( m_AlignFlag )
        {
                ofstream fOrgAlign("MucStripPlaneOriginAligned.dat", ios::out);
                fOrgAlign  << "part\tsegment\tlayer\tW\tH\tL\tLoc_x\tLoc_y\tLoc_z" << endl;
		double offset[3]; 
                for( int i=0; i<PART_MAX; i++ )
                {
                        if( i == BRID )
                        {
                                for( int j=0; j<B_SEG_NUM; j++ )
                                {
                                        for( int k=0; k<B_LAY_NUM; k++ )
                                        {
                                                MucStripPlane *aMucStripPlane = new MucStripPlane( i, j, k, 0 );
						offset[0] =m_StripPlaneOffset[i][j][k][0];
					        offset[1] =m_StripPlaneOffset[i][j][k][1];
					        offset[2] =m_StripPlaneOffset[i][j][k][2];
						aMucStripPlane->SetAlignment( offset[0], offset[1], offset[2] );
						
                                                fOrgAlign << i << "\t" << j << "\t" << k << "\t"
                                                        << aMucStripPlane->GetW() <<"\t"
                                                        << aMucStripPlane->GetH() <<"\t"
                                                        << aMucStripPlane->GetL() <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						// delete aMucStripPlane;
                                        } // layer
                                } // segment
                        } // barrel
                        else
                        {
                                for( int j=0; j<E_SEG_NUM; j++ )
                                {
                                        for( int k=0; k<E_LAY_NUM; k++ )
                                        {
                                                MucStripPlane *aMucStripPlane = new MucStripPlane( i, j, k, -1 );
						offset[0] =m_StripPlaneOffset[i][j][k][0];
					        offset[1] =m_StripPlaneOffset[i][j][k][1];
					        offset[2] =m_StripPlaneOffset[i][j][k][2];
						aMucStripPlane->SetAlignment( offset[0], offset[1], offset[2] );

                                                 fOrgAlign<< i << "\t" << j << "\t" << k << "\t"
                                                        << aMucStripPlane->GetW() <<"\t"
                                                        << aMucStripPlane->GetH() <<"\t"
                                                        << aMucStripPlane->GetL() <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStripPlane->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						// delete aMucStripPlane;
                                        } // layer
                                } // segment
                        } // endcap
                } // for

		fOrgAlign.close();
        } // if
	

        log << MSG::INFO << totalObject << "\tstrip_planes created." << endreq;

        return StatusCode::SUCCESS;

} // MucStripPlane

//------------------MucStrip------------------
StatusCode MucGeoMgr::CreateStrip()
{
	MsgStream log(msgSvc, "MucGeoMgr");

        //-------------------------- ideal geometry----------------------
        ofstream fOrigin("MucStrip.dat", ios::out);
        ofstream fPos("MucStripPos.dat", ios::out);	

        if( fOrigin.bad() || fPos.bad() )
        {
                log << MSG::INFO << "Strip: create ouput file error!" << endl;
		return StatusCode::FAILURE;
        }
        fOrigin << "part\tsegment\tlayer\tstrip\ttype\tW\tH\tL\tLoc_x\tLoc_y\tLoc_z\tBes_x\tBes_y\tBes_z" << endl;
	fPos 	<< "part\tsegment\tlayer\tstrip\ttype\tLBes_x\tLBes_y\tLBes_z\tOBes_x\tOBes_y\tOBes_z\tLoc_x\tLoc_y\tLoc_z" << endl;
	
	int totalObject = 0;
	
        for( int i=0; i<PART_MAX; i++ )
        {
                if( i == BRID )
                {
                        for( int j=0; j<B_SEG_NUM; j++ )
                        {
                                for( int k=0; k<B_LAY_NUM; k++ )
                                {
			                // Set maximum strip 
                			int maxStrip;
                			if( ( k+1 )%2 == 1 )
                        			maxStrip = B_ZSTR_NUM;          // odd layer
                			else if( j != B_TOP )
                        			maxStrip = B_PHISTR_NUM;        // even layer not top segment
                			else
                        			maxStrip = B_TOPSTR_NUM;        // even layer top segment 

                                        for( int n=0; n<maxStrip; n++ )
                                        {
                                                MucStrip *aMucStrip = new MucStrip( i, j, k, n );

						fOrigin	<< i << "\t" << j << "\t" << k << "\t" << n << "\t"
						     	<< aMucStrip->GetType()		  << "\t"
						     	<< aMucStrip->GetW() 		  << "\t"
						     	<< aMucStrip->GetH() 		  << "\t"
                                                        << aMucStrip->GetL() 		  <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(3)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(3)   <<"\t"
                                                        << endl;

						fPos    << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucStrip->GetType()           <<"\t"
                                                        << aMucStrip->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucStrip->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucStrip->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;

						totalObject++;
						// delete aMucStrip;
                                        } // for
                                } // layer
                        } // segment
                } // barrel
                else
                {
                        for( int j=0; j<E_SEG_NUM; j++ )
                        {
                                for( int k=0; k<E_LAY_NUM; k++ )
                                {
                                        for( int n=0; n<E_STR_NUM; n++ )
                                        {
                                                MucStrip *aMucStrip = new MucStrip( i, j, k, n );

                                                fOrigin << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucStrip->GetType()   	  << "\t"
                                                        << aMucStrip->GetW() 		  <<"\t"
                                                        << aMucStrip->GetH() 	 	  <<"\t"
                                                        << aMucStrip->GetL() 		  <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(3)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(3)   <<"\t"
                                                        << endl;

                                                fPos  	<< i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucStrip->GetType()           << "\t"
                                                        << aMucStrip->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucStrip->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucStrip->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucStrip->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucStrip->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
						totalObject++;
						// delete aMucStrip;
                                        } // for
                                } // layer
                        } // segment
                } // endcap
        } // for

	fOrigin.close();
	fPos.close();	

        log << MSG::INFO << totalObject << "\tstrips created." << endreq;

        return StatusCode::SUCCESS;

} // MucStrip


//------------------MucRpc--------------------
StatusCode MucGeoMgr::CreateRpc()
{
        MsgStream log(msgSvc, "MucGeoMgr");

        //-------------------------- ideal geometry----------------------

        ofstream fOrigin("MucRpc.dat", ios::out);
        ofstream fPos("MucRpcPos.dat", ios::out);

        if( fOrigin.bad() || fPos.bad() )
        {
                log << MSG::INFO << "Rpc: create ouput file error!" << endl;
                return StatusCode::FAILURE;
        }
        fOrigin << "part\tsegment\tlayer\tupDown\trpc\tWu\tWd\tH\tL\tLoc_x\tLoc_y\tLoc_z\tBes_x\tBes_y\tBes_z" << endl;
        fPos    << "part\tsegment\tlayer\tupDown\trpc\tLBes_x\tLBes_y\tLBes_z\tOBes_x\tOBes_y\tOBes_z\tLoc_x\tLoc_y\tLoc_z" << endl;

        int totalObject = 0;

        for( int i=0; i<PART_MAX; i++ )
        {
                if( i == BRID )
                {
                        for( int j=0; j<B_SEG_NUM; j++ )
                        {
                                // Set maximum rpc
                                int maxRpc;
                                if( j ==B_TOP )
                                        maxRpc = B_TOP_RPC_NUM;    // top segment
                                else
                                        maxRpc = B_RPC_NUM;        // not top segment

                                for( int k=0; k<B_LAY_NUM; k++ )
                                {
                                        for( int m=0; m<SL_NUM; m++ )
                                        {
                                                for( int n=0; n<maxRpc; n++ )
                                                {
                                                        MucRpc *aMucRpc = new MucRpc( i, j, k, m, n );

                                                fOrigin << i << "\t" << j << "\t" << k << "\t"
                                                        << m << "\t" << n << "\t"
                                                        << aMucRpc->GetWu()             << "\t"
                                                        << aMucRpc->GetWd()             << "\t"
                                                        << aMucRpc->GetH()              << "\t"
                                                        << aMucRpc->GetL()              <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(3)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(3)   <<"\t"
                                                        << endl;

                                                fPos    << i << "\t" << j << "\t" << k << "\t" 
							<< m << "\t" << n << "\t"
                                                        << aMucRpc->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucRpc->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucRpc->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;

                                                totalObject++;
						// delete aMucRpc;
                                                } // for
                                        } // super layer
                                } // layer
                        } // segment
                } // barrel
                else
                {
                        for( int j=0; j<E_SEG_NUM; j++ )
                        {
                                for( int k=0; k<E_LAY_NUM; k++ )
                                {
                                        for( int m=0; m<SL_NUM; m++ )
                                        {
                                                for( int n=0; n<E_RPC_NUM[m]; n++ )
                                                {
                                                        MucRpc *aMucRpc = new MucRpc( i, j, k, m, n );

                                                fOrigin << i << "\t" << j << "\t" << k << "\t"
                                                        << m << "\t" << n << "\t"
                                                        << aMucRpc->GetWu()              <<"\t"
                                                        << aMucRpc->GetWd()              <<"\t"
                                                        << aMucRpc->GetH()              <<"\t"
                                                        << aMucRpc->GetL()              <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(3)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(3)   <<"\t"
                                                        << endl;

                                                fPos    << i << "\t" << j << "\t" << k << "\t"
                                                        << m << "\t" << n << "\t"
                                                        << aMucRpc->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucRpc->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucRpc->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucRpc->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucRpc->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
                                                totalObject++;
						// delete aMucRpc;
                                                } // for
                                        } // super layer
                                } // layer
                        } // segment
                } // endcap
        } // for, part

	fOrigin.close();
	fPos.close();

        log << MSG::INFO << totalObject << "\t RPCs created." << endreq;

        return StatusCode::SUCCESS;

} // MucRpc

//------------------MucGas--------------------
StatusCode MucGeoMgr::CreateGas()
{
        MsgStream log(msgSvc, "MucGeoMgr");

        //-------------------------- ideal geometry----------------------

        ofstream fOrigin("MucGas.dat", ios::out);
        ofstream fPos("MucGasPos.dat", ios::out);

        if( fOrigin.bad() || fPos.bad() )
        {
                log << MSG::INFO << "Gas: create ouput file error!" << endl;
                return StatusCode::FAILURE;
        }
        fOrigin << "part\tsegment\tlayer\tupDown\tgas\tWu\tWd\tH\tL\tLoc_x\tLoc_y\tLoc_z\tBes_x\tBes_y\tBes_z" << endl;
        fPos    << "part\tsegment\tlayer\tupDown\tgas\tLBes_x\tLBes_y\tLBes_z\tOBes_x\tOBes_y\tOBes_z\tLoc_x\tLoc_y\tLoc_z" << endl;

        int totalObject = 0;

        for( int i=0; i<PART_MAX; i++ )
        {
                if( i == BRID )
                {
                        for( int j=0; j<B_SEG_NUM; j++ )
                        {
                                // Set maximum gas
                                int maxRpc;
                                if( j ==B_TOP )
                                        maxRpc = B_TOP_RPC_NUM;    // top segment
                                else
                                        maxRpc = B_RPC_NUM;        // not top segment

                                for( int k=0; k<B_LAY_NUM; k++ )
                                {
                                        for( int m=0; m<SL_NUM; m++ )
                                        {
                                                for( int n=0; n<maxRpc; n++ )
                                                {
                                                        MucGas *aMucGas = new MucGas( i, j, k, m, n, 0);

                                                fOrigin << i << "\t" << j << "\t" << k << "\t"
                                                        << m << "\t" << n << "\t"
                                                        << aMucGas->GetWu()             <<"\t"
                                                        << aMucGas->GetWd()             <<"\t"
                                                        << aMucGas->GetH()              <<"\t"
                                                        << aMucGas->GetL()              <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(3)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(3)   <<"\t"
                                                        << endl;

                                                fPos    << i << "\t" << j << "\t" << k << "\t" << n << "\t"
                                                        << aMucGas->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucGas->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucGas->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;

                                                totalObject++;
						// delete aMucGas;
                                                } // for
                                        } // super layer
                                } // layer
                        } // segment
                } // barrel
                else
                {
                        for( int j=0; j<E_SEG_NUM; j++ )
                        {
                                for( int k=0; k<E_LAY_NUM; k++ )
                                {
                                        for( int m=0; m<SL_NUM; m++ )
                                        {
                                                for( int n=0; n<E_RPC_NUM[m]; n++ )
                                                {
                                                        MucGas *aMucGas = new MucGas( i, j, k, m, n, 0 );

                                                fOrigin << i << "\t" << j << "\t" << k << "\t"
                                                        << m << "\t" << n << "\t"
                                                        << aMucGas->GetWu()              <<"\t"
                                                        << aMucGas->GetWd()              <<"\t"
                                                        << aMucGas->GetH()              <<"\t"
                                                        << aMucGas->GetL()              <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(3)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(3)   <<"\t"
                                                        << endl;

                                                fPos    << i << "\t" << j << "\t" << k << "\t"
                                                        << m << "\t" << n << "\t"
                                                        << aMucGas->GetLocOrgInBes(1)   <<"\t"
                                                        << aMucGas->GetLocOrgInBes(2)   <<"\t"
                                                        << aMucGas->GetLocOrgInBes(3)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(1)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(2)   <<"\t"
                                                        << aMucGas->GetObjOrgInBes(3)   <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(1)   <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(2)   <<"\t"
                                                        << aMucGas->GetObjOrgInLoc(3)   <<"\t"
                                                        << endl;
                                                totalObject++;
						// delete aMucGas;
                                                } // for
                                        } // super layer
                                } // layer
                        } // segment
                } // endcap
        } // for, part

	fOrigin.close();
	fPos.close();

        log << MSG::INFO << totalObject << "\tgases created." << endreq;

        return StatusCode::SUCCESS;

} // MucGas

//------------------MucBakelite---------------
StatusCode MucGeoMgr::CreateBakelite()
{
        MsgStream log(msgSvc, "MucGeoMgr");

        //-------------------------- ideal geometry----------------------

        ofstream fOrigin("MucBakelite.dat", ios::out);
        ofstream fPos("MucBakelitePos.dat", ios::out);

        if( fOrigin.bad() || fPos.bad() )
        {
                log << MSG::INFO << "Bakelite: create ouput file error!" << endl;
                return StatusCode::FAILURE;
        }
        fOrigin << "part\tsegment\tlayer\tupDown\trpcId\tBKLT\t"
                << "Wu\tWd\tH\tL\tLoc_x\tLoc_y\tLoc_z\tBes_x\tBes_y\tBes_z" << endl;
        fPos    << "part\tsegment\tlayer\tupDown\trpcId\tBKLT\t"
                << "LBes_x\tLBes_y\tLBes_z\tOBes_x\tOBes_y\tOBes_z\tLoc_x\tLoc_y\tLoc_z" << endl;

        int totalObject = 0;

        for( int i=0; i<PART_MAX; i++ )
        {
                if( i == BRID )
                {
                        for( int j=0; j<B_SEG_NUM; j++ )
                        {
                                // Set maximum rpc number
                                int maxRpc;
                                if( j ==B_TOP )
                                        maxRpc = B_TOP_RPC_NUM;    // top segment
                                else
                                        maxRpc = B_RPC_NUM;        // not top segment

                                for( int k=0; k<B_LAY_NUM; k++ )
                                {
                                        for( int m=0; m<SL_NUM; m++ )
                                        {
                                                for( int n=0; n<maxRpc; n++ )
                                                {
                                                        for( int t=0; t<BKLT_NUM; t++)
                                                        {
                                                         MucBakelite *aMucBakelite = new MucBakelite( i, j, k, m, n, t );

                                                         fOrigin<< i << "\t" << j << "\t" << k << "\t"
                                                                << m << "\t" << n << "\t" << t << "\t"
                                                                << aMucBakelite->GetWu()             <<"\t"
                                                                << aMucBakelite->GetWd()             <<"\t"
                                                                << aMucBakelite->GetH()              <<"\t"
                                                                << aMucBakelite->GetL()              <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(1)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(2)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(3)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(1)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(2)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(3)   <<"\t"
                                                                << endl;

                                                         fPos   << i << "\t" << j << "\t" << k << "\t"
                                                                << m << "\t" << n << "\t" << t << "\t"
                                                                << aMucBakelite->GetLocOrgInBes(1)   <<"\t"
                                                                << aMucBakelite->GetLocOrgInBes(2)   <<"\t"
                                                                << aMucBakelite->GetLocOrgInBes(3)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(1)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(2)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(3)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(1)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(2)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(3)   <<"\t"
                                                                << endl;

                                                         totalObject++;
							 // delete aMucBakelite;	
                                                        } // bakelite
                                                } // rpc
                                        } // super layer
                                } // layer
                        } // segment
                } // barrel
                else
                {
                        for( int j=0; j<E_SEG_NUM; j++ )
                        {
                                for( int k=0; k<E_LAY_NUM; k++ )
                                {
                                        for( int m=0; m<SL_NUM; m++ )
                                        {
                                                for( int n=0; n<E_RPC_NUM[m]; n++ )
                                                {
                                                        for( int t=0; t<BKLT_NUM; t++ )
                                                        {
                                                         MucBakelite *aMucBakelite = new MucBakelite( i, j, k, m, n, t );

                                                         fOrigin<< i << "\t" << j << "\t" << k << "\t"
                                                                << m << "\t" << n << "\t" << t << "\t"
                                                                << aMucBakelite->GetWu()             <<"\t"
                                                                << aMucBakelite->GetWd()             <<"\t"
                                                                << aMucBakelite->GetH()              <<"\t"
                                                                << aMucBakelite->GetL()              <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(1)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(2)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(3)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(1)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(2)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(3)   <<"\t"
                                                                << endl;

                                                         fPos   << i << "\t" << j << "\t" << k << "\t"
                                                                << m << "\t" << n << "\t" << t << "\t"
                                                                << aMucBakelite->GetLocOrgInBes(1)   <<"\t"
                                                                << aMucBakelite->GetLocOrgInBes(2)   <<"\t"
                                                                << aMucBakelite->GetLocOrgInBes(3)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(1)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(2)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInBes(3)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(1)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(2)   <<"\t"
                                                                << aMucBakelite->GetObjOrgInLoc(3)   <<"\t"
                                                                << endl;

                                                         totalObject++;
							 // delete aMucBakelite;
                                                        } // bakelite
                                                } // rpc
                                        } // super layer
                                } // layer
                        } // segment
                } // endcap
        } // for, part

	fOrigin.close();
	fPos.close();

        log << MSG::INFO << totalObject << "\tbakelites created." << endreq;

        return StatusCode::SUCCESS;

} // MucBakelite

//------------------MucBoxCover---------------
StatusCode MucGeoMgr::CreateBoxCover()
{
        MsgStream log(msgSvc, "MucGeoMgr");

        //-------------------------- ideal geometry----------------------
        ofstream fOrigin("MucBoxCoverOrigin.dat", ios::out);
        ofstream fPanel("MucBoxCoverPanel.dat", ios::out);
        ofstream fPos("MucBoxCoverPanelPos.dat", ios::out);

        if( fOrigin.bad() || fPanel.bad() || fPos.bad() )
        {
                log << MSG::INFO << "BoxCover: create ouput file error!" << endl;
                return StatusCode::FAILURE;
        }
        fOrigin << "part\tsegment\tlayer\tU/D\tW\tH\tL\tLoc_x\tLoc_y\tLoc_z" << endl;
        fPanel  << "part\tsegment\tlayer\tU/D\tpanel\tWu\tWd\tH\tL\tLoc_x\tLoc_y\tLoc_z" << endl;
        fPos    << "part\tsegment\tlayer\tU/D\tpanel\tLBes_x\tLBes_y\tLBes_z\tOBes_x\tOBes_y\tOBes_z\tLoc_x\tLoc_y\tLoc_z" << endl;

        int totalObject = 0;

        for( int i=0; i<PART_MAX; i++ )
        {
                if( i == BRID )
                {
                        for( int j=0; j<B_SEG_NUM; j++ )
                        {
                                // set panel number
                                int idMin, idMax;
                                if( j != B_TOP ) { idMin = 0; idMax = B_GP_NUM; }
                                else             { idMin = -1; idMax = 3;       }
				
                                for( int k=0; k<B_LAY_NUM; k++ )
                                {
                                        for( int m=0; m<SL_NUM; m++ )
                                        {	
						for( int n = idMin; n<idMax; n++ )
						{
                                                	MucBoxCover *aMucBoxCover = new MucBoxCover( i, j, k, m, n );
						 	if( j == B_TOP || n != -1 ) // barrel top segment panels
                                                 	{
                                                  	  fPanel << i << "\t" << j << "\t" << k << "\t"
                                                        	<< m << "\t" << n << "\t"
	                                                        << aMucBoxCover->GetW() <<"\t"
                                                                << aMucBoxCover->GetW() <<"\t"
        	                                                << aMucBoxCover->GetH() <<"\t"
                	                                        << aMucBoxCover->GetL() <<"\t"
                        	                                << aMucBoxCover->GetObjOrgInLoc(1)   <<"\t"
                                	                        << aMucBoxCover->GetObjOrgInLoc(2)   <<"\t"
                                        	                << aMucBoxCover->GetObjOrgInLoc(3)   <<"\t"
                                                	        << endl;
							  	// delete aMucBoxCover;
							 }

							 if( j != B_TOP || n == -1 ) // box cover
							 {
                	                                  fOrigin << i << "\t" << j << "\t" << k << "\t" 
								<< m << "\t"  
                                	                        << aMucBoxCover->GetW() <<"\t"
                                        	                << aMucBoxCover->GetH() <<"\t"
                                                	        << aMucBoxCover->GetL() <<"\t"
	                                                        << aMucBoxCover->GetObjOrgInLoc(1)   <<"\t"
        	                                                << aMucBoxCover->GetObjOrgInLoc(2)   <<"\t"
                	                                        << aMucBoxCover->GetObjOrgInLoc(3)   <<"\t"
                        	                                << endl;
	
        	                                         	totalObject++;
								// delete aMucBoxCover;
							 }

                                                  	fPos << i << "\t" << j << "\t" << k << "\t" 
								<< m << "\t" << n << "\t"
	                                                        << aMucBoxCover->GetLocOrgInBes(1)   <<"\t"
        	                                                << aMucBoxCover->GetLocOrgInBes(2)   <<"\t"
                	                                        << aMucBoxCover->GetLocOrgInBes(3)   <<"\t"
                        	                                << aMucBoxCover->GetObjOrgInBes(1)   <<"\t"
                                	                        << aMucBoxCover->GetObjOrgInBes(2)   <<"\t"
                                        	                << aMucBoxCover->GetObjOrgInBes(3)   <<"\t"
                                                	        << aMucBoxCover->GetObjOrgInLoc(1)   <<"\t"
                                                        	<< aMucBoxCover->GetObjOrgInLoc(2)   <<"\t"
	                                                        << aMucBoxCover->GetObjOrgInLoc(3)   <<"\t"
        	                                                << endl;
								// delete aMucBoxCover;
						} // panel
                                        }// super layer
                                } // layer
                        } // segment
                } // barrel
                else
                {
                        for( int j=0; j<E_SEG_NUM; j++ )
                        {
                                for( int k=0; k<E_LAY_NUM; k++ )
                                {
                                        for( int m=0; m<SL_NUM; m++ )
                                        {
						for( int n=-1; n<E_PANEL_NUM; n++ )
						{
                                                	MucBoxCover *aMucBoxCover = new MucBoxCover( i, j, k, m, n );
                                                	if( n == -1 )
                                                	{
	                                                 fOrigin<< i << "\t" << j << "\t" << k << "\t" 
								<< m << "\t" 
        	                                                << aMucBoxCover->GetW() <<"\t"
                	                                        << aMucBoxCover->GetH() <<"\t"
                        	                                << aMucBoxCover->GetL() <<"\t"
                                	                        << aMucBoxCover->GetObjOrgInLoc(1)   <<"\t"
                                                	        << aMucBoxCover->GetObjOrgInLoc(2)   <<"\t"
                                        	                << aMucBoxCover->GetObjOrgInLoc(3)   <<"\t"
                                                        	<< endl;
	                                                 totalObject++;
							 // delete aMucBoxCover;
        	                                        }
                	                                else
                        	                        {
                                	                  fPanel<< i << "\t" << j << "\t" << k << "\t" 
								<< m << "\t" << n << "\t"
                                        	                << aMucBoxCover->GetWu() <<"\t"
                                                	        << aMucBoxCover->GetWd() <<"\t"
                                                        	<< aMucBoxCover->GetH()  <<"\t"
	                                                        << aMucBoxCover->GetL()  <<"\t"
        	                                                << aMucBoxCover->GetObjOrgInLoc(1)   <<"\t"
                	                                        << aMucBoxCover->GetObjOrgInLoc(2)   <<"\t"
                        	                                << aMucBoxCover->GetObjOrgInLoc(3)   <<"\t"
	                               	                        << endl;
							 // delete aMucBoxCover;
							}

	                                                  fPos  << i << "\t" << j << "\t" << k << "\t" 
								<< m << "\t" << n << "\t"
        	                                                << aMucBoxCover->GetLocOrgInBes(1)   <<"\t"
                	                                        << aMucBoxCover->GetLocOrgInBes(2)   <<"\t"
                        	                                << aMucBoxCover->GetLocOrgInBes(3)   <<"\t"
                                	                        << aMucBoxCover->GetObjOrgInBes(1)   <<"\t"
                                        	                << aMucBoxCover->GetObjOrgInBes(2)   <<"\t"
                                                	        << aMucBoxCover->GetObjOrgInBes(3)   <<"\t"
                                                        	<< aMucBoxCover->GetObjOrgInLoc(1)   <<"\t"
	                                                        << aMucBoxCover->GetObjOrgInLoc(2)   <<"\t"
        	                                                << aMucBoxCover->GetObjOrgInLoc(3)   <<"\t"
                	                                        << endl;
							  // delete aMucBoxCover;
						} // panel
                                        } // super layer
                                } // layer
                        } // segment
                } // endcap
        } // for

        fOrigin.close();
        fPanel.close();
        fPos.close();

        log << MSG::INFO << totalObject << "\tbox_covers created." << endreq;

        return StatusCode::SUCCESS;

} // MucBoxCover



//=================================== Geometry entities getting methods====================================
MucAbsorber* MucGeoMgr::GetAbsorber( int part, int segment, int layer, int id )
{
	if( m_MucAbsorber != NULL ) delete m_MucAbsorber;

	return ( m_MucAbsorber = new MucAbsorber(part, segment, layer, id) );
}

MucGap* MucGeoMgr::GetGap( int part, int segment, int layer, int id )
{
        if( m_MucGap != NULL ) delete m_MucGap;

	return ( m_MucGap = new MucGap(part, segment, layer, id) );
}

MucBox* MucGeoMgr::GetBox( int part, int segment, int layer, int id )
{
        if( m_MucBox != NULL ) delete m_MucBox;

	return ( m_MucBox = new MucBox(part, segment, layer, id) );
}

MucStripPlane* MucGeoMgr::GetStripPlane( int part, int segment, int layer, int id )
{
        if( m_MucStripPlane != NULL ) delete m_MucStripPlane;

        return ( m_MucStripPlane = new MucStripPlane(part, segment, layer, id) );
}

MucStrip* MucGeoMgr::GetStrip( int part, int segment, int layer, int id )
{
        if( m_MucStrip != NULL ) delete m_MucStrip;

        return ( m_MucStrip = new MucStrip(part, segment, layer, id) );
}

MucRpc* MucGeoMgr::GetRpc( int part, int segment, int layer, int upDown, int id )
{
        if( m_MucRpc != NULL ) delete m_MucRpc;

	return ( m_MucRpc = new MucRpc(part, segment, layer, upDown, id) );
}

MucGas* MucGeoMgr::GetGas( int part, int segment, int layer, int upDown, int rpcId, int id )
{
        if( m_MucGas != NULL ) delete m_MucGas;

        return ( m_MucGas = new MucGas(part, segment, layer, upDown, rpcId, id) );
}

MucBakelite* MucGeoMgr::GetBakelite( int part, int segment, int layer, int upDown, int rpcId, int id )
{
        if( m_MucBakelite != NULL ) delete m_MucBakelite;

        return ( m_MucBakelite = new MucBakelite(part, segment, layer, upDown, rpcId, id) );
}

MucBoxCover* MucGeoMgr::GetBoxCover( int part, int segment, int layer, int upDown, int id )
{
        if( m_MucBoxCover != NULL ) delete m_MucBoxCover;

	return ( m_MucBoxCover = new MucBoxCover(part, segment, layer, upDown, id) );
}

// END
