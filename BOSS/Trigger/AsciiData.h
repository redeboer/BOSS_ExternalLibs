#ifndef _AsciiData_h_
#define _AsciiData_h_

#include <string>
#include <iostream>
#include <vector>

#include "dmplib.h"

using std::string;
 
class VERSIONNUM : public Tagged {
public:
    VERSIONNUM() {} ;
public:
    int major;
    int minor;
    friend std::istream& operator>>(std::istream&, VERSIONNUM& x);
    friend std::ostream& operator<<(std::ostream&, const VERSIONNUM& x);
};

class EVHEAD : public Tagged { 
public:
    EVHEAD() {};
public:
    int	runNo;
    int	eventNo;
    friend std::istream& operator>>(std::istream&, EVHEAD& x);
    friend std::ostream& operator<<(std::ostream&, const EVHEAD& x);
};

class DECAYMODE : public Tagged {
  public:
    DECAYMODE () {};
  public:
    int size;
    int data[10];
    friend std::istream& operator>>(std::istream&, DECAYMODE& x);
    friend std::ostream& operator<<(std::ostream&, const DECAYMODE& x);
};

class TrackTruthType {
public:
    TrackTruthType() {};
public:
    int trackIndex;
    int PDGCode;
    float PDGCharge;
    int v0Index;
    int v1Index;
    float px;
    float py;
    float pz;
    float E;
    int minDaughterIndex;
    int maxDaughterIndex;
};

class TRACKTRUTH : public Tagged {
public:
    TRACKTRUTH() {};
public:
    int	nTruth;
    std::vector<TrackTruthType>	truthCol;
    friend std::istream& operator>>(std::istream&, TRACKTRUTH& x);
    friend std::ostream& operator<<(std::ostream&, const TRACKTRUTH& x);
};

class VertexTruthType {
public:
    VertexTruthType() {};
public:
    int vertexIndex;
    int parentTrackIndex;
    float posX;
    float posY;
    float posZ;
    float time;
};

class VERTEXTRUTH : public Tagged {
public:
    VERTEXTRUTH() {};
public:
    int	nTruth;
    std::vector<VertexTruthType>	truthCol;
    friend std::istream& operator>>(std::istream&, VERTEXTRUTH& x);
    friend std::ostream& operator<<(std::ostream&, const VERTEXTRUTH& x);
};


class MdcTruthType {
public:
    MdcTruthType() {};
public:
    int trackIndex;
    int layerNo;
    int cellNo;
    float edep;
    float driftD;
    float posX;
    float posY;
    float posZ;
    int posFlag;
    friend std::istream& operator>>(std::istream&, MdcTruthType& x);
    friend  std::ostream& operator<<(std::ostream&, const MdcTruthType& x);
};

class MDCTRUTH : public Tagged { 
public:
    MDCTRUTH() {};
public:
    int	nTruth;
    std::vector<MdcTruthType>	truthCol;
    friend std::istream& operator>>(std::istream&, MDCTRUTH& x);
    friend std::ostream& operator<<(std::ostream&, const MDCTRUTH& x);
};

class TofTruthType {
public:
    TofTruthType() {};
public:
    int trackIndex;
    int partId;
    int scinNb;
    float posX;
    float posY;
    float posZ;
    float px;
    float py;
    float pz;
    float trackL;
    float time;
    friend std::istream& operator>>(std::istream&, TofTruthType& x);
    friend  std::ostream& operator<<(std::ostream&, const TofTruthType& x);
};

class TOFTRUTH : public Tagged { 
public:
    TOFTRUTH() {};
public:
    int	nTruth;
    std::vector<TofTruthType>	truthCol;
    friend std::istream& operator>>(std::istream&, TOFTRUTH& x);
    friend std::ostream& operator<<(std::ostream&, const TOFTRUTH& x);
};

class EmcTruthType {
public:
    EmcTruthType() {};
public:
    int trackIndex;
    int partId;
    int numTheta;
    int numPhi;
    float posX;
    float posY;
    float posZ;
    float px;
    float py;
    float pz;
    float totalEdep;
    friend std::istream& operator>>(std::istream&, EmcTruthType& x);
    friend  std::ostream& operator<<(std::ostream&, const EmcTruthType& x);
};

class EMCTRUTH : public Tagged { 
public:
    EMCTRUTH() {};
public:
    int	nTruth;
    std::vector<EmcTruthType>	truthCol;
    friend std::istream& operator>>(std::istream&, EMCTRUTH& x);
    friend std::ostream& operator<<(std::ostream&, const EMCTRUTH& x);
};

class MucTruthType {
public:
    MucTruthType() {};
public:
    int trackIndex;
    int partId;
    int segId;
    int gapId;
    int stripId;
    float posX;
    float posY;
    float posZ;
    float px;
    float py;
    float pz;
    friend std::istream& operator>>(std::istream&, MucTruthType& x);
    friend  std::ostream& operator<<(std::ostream&, const MucTruthType& x);
};

class MUCTRUTH : public Tagged { 
public:
    MUCTRUTH() {};
public:
    int	nTruth;
    std::vector<MucTruthType>	truthCol;
    friend std::istream& operator>>(std::istream&, MUCTRUTH& x);
    friend std::ostream& operator<<(std::ostream&, const MUCTRUTH& x);
};

class MdcDigiType { 
public:
    MdcDigiType() {};
public:
    int trackIndex;
    int	layerNo;
    int	cellNo;
    float energyDeposit;
    float driftTime;
    friend std::istream& operator>>(std::istream&, MdcDigiType& x);
    friend  std::ostream& operator<<(std::ostream&, const MdcDigiType& x);
};

class MDCDIGI : public Tagged { 
public:
    MDCDIGI() {};
public:
    int	nDigi;
    std::vector<MdcDigiType>	digiCol;
    friend std::istream& operator>>(std::istream&, MDCDIGI& x);
    friend std::ostream& operator<<(std::ostream&, const MDCDIGI& x);
};

class TofDigiType { 
public:
    TofDigiType() {};
public:
    int trackIndex;
    int partId;
    int	scinNb;
    float forwADC;
    float forwTDC;
    float backADC;
    float backTDC;

    friend std::istream& operator>>(std::istream&, TofDigiType& x);
    friend  std::ostream& operator<<(std::ostream&, const TofDigiType& x);
};

class TOFDIGI : public Tagged { 
public:
    TOFDIGI() {};
public:
    int	nDigi;
    std::vector<TofDigiType>	digiCol;
    friend std::istream& operator>>(std::istream&, TOFDIGI& x);
    friend std::ostream& operator<<(std::ostream&, const TOFDIGI& x);
};

class EmcDigiType { 
public:
    EmcDigiType() {};
public:
    int trackIndex;
    int partId;
    int	numTheta;
    int	numPhi;
    float energyDeposit;
    float hitTime;
    friend std::istream& operator>>(std::istream&, EmcDigiType& x);
    friend  std::ostream& operator<<(std::ostream&, const EmcDigiType& x);
};

class EMCDIGI : public Tagged { 
public:
    EMCDIGI() {};
public:
    int	nDigi;
    std::vector<EmcDigiType>	digiCol;
    friend std::istream& operator>>(std::istream&, EMCDIGI& x);
    friend std::ostream& operator<<(std::ostream&, const EMCDIGI& x);
};

class MucDigiType { 
public:
    MucDigiType() {};
public:
    int trackIndex;
    int	partNo;
    int segNo;
    int	gapNo;
    int stripNo;
    friend std::istream& operator>>(std::istream&, MucDigiType& x);
    friend  std::ostream& operator<<(std::ostream&, const MucDigiType& x);
};

class MUCDIGI : public Tagged { 
public:
    MUCDIGI() {};
public:
    int	nDigi;
    std::vector<MucDigiType>	digiCol;
    friend std::istream& operator>>(std::istream&, MUCDIGI& x);
    friend std::ostream& operator<<(std::ostream&, const MUCDIGI& x);
};

class EVENT : public Tagged { 
public:
    EVENT() {};
public:
    EVHEAD	header;
    DECAYMODE   decayMode;
    TRACKTRUTH  trackTruth;
    VERTEXTRUTH vertexTruth;
    MDCTRUTH    mdcTruth;
    MDCDIGI     mdcDigi;
    TOFTRUTH    tofTruth;
    TOFDIGI     tofDigi;
    EMCTRUTH    emcTruth;
    EMCDIGI     emcDigi;
    MUCTRUTH    mucTruth;
    MUCDIGI     mucDigi;
    friend std::istream& operator>>(std::istream&, EVENT& x);
    friend std::ostream& operator<<(std::ostream&, const EVENT& x);
};

#endif // _AsciiData_h_
