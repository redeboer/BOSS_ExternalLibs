//Dear emacs, this is -*- c++ -*-

/**
 * @file eformat/eformat.h
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: zhangy $
 * $Revision: 1.1.1.1 $
 * $Date: 2009/06/19 07:35:41 $
 *
 * @brief Includes all entities from the Event Format Library (eformat)
 */

#ifndef EFORMAT_EFORMAT_H
#define EFORMAT_EFORMAT_H

//utilities
#include "eformat/util.h"

//core stuff
#include "eformat/ROBFragment.h"
#include "eformat/ROSFragment.h"
#include "eformat/SubDetectorFragment.h"
#include "eformat/FullEventFragment.h"
#include "eformat/PagedMemory.h"

//eformat issues
#include "eformat/Issue.h"
#include "eformat/BadVersionIssue.h"
#include "eformat/NoSuchChildIssue.h"
#include "eformat/NotAlignedIssue.h"
#include "eformat/OutOfBoundsIssue.h"
#include "eformat/SizeCheckIssue.h"
#include "eformat/TooBigCountIssue.h"
#include "eformat/WrongMarkerIssue.h"

//helpers
#include "eformat/DateAndTime.h"
#include "eformat/SourceIdentifier.h"
#include "eformat/Version.h"
#include "eformat/RunNumber.h"
#include "eformat/Status.h"

#endif /* EFORMAT_EFORMAT_H */
