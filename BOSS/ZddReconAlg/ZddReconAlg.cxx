#include "ZddReconAlg/ZddReconAlg.h"
#include "ReconEvent/ReconEvent.h"
#include "ZddEvent/ZddBoard.h"
#include "ZddEvent/RecZddChannel.h"
#include "EvTimeEvent/RecEsTime.h"
#include "EventModel/EventHeader.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
ZddReconAlg::ZddReconAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    Algorithm(name, pSvcLocator),
    m_errStat(false)
{
    declareProperty("BreakWithError", m_errQuit = true);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode ZddReconAlg::initialize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "in initialize()" << endreq;

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode ZddReconAlg::execute()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "in execute()" << endreq;

    // Part 1: Get the event header, print out event and run number
    SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
    if (!eventHeader) {
	log << MSG::FATAL << "Could not find Event Header" << endreq;
	return StatusCode::FAILURE;
    }
    log << MSG::DEBUG << "Retrieved event: " << eventHeader->eventNumber()
	<< "  run: " << eventHeader->runNumber() << endreq;

    // Part 2: Register RecZddChannel to TDS
    DataObject* aRecEvent = 0;
    eventSvc()->findObject("/Event/Recon", aRecEvent);
    if ( aRecEvent == 0 ) {
	aRecEvent = new ReconEvent();
	StatusCode sc = eventSvc()->registerObject("/Event/Recon", aRecEvent);
	if ( sc.isFailure() ) {
	    log << MSG::FATAL << "Could not register ReconEvent" << endreq;
	    return StatusCode::FAILURE;
	}
    }

    RecZddChannelCol* recZddCol = new RecZddChannelCol;
    StatusCode sc = eventSvc()->registerObject(EventModel::Recon::RecZddChannelCol, recZddCol);
    if ( sc.isFailure() ) {
	log << MSG::FATAL << "Could not register RecZddChannelCol" << endreq;
	return StatusCode::FAILURE;
    }

    // Part 3: check the ZDD data
    // 3.1: some errors happened before, ignore the rest calculations
    if ( m_errStat ) return StatusCode::SUCCESS;

    // 3.2: check the status of the current ZDD event
    SmartDataPtr<Event::ZddEvent> zddEvt(eventSvc(),"/Event/ZddEvent");
    int zddCheck = zddDataStat(zddEvt.ptr(), eventHeader->eventNumber()+1);

    if ( zddCheck != 0 ) {
	if ( zddCheck < 0 ) {
	    // serious error happened, ignore all of the rest ZDD data
	    m_errStat = true;

	    if ( m_errQuit ) { // whether to break the data processing
		return StatusCode::FAILURE;
	    }
	}
	// else:
	// problems in the current event, only ignore this event
	return StatusCode::SUCCESS;
    }

    // Part 4: Get event T0
    double bes3_t0 = -10000.0;
    SmartDataPtr<RecEsTimeCol> evTimeCol(eventSvc(), "/Event/Recon/RecEsTimeCol");
    if ( !evTimeCol || evTimeCol->size() == 0 ) {
	log << MSG::WARNING << " Could not find RecEsTimeCol" << endreq;
	//return StatusCode::SUCCESS;
    }
    else {
	RecEsTimeCol::iterator iter_evt = evTimeCol->begin();
	if (iter_evt != evTimeCol->end()) {
	    bes3_t0 = (*iter_evt)->getTest();
	    //std::cout << "BESIII t0: " << bes3_t0 << std::endl; //wangyadi
	    bes3_t0 = 6400 - bes3_t0;  // T_L1 - T_collision
	}
    }

    // Part 5: ZDD data processing
    const Event::ZddEvent::Channels& chs = zddEvt->channels();
    //cout << "ZDD has n channel: " << chs.size() << endl;
    // loop the channels
    Event::ZddEvent::Channels::const_iterator end_ch = chs.end();
    for ( Event::ZddEvent::Channels::const_iterator it = chs.begin(); it != end_ch; ++it ) {
	const ZddChannel* pch = *it;
	const ZddChannel::Fragments& frags = pch->fragments();
	//cout << "Channel " << pch->getChId() << "  has " << frags.size() << " fragments: " << endl;

	RecZddChannel* recZddCh = new RecZddChannel;
	recZddCh->setChannelId(pch->getChId());
	recZddCh->setScanCode(pch->getScanCode());
	double e_K = getEK(pch->getChId());

	// loop the fragments, rebuild the original channel waveform into one buffer
        int maxSamples = 800;                                         // This value may change!
        unsigned char waveform[800];
        memset(waveform, 0, maxSamples);
	ZddChannel::Fragments::const_iterator end_fg = frags.end();
        bool quit_event = false;
        int start = 0;
	for ( ZddChannel::Fragments::const_iterator jt = frags.begin(); jt != end_fg; ++jt) {
	    const ZddFragment& frag = *jt;
	    //cout << "\tindex from " << frag.start_index << ",\t length " << frag.length << ":\t ";
	    //for ( int i = 0; i < frag.length; ++i ) {
	    //    cout << int(frag.sample[i]) << " ";
	    //}
	    //cout << endl;
            start = frag.start_index;

	    // check for CAEN data corruption ( absent from 2013 onward ?? )
            if ( start+frag.length > maxSamples ) {
                recZddCh->setScanCode( 2*10 + pch->getScanCode() );
                MsgStream log(msgSvc(), name());
                log << MSG::ERROR << "ZDD BAD DATA: CAEN corruption problem" << endreq;
                quit_event = true; // abandon this event...
                break;             // ...starting from this channel
            }

	    for ( int i = 0; i < frag.length; ++i ) waveform[start++] = frag.sample[i];
	}

	// then reclusterize with a slightly higher threshold
        unsigned char threshold = 20;                                 // threshold for reclustering
        unsigned char rephaseThreshold = 40;                          // threshold for waveform rephasing
        unsigned char minSample = 255, maxSample = -1;
        int maxTime = -1;
        bool closed = true;                                           // no cluster is running yet
	int phases[4] = {-1,-1,-1,-1};                                // a 4-channel histogram
        for ( int pt=0; pt<maxSamples; pt++ ) {
            bool notZero = waveform[pt]>0;
            bool smaller = waveform[pt] < minSample;
            if ( notZero && smaller ) minSample = waveform[pt];       // find baseline for whole channel
            if ( waveform[pt] > threshold ) {
                if ( closed ) {                                       // start a new cluster, initialize max and index
                  closed    = false;
                  maxSample = waveform[pt];
                  maxTime   = pt;
                } else {                                              // continue the old cluster, update max and index
                  if ( waveform[pt] > maxSample ) {
                      maxSample = waveform[pt];
                      maxTime   = pt;
                  }
	        }
            } else {
                if ( ! closed ) {                                     // close the old cluster and store it
                    closed = true;
                    double tNsec = 2.*maxTime;
                    recZddCh->addFragment(tNsec, maxSample*e_K);        // time relative to start of FADC window
                    // std::cout << "       ZDD E & T: " << int(maxSample) << ", " << maxTime << std::endl;
		    if ( maxSample > rephaseThreshold ) {             // most peaks are at multiples of 8ns
	                int phase = maxTime%4;                        // only one of these bins will be most used
		        phases[phase]++;                              // but we do not know which one yet
      		    }
	        }
            }
        }
        if ( ! closed ) {                                             // close and store the last cluster if still running
            closed = true;
            double tNsec = 2.*maxTime;        // time relative to start of FADC window
            recZddCh->addFragment(tNsec, maxSample*e_K);
            // std::cout << "       ZDD E & T: " << int(maxSample) << ", " << maxTime << std::endl;
	    if ( maxSample > rephaseThreshold ) {
                int phase = maxTime%4;
	        phases[phase]++;
            }
        }

	// Compute channel phase
        int mostProb = -1;
        int chPhase  = -1;
        for (int ph=0; ph<4; ph++) {
            if ( phases[ph] > mostProb ) {
                mostProb = phases[ph];
                chPhase = ph;
            }
        }

        if ( chPhase==-1 ) {// mark channel as not rephasable (no samples above rephaseThreshold)
          recZddCh->setScanCode( 3*10 + pch->getScanCode() );
          chPhase = -2;
        }

	recZddCh->setBaseLine(minSample);
	recZddCh->setPhase(chPhase);
	recZddCol->push_back(recZddCh);

/* Previous code, left for reference
	// loop the fragments
	ZddChannel::Fragments::const_iterator end_fg = frags.end();
	for ( ZddChannel::Fragments::const_iterator jt = frags.begin(); jt != end_fg; ++jt) {
	    const ZddFragment& frag = *jt;
	    int efrag = -1, tfrag = -1;
	    calFragEandT(frag, efrag, tfrag);
	    recZdd->addFragment(bes3_t0-tfrag, efrag*e_K);
	}
*/

        if ( quit_event ) break; // abandon the event entirely after a CAEN data corruption instance

    } // end of loop on channels
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode ZddReconAlg::finalize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "in finalize()" << endreq;

    return StatusCode::SUCCESS;
}

double ZddReconAlg::getEK(int chId)
{
    return 1.0/16.0;
}

int ZddReconAlg::zddDataStat(const Event::ZddEvent* zddEvt, int evtId)
{
    MsgStream log(msgSvc(), name());

    if ( !zddEvt ) {
	log << MSG::FATAL << "Could not find ZddEvent" << endreq;
	return 1; /*-1;*/
    }

    // check the ZDD trigger number
    const std::vector<ZddBoard*>& boards = zddEvt->boards();
    if ( boards.size() != 2 ) {
	log << MSG::FATAL << "incomplete ZDD data, no more ZDD data this run!" << endreq;
	return -2;
    }

    if ( boards[0]->getCounter() != evtId || boards[1]->getCounter() != evtId )
    {
	log << MSG::FATAL << "missaligned ZDD triggers, no more ZDD data this run!" << endreq;
	return -3;
    }

/* This check is not useful anymore
    // check the Trigger Time Tag
    static unsigned int bd1_tt = boards[0]->getTimeTag() - 1026;
    static unsigned int bd2_tt = boards[1]->getTimeTag() - 1026;

    unsigned int tt0 = bd1_tt;
    unsigned int tt1 = bd2_tt;
    bd1_tt = boards[0]->getTimeTag();
    bd2_tt = boards[1]->getTimeTag();

    if ( tt0 > bd1_tt ) tt0 -= 0x80000000;
    if ( tt1 > bd2_tt ) tt1 -= 0x80000000;

    if ( (bd1_tt-tt0) < 1026 || (bd2_tt-tt1) < 1026 ) {
	log << MSG::INFO << "overlaped ZDD triggers" << endreq;
	return 1;
    }
*/
    return 0;
}

// This code will be needed when we finally have a calibration
int ZddReconAlg::calFragEandT(const ZddFragment& frag, int& efrag, int& tfrag)
{
    int minIndex = 0;
    unsigned char min = 255, max = 0;
    for ( int i = 0; i < frag.length; ++i ) {
	unsigned char& sample = frag.sample[i];
	if ( sample < min ) {
	    min = sample;
	    minIndex = i;
	}
	if ( sample > max ) {
	    max = sample;
	}
    }

    efrag = max - min;
    tfrag = 8160 - 2 * (minIndex + frag.start_index);

    return 0;
}
