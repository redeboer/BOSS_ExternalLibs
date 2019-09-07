//-----------------------------------------------------------------------------
// $Id: TWindow.h,v 1.7 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TWindow.h
// Section  : Tracking MDC
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to display tracking object.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifdef TRKRECO_WINDOW

#ifndef TWINDOW_FLAG_
#define TWINDOW_FLAG_

#include <string>

//#include "belle.h"
#ifdef HAVE_LEDA
//#include <LEDA/window.h>
//#include <LEDA/stack.h>
//#include <LEDA/color.h>
#endif
#define HEP_SHORT_NAMES
#include "CLHEP/Alist/ConstAList.h"
#include "CLHEP/Alist/CList.h"
#include "CLHEP/Geometry/Vector3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Vector3D<double> HepVector3D;
#endif

class TTrackBase;
class TTrack;
class TMLine;
class TWindow;
class TSegment;
class TMLink;
class TPoint2D;

/// A class to display tracking object.
class TWindow {
    
  public:
    /// Default constructor
    TWindow(const TWindow &);
    
    /// Constructor
    TWindow(const std::string & name);
    
    /// Destructor
    virtual ~TWindow();
    
  public:
    /// opens window.
    void open(void);

    /// clear window contents.
    void clear(void);

    /// appends/remove objects.
    void append(const TTrackBase &,
		leda_color = leda_black,
		bool toBeSelfObject = false);
    void append(const AList<TSegment> &,
		leda_color = leda_black,
		bool toBeSelfObject = false);
    void append(const AList<TTrack> &,
		leda_color = leda_black,
		bool toBeSelfObject = false);
    void remove(const TTrackBase &);
    void remove(const AList<TSegment> &);
    void remove(const AList<TTrack> &);
    void appendSz(const TTrack &, const AList<TMLink> &, leda_color=leda_black);
    void appendSz(const TTrack &,
		  const AList<TSegment> &,
		  leda_color=leda_black);

    /// appends TMLink's. You can't remove TMLink's but you can clear.
    void append(const AList<TMLink> &, leda_color = leda_black);

    /// displays objects w/o appending.
    void oneShot(const TTrackBase &, leda_color = leda_black);
    void oneShot(const AList<TSegment> &, leda_color = leda_black);

    /// waits for user actions.
    void wait(void);

    /// draws objects in stack.
    void draw(void);

    /// draws a point.
    void draw(const TPoint2D &, leda_color = leda_black);

    /// returns/draws text.
    std::string text(void) const;
    void text(const std::string & text);

    /// sets run, farm, and event number of target event to display.
    void target(unsigned run, unsigned farm, unsigned event);

    /// sets drawing mode. (0:rphi, 1:conformal, 2:s-z);
    unsigned mode(unsigned);

    /// sets skip mode.
    bool skip(bool);
    bool skipAllWindow(bool);

  private:
    void drawHeader(void);
    void drawBase(const TTrackBase &, leda_color = leda_black);
    void drawSegment(const TSegment &, leda_color = leda_black);
    void drawTrack(const TTrack &, leda_color = leda_black);
    void drawLine(const TMLine &, leda_color = leda_black);
    void drawAxes(void);
    void drawCdc(void);
    bool target(void) const;
    std::string wireName(const TMLink &) const;

  private:
    std::string _name;
    leda_window _window;
    double _xmin, _xmax, _ymin;
    leda_stack<HepVector3D> _zoomHistory[4];
    int _closeButton;
    int _wireNameButton;
    int _axialButton;
    int _stereoButton;
    int _coordinateButton;
    int _zoomInButton;
    int _zoomOutButton;
    int _skipButton;
    int _skipAllButton;
    int _szButton;
    int _confButton;
    int _segmentLinkButton;
    int _segmentPropertyButton;
    int _copyButton;

    bool _wireName;
    bool _axial;
    bool _stereo;
    bool _coordinate;
    bool _display;
    bool _skip;
    bool _segmentLink;
    bool _segmentProperty;
    static bool _skipAllWindow;
    unsigned _nYLine;

    bool _canDelete;
    CAList<TTrackBase> _objects;
    AList<TTrackBase> _selfObjects;
    AList<TMLink> _selfTLinks;
    AList<leda_color> _colors;
    std::string _text;
    double _xPositionText;
    double _yPositionText;
    double _yPositionStep;

    unsigned _mode;
    unsigned _targetRun;
    unsigned _targetEvent;
    unsigned _nEvents;
};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TWINDOW_INLINE_DEFINE_HERE
#endif

#ifdef TWINDOW_INLINE_DEFINE_HERE

inline
void
TWindow::append(const TTrackBase & a, leda_color b, bool self) {
    _objects.append(a);
    _colors.append(new leda_color(b));
    if (self)
	_selfObjects.append((TTrackBase &) a);
}

inline
void
TWindow::append(const AList<TSegment> & a, leda_color b, bool self) {
    for (unsigned i = 0; i < a.length(); i++) {
	append(* (TTrackBase *) a[i], b);
	if (self)
	    _selfObjects.append(* (TTrackBase *) a[i]);
    }
}

inline
void
TWindow::append(const AList<TTrack> & a, leda_color b, bool self) {
    for (unsigned i = 0; i < a.length(); i++) {
	append(* (TTrackBase *) a[i], b);
	if (self)
	    _selfObjects.append(* (TTrackBase *) a[i]);
    }
}

inline
void
TWindow::remove(const TTrackBase & a) {
    int i;
    while ((i = _objects.fIndex(a)) != -1) {
	_objects.remove(i);
	leda_color * b = _colors[i];
	_colors.remove(i);
	delete b;
    }
}

inline
void
TWindow::remove(const AList<TSegment> & a) {
    for (unsigned i = 0; i < a.length(); i++) {
	remove(* (TTrackBase *) a[i]);
    }
}

inline
void
TWindow::remove(const AList<TTrack> & a) {
    for (unsigned i = 0; i < a.length(); i++) {
	remove(* (TTrackBase *) a[i]);
    }
}

inline
void
TWindow::open(void) {
    if (! target()) return;
    if (! _display) {
	if (_mode == 1) {
	    _xmin = -1;
	    _xmax = 7;
	    _ymin = -1;
	    _window.init(_xmin, _xmax, _ymin);
	}
	_window.display();
    }
    _display = true;
}

inline
unsigned
TWindow::mode(unsigned a) {
    if (a == 2) {
	_stereo = true;
	_axial = false;
    }
    return _mode = a;
}

inline
bool
TWindow::skip(bool a) {
    return _skip = a;
}

inline
bool
TWindow::skipAllWindow(bool a) {
    return _skipAllWindow = a;
}

inline
void
TWindow::text(const std::string & text) {
    _text = text;
}

inline
std::string
TWindow::text(void) const {
    return _text;
}

#endif

#undef inline

#endif TWINDOW_FLAG_

#endif /* TRKRECO_WINDOW */
