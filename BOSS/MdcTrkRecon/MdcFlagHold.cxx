// MdcFlagHold.cxx

/* Read instructions contained in file (file name in argument).  Return 
   input and output file names as arguments. */

//----------------------------------------------------------------------------
#include <stdio.h>
#include <iostream>
#include "MdcTrkRecon/MdcFlagHold.h"
#include "MdcGeom/MdcTrkReconCut.h"
#include "TrkFitter/TrkHelixFitter.h"
extern double MdcTrkReconCut_combAxPhi0;
extern double MdcTrkReconCut_combAxCurv;
extern double MdcTrkReconCut_combAxPhi0Cut;
extern double MdcTrkReconCut_combAxCurvCut;
//----------------------------------------------------------------------------
void 
MdcFlagHold::setDebug(int flag) {
//----------------------------------------------------------------------------
  m_debug = flag;
  tkParTight.lPrint = flag;
  segPar.lPrint = tkParTight.lPrint;
}

//----------------------------------------------------------------------------
void MdcFlagHold::setHist(int flag){
//----------------------------------------------------------------------------
  lHist = flag;
  segPar.lHist = flag;
  tkParTight.lHist = flag;
}

//----------------------------------------------------------------------------
void
MdcFlagHold::readPar(std::string inname) {
//----------------------------------------------------------------------------

  /* Declare variables */
  FILE *ifp;
  char comment[80];
  std::cout<< 0.  <<"  readPar "<< inname  << std::endl;
  /**************************************************************************/
  
  if ((ifp = fopen(inname.c_str(),"r")) != NULL) {
    if (debugFlag()>0) {
      std::cout << "Reading MdcTrackFinder params from " << inname << std::endl;
    }
    int idebug= 0;
    fscanf(ifp,"%d%s", &idebug, comment); // debug?
    fscanf(ifp,"%d%s", &lHist, comment);// histogram?
    fscanf(ifp,"%d%s", &findTracks, comment); // find tracks?
    fscanf(ifp,"%d%s", &findSegs, comment); // find segs?
    fscanf(ifp,"%d%s", &(segPar.lHist), comment); // hist segs?
    fscanf(ifp,"%d%s", &(tkParTight.lHist), comment); // hist tracks?
    fscanf(ifp,"%d%s", &(segPar.lPrint), comment); // print segs?
    fscanf(ifp,"%d%s", &(tkParTight.lPrint), comment); // print track?
    fscanf(ifp,"%d%s", &(segPar.segRefit), comment); // refit segs?
    fscanf(ifp,"%d%s", &(segPar.useAllAmbig), comment); // use all ambig?
    fscanf(ifp,"%d%s", &(segPar.dropDups), comment); // drop duplicate segs?
    //std::cout<< __FILE__ << "   " << __LINE__ << "   "<<std::endl;
    //fscanf(ifp,"%d%s", &(segPar.find3[0]),comment);
    fscanf(ifp,"%d %d %d %d %d %d %d %d %d %d %d%s", 
	&(segPar.find3[0]), &(segPar.find3[1]), &(segPar.find3[2]), &(segPar.find3[3]),
	&(segPar.find3[4]), &(segPar.find3[5]), &(segPar.find3[6]), &(segPar.find3[7]),
	&(segPar.find3[8]), &(segPar.find3[9]), &(segPar.find3[10]), comment); // find3 hit segs?
    //std::cout<< __FILE__ << "   " << __LINE__ << "   "<<std::endl;
    //fscanf(ifp,"%d%s", &(segPar.find3), comment); // find 3 hit segs? 2011-05-27 
    fscanf(ifp,"%d%s", &(segPar.addHits), comment); // add hits in seg finding?
    fscanf(ifp,"%d%s", &(tkParTight.lUseQualCuts), comment); // use quality cut?
    fscanf(ifp,"%lf%s", &(segPar.maxChisq), comment); 
    fscanf(ifp,"%lf%s", &(segPar.nsigAddHit), comment);
    fscanf(ifp,"%lf%s", &(segPar.z0cut), comment);
    fscanf(ifp,"%lf%s", &(segPar.ctcut), comment);
    fscanf(ifp,"%lf%s", &(segPar.phiDiffDropMult), comment);
    fscanf(ifp,"%lf%s", &(segPar.slopeDiffDrop), comment);
    fscanf(ifp,"%lf%s", &(segPar.delZ0Cut), comment);
    fscanf(ifp,"%lf%s", &(segPar.delCtCut), comment);
    fscanf(ifp,"%d%s",  &(tkParTight.minHits), comment);
    fscanf(ifp,"%lf%s", &(tkParTight.maxChisq), comment);
    fscanf(ifp,"%lf%s", &(tkParTight.maxNmissTrack), comment);
    fscanf(ifp,"%lf%s", &(tkParTight.maxNmissNorm), comment);
    fscanf(ifp,"%d%s",  &(tkParTight.nOverlap), comment);
    fscanf(ifp,"%lf%s", &(tkParTight.maxSegChisqO), comment);
    fscanf(ifp,"%lf%s", &(tkParTight.maxInactiveResid), comment);
    fscanf(ifp,"%lf%s", &(tkParTight.maxActiveSigma), comment);
    fscanf(ifp,"%lf%s", &(tkParTight.pickHitFract), comment);
    fscanf(ifp,"%lf%s", &(tkParTight.pickHitMargin), comment);
    fscanf(ifp,"%lf%s", &MdcTrkReconCut_combAxPhi0, comment);
    fscanf(ifp,"%lf%s", &MdcTrkReconCut_combAxCurv, comment);
    fscanf(ifp,"%lf%s", &MdcTrkReconCut_combAxPhi0Cut, comment);
    fscanf(ifp,"%lf%s", &MdcTrkReconCut_combAxCurvCut, comment);
    fscanf(ifp,"%lf%s", &(tkParTight.pickHitFactor), comment);//yzhang add 2010-05-12 
    fscanf(ifp,"%d%s", &(tkParTight.lRemoveInActive), comment);//yzhang add 2010-05-14 
    fscanf(ifp,"%lf%s", &(tkParTight.pickHitPhiFactor), comment);//yzhang add 2010-05-18 
    fscanf(ifp,"%d%s", &(tkParTight.pickUitlLastLayer), comment);//yzhang add 2010-09-10 
    fscanf(ifp,"%d%s", &(tkParTight.nHitDeleted), comment);//yzhang add 2010-09-13 
    fscanf(ifp,"%d%s", &(tkParTight.pickSkipExistLayer ), comment);//yzhang add 2011-05-04
    fscanf(ifp,"%d%s", &(tkParTight.combineByFitHits), comment);//yzhang add 2011-05-11 
    fscanf(ifp,"%d%s", &(tkParTight.nGapGE2), comment);//yzhang add 2011-07-29 
    fscanf(ifp,"%d%s", &(tkParTight.nGapGE3), comment);//yzhang add 2011-07-29 
    fscanf(ifp,"%d%s", &(tkParTight.maxGapLength), comment);//yzhang add 2011-07-29 
    setDebug(idebug);
    if (plot == 0) {
      plotTracks = 0;
      plotSegs = 0;
      plotTrackList = 0;
    }
    fclose(ifp);
  }else{
      std::cout << "Reading MdcTrackFinder params from default param!" << std::endl;
  }

  tkParTight.lPlot = plot;
  segPar.lPlot = plot;
  
  if (debugFlag()) { printPar(); }
  return;
}

//----------------------------------------------------------------------------
void
MdcFlagHold::printPar() {
  std::cout << "MdcFlagHold:\ndebug:"<<debugFlag()
       << "\nhist:"<<lHist
       << "\nfindTracks:"<<findTracks
       <<"\nfindSegs:"<<findSegs
       <<"\nhistSegs:"<<segPar.lHist
       <<"\nhistTracks:"<<tkParTight.lHist
       <<"\nprintSeg:"<<segPar.lPrint
       <<"\nprintTrack:"<<tkParTight.lPrint
       <<"\nrefitSeg:"<<segPar.segRefit
       <<"\nuseAllAmbig:"<<segPar.useAllAmbig
       <<"\nsegPar.dropDups:"<<segPar.dropDups
       <<"\nsegPar.find3:";
  for(int ii=0;ii<11;ii++){
    std::cout <<segPar.find3[ii] << " ";
  }
  std::cout <<"\nsegPar.addHits:"<<segPar.addHits
    <<"\ntkParTight.lUseQualCuts:"<<tkParTight.lUseQualCuts
    <<"\nsegPar.maxChisq:"<<segPar.maxChisq
    <<"\nsegPar.nsigAddHit:"<<segPar.nsigAddHit
    <<"\nsegPar.z0:"<<segPar.z0cut
    <<"\nsegPar.ct:"<<segPar.ctcut
    <<"\nsegPar.phiDiffDropMult:"<<segPar.phiDiffDropMult
    <<"\nsegPar.slopeDiffDrop:"<<segPar.slopeDiffDrop
    <<"\nsegPar.delZ0Cut:"<<segPar.delZ0Cut
    <<"\nsegPar.delCtCut:"<<segPar.delCtCut
    <<"\ntkParTight.minHits:"<<tkParTight.minHits
    <<"\ntkParTight.maxChisq:"<<tkParTight.maxChisq
    <<"\ntkParTight.maxNmissTrack:"<<tkParTight.maxNmissTrack
    <<"\ntkParTight.maxNmissNorm:"<<tkParTight.maxNmissNorm
    <<"\ntkParTight.nOverlap:"<<tkParTight.nOverlap
    <<"\ntkParTight.maxsegChisq:"<<tkParTight.maxSegChisqO
    <<"\ntkParTight.maxInactiveResid:"<<tkParTight.maxInactiveResid
    <<"\ntkParTight.maxActiveSigma:"<<tkParTight.maxActiveSigma
    <<"\ntkParTight.picHitFract:"<<tkParTight.pickHitFract
    <<"\ntkParTight.pickHitMargin:"<<tkParTight.pickHitMargin
    <<"\nMdcTrkReconCut_combAxPhi0:"<<MdcTrkReconCut_combAxPhi0
    <<"\nMdcTrkReconCut_combAxCurv:"<<MdcTrkReconCut_combAxCurv
    <<"\nMdcTrkReconCut_combAxPhi0Cut:"<<MdcTrkReconCut_combAxPhi0Cut
    <<"\nMdcTrkReconCut_combAxCurvCut:"<<MdcTrkReconCut_combAxCurvCut
    <<"\ntkParTight.pickHitFactor:"<<tkParTight.pickHitFactor
    <<"\ntkParTight.lRemoveInActive:"<<tkParTight.lRemoveInActive
    <<"\ntkParTight.pickHitPhiFactor:"<<tkParTight.pickHitPhiFactor
    <<"\ntkParTight.pickUitlLastLayer:"<<tkParTight.pickUitlLastLayer
    <<"\ntkParTight.nHitDeleted:"<<tkParTight.nHitDeleted
    <<"\ntkParTight.pickSkipExistLayer:"<<tkParTight.pickSkipExistLayer
    <<"\ntkParTight.combineByFitHits:"<<tkParTight.combineByFitHits
    <<"\ntkParTight.nGapGE2:"<<tkParTight.nGapGE2
    <<"\ntkParTight.nGapGE3:"<<tkParTight.nGapGE3
    <<"\ntkParTight.maxGapLength:"<<tkParTight.maxGapLength
    <<std::endl;
  std::cout<<"nSigmaCut:"<<std::endl;
  for (int i=0;i<43;i++) {
    std::cout<<TrkHelixFitter::nSigmaCut[i]<<" ";
    if((i+1)%4==0) std::cout<<std::endl;
  }
  std::cout<<std::endl;
  return;
}
//----------------------------------------------------------------------------


MdcFlagHold::MdcFlagHold() { 
  m_debug = 0;  // => modest per-event output
  plot = 0;
  plotSegs = 0;
  plotSegList = 0;
  plotTracks = 0;
  plotTrackList = 0;
  lHist = 0;
  findTracks = 1;
  findSegs = 1;
}
