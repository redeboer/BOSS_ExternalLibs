//--------------------------------------------------------------------------------------|
//      [File  ]:                       MucMappingAlg.h                                 |
//      [Brief ]:       Header file of MUC electronics and geometry mapping class       |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                            |
//      [Date  ]:       Jun 6, 2006                                                     |
//      [Log   ]:       See ChangLog                                                    |
//--------------------------------------------------------------------------------------|

#include<iostream>
#include<fstream>
#include<string>

#include "GaudiKernel/Algorithm.h"

#include "MucChain.h"

using namespace std;

const int ID_NUM         = 4;            // ID numbers of readout strips including part, segment, layer and strip id in a box
const int MODULE_MAX     = 10;           // Total VME modules for MUC 
const int SOCKET_MAX     = 4;            // Maximum sockets in one module
const int CHAIN_MAX      = 40;           // Total number of data chains 
const int FEC_MAX        = 572;          // Total number of FECs 
const int STRIP_MAX      = 9152;         // Total number of readout strips

const unsigned int MARK_NUM = 4;   // Mark number of chain name
const int	         DATA_NUM	= 14;  // Data number of output

const std::string DEFAULT_MAP[MODULE_MAX][SOCKET_MAX] = {
                                    {"EE1F", "EE4F", "EE3F", "EE2F"},
                                    {"EE1S", "EE4S", "EE3S", "EE2S"},
                                    {"B030", "B020", "B010", "B080"},
                                    {"B070", "B060", "B050", "B040"},
                                    {"BW30", "BE30", "BW20", "BE20"},
                                    {"BW10", "BE10", "BW80", "BE80"},
                                    {"BW70", "BE70", "BW60", "BE60"},
                                    {"BW50", "BE50", "BW40", "BE40"},
                                    {"EW1F", "EW4F", "EW3F", "EW2F"},
                                    {"EW1S", "EW4S", "EW3S", "EW2S"} };

const std::string DATA_NAME[DATA_NUM] = { "Chain", "Module", "Socket", "FecID", "VmeInt", "VmeBin",
             			    "Part", "Seg", "Lay", "1stStr", "StrSqc", "GeoInt", "GeoBin"};

class MucMappingAlg : public Algorithm
{
        public:
        MucMappingAlg (const std::string& name, ISvcLocator* pSvcLocator);

        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

	private:
		
	int     m_fFecOrder;
	std::string  m_sFileName[2];
	std::string  m_sOutputOption;
	std::string  m_sChainMap[MODULE_MAX][SOCKET_MAX];

	MucChain*    m_mucChain[ CHAIN_MAX ];
};

//END

