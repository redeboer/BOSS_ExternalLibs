// Kludgy class to hold misc track parameters that are run-time settable
//    just a convenient way of moving them about
//    Note that assignment operator has not been redefined.

// Interface Dependencies ----------------------------------------------

#ifndef MDCTRACKPARAMS_H
#define MDCTRACKPARAMS_H

//  End Interface Dependencies -----------------------------------------

// Class //
class MdcTrackParams {
  public:
    MdcTrackParams();
    ~MdcTrackParams();

    int  	lPrint;  	// print track info
    int 	lPlot;
    int 	lHist;
    int 	lUseQualCuts;
    int 	minHits;       	// min # of hits in Track
    double 	maxChisq;      	// per DOF
    double 	maxNmissTrack; 	// # of missing (= inactive) hits on track
    double 	maxNmissNorm;  	// = fraction of hits "missing" on track
    int 	nOverlap;   	// if >= nOverLap hits in common between tracks, 
    //award all common hits to one or the other 
    double 	maxSegChisqO; 	// max chisq/dof allowed when combining segs from origin
    double 	maxInactiveResid;// max resid allowed for hit to be included at all
    double 	maxActiveSigma;	// max # sigma allowed for hit inclusion as active
    double 	pickHitFract; 	// min % of predicted hits actually found for good track
    double 	pickHitMargin; 	// portion (mm) of outer cell to ignore in calc. above
    double 	pickHitFactor; 	// yzhang add 2010-05-12 
    int    	lRemoveInActive;// remove in-active hits in arbitrate hits
    double    	pickHitPhiFactor;// factor in pickHits
    int         pickUitlLastLayer;// pick hits util last layer(in hit list) 
                                  // or max MDC layer(43)
    int         nHitDeleted;// delete track when number of removed hits reach nHitDeleted
    int		pickSkipExistLayer;// if pick hits skip layer exist hot 2011-05-04 
    int         combineByFitHits;// if true fit stereo hits, false fit stereo segment 2011-05-11 
    int 	nGapGE2; //delete track with # of gap with gap length >=2 
    int 	nGapGE3; //delete track with # of gap with gap length >=3 
    int 	maxGapLength; //delete track max gap length >=3 
};
#endif
