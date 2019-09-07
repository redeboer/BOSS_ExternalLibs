#ifndef TCURLFINDERPARAMETERS_FLAG_
#define TCURLFINDERPARAMETERS_FLAG_

#include <iostream>

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

class TCurlFinderParameter {
public:
  TCurlFinderParameter(void):
//    ALPHA_SAME_WITH_HELIX(222.376063),
    ALPHA_SAME_WITH_HELIX(333.564095) {};
	//jialk
    //MIN_RADIUS_OF_STRANGE_TRACK(65.0) {};

  ~TCurlFinderParameter(void){}

  void now(void) {
    std::cout << std::endl;
    std::cout << "===== CURLFINDER PARAMETERS =====" << std::endl;
    std::cout << "ALHPA : " << ALPHA_SAME_WITH_HELIX << std::endl
	 << std::endl
	 << "MIN_SEGMENT : " << MIN_SEGMENT << std::endl 
	 << std::endl
	 << "MIN_SALVAGE               : " << MIN_SALVAGE << std::endl
	 << "BAD_DISTANCE_FOR_SALVAGE  : " << BAD_DISTANCE_FOR_SALVAGE << std::endl
	 << "GOOD_DISTANCE_FOR_SALVAGE : " << GOOD_DISTANCE_FOR_SALVAGE << std::endl
	 << std::endl
	 << "MIN_SEQUENCE : " << MIN_SEQUENCE << std::endl
	 << "MAX_FULLWIRE : " << MAX_FULLWIRE << std::endl
	 << std::endl
	 << "RANGE_FOR_AXIAL_SEARCH          : " << RANGE_FOR_AXIAL_SEARCH << std::endl
	 << "RANGE_FOR_AXIAL_LAST2D_SEARCH   : " << RANGE_FOR_AXIAL_LAST2D_SEARCH << std::endl
	 << "RANGE_FOR_STEREO_LAST2D_SEARCH  : " << RANGE_FOR_STEREO_LAST2D_SEARCH << std::endl
	 << "STEREO_2DFIND                   : " << STEREO_2DFIND << std::endl
	 << "   SUPERLAYER_FOR_STEREO_SEARCH : " << SUPERLAYER_FOR_STEREO_SEARCH << std::endl
	 << "   RANGE_FOR_STEREO_SEARCH      : " << RANGE_FOR_STEREO_SEARCH << std::endl
	 << std::endl
	 << "TRACE2D_DISTANCE         : " << TRACE2D_DISTANCE << std::endl
	 << "TRACE2D_FIRST_SUPERLAYER : " << TRACE2D_FIRST_SUPERLAYER << std::endl
	 << std::endl
	 << "TRACE3D_DISTANCE : " << TRACE3D_DISTANCE << std::endl
	 << std::endl
	 << "RATIO_USED_WIRE : " << RATIO_USED_WIRE << std::endl
	 << std::endl
	 << "DETERMINE_ONE_TRACK : " << DETERMINE_ONE_TRACK << std::endl
	 << std::endl
	 << "MERGE_EXE       : " << MERGE_EXE << std::endl
	 << "   MERGE_RATIO  : " << MERGE_RATIO << std::endl
	 << "   MERGE_Z_DIFF : " << MERGE_Z_DIFF << std::endl
	 << std::endl
	 << "MASK_DISTANCE : " << MASK_DISTANCE << std::endl 
	 << std::endl
	 //jialk
	 //<< "MIN_RADIUS_OF_STRANGE_TRACK : " << MIN_RADIUS_OF_STRANGE_TRACK << std::endl
	 << std::endl
	 << "RANGE_FOR_STEREO_FIRST  : " << RANGE_FOR_STEREO_FIRST << std::endl
	 << "RANGE_FOR_STEREO_SECOND : " << RANGE_FOR_STEREO_SECOND << std::endl
	 << "RANGE_FOR_STEREO_THIRD  : " << RANGE_FOR_STEREO_THIRD << std::endl
	 << "RANGE_FOR_STEREO_FORTH  : " << RANGE_FOR_STEREO_FORTH << std::endl
	 << "RANGE_FOR_STEREO_FIFTH  : " << RANGE_FOR_STEREO_FIFTH << std::endl
         << "RANGE_FOR_STEREO_FIFTH  : " << RANGE_FOR_STEREO_SIXTH << std::endl
	 << std::endl
	 << "SVD_RECONSTRUCTION   : " << SVD_RECONSTRUCTION  << std::endl
	 << "   MIN_SVD_ELECTRONS : " << MIN_SVD_ELECTRONS   << std::endl
	 << "SELECTOR_MAX_IMPACT  : " << SELECTOR_MAX_IMPACT << std::endl
	 << "SELECTOR_MAX_SIGMA   : " << SELECTOR_MAX_SIGMA  << std::endl
	 << "SELECTOR_STRANGE_PZ  : " << SELECTOR_STRANGE_PZ << std::endl
	 << "SELECTOR_REPLACE_DZ  : " << SELECTOR_REPLACE_DZ << std::endl
	 << std::endl
	 << "Z_CUT : " << Z_CUT << std::endl
	 << "Z_DIFF_FOR_LAST_ATTEND : " << Z_DIFF_FOR_LAST_ATTEND << std::endl
	 << std::endl
	 << "ON_CORRECTION : " << ON_CORRECTION << std::endl
         << "OUTPUT_2DTRACKS : " << OUTPUT_2DTRACKS << std::endl
	 << std::endl
	 << "CURL_VERSION : " << CURL_VERSION << std::endl
	 << "             : 0 default, 1 b20010409_2122" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << std::endl;
  }
  
public :
  //
  // TCurlFinder.cc
  //
  
  // constant factor
  const double ALPHA_SAME_WITH_HELIX;

  // minimum size of segment
  unsigned MIN_SEGMENT;

  // Salvage Section
  unsigned MIN_SALVAGE;
  double   BAD_DISTANCE_FOR_SALVAGE;
  double   GOOD_DISTANCE_FOR_SALVAGE;
  
  // If size of the segment is equal to or larger than MIN_SEQUENCE,
  // it may be divided. --> exceptional type 1
  unsigned MIN_SEQUENCE;

  // exceptional type 3
  unsigned MAX_FULLWIRE;
  
  // range for appending axial wires to the segment. unit = cm.
  double   RANGE_FOR_AXIAL_SEARCH;
  double   RANGE_FOR_STEREO_SEARCH;   
  unsigned SUPERLAYER_FOR_STEREO_SEARCH;
  double   RANGE_FOR_AXIAL_LAST2D_SEARCH;
  double   RANGE_FOR_STEREO_LAST2D_SEARCH;
  
  // Trace Section
  double TRACE2D_DISTANCE;
  double TRACE2D_FIRST_SUPERLAYER;
  double TRACE3D_DISTANCE;

  // determine one track (curler) by dz
  unsigned DETERMINE_ONE_TRACK;

  // stereo find at 2D section
  unsigned STEREO_2DFIND;

  // merge
  unsigned MERGE_EXE;   
  double   MERGE_RATIO; 
  double   MERGE_Z_DIFF;
  
  // mask
  double MASK_DISTANCE;
  
  // # of wire which should be used
  double RATIO_USED_WIRE;
  
  //jialk
  // strange tracks or upper limit of radius -- Pt < ~300MeV
  //const double MIN_RADIUS_OF_STRANGE_TRACK;//65=..300MeV(FIX)
  
  // range for appending stereo wires of each super layer
  double RANGE_FOR_STEREO_FIRST;
  double RANGE_FOR_STEREO_SECOND;
  double RANGE_FOR_STEREO_THIRD;
  double RANGE_FOR_STEREO_FORTH;
  double RANGE_FOR_STEREO_FIFTH;
  double RANGE_FOR_STEREO_SIXTH;

  // Output 2D tracks
  unsigned OUTPUT_2DTRACKS;

  // CurlFinder Version ... for development
  // 0 is default
  // others ... depend on development process
  unsigned CURL_VERSION;

  //
  // TBuilderCurl.cc
  //
  // Global Fit
  unsigned ON_CORRECTION;

  // makes lines
  double Z_CUT;
  
  // appends stereo wires
  double Z_DIFF_FOR_LAST_ATTEND;

  // Svd Reconstruction Flag
  unsigned SVD_RECONSTRUCTION;
  double MIN_SVD_ELECTRONS;
  
  // Track Selector Class
  double SELECTOR_MAX_IMPACT;
  double SELECTOR_MAX_SIGMA;
  double SELECTOR_STRANGE_PZ;
  double SELECTOR_REPLACE_DZ;
  //jialk
  double minimum_seedLength;
  double minimum_2DTrackLength;
  double minimum_3DTrackLength;
  double minimum_closeHitsLength;
  double MIN_RADIUS_OF_STRANGE_TRACK;
  double ULTIMATE_MIN_RADIUS_OF_STRANGE_TRACK;
};

//
// TSegmentCurl.cc
//
const unsigned TCURL_LARGE_SEQUENCE = 4; // 4

#endif // TCURLFINDERPARAMETERS_FLAG_
