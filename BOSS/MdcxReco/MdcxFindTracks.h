//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxFindTracks.h,v 1.6 2011/12/08 06:52:29 zhangy Exp $
//
// Description: Object to find tracks given event data
//	Class Header
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      S. Wagner
//	Zhang Yao(zhangyao@ihep.ac.cn) Migration to BESIII
//	
// Copyright Information:
//	Copyright (C) 1994	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#ifndef MDCXFINDTRACKS_HH
#define MDCXFINDTRACKS_HH

#include <iostream>
#include <fstream>
#include "CLHEP/Alist/AList.h"
#include "MdcxReco/MdcxHel.h"
#include "MdcxReco/MdcxFittedHel.h"
#include "MdcxReco/MdcxSeg.h"

class MdcxFindTracks{
  public:
    MdcxFindTracks();
    MdcxFindTracks(const HepAList<MdcxSeg> &f, int debug = 0);
    virtual ~MdcxFindTracks();

    //functions
    inline const HepAList<MdcxFittedHel> &GetMdcxTrklist(){return MdcxTrklist;}
    void process( const HepAList<MdcxSeg> &f);
    void print(std::ostream &o);
    void plot()const;


  protected:
    //data members
    HepAList<MdcxFittedHel> MdcxTrklist;
    MdcxHel mhel;
    float probmin;	        // min probablity cut
    float curvmax;		// max curvature cut
    int ngood;			// number with prob>probmin
    int npure;			// prob>probmin & pure mc
    int nbad;			// number with prob<probmin
    int nfail;			// number with failed fit
    int nhitsmc;		// number of hits produced
    int nhitsgd;		//#of hits in pure clusters
    int m_debug;		//debug flag
    bool m_doSag;               //sag flag

    //functions
    void KillList(){HepAListDeleteAll(MdcxTrklist);}
    void beginmore();     	// begin specifics
    void endmore();	        // end specifics
    double findz_sl(int i1, int i2, const HepAList<MdcxSeg> &slclus);
    double findz_cyl(int i1, int i2, const HepAList<MdcxSeg> &slclus);
    double dlen(int slay1, double p1, int slay2, double p2, double om);
    //double dlen(double p1, double p2, double om);
    MdcxFittedHel drophits(MdcxFittedHel* f);
    MdcxHel TakeToOrigin(MdcxHel& );
    void resout(MdcxFittedHel* f);
    void printpar(std::ostream &o);	// print out parameters
    bool testFromSameTrack(MdcxSeg* seg1, MdcxSeg* seg2);

};//endof MdcxFindTracks

#endif
