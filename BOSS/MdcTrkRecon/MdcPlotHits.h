// MdcPlotHits.h
//  handle hit plotting, via MdcDisplay

// Interface Dependencies ----------------------------------------------

#ifndef MDCPLOTHITS_HH
#define MDCPLOTHITS_HH
#ifdef MDCDISPLAY

//  End Interface Dependencies -----------------------------------------

class TrkHitOnTrk;
class MdcHit;
template <class T> class HepAList;

// Class definition//
class MdcPlotHits {

public:
  MdcPlotHits();
  ~MdcPlotHits() { };
  
  void plotHits(const HepAList<TrkHitOnTrk> &hitlist, double bunchTime, 
		int color = 10) const;
  void plotAHit(const MdcHit *hit, double bunchTime, 
		int mode = -1, int color = -1, int timeIndex = 0) const;
  //  void plotinforight(const MdcHit *hit) const;
  //  void plotinfoleft(const MdcHit *hit) const;
  void plotinfo(const MdcHit *hit, double scale, double tbunch) const;

  static void setColors(int axcol, int poscol, int negcol) {
                                                axialColor = axcol;
                                                posStereoColor = poscol;
                                                negStereoColor = negcol;
  }
  
private:
  static int axialColor;
  static int posStereoColor;
  static int negStereoColor;
};

#endif
#endif







