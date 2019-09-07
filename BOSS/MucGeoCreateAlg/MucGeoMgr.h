//------------------------------------------------------------------------------|
//      [File  ]:                       MucGeoMgr.h      	                      |
//      [Brief ]:       Header file of MUC geometry manager class   		        |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       Mar 28, 2006                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_GEO_MGR_ALG_H
#define MUC_GEO_MGR_ALG_H

#include<iostream>
#include<string>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IMessageSvc.h"

#include "MucGeoCreateAlg/MucGeoConst.h"
#include "MucGeoCreateAlg/MucEntity.h"
#include "MucGeoCreateAlg/MucAbsorber.h"
#include "MucGeoCreateAlg/MucGap.h"
#include "MucGeoCreateAlg/MucBox.h"
#include "MucGeoCreateAlg/MucStripPlane.h"
#include "MucGeoCreateAlg/MucStrip.h"
#include "MucGeoCreateAlg/MucRpc.h"
#include "MucGeoCreateAlg/MucBoxCover.h"
#include "MucGeoCreateAlg/MucBakelite.h"
#include "MucGeoCreateAlg/MucGas.h"
#include "MucGeoCreateAlg/MucIdTransform.h"

/*
#include "TH1F.h"
#include "TGeometry.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TCanvas.h"
*/

using namespace std;

const unsigned int ENTITY_NUM = 9;

class MucGeoMgr 
{
	  public:
    	MucGeoMgr( const std::string createFlag, bool alignFlag, const std::string alignFile );
      ~MucGeoMgr();
			
		StatusCode CreateEntities();	
		StatusCode CreateRootGeo();
		StatusCode CreateOnlineStripGeo();
                
	  MucAbsorber	*GetAbsorber( 	int part, int segment, int layer, int id );
		MucGap		*GetGap( 	int part, int segment, int layer, int id );
		MucBox		*GetBox( 	int part, int segment, int layer, int id );
		MucStripPlane   *GetStripPlane( int part, int segment, int layer, int id );
	  MucStrip	*GetStrip( 	int part, int segment, int layer, int id );
    MucRpc		*GetRpc( 	int part, int segment, int layer, int upDown, int id );
		MucGas		*GetGas( 	int part, int segment, int layer, int upDown, int rpcId, int id );
		MucBakelite	*GetBakelite( 	int part, int segment, int layer, int upDown, int rpcId, int id );
		MucBoxCover	*GetBoxCover( 	int part, int segment, int layer, int upDown, int id );

		IMessageSvc	*msgSvc;
	
		protected:

	  StatusCode CreateAbsorber();
    StatusCode CreateGap();
		StatusCode CreateBox();
		StatusCode CreateStripPlane();
	  StatusCode CreateStrip();
    StatusCode CreateRpc();
		StatusCode CreateGas();
		StatusCode CreateBakelite();
		StatusCode CreateBoxCover();

		StatusCode InitOffset();
		bool 	   CheckBoxOffset( int part, int segment, int layer, int axis, double offset);
    bool       CheckStripPlaneOffset( int part, int segment, int layer, int axis, double offset);

    private:

    bool        	m_AlignFlag;
		std::string	m_CreateFlag;
		std::string	m_AlignFile;
		double	 	m_BoxOffset[PART_MAX][B_SEG_NUM][B_LAY_NUM][3];
		double    m_StripPlaneOffset[PART_MAX][B_SEG_NUM][B_LAY_NUM][3];
  
    MucIdTransform* m_IdTr;			

		MucAbsorber	*m_MucAbsorber;
		MucGap		*m_MucGap;
		MucBox		*m_MucBox;
		MucStripPlane	*m_MucStripPlane;
		MucStrip	*m_MucStrip;
		MucRpc		*m_MucRpc;
		MucBoxCover	*m_MucBoxCover;
		MucBakelite	*m_MucBakelite;
		MucGas		*m_MucGas;

		//TGeoManager *geoMgr;

};

#endif
