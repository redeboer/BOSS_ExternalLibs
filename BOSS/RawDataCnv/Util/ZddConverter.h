#ifndef ZDD_CONVERTER_H
#define ZDD_CONVERTER_H

#include "ZddEvent/ZddEvent.h"
#include <stdint.h>

class ZddChannel;

class ZddConverter
{
    public :

	static ZddConverter* instance(int runMode = 2);  //default OfflineMode
	static void destroy();

	bool convert(uint32_t* pdata, int size, Event::ZddEvent* evt);

    private :

	uint32_t* decodeBoard(uint32_t* pevt, Event::ZddEvent* evt);
	uint32_t* decodeChannel(uint32_t* pch, ZddChannel* evt);

	ZddConverter(int runMode);
	~ZddConverter();

	ZddConverter();  // not support


    private :// data members

	int m_runMode;

	static  ZddConverter* s_instance;
};

#endif
