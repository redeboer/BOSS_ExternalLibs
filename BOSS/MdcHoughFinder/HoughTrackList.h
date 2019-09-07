#ifndef  HOUGHTRACKLIST_H
#define  HOUGHTRACKLIST_H

#include "MdcHoughFinder/HoughMap.h"
#include "MdcHoughFinder/HoughPeak.h"
#include "MdcHoughFinder/HoughHitList.h"
//#include "Hough/HoughCircle.h"
#include <vector>
class HoughTrackList{
  public:
    HoughTrackList();
    HoughTrackList( const HoughMap& houghMap );
    int getTrackNum()   	      const {return _tracklist.size(); }
    HoughTrack& getTrack(int i)   {return (_tracklist[i]); }
	void remove(HoughTrack* );
	void add(HoughTrack* track) ;

  private:
    //class member
//    HoughMap _trackhoughmap;
    std::vector<HoughTrack>  _tracklist;
};
#endif
