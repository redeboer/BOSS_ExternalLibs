#include "MdcHoughFinder/HoughTrackList.h"
#include "MdcHoughFinder/HoughMap.h"
#include <vector>
#include <map>
#include <math.h>

HoughTrackList::HoughTrackList(){
}
HoughTrackList::HoughTrackList( const HoughMap& houghMap){
//  _trackhoughmap=houghMap;
  _tracklist=houghMap.getTrackList();
}
void HoughTrackList::remove( HoughTrack* track ){
  
  vector<HoughTrack>::iterator iter = _tracklist.begin();
  for(;iter!=_tracklist.end();iter++){
	if( track == &(*iter) ) { _tracklist.erase(iter);iter--;}
  }
  //if( iter !=_tracklist.end() ) _tracklist.erase(iter);
}
void HoughTrackList::add( HoughTrack* track ){
  _tracklist.push_back(*track);
}


