//$id$
//
//$log$

/*
 *    2003/12/22   Zhengyun You     Peking University
 * 
 *    2005/02/24   Zhengyun You      Peking University
 *                 transplanted to Gaudi framework
 */

using namespace std;

#include <iostream>
#include <vector>
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Geometry/Point3D.h>

#include "Identifier/MucID.h"
#include "MucGeomSvc/MucGeomSvc.h"
#include "MucGeomSvc/MucConstant.h"
#include "MucRecEvent/MucRec2DRoad.h"
#include "MucRecEvent/MucRecLineFit.h"
#include "MucRecEvent/MucRecQuadFit.h"

const double muckInfinity = 1e30;
// Constructor.
MucRec2DRoad::MucRec2DRoad(const int& part,
			   const int& seg,
			   const int& orient,
			   const float& xVertex,
			   const float& yVertex,
			   const float& zVertex,
			   const int& fittingMethod)
  : m_VertexPos(xVertex, yVertex, zVertex),
    m_VertexSigma(0.0, 0.0, 0.0),
    m_Part(part), m_Seg(seg), m_Orient(orient),
    m_Chi2(0.0), m_DOF(0),
    m_MaxHitsPerGap(0), m_LastGap(0), m_TotalHits(0),
    m_FitOK(false), m_QuadFitOK(false),
    m_HitDistance(5, float(muckInfinity)),
    m_pHits(0), m_fittingMethod(fittingMethod)
{ }

// Assignment constructor.
MucRec2DRoad&
MucRec2DRoad::operator=(const MucRec2DRoad& orig)
{
  // Assignment operator.
  if ( this != &orig ) {             // Watch out for self-assignment!
    m_VertexPos     = orig.m_VertexPos;
    m_VertexSigma   = orig.m_VertexSigma;
    m_Index         = orig.m_Index;
    m_Part          = orig.m_Part;
    m_Seg           = orig.m_Seg;
    m_Orient        = orig.m_Orient;
    m_Chi2          = orig.m_Chi2; 
    m_DOF           = orig.m_DOF;
    m_FitOK         = orig.m_FitOK;
    m_MaxHitsPerGap = orig.m_MaxHitsPerGap;
    m_LastGap       = orig.m_LastGap;
    m_TotalHits     = orig.m_TotalHits;
    m_HitDistance   = orig.m_HitDistance;
    m_pHits         = orig.m_pHits;
    m_fittingMethod = orig.m_fittingMethod;
  }
  
  return *this;
}

// Copy constructor.
MucRec2DRoad::MucRec2DRoad(const MucRec2DRoad& source)
  : m_VertexPos(source.m_VertexPos),
    m_VertexSigma(source.m_VertexSigma),
    m_Index(source.m_Index),
    m_Part(source.m_Part), m_Seg(source.m_Seg), m_Orient(source.m_Orient),
    m_Chi2(source.m_Chi2), m_DOF(source.m_DOF),
    m_MaxHitsPerGap(source.m_MaxHitsPerGap),
    m_LastGap(source.m_LastGap), m_TotalHits(source.m_TotalHits),
    m_FitOK(source.m_FitOK), 
    m_HitDistance(source.m_HitDistance),
    m_pHits(source.m_pHits),
    m_fittingMethod(source.m_fittingMethod)
{ }

// Destructor.
MucRec2DRoad::~MucRec2DRoad()
{ }

// Set the index for this road.
void
MucRec2DRoad::SetIndex(const int& index)
{
  if (index >= 0) m_Index = index;
}

// Attach the given hit this road.
// Assume that this hit has been verified to be consistent with the road.
void
MucRec2DRoad::AttachHit(MucRecHit* hit)
{
  //  cout << "MucRec2DRoad::AttachHit-I0  hit = " << hit << endl;
  if (!hit) {
    cout << "MucRec2DRoad::AttachHit-E1  null hit pointer!" << endl;
    return ;
  }
  
  int gap = hit->Gap();
  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    // The gap number of the hit is out of range.
    cout << "MucRec2DRoad::AttachHit-W3  bad gap number = " << gap
	 << endl;
    return;
  }

  // Attach the hit to the road.
  //bool hitExist = false;
  for (int i = 0; i < (int)m_pHits.size(); i++) {
    if (m_pHits[i] == hit) return;
  }
  m_pHits.push_back(hit);
  //cout << "before Count " << m_pHits.size() << endl;
   //  m_HitDistance[gap] = dX;

  // Now recalculate the total number of hits and the max. number of
  // hits per gap.
  CountHits();
  //cout << "after Count " << m_pHits.size() << endl;

  // Redo the "simple" least-squares fit to the positions ...
  float a, sa, b, sb, chi;
  int n;
  int status = SimpleFit(a, b, sa, sb, chi, n);
  if (status < 0) {
    //cout << "MucRec2DRoad::AttachHit-E5  simple fit fail status = " << status << endl;
  }

  //cout << "Hit center = " << hit->GetCenterPos() << endl;
  //cout << "After attach hit, slope = " << a << "  intercept = " << b << endl;
}

// Attach the given hit this road.
// Assume that this hit has been verified to be consistent with the road.
void
MucRec2DRoad::AttachHitNoFit(MucRecHit* hit)
{
  //  cout << "MucRec2DRoad::AttachHit-I0  hit = " << hit << endl;
  if (!hit) {
    cout << "MucRec2DRoad::AttachHit-E1  null hit pointer!" << endl;
    return ;
  }
  
  int gap = hit->Gap();
  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    // The gap number of the hit is out of range.
    cout << "MucRec2DRoad::AttachHit-W3  bad gap number = " << gap
	 << endl;
    return;
  }

  // Attach the hit to the road.
  //bool hitExist = false;
  for (int i = 0; i < (int)m_pHits.size(); i++) {
    if (m_pHits[i] == hit) return;
  }
  m_pHits.push_back(hit);
  //cout << "before Count " << m_pHits.size() << endl;
   //  m_HitDistance[gap] = dX;

  // Now recalculate the total number of hits and the max. number of
  // hits per gap.
  CountHits();
  //cout << "after Count " << m_pHits.size() << endl;

}

// Max number of consecutive gaps allowed with no hits attached.
// This parameter affects the calculation of the last gap.
void
MucRec2DRoad::SetMaxNSkippedGaps(const int& numGaps)
{
  m_MaxNSkippedGaps = numGaps;
  CountHits();    // recalculate the last gap and the hit counts.
}

// Calculate the best-fit straight line with "simple" weights.
int
MucRec2DRoad::SimpleFit(float& slope, 
			float& intercept,
			float& sigmaSlope,
			float& sigmaIntercept,
			float& chisq,
			int&   ndof)
{
//hits in one track can not more than 100.
  if(m_pHits.size()>100){
	cout<<"MucRec2DRoad: too many hits in this track!"<<endl;
	return -1;
	}
  // Assign to temporary arrays to be used in fit.
  float px[100];
  float py[100];
  float pw[100];
  int npoints = 0;

  vector<MucRecHit*>::const_iterator iHit;

  float weight[100];

//    for (int i = 0; i < m_pHits.size(); i++) {
//	cout<<"info: "<<m_pHits[i]->Seg()<<" "<<m_pHits[i]->Gap()<<" "<< m_pHits[i]->Strip()<<endl;
//    }
  for (int i = 0; i < m_pHits.size(); i++) {
     weight[i] = 1;
	  
     for(int j = 0; j < m_pHits.size(); j++){

	if(j == i) continue;
     	if(m_pHits[i]->Part() == m_pHits[j]->Part() &&
	   m_pHits[i]->Seg()  == m_pHits[j]->Seg() &&
	   m_pHits[i]->Gap()  == m_pHits[j]->Gap() )
	 {
	   int deltaStrip = fabs(m_pHits[i]->Strip()- m_pHits[j]->Strip());

	   //cout<<i<<"  "<<m_pHits[i]->Seg()<<"  "<<m_pHits[i]->Gap()<<"  "<<m_pHits[i]->Strip()<<" - "<<m_pHits[j]->Strip()<<endl;
	   if(deltaStrip == 0){
             cout<<"deltaStrip == 0 ? check it"<<endl;
	   } 
	   else{
	     weight[i] *=  (deltaStrip+1) * (deltaStrip+1);
	   }

	 }		 
     }
  }
  
  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      
      /*
	float localx, localy, localz;
      (*iHit)->GetStrip()->GetCenterPos(localx, localy, localz);
      if ( m_Orient == 0) {
	px[npoints] = localy;
	py[npoints] = localz;
      }
      else {
	px[npoints] = localx;
	py[npoints] = localz;
      }
      npoints++;
      }
      }
      */
      
      
      Hep3Vector center = (*iHit)->GetCenterPos();
      Hep3Vector sigma  = (*iHit)->GetCenterSigma();
      //Hep3Vector sigma(1.0, 1.0, 1.0);

      if (m_Part == 1) {
        if ( m_Orient == 0) {
          px[npoints] = center.z();
          py[npoints] = sqrt(center.x()*center.x() 
              + center.y()*center.y());
          if(m_Seg==2) py[npoints] = center.y();  //deal with seg2 seperately! because there is a hole here. 2006.11.9

          pw[npoints] = 4.0 * 1.0/(sigma.y()*sigma.y()) / weight[npoints];
        } 
        else {
          px[npoints] = center.x();
          py[npoints] = center.y();
          pw[npoints] = 4.0 * 1.0/(sigma.x()*sigma.x()) / weight[npoints];
        }
      }
      else {
        if ( m_Orient == 0) {
          px[npoints] = center.z();
          py[npoints] = center.y();
          pw[npoints] = 4.0 * 1.0/(sigma.y()*sigma.y()) / weight[npoints];
        }
        else {
          px[npoints] = center.z();
          py[npoints] = center.x();
          pw[npoints] = 4.0 * 1.0/(sigma.x()*sigma.x()) / weight[npoints];
        }
      }  

      //cout << " in MucRec2DRoad  ID: " <<(*iHit)->Part()<<" "<<(*iHit)->Seg()<<" "<<(*iHit)->Gap()<<" "<< (*iHit)->Strip()<<" "<<px[npoints] << "  " << py[npoints] << "  " << pw[npoints] << endl;
      npoints++;

      if(npoints > 99) 
      {  cout<<"MucRec2DRoad: to many hits in this track, ignore it!"<<endl;
        return -1;
      }   
  }
}
/*
float px2[100];
float py2[100];
for (int i = 0; i < m_pHits.size(); i++) {
	int hitsInGap = 1;
	px2[i] = -999; py2[i] = -999;
	for(int j = 0; j < m_pHits.size(); j++){

		if(j == i) continue;
		if(m_pHits[i]->Part() == m_pHits[j]->Part() &&
				m_pHits[i]->Seg()  == m_pHits[j]->Seg() &&
				m_pHits[i]->Gap()  == m_pHits[j]->Gap() )
		{
			hitsInGap++;
			px2[i] = (px[i]*(hitsInGap-1) + px[j])/hitsInGap;
			py2[i] = (py[i]*(hitsInGap-1) + py[j])/hitsInGap;
			cout<<hitsInGap<<"  "<<px[i]<<"  "<<px[j]<<"  "<<px2[i]<<endl;
		}
	}
}

for(int i = 0; i < m_pHits.size(); i++){
	if(px2[i] != -999&&py2[i] != -999){
	       	px[i] = px2[i]; py[i] = py2[i];
	}
}
*/

ndof = npoints - 2;
if (ndof < 0) return -1;

if (npoints == 1) {
  if (m_Part == 1) {
    if ( m_Orient == 0) {
      px[npoints] = m_VertexPos.z();
      py[npoints] = sqrt(m_VertexPos.x()*m_VertexPos.x() 
          + m_VertexPos.y()*m_VertexPos.y());
      if(m_Seg==2) py[npoints] = m_VertexPos.y();
    } 
    else {
      px[npoints] = m_VertexPos.x();
      py[npoints] = m_VertexPos.y();
    }
  }
  else {
    if ( m_Orient == 0) {
      px[npoints] = m_VertexPos.z();
      py[npoints] = m_VertexPos.y();
    }
    else {
      px[npoints] = m_VertexPos.z();
      py[npoints] = m_VertexPos.x();
    }
  }  
  pw[npoints] = 1.0;
  npoints++;
} 
else {
  if (npoints == 0 ) {
    return -1;
  }
}

// Do the fits here.
MucRecLineFit fit;
  int status = fit.LineFit(px, py, pw, npoints,
			   &slope, &intercept, &chisq,
			   &sigmaSlope, &sigmaIntercept);


  float tempslope, tempintercept,tempchisq, tempsigmaslope, sigmaPos;
  int status4 = fit.LineFit(px, py, pw,m_Part,m_Seg,m_Orient, npoints,
      &tempslope, &tempintercept, &tempchisq,
      &tempsigmaslope, &sigmaPos);
  
  MucRecQuadFit quadfit;
  float quad_a, quad_b, quad_c, sigmaquad_a, sigmaquad_b, sigmaquad_c, chisq_quad;
  int whichhalf, status2;

  if(m_fittingMethod == 2){
  status2 = quadfit.QuadFit(px, py, pw, npoints,
			    &quad_a, &quad_b, &quad_c, &whichhalf, &chisq_quad,
			    &sigmaquad_a, &sigmaquad_b, &sigmaquad_c);

  }
  //cout << " in MucRec2DRoad slope " << slope << " "<<intercept<<endl;
  
  if (slope > 1.0e10 || slope < -1.0e10) {
    if (m_Seg > 4) slope *= -1.0;  // to avoid wrong direction
  }

  m_SimpleSlope          = slope;
  m_SimpleSlopeSigma     = sigmaSlope;
  m_SimpleIntercept      = intercept;
  m_SimpleInterceptSigma = sigmaIntercept;
  m_SimplePosSigma       = sigmaPos;   //new 20071227
  m_SimpleChi2           = chisq;
  m_SimpleDOF            = ndof;
  m_SimpleFitOK          = (status == 0) && (chisq < 1000.0);
  m_QuadFitOK            = (status2 == 1);

  m_SimpleQuad_a         = quad_a;
  m_SimpleQuad_b         = quad_b;
  m_SimpleQuad_c         = quad_c;
  m_SimpleQuad_whichhalf = whichhalf;
  m_SimpleQuad_aSigma    = sigmaquad_a;
  m_SimpleQuad_bSigma    = sigmaquad_b;
  m_SimpleQuad_cSigma    = sigmaquad_c;

  return status;
}


/////////////////////////////////////////////////
// Calculate the best-fit straight line with "simple" weights. add not use current gap!!!
int   
MucRec2DRoad::SimpleFitNoCurrentGap(
      int currentgap,
      float& slope,
      float& intercept,
      float& sigmaSlope,
      float& sigmaIntercept,
      float& chisq,
      int&   ndof)
{
  // Assign to temporary arrays to be used in fit.
  float px[100];
  float py[100];
  float pw[100];
  int npoints = 0;

  int notused = 0;

  vector<MucRecHit*>::const_iterator iHit;

  float pw2[100];
  for(int i = 0; i< 100; i++) {
    pw2[i] = 1;
    //----if( m_pHits[i]->Gap()==currentgap ) pw2[i] = 9999;
  }

  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      if( (*iHit)->Gap()==currentgap ) continue;

      Hep3Vector center = (*iHit)->GetCenterPos();
      Hep3Vector sigma  = (*iHit)->GetCenterSigma();
      //Hep3Vector sigma(1.0, 1.0, 1.0);

      if (m_Part == 1) {
        if ( m_Orient == 0) {
          px[npoints] = center.z();
          py[npoints] = sqrt(center.x()*center.x()
              + center.y()*center.y());
          if(m_Seg==2) py[npoints] = center.y();  //deal with seg2 seperately! because there is a hole here. 2006.11.9

          pw[npoints] = 1.0/(sigma.y()*sigma.y())/pw2[npoints]/pw2[npoints];
        }
        else {
          px[npoints] = center.x();
          py[npoints] = center.y();
          pw[npoints] = 1.0/(sigma.x()*sigma.x())/pw2[npoints]/pw2[npoints];
        }
      }
      else {
        if ( m_Orient == 0) {
          px[npoints] = center.z();
          py[npoints] = center.y();
          pw[npoints] = 1.0/(sigma.y()*sigma.y())/pw2[npoints]/pw2[npoints];
        }
        else {
          px[npoints] = center.z();
          py[npoints] = center.x();
          pw[npoints] = 1.0/(sigma.x()*sigma.x())/pw2[npoints]/pw2[npoints];
        }
      }

      //cout << " in MucRec2DRoad  ID: " <<(*iHit)->Part()<<" "<<(*iHit)->Seg()<<" "<<(*iHit)->Gap()<<" "<< (*iHit)->Strip()<<" "<<px[npoints] << "  " << py[npoints] << "  " << pw[npoints] << endl;
      //cout<<"process ngap: "<<currentgap<<" current: "<<(*iHit)->Gap()<<"  x: "<<px[npoints]<<"  y: "<<py[npoints]<<"  weight: "<<pw[npoints]<<endl;
      npoints++;

      if(npoints > 99)
      {  cout<<"MucRec2DRoad: to many hits in this track, ignore it!"<<endl;
        return -1;
      }
    }
  }

  ndof = npoints - 2;
  if (ndof < 0) return -1;

  if (npoints == 1) {
    if (m_Part == 1) {
      if ( m_Orient == 0) {
        px[npoints] = m_VertexPos.z();
        py[npoints] = sqrt(m_VertexPos.x()*m_VertexPos.x()
            + m_VertexPos.y()*m_VertexPos.y());
        if(m_Seg==2) py[npoints] = m_VertexPos.y();
      }
      else {
        px[npoints] = m_VertexPos.x();
        py[npoints] = m_VertexPos.y();
      }
    }
    else {
      if ( m_Orient == 0) {
        px[npoints] = m_VertexPos.z();
        py[npoints] = m_VertexPos.y();
      }
      else {
        px[npoints] = m_VertexPos.z();
        py[npoints] = m_VertexPos.x();
      }
    }
    pw[npoints] = 1.0;
    npoints++;
  }
else {
  if (npoints == 0 ) {
    return -1;
  }

}

// Do the fits here.
MucRecLineFit fit;
int status = fit.LineFit(px, py, pw, npoints,
    &slope, &intercept, &chisq,
    &sigmaSlope, &sigmaIntercept);

MucRecQuadFit quadfit;
float quad_a, quad_b, quad_c, sigmaquad_a, sigmaquad_b, sigmaquad_c, chisq_quad;
int whichhalf, status2;

if(m_fittingMethod == 2){
  status2 = quadfit.QuadFit(px, py, pw, npoints,
      &quad_a, &quad_b, &quad_c, &whichhalf, &chisq_quad,
      &sigmaquad_a, &sigmaquad_b, &sigmaquad_c);

}
//cout << " in MucRec2DRoad slope " << slope << " "<<intercept<<endl;

if (slope > 1.0e10 || slope < -1.0e10) {
  if (m_Seg > 4) slope *= -1.0;  // to avoid wrong direction
}

////////2009-03-12  
  m_SimpleSlope          = slope;
  m_SimpleSlopeSigma     = sigmaSlope;
  m_SimpleIntercept      = intercept;
  m_SimpleInterceptSigma = sigmaIntercept;
  //m_SimplePosSigma       = sigmaPos;   //new 20071227
  m_SimpleChi2           = chisq;
  m_SimpleDOF            = ndof;
  m_SimpleFitOK          = (status == 0) && (chisq < 1000.0);
  m_QuadFitOK            = (status2 == 1);

  m_SimpleQuad_a         = quad_a;
  m_SimpleQuad_b         = quad_b;
  m_SimpleQuad_c         = quad_c;
  m_SimpleQuad_whichhalf = whichhalf;
  m_SimpleQuad_aSigma    = sigmaquad_a;
  m_SimpleQuad_bSigma    = sigmaquad_b;
  m_SimpleQuad_cSigma    = sigmaquad_c;


return status;
}




// Fit (with weights) the hit centers to a straight line.
// This is a helper function for the Project methods.
// Give an estimated position (x,y,z) of the point to which we will
// project.
  int
MucRec2DRoad::Fit(const float& x,
    const float& y,
    const float& z,
    float& slope,      float& intercept,
    float& sigmaSlope, float& sigmaIntercept,
    float& chisq,      int&   ndof)
{
  int status;

  // If the "simple" fit has not been done yet, do it now.
  if (!m_SimpleFitOK) {
    // Least-squares fit to the positions ...
    float a, sa, b, sb, chi;
    int n;
    status = SimpleFit(a, b, sa, sb, chi, n);
    if (status < 0) {
      //cout << "MucRec2DRoad::Fit-E2  simple fit fail status = "
      //  << status << endl;
      return status;
    }
  }

  // Assign to temporary arrays to be used in fit.
  float px[100];
  float py[100];
  float pw[100];
  int npoints = 0;
  float dx, dy, dr;


  // include vertex point when do the fancy fitting
  //if (m_Orient == kHORIZ) {
  //  px[npoints] = m_VertexPos.y();
  //  dx = px[npoints] - y;
  //} else {
  //  px[npoints] = m_VertexPos.x();
  //  dx = px[npoints] - x;
  //}
  //pz[npoints] = m_VertexPos.z();
  //dz = pz[npoints] - z;
  //dr = sqrt(dx*dx + dz*dz);
  //pw[npoints] = WeightFunc(m_SimpleChi2,dr);  
  //npoints++;

  vector<MucRecHit*>::const_iterator iHit;

  // Determine the weights based on the chi-square of the fit
  // (the scatter of the points should be roughly related to the
  // momentum) and the distance from the center to the estimated
  // location.

  //cout << m_pHits.size() << endl;
  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.

      Hep3Vector center = (*iHit)->GetCenterPos();
      Hep3Vector sigma  = (*iHit)->GetCenterSigma();

      if (m_Part == 1) {    // change from 0 to 1 at 2006.11.30
        if ( m_Orient == 0) {
          px[npoints] = center.z();
          dx = px[npoints] - z;
          py[npoints] = sqrt(center.x()*center.x() 
              + center.y()*center.y());
          if(m_Seg==2) py[npoints] = center.y();  //deal with seg2 seperately! because there is a hole here. 2006.11.9
          dy = py[npoints] - sqrt(x*x + y*y);
        } 
        else {
          px[npoints] = center.x();
          dx = px[npoints] - x;
          py[npoints] = center.y();
          dy = py[npoints] - y;
        }
      }
      else {
        if ( m_Orient == 0) {
          px[npoints] = center.z();
          dx = px[npoints] - z;
          py[npoints] = center.y();
          dy = py[npoints] - y;
        }
        else {
          px[npoints] = center.z();
          dx = px[npoints] - z;
          py[npoints] = center.x();
          dy = py[npoints] - x;
        }
      }  

      dr = sqrt(dx*dx + dy*dy);
      pw[npoints] = WeightFunc(m_SimpleChi2, dr);
      //pw[npoints] = WeightFunc(m_SimpleChi2,dr);

      //    cout << "       " << npoints  << "     "
      // << px[npoints] << "  " << py[npoints] << "  " << pw[npoints]
      // << "         " << dx << "  " << dy << "  " << dr
      // << endl;

      npoints++;

      if(npoints > 99)
      {  cout<<"MucRec2DRoad: to many hits in this track, ignore it!"<<endl;
        return -1;
      }   

    }
  }

  // Refit ...
  ndof = npoints - 2;
  if (npoints == 1) {
    if (m_Part == 1) {     // change from 0 to 1 at 2006.11.30
      if ( m_Orient == 0) {
        px[npoints] = m_VertexPos.z();
        py[npoints] = sqrt(m_VertexPos.x()*m_VertexPos.x() 
            + m_VertexPos.y()*m_VertexPos.y());
        if(m_Seg==2) py[npoints] = m_VertexPos.y();
      } 
      else {
        px[npoints] = m_VertexPos.x();
        py[npoints] = m_VertexPos.y();
      }
    }
    else {
      if ( m_Orient == 0) {
        px[npoints] = m_VertexPos.z();
        py[npoints] = m_VertexPos.y();
      }
      else {
        px[npoints] = m_VertexPos.z();
        py[npoints] = m_VertexPos.x();
      }
    }  
    pw[npoints] = 1.0;
    npoints++;
  } 
  else {
    if (npoints == 0) {
      return -1;
    }
  }

  MucRecLineFit fit;
  //cout << "npoints " << npoints << endl;
  status = fit.LineFit(px, py, pw, npoints,
      &slope, &intercept, &chisq,
      &sigmaSlope, &sigmaIntercept);
  m_DOF  = ndof;
  m_Chi2 = chisq;

  if (status < 0) {
    //cout << "MucRec2DRoad::Fit-E3  fit fail status = " << status << endl;
  }

  return status;
}

// Where does the trajectory of this road intersect a specific gap?
void
MucRec2DRoad::Project(const int& gap, 
			 float& x, float& y, float& z, float& x2, float& y2, float& z2)
{
  float sigmaX, sigmaY, sigmaZ;

  x = 0.0; sigmaX = 0.0; x2 = 0.0;
  y = 0.0; sigmaY = 0.0; y2 = 0.0;
  z = 0.0; sigmaZ = 0.0; z2 = 0.0;

  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec2DRoad::Project-E1  invalid gap = " << gap << endl;
    return;
  }
  
  // Determine the projection point of the "simple" fit to the desired gap.
  float x0, y0, z0, sigmaX0, sigmaY0, sigmaZ0;
  float phi = m_Seg*0.25*kPi;
  MucGeoGeneral *geom = MucGeoGeneral::Instance();

  if (m_Part == 1) {
    if (m_Orient == 0) {
      geom->FindIntersection(m_Part, m_Seg, gap,
			     m_SimpleSlope*cos(phi), m_SimpleSlope*sin(phi), 1.0,
			     m_SimpleIntercept*cos(phi),m_SimpleIntercept*sin(phi), 0.0,
			     m_SimpleSlopeSigma,     0.0, 0.0,
			     m_SimpleInterceptSigma, 0.0, 0.0, 
			     x0, y0, z0,
           sigmaX0, sigmaY0, sigmaZ0);

      if(m_SimpleSlope>10000){  //the line is in right center of this segment, we can not get intersection in y coordinate, in this case, m_SimpleIntercept is in z coordinate.                                               
        geom->FindIntersection(m_Part, m_Seg, gap,
        m_SimpleSlope*cos(phi), m_SimpleSlope*sin(phi), 1.0,
        0.0, 0.0, m_SimpleIntercept,
        m_SimpleSlopeSigma,     0.0, 0.0,
        m_SimpleInterceptSigma, 0.0, 0.0,
        x0, y0, z0,
        sigmaX0, sigmaY0, sigmaZ0);

      }

    }
    else {
      geom->FindIntersection(m_Part, m_Seg, gap,
			     1.0, m_SimpleSlope,          0.0,
			     0.0, m_SimpleIntercept,      0.0,
			     0.0, m_SimpleSlopeSigma,     0.0,
			     0.0, m_SimpleInterceptSigma, 0.0, 
			     x0, y0, z0,
			     sigmaX0, sigmaY0, sigmaZ0);
      //cout<<"in MucRec2DRoad line Project xyz0 = "<<x0<<" "<<y0<<" "<<z0<<endl;

    }
  }
  else {
    if (m_Orient == 0) {
      geom->FindIntersection(m_Part, m_Seg, gap,
			     m_SimpleSlope, m_SimpleSlope, 1.0,
			     0.0, m_SimpleIntercept,      0.0,
			     0.0, m_SimpleSlopeSigma,     0.0,
			     0.0, m_SimpleInterceptSigma, 0.0, 
			     x0, y0, z0,
			     sigmaX0, sigmaY0, sigmaZ0);
    }
    else {
      geom->FindIntersection(m_Part, m_Seg, gap,
			     m_SimpleSlope, m_SimpleSlope, 1.0,
			     m_SimpleIntercept,      0.0, 0.0,
			     m_SimpleSlopeSigma,     0.0, 0.0,
			     m_SimpleInterceptSigma, 0.0, 0.0,
			     x0, y0, z0,
			     sigmaX0, sigmaY0, sigmaZ0);
    }
    //cout << " part " << m_Part 
    // << " seg "  << m_Seg 
    // << " gap "  << gap
    // << " orient "  << m_Orient
    // << " slope = " << m_SimpleSlope
    // << endl;
  }
    
  //cout << "In find intersection x0 = " << x0 << " y0 = " << y0 << " z0 = " << z0 << endl; 

  float a,b,sa,sb,chi2;
  int ndof;

  int status = Fit(x0, y0, z0, a, b, sa, sb, chi2, ndof);

//   m_FitOK = (status == 0) && (chi2<1000.0);
//   if (!fFitOK) {
//     cout << "MucRec2DRoad::Project-E2  fit fail status = "
// 	 << status << "  npoints = " << npoints << "  chi-sq = "
// 	 << chi2 << endl;
//   }

//   // Assign to fields of TMuiRoad object.
//   m_DOF  = npoints - 2;
//   m_Chi2 = chi2;

  if (m_Part == 1) {    //change from 0 to 1 at 2006.11.30
    if (m_Orient == 0) {
      geom->FindIntersection(m_Part, m_Seg, gap,
			     a*cos(phi), a*sin(phi), 1.0,
			     //a,  0.0, 1.0,
			     b*cos(phi), b*sin(phi), 0.0,
			     sa, 0.0, 0.0,
			     sb, 0.0, 0.0, 
			     x,  y,   z,
			     sigmaX, sigmaY, sigmaZ);

      if(fabs(a)>10000){ /// 
        geom->FindIntersection(m_Part, m_Seg, gap,
            a*cos(phi), a*sin(phi), 1.0,
            0.0, 0.0, b,
            //a,  0.0, 1.0,
            //b,  0.0, 0.0,
            sa, 0.0, 0.0,
            sb, 0.0, 0.0,
            x,  y,   z,
            sigmaX, sigmaY, sigmaZ);
      
    }
    }
    else {
      geom->FindIntersection(m_Part, m_Seg, gap,
			     1.0, a,  0.0,
			     0.0, b,  0.0,
			     0.0, sa, 0.0,
			     0.0, sb, 0.0, 
			     x,   y,  z,
			     sigmaX, sigmaY, sigmaZ);

      if(m_fittingMethod == 2 && m_QuadFitOK ){
	float a, b, c;
	float sa, sb, sc, chi2x; int ydof; int whichhalf;
	
	GetSimpleFitParams(a, b, c, whichhalf, sa, sb, sc, chi2x, ydof);
	geom->FindIntersection(m_Part, m_Seg, gap,
			       10E30, 0.0, m_SimpleIntercept,  0.0,   //vy = infinite
			       a, b, c,  //y = a*x*x + b*x +c
			       whichhalf,
			       0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			       x, y, z, x2, y2, z2,
			       sigmaX, sigmaY, sigmaZ);


      }

    }
  }
  else {
    if (m_Orient == 0) {
      geom->FindIntersection(m_Part, m_Seg, gap,
			     a,   a,  1.0,
			     0.0, b,  0.0,
			     0.0, sa, 0.0,
			     0.0, sb, 0.0, 
			     x,  y,   z,
			     sigmaX, sigmaY, sigmaZ);
    }
    else {
      geom->FindIntersection(m_Part, m_Seg, gap,
			     a,  a, 1.0,
			     b,  0.0, 0.0,
			     sa, 0.0, 0.0,
			     sb, 0.0, 0.0,
			     x,  y,   z,
			     sigmaX, sigmaY, sigmaZ);
    }
  }

  return;
}

// A unique identifier for this road in the current event.
int
MucRec2DRoad::GetIndex() const
{
  return m_Index;
}

// In which part was this road found?
int
MucRec2DRoad::GetPart() const { return m_Part; }

// In which seg was this road found?
int
MucRec2DRoad::GetSeg() const { return m_Seg; }

// In which orientation was this road found?
int
MucRec2DRoad::GetOrient() const { return m_Orient; }

// Position of the vertex point.
void
MucRec2DRoad::GetVertexPos(float& x, float& y, float& z) const
{
  x = m_VertexPos.x();
  y = m_VertexPos.y();
  z = m_VertexPos.z();

  return;
}

// Which gap is the last one with hits attached to this road?
int
MucRec2DRoad::GetLastGap() const { return m_LastGap; }

// Return the number of hits in the gap with the most attached hits.
int
MucRec2DRoad::GetMaxHitsPerGap() const { return m_MaxHitsPerGap; }

// Return the total number of hits attached to this road.
int
MucRec2DRoad::GetTotalHits() const { return m_TotalHits; }

// How many gaps provide hits attached to this road?
int
MucRec2DRoad::GetNGapsWithHits() const
{
  const int ngap = (int)MucID::getGapMax();
  int gap, count = 0;
  vector<int> firedGap;
  for ( gap = 0; gap < ngap; gap++) {
    firedGap.push_back(0);
  }

  vector<MucRecHit*>::const_iterator iHit;
  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      gap = (*iHit)->Gap();
      firedGap[gap] = 1;
    }
  }
  
  for ( gap = 0; gap < ngap; gap++) {
    count += firedGap[gap];
  }

  return count;
}

// How many hits per gap does this road contain?
int
MucRec2DRoad::GetHitsPerGap(const int& gap) const
{
  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec2DRoad::GetHitsPerGap-E1  invalid gap = " << gap << endl;
    return 0;
  } 

  vector<MucRecHit*>::const_iterator iHit;
  int hitsInGap = 0;

  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    
    if ( !(*iHit) ) {
      cout << "MucRec2DRoad::GetHitsPerGap()-E2 null hit pointer !"  << endl;
      return 0;
    }
    else {
      if( gap == (*iHit)->Gap() ) hitsInGap += 1;
    }
  }

  return hitsInGap;
}

// Does this road contain any hits in the given seg in a gap?
bool
MucRec2DRoad::HasHitInGap(const int& gap) const
{
  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec2DRoad::HasHitInGap-E2  invalid gap = " << gap << endl;
    return false;
  }

  bool found = false;
  vector<MucRecHit*>::const_iterator iHit;

  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      if ( (*iHit)->Gap() == gap ) {
	found = true;
      }
    }
  }

  return found;
}

// How many hits do two roads share?
int
MucRec2DRoad::GetNSharedHits(const MucRec2DRoad* road2) const
{
  if (!road2) {
    return 0;
  }

  int count = 0;
  vector<MucRecHit*>::const_iterator iHit1;
  vector<MucRecHit*>::const_iterator iHit2;
  MucRecHit *hit1, *hit2;

  for( iHit1 = m_pHits.begin(); iHit1 != m_pHits.end(); iHit1++){
    for( iHit2 = road2->m_pHits.begin(); 
	 iHit2 != road2->m_pHits.end(); iHit2++){
      hit1 = (*iHit1);
      hit2 = (*iHit2);

      if ( (hit1 != 0) && (hit2 != 0) ) {
         if (hit1->GetID() == hit2->GetID()) {
	   count++;
	 }
      }
    }
  }
  
  return count;
}

// Slope of trajectory.
float 
MucRec2DRoad::GetSlope() const { return m_SimpleSlope; }

// Intercept of trajectory.
float 
MucRec2DRoad::GetIntercept() const { return m_SimpleIntercept; }

// Degrees of freedom of trajectory fit.
int
MucRec2DRoad::GetDegreesOfFreedom() const { return m_SimpleDOF; }

// Chi-square parameters (per degree of freedom) of trajectory fit.
float
MucRec2DRoad::GetReducedChiSquare() const
{
  if ( (!m_SimpleFitOK) || (m_SimpleDOF < 0) ) {
    return -1.0;
  }
  else if (m_SimpleDOF == 0) {
    return 0.0;
  }
  else {
    return (m_SimpleChi2 / m_SimpleDOF);
  }
}

// Get the parameters from the simple fit.
void
MucRec2DRoad::GetSimpleFitParams(float& slope,      float& intercept,
				    float& sigmaSlope, float& sigmaIntercept,
				    float& chisq,      int& ndof) const
{
  slope           = m_SimpleSlope;
  intercept       = m_SimpleIntercept;
  sigmaSlope      = m_SimpleSlopeSigma;
  sigmaIntercept  = m_SimpleInterceptSigma;
  chisq           = m_SimpleChi2;
  ndof            = m_SimpleDOF;

  return;
}


void
MucRec2DRoad::GetPosSigma(float &possigma)const
{
  possigma = m_SimplePosSigma;

}


// Get the parameters from the simple quad fit.
void
MucRec2DRoad::GetSimpleFitParams(float& a,      float& b, float& c, int& whichhalf,
				 float& sigmaa, float& sigmab,  float& sigmac,
				    float& chisq,      int& ndof) const
{
  a           = m_SimpleQuad_a;
  b           = m_SimpleQuad_b;
  c           = m_SimpleQuad_c;
  whichhalf   = m_SimpleQuad_whichhalf;

  sigmaa      = m_SimpleQuad_aSigma; 
  sigmab      = m_SimpleQuad_bSigma;
  sigmac      = m_SimpleQuad_cSigma;

  chisq           = m_SimpleChi2;
  ndof            = m_SimpleDOF;

  return;
}



// Get a pointer to the first found hit attached in a particular gap.
MucRecHit*
MucRec2DRoad::GetHit(const int& gap) const
{
  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec2DRoad::Hit-E1  invalid gap = " << gap << endl;
    return 0;
  }
  
  vector<MucRecHit*>::const_iterator iHit;
  
  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      if ( (*iHit)->Gap() == gap) { 
	return (*iHit);
      }
    }
  }
  
  return 0L;
}

// Distance from a hit to the projection of the road to the gap
// containing the hit.
float
MucRec2DRoad::GetHitDistance(const MucRecHit* hit)
{
  // Use straight-line projection?
  if (!hit) {
    cout << "MucRec2DRoad::GetHitDistance-E1  null hit pointer!" << endl;
    return muckInfinity;
  }

  int gap = hit->Gap();
  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec2DRoad::HitDistance-W2  bad gap number = " << gap << endl;
    return muckInfinity;
  }

  if ( hit->GetGap()->Orient() != m_Orient) {
    // The orientation of the hit is different from that of the road.
    cout << "MucRec2DRoad::GetHitDistance-W3 "
	 << " hit has wrong orientation = "
	 << hit->GetGap()->Orient()
	 << endl;
    return muckInfinity;
  }

  HepPoint3D r  = hit->GetCenterPos();
  HepPoint3D rl = hit->GetGap()->TransformToGap(r);
  // cout << "hit center " << r << endl;
  // cout << "hit center local " << rl << endl;

  // Determine the projection point of the "simple" fit to the desired gap.
  // FIXME(?):  need to use fit with fancy weights instead?
  float delta, delta1,delta2;
  float x0, y0, z0;
  float x2, y2, z2;
  //float sigmaX0, sigmaY0, sigmaZ0;

  //cout << "y:x = " << m_SimpleSlope << endl;

  Project(gap, x0, y0, z0, x2, y2, z2);
  // cout << " gap = " << gap << " x0 = " << x0
  //      << " y0 = "  << y0  << " z0 = " << z0
  //      << endl;

  if(x0==y0&&x0==z0&&x0==0) {x0+=-9999;y0+=-9999;z0+=-9999;}//cout<<"wrong intersection"<<endl;}
  if(x2==y2&&x2==z2&&x2==0) {x2+=-9999;y2+=-9999;z2+=-9999;}//cout<<"wrong intersection"<<endl;}   //wrong intersection!!!
  HepPoint3D s  = HepPoint3D(x0, y0, z0);
  HepPoint3D s_2  = HepPoint3D(x2, y2, z2);
  HepPoint3D sl = hit->GetGap()->TransformToGap(s);
  HepPoint3D s_2l = hit->GetGap()->TransformToGap(s_2);
  //cout << "project center       " << s  << endl;
  
//   cout << "project center local sl= " << sl << endl;
//   cout << "project center local sl= " << s_2l << endl;
//   cout << "project center local rl= " << rl << endl;
  if ( m_Part == 0 ) {
    if ( m_Orient == 0 ) {
      delta1 = fabs( sl.y() - rl.y() );
      delta2= fabs( s_2l.y() - rl.y() );
    } 
    else {
      delta1 = fabs( sl.x() - rl.x() );
      delta2= fabs( s_2l.x() - rl.x() );
    }
  }
  else {
    if ( m_Orient == 0 ) {
      delta1 = fabs( sl.y() - rl.y() );
      delta2= fabs( s_2l.y() - rl.y() );
    } 
    else {
      delta1 = fabs( sl.x() - rl.x() );
      delta2= fabs( s_2l.x() - rl.x() );
    }
  }
//   if(which==0) {
//     if(delta1<delta2)delta = delta1;
//     else delta = delta2;
//   }
  //cout<<"which = "<<which<<"  distance = "<<delta1<<" "<<delta2<<endl;
  
  if(delta1 < delta2)  return delta1;
  else return delta2;
}

// Determine the size of the search window in the given gap.
float
MucRec2DRoad::GetSearchWindowSize( const int& gap) const
{
  if ( (gap < 0) || (gap >= (int)MucID::getGapMax()) ) {
    cout << "MucRec2DRoad::GetSearchWindowSize-E1  invalid gap = " << gap << endl;
    return 0.0;
  }

  // Determine the projection point of the "simple" fit to the last
  // gap and the desired gap.
  // FIXME?  the "delta-x" variable is calculated as the scalar
  // difference between the positions obtained by projecting to the
  // last gap and to the desired gap.

  MucGeoGeneral *geom = MucGeoGeneral::Instance();
  float x1, y1, z1, x2, y2, z2, dx, dy, dr;
  float sigmaX, sigmaY, sigmaZ;

  if (m_Part == 0) {
    if (m_Orient == 0) {
      geom->FindIntersection(m_Part, 0, m_LastGap,
			     1.0, m_SimpleSlope,          0.0,
			     0.0, m_SimpleIntercept,      0.0,
			     0.0, m_SimpleSlopeSigma,     0.0,
			     0.0, m_SimpleInterceptSigma, 0.0, 
			     x1, y1, z1,
			     sigmaX, sigmaY, sigmaZ);
      geom->FindIntersection(m_Part, 0, gap,
			     1.0, m_SimpleSlope,          0.0,
			     0.0, m_SimpleIntercept,      0.0,
			     0.0, m_SimpleSlopeSigma,     0.0,
			     0.0, m_SimpleInterceptSigma, 0.0, 
			     x2, y2, z2,
			     sigmaX, sigmaY, sigmaZ);
      dx = z2 - z1;
      dy = sqrt(x2*x2 + y2*y2) - sqrt(x1*x1 + y1*y1);
    }
    else {
      geom->FindIntersection(m_Part, m_Seg, m_LastGap,
			     m_SimpleSlope,          0.0, 1.0,
			     m_SimpleIntercept,      0.0, 0.0,
			     m_SimpleSlopeSigma,     0.0, 0.0,
			     m_SimpleInterceptSigma, 0.0, 0.0, 
			     x1, y1, z1,
			     sigmaX, sigmaY, sigmaZ);
      geom->FindIntersection(m_Part, m_Seg, gap,
			     m_SimpleSlope,          0.0, 1.0,
			     m_SimpleIntercept,      0.0, 0.0,
			     m_SimpleSlopeSigma,     0.0, 0.0,
			     m_SimpleInterceptSigma, 0.0, 0.0, 
			     x2, y2, z2,
			     sigmaX, sigmaY, sigmaZ);
      dx = x2 - x1;
      dy = y2 - y1;
    }
  }
  else {
    if (m_Orient == 0) {
      geom->FindIntersection(m_Part, m_Seg, m_LastGap,
			     0.0, m_SimpleSlope,          1.0,
			     0.0, m_SimpleIntercept,      0.0,
			     0.0, m_SimpleSlopeSigma,     0.0,
			     0.0, m_SimpleInterceptSigma, 0.0, 
			     x1, y1, z1,
			     sigmaX, sigmaY, sigmaZ);
      geom->FindIntersection(m_Part, m_Seg, gap,
			     0.0, m_SimpleSlope,          1.0,
			     0.0, m_SimpleIntercept,      0.0,
			     0.0, m_SimpleSlopeSigma,     0.0,
			     0.0, m_SimpleInterceptSigma, 0.0, 
			     x2, y2, z2,
			     sigmaX, sigmaY, sigmaZ);
      dx = z2 - z1;
      dy = y2 - y1;
    }
    else {
      geom->FindIntersection(m_Part, m_Seg, m_LastGap,
			     m_SimpleSlope,          0.0, 1.0,
			     m_SimpleIntercept,      0.0, 0.0,
			     m_SimpleSlopeSigma,     0.0, 0.0,
			     m_SimpleInterceptSigma, 0.0, 0.0,
			     x1, y1, z1,
			     sigmaX, sigmaY, sigmaZ);
      geom->FindIntersection(m_Part, m_Seg, gap,
			     m_SimpleSlope,          0.0, 1.0,
			     m_SimpleIntercept,      0.0, 0.0,
			     m_SimpleSlopeSigma,     0.0, 0.0,
			     m_SimpleInterceptSigma, 0.0, 0.0,
			     x2, y2, z2,
			     sigmaX, sigmaY, sigmaZ);
      dx = z2 - z1;
      dy = x2 - x1;
    }
  }
  
  dr = sqrt(dx*dx + dy*dy);

  return WindowFunc(m_SimpleChi2,dr);
}

// Calculate the hit counts and last gap quantities.
void
MucRec2DRoad::CountHits()
{
  m_MaxHitsPerGap = 0;
  m_TotalHits     = 0;
  m_LastGap       = 0;

  vector<MucRecHit*>::const_iterator iHit;
  const int ngap = (int)MucID::getGapNum(m_Part);
  vector<int> HitsPerGap;
  for (int gap = 0; gap < ngap; gap++) {
    HitsPerGap.push_back(0);
  }
  
  // Fill HitsPerGap
  for (iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if ( !(*iHit) ) {
      cout << "MucRec2DRoad::CountHits()-E1 null hit pointer !"  << endl;
      return;
    }
    else {
      int gap = (*iHit)->Gap(); 
      HitsPerGap[gap]++;
      //cout << "gap " << gap << endl;
    }
  } 

  // Find the gap from where we should stop count, 
  // namely in front of the gap there are more than 
  // m_MaxNSkippedGaps gaps containing no hit. 

  int StopGap = ngap;
   /*
  for (int gap = m_MaxNSkippedGaps; gap < ngap; gap++) { 
    int SubTotalHits = 0; 
    for (int igap = gap-m_MaxNSkippedGaps; igap <= gap; igap++) { 
      SubTotalHits += HitsPerGap[igap]; 
    } 
    if (SubTotalHits == 0 ) {
      StopGap = gap - m_MaxNSkippedGaps;
      break; 
    }
  } 
  */

  //cout << "StopGap " << StopGap << endl;
  // Now we might get correct counting on hits, last gap and MaxHitsPerGap 
  for (int gap = 0; gap < StopGap; gap++) { 
    m_TotalHits += HitsPerGap[gap]; 
    if(m_MaxHitsPerGap < HitsPerGap[gap]) m_MaxHitsPerGap = HitsPerGap[gap];  
    if(HitsPerGap[gap] > 0) m_LastGap = gap; 
  } 
} 

// Does the Hit exist in the road  .
bool 
MucRec2DRoad::HasHit(MucRecHit* hit) const
{

  vector<MucRecHit*>::const_iterator iHit;
  bool HitExist = false;

  // Also, if a track hits overlap region of two panels, we avoid
  // to double count hits in two panels
  
  Identifier id = hit->GetID();

  for ( iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++ ) {
    if ( *iHit ) {  // Check for a null pointer.
      if ( (*iHit)->GetID() == id ) HitExist = true;	
    }
    if (HitExist) break;
  }
  
  return HitExist;
}

// Get indices of all hits in the road.
vector<Identifier> 
MucRec2DRoad::GetHitsID() const
{
  vector<Identifier> idCon;
  
  vector<MucRecHit*>::const_iterator iHit;
  for ( iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      Identifier id = (*iHit)->GetID();
      idCon.push_back(id);
      /*
      cout << " MucRec2DRoad::HitIndices  gap  orientation  twopack= "
	   <<  (*iHit)->ChannelID().Plane() << "   "
	   <<  (*iHit)->ChannelID().Orient()  << "   "
	   <<  (*iHit)->ChannelID().TwoPack() << endl; 
      */
    }
  }
  return idCon;
}

vector<MucRecHit*>
MucRec2DRoad::GetHits() const
{
return m_pHits;

}



// Print Hits Infomation.
void
MucRec2DRoad::PrintHitsInfo() const
{
  vector<MucRecHit*>::const_iterator iHit;
  for ( iHit = m_pHits.begin(); iHit != m_pHits.end(); iHit++) {
    if (*iHit) {  // Check for a null pointer.
      float xl, yl, zl;
      (*iHit)->GetStrip()->GetCenterPos(xl, yl, zl);
      HepPoint3D vl(xl, yl, zl);
      HepPoint3D vg = (*iHit)->GetGap()->TransformToGlobal(vl);

      cout << " orient " << m_Orient
	   << " part "   << (*iHit)->Part() 
	   << " seg  "   << (*iHit)->Seg()
	   << " gap  "   << (*iHit)->Gap()
	   << " strip "  << (*iHit)->Strip()
	   << " pos ("   << vg.x() << ", " << vg.y() << ", " << vg.z() << ")"  
	   << endl;
    }
  }

}

// Function used to determine the search weight size
float 
MucRec2DRoad::WeightFunc(const float& chisq, const float& distance) const
{
  return 1.0;
}

// Function used to determine the search window size
float 
MucRec2DRoad::WindowFunc(const float& chisq, const float& distance) const
{
  return 1.0;
}

