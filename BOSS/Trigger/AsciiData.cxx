
#include "Trigger/AsciiData.h"

//std::string Tagged::s_saved_tag;

std::istream& operator>>(std::istream& is, VERSIONNUM& x)
{
    x.check_start_tag(is,"VERSION");
    if(!x.initialized()) return is;
    is >> x.major;
    is >> x.minor;
    x.check_end_tag(is, "VERSION");
    return is;
}

std::ostream& operator<<(std::ostream& os, const VERSIONNUM& x)
{
    os << std::endl << "{ VERSION" << std::endl;
    if(x.initialized()) {
    os << " " << x.major;
    os << " " << x.minor;
    }
    os << std::endl << "} VERSION" << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, EVHEAD& x)
{
    x.check_start_tag(is,"EVHEAD");
    if(!x.initialized()) return is;
    is >> x.runNo;
    is >> x.eventNo;
    x.check_end_tag(is, "EVHEAD");
    return is;
}

std::ostream& operator<<(std::ostream &os,const EVHEAD& x)
{
    os << std::endl << "{ EVHEAD" << std::endl;
    if(x.initialized()) {
    os << " " << x.runNo;
    os << " " << x.eventNo;
    }
    os << std::endl << "} EVHEAD" << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, DECAYMODE& x)
{
   x.check_start_tag(is,"DECAYMODE");
   if(!x.initialized()) return is;
   is >> x.size;
   for (int index=0; index<x.size; index++) {
     is >> x.data[index];
   }
   x.check_end_tag(is, "DECAYMODE");
   return is;
}

std::ostream& operator<<(std::ostream &os,const DECAYMODE& x)
{
  os << std::endl << "{ DECAYMODE" << std::endl;
  if(x.initialized()) {
    os << " " << x.size;
    for (int index=0; index<x.size; index++) {
      os << " " << x.data[index];
    }
  }
  os << std::endl << "} DECAYMODE" << std::endl;
  return os;
}

std::istream& operator>>(std::istream& is, TrackTruthType& x)
{
    is >> x.trackIndex;
    is >> x.PDGCode;
    is >> x.PDGCharge;
    is >> x.v0Index;
    is >> x.v1Index;
    is >> x.px;
    is >> x.py;
    is >> x.pz;
    is >> x.E;
    is >> x.minDaughterIndex;
    is >> x.maxDaughterIndex;
    return is;
}

std::ostream& operator<<(std::ostream &os,const TrackTruthType& x)
{
    os << " " << x.trackIndex;
    os << " " << x.PDGCode;
    os << " " << x.PDGCharge;
    os << " " << x.v0Index;
    os << " " << x.v1Index;
    os << " " << x.px;
    os << " " << x.py;
    os << " " << x.pz;
    os << " " << x.E;
    os << " " << x.minDaughterIndex;
    os << " " << x.maxDaughterIndex;
    os << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, TRACKTRUTH& x)
{
    x.check_start_tag(is,"TRACKTRUTH");
    if(!x.initialized()) return is;
    is >> x.nTruth;
    x.truthCol.resize(x.nTruth);
    {for(int i = 0; i < x.nTruth; i++) {
        is >> x.truthCol[i];
    }}
    x.check_end_tag(is, "TRACKTRUTH");
    return is;
}

std::ostream& operator<<(std::ostream &os,const TRACKTRUTH& x)
{
    os << std::endl << "{ TRACKTRUTH" << std::endl;
    if(x.initialized()) {
    os << " " << x.nTruth;
    {for(int i = 0; i < x.nTruth; i++) {
        os << " " << x.truthCol[i];
    }}
    }
    os << std::endl << "} TRACKTRUTH" << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, VertexTruthType& x)
{
    is >> x.vertexIndex;
    is >> x.parentTrackIndex;
    is >> x.posX;
    is >> x.posY;
    is >> x.posZ;
    is >> x.time;
    return is;
}

std::ostream& operator<<(std::ostream &os,const VertexTruthType& x)
{
    os << " " << x.vertexIndex;
    os << " " << x.parentTrackIndex;
    os << " " << x.posX;
    os << " " << x.posY;
    os << " " << x.posZ;
    os << " " << x.time;
    os << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, VERTEXTRUTH& x)
{
    x.check_start_tag(is,"VERTEXTRUTH");
    if(!x.initialized()) return is;
    is >> x.nTruth;
    x.truthCol.resize(x.nTruth);
    {for(int i = 0; i < x.nTruth; i++) {
        is >> x.truthCol[i];
    }}
    x.check_end_tag(is, "VERTEXTRUTH");
    return is;
}

std::ostream& operator<<(std::ostream &os,const VERTEXTRUTH& x)
{
    os << std::endl << "{ VERTEXTRUTH" << std::endl;
    if(x.initialized()) {
    os << " " << x.nTruth;
    {for(int i = 0; i < x.nTruth; i++) {
        os << " " << x.truthCol[i];
    }}
    }
    os << std::endl << "} VERTEXTRUTH" << std::endl;
    return os;
}


std::istream& operator>>(std::istream& is, MdcTruthType& x)
{
    is >> x.trackIndex;
    is >> x.layerNo;  
    is >> x.cellNo;
    is >> x.edep;
    is >> x.driftD;
    is >> x.posX;
    is >> x.posY;
    is >> x.posZ;
    is >> x.posFlag;
    return is;
}

std::ostream& operator<<(std::ostream &os,const MdcTruthType& x)
{
    os << " " << x.trackIndex;
    os << " " << x.layerNo;
    os << " " << x.cellNo;
    os << " " << x.edep;
    os << " " << x.driftD;
    os << " " << x.posX;
    os << " " << x.posY;
    os << " " << x.posZ;
    os << " " << x.posFlag;
    os << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, MDCTRUTH& x)
{
    x.check_start_tag(is,"MDCTRUTH");
    if(!x.initialized()) return is;
    is >> x.nTruth;
    x.truthCol.resize(x.nTruth);
    {for(int i = 0; i < x.nTruth; i++) {
        is >> x.truthCol[i];
    }}
    x.check_end_tag(is, "MDCTRUTH");
    return is;
}

std::ostream& operator<<(std::ostream &os,const MDCTRUTH& x)
{
    os << std::endl << "{ MDCTRUTH" << std::endl;
    if(x.initialized()) {
    os << " " << x.nTruth;
    {for(int i = 0; i < x.nTruth; i++) {
        os << " " << x.truthCol[i];
    }}
    }
    os << std::endl << "} MDCTRUTH" << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, TofTruthType& x)
{
    is >> x.trackIndex;
    is >> x.partId;  
    is >> x.scinNb;
    is >> x.posX;
    is >> x.posY;
    is >> x.posZ;
    is >> x.px;
    is >> x.py;
    is >> x.pz;
    is >> x.trackL;
    is >> x.time;
    return is;
}

std::ostream& operator<<(std::ostream &os,const TofTruthType& x)
{
    os << " " << x.trackIndex;
    os << " " << x.partId;
    os << " " << x.scinNb;
    os << " " << x.posX;
    os << " " << x.posY;
    os << " " << x.posZ;
    os << " " << x.px;
    os << " " << x.py;
    os << " " << x.pz;
    os << " " << x.trackL;
    os << " " << x.time;
    os << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, TOFTRUTH& x)
{
    x.check_start_tag(is,"TOFTRUTH");
    if(!x.initialized()) return is;
    is >> x.nTruth;
    x.truthCol.resize(x.nTruth);
    {for(int i = 0; i < x.nTruth; i++) {
        is >> x.truthCol[i];
    }}
    x.check_end_tag(is, "TOFTRUTH");
    return is;
}

std::ostream& operator<<(std::ostream &os,const TOFTRUTH& x)
{
    os << std::endl << "{ TOFTRUTH" << std::endl;
    if(x.initialized()) {
    os << " " << x.nTruth;
    {for(int i = 0; i < x.nTruth; i++) {
        os << " " << x.truthCol[i];
    }}
    }
    os << std::endl << "} TOFTRUTH" << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, EmcTruthType& x)
{
    is >> x.trackIndex;
    is >> x.partId;
    is >> x.numTheta;  
    is >> x.numPhi;
    is >> x.posX;
    is >> x.posY;
    is >> x.posZ;
    is >> x.px;
    is >> x.py;
    is >> x.pz;
    is >> x.totalEdep;
    return is;
}

std::ostream& operator<<(std::ostream &os,const EmcTruthType& x)
{
    os << " " << x.trackIndex;
    os << " " << x.partId;
    os << " " << x.numTheta;
    os << " " << x.numPhi;
    os << " " << x.posX;
    os << " " << x.posY;
    os << " " << x.posZ;
    os << " " << x.px;
    os << " " << x.py;
    os << " " << x.pz;
    os << " " << x.totalEdep;
    os << std::endl;
    return os;
    
}

std::istream& operator>>(std::istream& is, EMCTRUTH& x)
{
    x.check_start_tag(is,"EMCTRUTH");
    if(!x.initialized()) return is;
    is >> x.nTruth;
    x.truthCol.resize(x.nTruth);
    {for(int i = 0; i < x.nTruth; i++) {
        is >> x.truthCol[i];
    }}
    x.check_end_tag(is, "EMCTRUTH");
    return is;
}

std::ostream& operator<<(std::ostream &os,const EMCTRUTH& x)
{
    os << std::endl << "{ EMCTRUTH" << std::endl;
    if(x.initialized()) {
    os << " " << x.nTruth;
    {for(int i = 0; i < x.nTruth; i++) {
        os << " " << x.truthCol[i];
    }}
    }
    os << std::endl << "} EMCTRUTH" << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, MucTruthType& x)
{
    is >> x.trackIndex;
    is >> x.partId;
    is >> x.segId;
    is >> x.gapId;
    is >> x.stripId;
    is >> x.posX;
    is >> x.posY;
    is >> x.posZ;
    is >> x.px;
    is >> x.py;
    is >> x.pz;
    return is;
}

std::ostream& operator<<(std::ostream &os,const MucTruthType& x)
{
    os << " " << x.trackIndex;
    os << " " << x.partId;
    os << " " << x.segId;
    os << " " << x.gapId;
    os << " " << x.stripId;
    os << " " << x.posX;
    os << " " << x.posY;
    os << " " << x.posZ;
    os << " " << x.px;
    os << " " << x.py;
    os << " " << x.pz;
    os << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, MUCTRUTH& x)
{
    x.check_start_tag(is,"MUCTRUTH");
    if(!x.initialized()) return is;
    is >> x.nTruth;
    x.truthCol.resize(x.nTruth);
    {for(int i = 0; i < x.nTruth; i++) {
        is >> x.truthCol[i];
    }}
    x.check_end_tag(is, "MUCTRUTH");
    return is;
}

std::ostream& operator<<(std::ostream &os,const MUCTRUTH& x)
{
    os << std::endl << "{ MUCTRUTH" << std::endl;
    if(x.initialized()) {
    os << " " << x.nTruth;
    {for(int i = 0; i < x.nTruth; i++) {
        os << " " << x.truthCol[i];
    }}
    }
    os << std::endl << "} MUCTRUTH" << std::endl;
    return os;
}


std::istream& operator>>(std::istream& is, MdcDigiType& x)
{
    is >> x.trackIndex;
    is >> x.layerNo;
    is >> x.cellNo;
    is >> x.energyDeposit;
    is >> x.driftTime;
    return is;
}

std::ostream& operator<<(std::ostream &os,const MdcDigiType& x)
{
    os << " " << x.trackIndex;
    os << " " << x.layerNo;
    os << " " << x.cellNo;
    os << " " << x.energyDeposit;
    os << " " << x.driftTime;
    os << std::endl;
    return os;
}


std::istream& operator>>(std::istream& is, MDCDIGI& x)
{
    x.check_start_tag(is,"MDCDIGI");
    if(!x.initialized()) return is;
    is >> x.nDigi;
    x.digiCol.resize(x.nDigi);
    {for(int i = 0; i < x.nDigi; i++) {
        is >> x.digiCol[i];
    }}
    x.check_end_tag(is, "MDCDIGI");
    return is;
}

std::ostream& operator<<(std::ostream &os,const MDCDIGI& x)
{
    os << std::endl << "{ MDCDIGI" << std::endl;
    if(x.initialized()) {
    os << " " << x.nDigi;
    {for(int i = 0; i < x.nDigi; i++) {
        os << " " << x.digiCol[i];
    }}
    }
    os << std::endl << "} MDCDIGI" << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, TofDigiType& x)
{
    is >> x.trackIndex;
    is >> x.partId;
    is >> x.scinNb;
    is >> x.forwADC;
    is >> x.forwTDC;
    is >> x.backADC;
    is >> x.backTDC;
    return is;
}

std::ostream& operator<<(std::ostream &os,const TofDigiType& x)
{
    os << " " << x.trackIndex;
    os << " " << x.partId;
    os << " " << x.scinNb;
    os << " " << x.forwADC;
    os << " " << x.forwTDC;
    os << " " << x.backADC;
    os << " " << x.backTDC;
    os << std::endl;
    return os;
}


std::istream& operator>>(std::istream& is, TOFDIGI& x)
{
    x.check_start_tag(is,"TOFDIGI");
    if(!x.initialized()) return is;
    is >> x.nDigi;
    x.digiCol.resize(x.nDigi);
    {for(int i = 0; i < x.nDigi; i++) {
        is >> x.digiCol[i];
    }}
    x.check_end_tag(is, "TOFDIGI");
    return is;
}

std::ostream& operator<<(std::ostream &os,const TOFDIGI& x)
{
    os << std::endl << "{ TOFDIGI" << std::endl;
    if(x.initialized()) {
    os << " " << x.nDigi;
    {for(int i = 0; i < x.nDigi; i++) {
        os << " " << x.digiCol[i];
    }}
    }
    os << std::endl << "} TOFDIGI" << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, EmcDigiType& x)
{
    is >> x.trackIndex;
    is >> x.partId;
    is >> x.numTheta;
    is >> x.numPhi;
    is >> x.energyDeposit;
    is >> x.hitTime;
    return is;
}

std::ostream& operator<<(std::ostream &os,const EmcDigiType& x)
{
    os << " " << x.trackIndex;
    os << " " << x.partId;
    os << " " << x.numTheta;
    os << " " << x.numPhi;
    os << " " << x.energyDeposit;
    os << " " << x.hitTime;
    os << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, EMCDIGI& x)
{
    x.check_start_tag(is,"EMCDIGI");
    if(!x.initialized()) return is;
    is >> x.nDigi;
    x.digiCol.resize(x.nDigi);
    {for(int i = 0; i < x.nDigi; i++) {
        is >> x.digiCol[i];
    }}
    x.check_end_tag(is, "EMCDIGI");
    return is;
}

std::ostream& operator<<(std::ostream &os,const EMCDIGI& x)
{
    os << std::endl << "{ EMCDIGI" << std::endl;
    if(x.initialized()) {
    os << " " << x.nDigi;
    {for(int i = 0; i < x.nDigi; i++) {
        os << " " << x.digiCol[i];
    }}
    }
    os << std::endl << "} EMCDIGI" << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, MucDigiType& x)
{
    is >> x.trackIndex;
    is >> x.partNo;
    is >> x.segNo;
    is >> x.gapNo;
    is >> x.stripNo;
    return is;
}

std::ostream& operator<<(std::ostream &os,const MucDigiType& x)
{
    os << " " << x.trackIndex;
    os << " " << x.partNo;
    os << " " << x.segNo;
    os << " " << x.gapNo;
    os << " " << x.stripNo;
    os << std::endl;
    return os;
}


std::istream& operator>>(std::istream& is, MUCDIGI& x)
{
    x.check_start_tag(is,"MUCDIGI");
    if(!x.initialized()) return is;
    is >> x.nDigi;
    x.digiCol.resize(x.nDigi);
    {for(int i = 0; i < x.nDigi; i++) {
        is >> x.digiCol[i];
    }}
    x.check_end_tag(is, "MUCDIGI");
    return is;
}

std::ostream& operator<<(std::ostream &os,const MUCDIGI& x)
{
    os << std::endl << "{ MUCDIGI" << std::endl;
    if(x.initialized()) {
    os << " " << x.nDigi;
    {for(int i = 0; i < x.nDigi; i++) {
        os << " " << x.digiCol[i];
    }}
    }
    os << std::endl << "} MUCDIGI" << std::endl;
    return os;
}
std::istream& operator>>(std::istream& is, EVENT& x)
{
    x.check_start_tag(is,"EVENT");
    if(!x.initialized()) return is;
    
    try {
      is >> x.header;
    } catch(AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading header block !!!" << std::endl;
    }
   
    try {
      is >> x.decayMode;
    } catch(AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading decay mode block !!!" << std::endl;
    }
    
    try {
      is >> x.trackTruth;
    } catch(AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading track truth block !!!" << std::endl;
    }

    try {
      is >> x.vertexTruth;
    } catch (AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading vertex truth block !!!" << std::endl;
    }
    
    try {
      is >> x.mdcTruth;
    } catch (AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading mdc truth block !!!" << std::endl;
    }
    
    try {
      is >> x.mdcDigi;
    } catch (AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading mdc digi block !!!" << std::endl;
    }
    
    try {
      is >> x.tofTruth;
    } catch (AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading tof truth block !!!" << std::endl;
    }

    try {
      is >> x.tofDigi;
    } catch (AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading tof digi block !!!" << std::endl;
    }
    
    try {
      is >> x.emcTruth;
    } catch (AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading emc truth block !!!" << std::endl;
    }

    try {
      is >> x.emcDigi;
    } catch (AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading emc digi block !!!" << std::endl;
    }
    
    try {
      is >> x.mucTruth;
    } catch (AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading muc truth block !!!" << std::endl;
    }
    
    try {
      is >> x.mucDigi;
    } catch (AsciiDumpException& ) {
      std::cerr << "Got AsciiDumpException eror while reading muc digi block !!!" << std::endl;
    }
    x.check_end_tag(is, "EVENT");
    return is;
}

std::ostream& operator<<(std::ostream &os,const EVENT& x)
{
    os << std::endl << "{ EVENT" << std::endl;
    if(x.initialized()) {
    os << " " << x.header;
    os << " " << x.decayMode;
    os << " " << x.trackTruth;
    os << " " << x.vertexTruth;
    os << " " << x.mdcTruth;
    os << " " << x.mdcDigi;
    os << " " << x.tofTruth;
    os << " " << x.tofDigi;
    os << " " << x.emcTruth;
    os << " " << x.emcDigi; 
    os << " " << x.mucTruth;
    os << " " << x.mucDigi; 
    }
    os << std::endl << "} EVENT" << std::endl;
    return os;
}

