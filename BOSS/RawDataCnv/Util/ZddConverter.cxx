#include "RawDataCnv/Util/ZddConverter.h"
#include "ZddEvent/ZddBoard.h"
#include "ZddEvent/ZddChannel.h"
#include <cstring>
#include <iomanip>  //FIXME: for debugging

ZddConverter* ZddConverter::s_instance = 0;

ZddConverter* ZddConverter::instance(int runMode)
{
    if ( s_instance == 0 ) {
	s_instance = new ZddConverter(runMode);
    }

    return s_instance;
}

void ZddConverter::destroy()
{
    if ( s_instance != 0 ) {
	delete s_instance;
	s_instance = 0;
    }
}

bool ZddConverter::convert(uint32_t* pdata, int size, Event::ZddEvent* evt)
{
    // for debugging
    //std::cout << "RAW buffer size: " << size << std::endl << std::hex;
    //for ( int i = 0; i < size; ++i ) {
    //    if ( i%8 == 0 ) std::cout << "0x" << std::setw(8) << std::setfill('0') << i << ":";
    //    std::cout << " 0x" << std::setw(8) << std::setfill('0') << pdata[i];
    //    if ( i%8 == 7 ) std::cout << std::endl;
    //}
    //std::cout << std::dec << std::endl;
    ///////////////////////////////////////////

    uint32_t* pend = pdata + size;

    while ( pdata < pend ) {
	pdata = decodeBoard(pdata, evt);
    }

    if ( pdata != pend ) {
	std::cout << "ZddConverter: there are problems within the event data size" << std::endl;
	exit(1);
    }

    return true;
}

uint32_t* ZddConverter::decodeBoard(uint32_t* pevt, Event::ZddEvent* evt)
{
    if ( (pevt[0] >> 28) != 10 ) {
	std::cout << "ZddConverter get wrong event marker!" << std::endl;
	exit(1);
    }

    int size = pevt[0] & 0xFFFFFFF;
    int board = pevt[1] >> 27;
    uint32_t chMask = pevt[1] & 0xFF;

    ZddBoard* zddBoard = new ZddBoard(evt);
    zddBoard->setBoardId(board);
    zddBoard->setCounter(pevt[2]&0xFFFFFF);
    zddBoard->setTimeTag(pevt[3]);

    int  ich = 0;

    uint32_t* pend = pevt + size;
    uint32_t* pchannel = pevt + 4;

    while ( pchannel < pend ) {
	while ( (chMask&(1<<ich)) == 0 ) {
	    ++ich;
	    if ( ich > 7 ) {
		std::cout << "ZddConverter get wrong channel mask!" << std::endl;
		exit(1);
	    }
	}
	ZddChannel* zddCh = new ZddChannel();
	zddCh->setChId(board*100+ich);
	zddBoard->addChannel(zddCh);
	pchannel = decodeChannel(pchannel, zddCh);
	++ich;
    }

    if ( pchannel != pend ) {
	std::cout << "ZddConverter: there are problems within the channel data size" << std::endl;
	exit(1);
    }

    return pend;
}

uint32_t* ZddConverter::decodeChannel(uint32_t* pch, ZddChannel* zddCh)
{
    uint32_t* pend = pch + pch[0];
    uint32_t* pfrag = pch + 1;

    ZddFragment zddFrag;

    int nCtrlWord = 0;  //FIXME: check the good/skip transfrom, N < 14

    int lstat = 0;
    int index = 0;
    int samples = 800; // FIXME: writing 800 here is a bad idea

    while ( pfrag < pend ) {
	uint32_t& ctrl_word = pfrag[0];
	int  size = (ctrl_word & 0x1FFFFF) * 4;  //from words to bytes

	if ( ctrl_word >> 31 ) {  //fragment passed threshold

            if ( ctrl_word == 0xFEFEFEFE ) break; // CAEN FW bug discovered June 2014, not much serious:
                                                  // not really a control word, end of channel instead

            // previous treatment by Jaheng
	    if ( (pfrag + size/4 + 1) > pend ) {  //FIXME: error ZDD data
		std::cout << "BAD ZDD RAW DATA!" << std::endl;
		//exit(1);
		break;
	    }
	    zddFrag.start_index = index;
	    zddFrag.length = size;
	    zddFrag.sample = new unsigned char[size];
	    memcpy(zddFrag.sample, pfrag+1, size);
	    zddCh->addFragments(zddFrag);
	    pfrag += size/4 + 1;
	    if ( lstat < 0 ) ++nCtrlWord;
	    lstat = 1;
	}
	else {  //fragment skipped
	    pfrag += 1;
	    if ( lstat > 0 ) ++ nCtrlWord;
	    lstat = -1;
	}

	index += size;
/* Following check UNNEEDED
	if ( nCtrlWord == 14 ) {  //FIXME: to be fixed
	    if ( pfrag < pend ) {
		zddFrag.start_index = index;
		zddFrag.length = (pend-pfrag)*4;
		zddFrag.sample = new unsigned char[zddFrag.length];
		memcpy(zddFrag.sample, pfrag, zddFrag.length);
		zddCh->addFragments(zddFrag);
	    }
	    break;
	}
*/
    }

    if ( index < samples ) {
        zddCh->setScanCode(-1); // mark channel as "Incomplete scan"
//        std::cout << "ZDD MISS channelId=" << zddCh->getChId()
//                  << " missing samples:" << samples-index << " marked INCOMPLETE SCAN" << std::endl;
    }

    return pend;
}

ZddConverter::ZddConverter(int runMode)
    : m_runMode(runMode)
{
}

ZddConverter::~ZddConverter()
{
}
