//-----------------------------------------------------------------------------
// $Id: TUpdater.h,v 1.4 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TUpdater.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to handle update timing of static objects of tracking.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TUpdater_FLAG_
#define TUpdater_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

/// A class to handle update timing of static objects of tracking.
class TUpdater {

  public:
    /// Constructor
    TUpdater();

  public:
    /// Destructor
    virtual ~TUpdater();

  public:
    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

  public:
    /// returns true if an object is updated.
    virtual bool updated(void) const;

    /// updates an object.
    virtual void update(void);

    /// returns true if an object is initialized.
    virtual bool initialized(void) const;

    /// initializes an object.
    virtual void initialize(void);

    /// clears an object.
    virtual void clear(void);

  private:
    int _expN;
    int _runN;
    int _evtN;
    bool _init;
};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TUpdater_INLINE_DEFINE_HERE
#endif

#ifdef TUpdater_INLINE_DEFINE_HERE

inline
bool
TUpdater::initialized(void) const {
    return _init;
}

inline
void
TUpdater::initialize(void) {
    _init = true;
}

inline
void
TUpdater::clear(void) {
    _expN = -1;
}

#endif

#undef inline

#endif /* TUpdater_FLAG_ */
