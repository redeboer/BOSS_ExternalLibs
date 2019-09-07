//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHotList.cxx,v 1.3 2010/09/26 00:31:59 zhangy Exp $
//
// Description:
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

#include "TrkBase/TrkHotList.h"
#include "TrkBase/TrkHitOnTrk.h"
#include "TrkBase/TrkView.h"
#include "MdcRecoUtil/BesCollectionUtils.h"
#include <iostream>
using std::endl;
using std::ostream;

TrkHotList::TrkHotList()
{
}

TrkHotList::~TrkHotList()
{
}

void
TrkHotList::print(ostream& o) const
{
  o << " hitCapable: " << (hitCapable()?"yes":"no")
    << " nActive: " << nActive()
    << " nHit: " << nHit()
    << " startFoundRange: " <<startFoundRange()
    << " endFoundRange: " << endFoundRange();
}

void
TrkHotList::printAll(ostream &o) const
{
  print(o); o << "\n";
  TrkHotList::hot_iterator i= begin();
  int kk=0;
  while (i!=end()) {
    kk++;
    std::cout<< kk << ":" ;
    i->print(o); o << endl;
    i->hit()->printAll(o);//yzhang debug
    i++;
  }
}

bool
TrkHotList::hasSvtView(TrkEnums::TrkViewInfo view,int layer) const
{
  TrkView need(view);
  TrkView have = svtView(layer);
  return have.contains(need);
}

  TrkHotList*
TrkHotList::resetParent(TrkBase::Functors::setParent f)
{
  std::for_each(begin(),end(),f);
  return this;
}

  void
TrkHotList::sort()
{
  std::sort(hotlist().begin(),
      hotlist().end(),
      bes::Collection::PtrLess());
}
