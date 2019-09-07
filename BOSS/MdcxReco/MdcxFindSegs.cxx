#include "MdcxReco/MdcxFindSegs.h"
#include "MdcxReco/MdcxHit.h"
#include "MdcGeom/MdcDetector.h"
#include "MdcTrkRecon/mdcWrapWire.h"
#include "MdcGeom/BesAngle.h"
#include "MdcxReco/MdcxParameters.h"

using std::cout;
using std::endl;
using std::ostream;
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
extern AIDA::IHistogram1D*  g_csmax4;
extern AIDA::IHistogram1D*  g_csmax3;

int MdcxFindSegs::wireGroups[11][288][16] = { { {0} } };

MdcxFindSegs::MdcxFindSegs(MdcDetector* g, const HepAList<MdcxHit> &hitlist, int debug){
  m_debug = debug;
  gm = g;
  dchitlist = hitlist;
  initWireGroups();
  process();
}

MdcxFindSegs::MdcxFindSegs(const HepAList<MdcxHit> &hitlist, int debug){
  m_debug = debug;
  gm = MdcDetector::instance();
  dchitlist = hitlist;
  initWireGroups();
  process();
}

MdcxFindSegs::~MdcxFindSegs() { KillList(); }

void MdcxFindSegs::initWireGroups(void) {
  //Assure initWireGroups run only once
  static bool alreadyInit = false;
  if (alreadyInit) return;

  //Make wire groups
  int lastsl= gm->nSuper();
  for (int sl = 0; sl < lastsl; sl++) { // loop over superLayers
    const MdcLayer *layArray[4];
    const MdcSuperLayer* slayer = gm->SuperLayer(sl);
    int nslay = slayer->nLayers();
    for (int i = 0; i < nslay; i++) layArray[i] = slayer->layer(i);
    int cellMax = layArray[1]->nWires();
    // loop over cells
    for (int cellIndex = 0; cellIndex < cellMax ; cellIndex++) {
      int wireIndex[4]; 
      if (slayer->slayNum() > 4) {
	wireIndex[0]= cellIndex;
	wireIndex[1]= cellIndex+1;
	wireIndex[2]= cellIndex;
	wireIndex[3]= cellIndex+1;

	wireIndex[1] = mdcWrapWire(wireIndex[1], layArray[1]->nWires());
	wireIndex[3] = mdcWrapWire(wireIndex[3], layArray[3]->nWires());
      } else {
	wireIndex[1]= cellIndex+1; 
	//take No.2 wire as referenc wire
	double phi = layArray[1]->getWire(wireIndex[1])->phiE();
	for(int i = 0; i < 4; i++ ) {
	  if ( i == 1 ) continue;
	  //change reference wire
	  if ( i == 3 ) phi = layArray[2]->getWire(wireIndex[2])->phiE();
	  BesAngle tmp(phi - layArray[i]->phiEPOffset());//yzhang temp
	  if ( i==3 ) {
	    wireIndex[i]=(tmp==0)?1:(int)ceil(layArray[i]->nWires()*tmp.rad()/twopi);
	  }else{
	    wireIndex[i]=(tmp==0)?-1:(int)floor(layArray[i]->nWires()*tmp.rad()/twopi);
	  }
	  wireIndex[i] = mdcWrapWire(wireIndex[i], layArray[i]->nWires());
	}
      }
      // set wireGroups
      int* w = wireGroups[sl][cellIndex];
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          //wireGroups[sl][cellIndex][4*i+j] = wireIndex[i] + (j - 1);
          w[4*i+j] = mdcWrapWire((wireIndex[i]+j-1), layArray[i]->nWires());
        }
      }
    } //end loop cells
  } //end loop superLayers

  //Set alreadyInit flag
  alreadyInit = true;
  return;
}

void MdcxFindSegs::process() {
  //if(3 == m_debug) cout <<" Event contains "<<dchitlist.length()<<" MdcxHit "<<endl;
  // for last super layer use 44
  int lflag[44][288] = { {0} };
  int pflag[44][288] = { {0} };
  int ix,iy,iyp,iyn,cellMax;

  //yzhang 2009-10-20 do not use poison
  int ipoison=0;
  //number of last layer
  int lastlay= gm->nLayer();

  // tag hit's lfalg
  int k=0; 
  while(dchitlist[k]){
    MdcxHit* temp = dchitlist[k]; 
    k++; 
    lflag[temp->Layer()][temp->WireNo()]=k;
  }

  // try poisoning
  if (ipoison){
    for (ix=0; ix<lastlay; ix++){
      cellMax = gm->Layer(ix)->nWires();
      // if have hits in left and right , poison this cell
      for (iy=0; iy<cellMax; iy++){
        iyp=iy+1; if (iyp == cellMax)iyp=0; 
	iyn=iy-1; if (iyn == -1)iyn=cellMax-1;
	if ((lflag[ix][iyp] != 0) && (lflag[ix][iyn] != 0)) {
	  pflag[ix][iy] = 1;
	  lflag[ix][iy] = 0;
	  //g_poison->fill(ix,iy);
	}
      }
      //for (iy=0; iy<cellMax; iy++) {if (pflag[ix][iy] != 0) lflag[ix][iy] = 0;}
    }
  }  //FIXME (optimize ???)
  
  float csmax_4 = MdcxParameters::csmax4;
  float csmax_3 = MdcxParameters::csmax3;
  int lastsl= gm->nSuper();//lastSLayNum;

  // loop superlayer
  for (sl = 0; sl < lastsl; sl++) {
    int fsl=4*sl;
    int l0=fsl, l1=fsl+1, l2=fsl+2, l3=fsl+3; 
    int iprt = 0; 
    //initialize superlayer pointer in layArray[]
    const MdcLayer *layArray[4];
    const MdcSuperLayer* slayer = gm->SuperLayer(sl);
    int nslay = slayer->nLayers();
    for (int i = 0; i < nslay; i++) layArray[i] = slayer->layer(i);
    if(3 == m_debug){
      cout<<"slayer No.  "<<slayer->index()<<endl;//yzhang debug
    }
    //reference point (2,1)
    cellMax = layArray[1]->nWires();

    // loop over cells
    for (int cellIndex=0; cellIndex<cellMax ; cellIndex++) {
      int* w = wireGroups[sl][cellIndex];
      unsigned int sig_mark = 0;
      for (int ilayer = l0; ilayer <= l3; ilayer++) {
	for (int iwire = 3; iwire >= 0; iwire--) {
	  if (lflag[ilayer][w[4*(ilayer-l0)+iwire]]) {
	    sig_mark |= 0x1;
	  }
	  sig_mark <<= 1;
	}
      }
      sig_mark >>= 1;

      int goodSegNo = 0;
      int nPat = m_segPat.patt4_size;
      int iPat = (sig_mark & 0x0200) ? 0 : 11;
      for ( ; iPat < nPat; iPat++) {
	int pat = m_segPat.patt4[iPat];
	if ((pat & sig_mark) == pat) {
	  if(3 == m_debug) {
	    cout<< "pat " << std::hex << pat << std::dec << "  with wire";
	    for (int tmpi = 0; tmpi < 4; tmpi++) {
	      cout<<"("<<l0+tmpi<<","<<w[4*tmpi+m_segPat.wirePat4[iPat][tmpi]-1]<<")";
	    }
	    cout << endl;
	  }
	  int w0 = lflag[l0][w[0 +m_segPat.wirePat4[iPat][0]-1]] - 1;
	  int w1 = lflag[l1][w[4 +m_segPat.wirePat4[iPat][1]-1]] - 1;
	  int w2 = lflag[l2][w[8 +m_segPat.wirePat4[iPat][2]-1]] - 1;
	  int w3 = lflag[l3][w[12+m_segPat.wirePat4[iPat][3]-1]] - 1;
	  int tw[4] = {w0, w1, w2, w3};

	  int namb = m_segPat.ambPat4_size[iPat];
	  for (int iamb = 0; iamb < namb; iamb++) {
	    int amb = m_segPat.ambigPatt4[iPat][iamb];
	    fithel = trial(w0, w1, w2, w3, amb);
	    if(3 == m_debug){
	      cout<<"chisq "<<fithel.Chisq()
		<<" <? csmax4 "<<csmax_4<<  endl;
	      if (fithel.Chisq() < csmax_4) {
		cout<<"Accept this seg"<<endl;
	      }else{
		cout<<"DROP this seg"<<endl;
	      }
	    }
	    if(g_csmax4) g_csmax4->fill(fithel.Chisq());
	    if (fithel.Chisq() < csmax_4) {
	      if (iprt) printseg(fithel, pat,amb);
	      appendseg(fithel, pat, amb);
	      goodSegNo++;
	    }
	  }
	}
      }
      if (goodSegNo != 0) continue;
      nPat = m_segPat.patt3_size;
      iPat = (sig_mark & 0x0200) ? 0 : 14;
      for ( ; iPat < nPat; iPat++) {
	if (iPat > nPat-3) {
	  if ((iPat == nPat-2) && (sig_mark&0x2121 == 0x2121)) continue;
	  if ((iPat == nPat-1) && (sig_mark&0x2122 == 0x2122)) continue;
	}
	int pat = m_segPat.patt3[iPat];
	if ((pat & sig_mark) == pat) {
	  if(3 == m_debug) {
	    cout<<"MdcxFindSegs: in pat "<<std::hex<<pat<<std::dec<<" with wire";
	    for (int tmpi = 0; tmpi < 4; tmpi++) {
	      if (m_segPat.wirePat3[iPat][tmpi] == 0) continue;
	      cout<< " (" << l0+tmpi << "," << w[4*tmpi+m_segPat.wirePat3[iPat][tmpi]-1] << ")";
	    }
	    cout<< endl;
	  }
	  int wn[3];
	  for (int iw = 0, iwp = 0; iwp < 4; iwp++) {
	    int wireNo = m_segPat.wirePat3[iPat][iwp];
	    if( wireNo == 0 ) continue;
	    wn[iw++] = lflag[l0+iwp][w[4*iwp+wireNo-1]] - 1;
	  }

	  int namb = m_segPat.ambPat3_size[iPat];
	  for (int iamb = 0; iamb < namb; iamb++) {
	    int amb = m_segPat.ambigPatt3[iPat][iamb];
	    fithel = trial(wn[0], wn[1], wn[2], amb);//FIXME
	    if(3 == m_debug){
	      cout<<"chisq "<<fithel.Chisq()
		<<" <? csmax3 "<<csmax_3<<  endl;
	      if (fithel.Chisq() < csmax_3) {
		cout<<"Accept this seg"<<endl;
	      }else{
		cout<<"DROP this seg"<<endl;
	      }
	    }
	    if(g_csmax3) g_csmax3->fill(fithel.Chisq());
	    if (fithel.Chisq() < csmax_3) {
	      if (iprt) printseg(fithel, pat, amb);
	      appendseg(fithel, pat ,amb);
	    }
	  }
	}
      }//end of nPat3
    }
  }

  if(3 == m_debug){
    cout << "MdcxFindSegs found " << MdcxSeglist.length() << " segs" << endl;
  }
  return;
}

void MdcxFindSegs::print(ostream &o, int pmax)const {
  int mcheck=pmax; if (MdcxSeglist.length() < pmax)mcheck=MdcxSeglist.length();
  o << " First " << mcheck << " Drift Chamber Segs:" << endl;
  for(int i=0; i<mcheck; i++){
    o << " Superlayer # " << MdcxSeglist[i]->SuperLayer();
    o << " # of hits " << MdcxSeglist[i]->Nhits() << endl;
  } 
} // end of print

MdcxFittedHel MdcxFindSegs::trial(int i1, int i2, int i3, int i4, int amb) {
  HepAList<MdcxHit> seg;
  MdcxHit* t1 = dchitlist[i1]; MdcxHit* t2 = dchitlist[i2];  
  MdcxHit* t3 = dchitlist[i3]; MdcxHit* t4 = dchitlist[i4];  
  seg.append(t1); seg.append(t2); seg.append(t3); seg.append(t4); 
  double dx, dy, d0; 
  if (amb==0){dx=t4->xneg()-t1->xneg(); dy=t4->yneg()-t1->yneg(); d0=-t1->d();}
  if (amb==1){dx=t4->xneg()-t1->xpos(); dy=t4->yneg()-t1->ypos(); d0= t1->d();}
  if (amb==2){dx=t4->xpos()-t1->xneg(); dy=t4->ypos()-t1->yneg(); d0=-t1->d();}
  if (amb==3){dx=t4->xpos()-t1->xpos(); dy=t4->ypos()-t1->ypos(); d0= t1->d();}
  double phi0 = atan2(dy,dx); dx=t1->x(); dy=t1->y(); 
  MdcxHel hel(d0,phi0,0.0,0.0,0.0,0.0,111,0,dx,dy);
  hel.SetTurnFlag(1);
  MdcxFittedHel temp(seg, hel, 1.0);
  if (3 == m_debug) {
    cout<<"trial4 amb "<<amb
      << ": phi0 " << phi0 << " d0 " << d0 
      << " dx " << dx << " dy " << dy << endl;
    //temp.FitPrint();
  }
  return temp;
}

MdcxFittedHel MdcxFindSegs::trial(int i1, int i2, int i3, int amb) {
  HepAList<MdcxHit> seg;
  MdcxHit* t1 = dchitlist[i1];
  MdcxHit* t2 = dchitlist[i2];
  MdcxHit* t3 = dchitlist[i3];
  seg.append(t1); seg.append(t2); seg.append(t3);
  double dx,dy,d0; 
  if (amb==0){dx=t3->xneg()-t1->xneg(); dy=t3->yneg()-t1->yneg(); d0=-t1->d();}
  if (amb==1){dx=t3->xneg()-t1->xpos(); dy=t3->yneg()-t1->ypos(); d0= t1->d();}
  if (amb==2){dx=t3->xpos()-t1->xneg(); dy=t3->ypos()-t1->yneg(); d0=-t1->d();}
  if (amb==3){dx=t3->xpos()-t1->xpos(); dy=t3->ypos()-t1->ypos(); d0= t1->d();}

  double phi0 = atan2(dy,dx); dx=t1->x(); dy=t1->y(); 
  MdcxHel hel(d0,phi0,0.0,0.0,0.0,0.0,111,0,dx,dy);
  hel.SetTurnFlag(1);
  MdcxFittedHel temp(seg,hel,1.0);

  if (3 == m_debug) {
    cout<<"trial3 amb "<<amb
      << ": phi0 " << phi0 << " d0 " << d0 
      << " dx " << dx << " dy " << dy << endl;
  }
  return temp;
}

void MdcxFindSegs::appendseg(MdcxFittedHel& fithel,int pat,int amb){
  MdcxSeg *tempseg = new MdcxSeg(fithel, pat, amb);
  MdcxSeglist.append(tempseg);
}

void MdcxFindSegs::printseg(MdcxFittedHel& fithel, int pat, int amb, int subtry){
  cout << "Seg: pat " << pat << " amb " << amb << " subtry " << subtry 
    << " sl " << sl  << " fail "
    << fithel.Fail() << " chi2 " << fithel.Chisq() << endl;
}

