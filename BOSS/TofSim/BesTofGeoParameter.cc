//This file reads the data from the TOF. For me the function ReadData is the important one. Here the standard data from the TOF is read

//      BOOST --- BESIII Object_Oriented Simulation Tool               //
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------//
//$ID: BesTofGeoParameter.cc

#include "BesTofGeoParameter.hh"
#include <fstream>
#include <strstream>
#include <string>
#include <vector>
#include "ReadBoostRoot.hh"

BesTofGeoParameter* BesTofGeoParameter::m_instance = 0;

BesTofGeoParameter* BesTofGeoParameter::GetInstance()
{
    if (m_instance == 0){
        m_instance = new BesTofGeoParameter;
    }
    return m_instance;
}

//Construchttor --> Initialize the path, where data will be read from
BesTofGeoParameter::BesTofGeoParameter()
{
    UNIT["mm"]=1.;
    UNIT["cm"]=10.;
    UNIT["rag"]=1.;
    UNIT["deg"]=(acos(-1))/180.;

    m_dataPath = getenv("TOFSIMROOT");
    if (!m_dataPath){
        G4Exception("BOOST environment not set!");
    }
    ReadData();
    ReadBrData();
    ReadMrpcData();
}

BesTofGeoParameter::~BesTofGeoParameter()
{
}

void BesTofGeoParameter::ReadBrData()
{
    // read file
    const G4String pmtDataPath = m_dataPath + "/dat/TofBr.txt";

    std::ifstream fin(pmtDataPath);
    if (!fin) {
        std::cerr << "cannot open " << pmtDataPath << " for initialization.";
        exit(-1);
    }

    using std::string;
    using std::vector;
    string tempString;
    string::size_type pre_position = 0;
    string::size_type post_position = 0;
    vector<string> stringVector;
    while (getline(fin, tempString, '\n')){

        // remove comments
        pre_position = tempString.find_first_of("#");
        if (pre_position != string::npos){  // there is no "#" in string
            tempString.erase(tempString.begin() + pre_position, tempString.end());
        }

        // format string
        pre_position = 0;
        post_position = 0;
        int size;
        while (tempString.find_first_of(" ", pre_position) != string::npos){
            pre_position = tempString.find_first_of(" ", pre_position);
            post_position = tempString.find_first_not_of(" ", pre_position);
            size = post_position - pre_position;
            tempString.replace(pre_position, size, " ");
            pre_position = post_position - size + 1;
        }

        // insert into string vector
        if (tempString.size() > 10){
            stringVector.push_back(tempString);
        }
    }

    // for tests, read from string, write to variables
    int scinNb;
    double tempEPMTgain;
    double tempERiseTime;
    double tempWPMTgain;
    double tempWRiseTime;
    double tempAtten;
    vector<string>::iterator it = stringVector.begin();
    for (it; it != stringVector.end(); it++){
        // string 2 buffer, buffer 2 temp Variables
        std::istrstream buff((*it).c_str(), strlen((*it).c_str()));
        buff >> scinNb >> tempEPMTgain >> tempERiseTime >> tempWPMTgain >> tempWRiseTime >> tempAtten;

        m_BrEPMTgain[scinNb]  = tempEPMTgain;
        m_BrERiseTime[scinNb] = tempERiseTime;
        m_BrWPMTgain[scinNb]  = tempWPMTgain;
        m_BrWRiseTime[scinNb] = tempWRiseTime;
        m_atten[scinNb]       = tempAtten;

    }
    fin.close();
}

void BesTofGeoParameter::ReadData()
{
    //G4String GeometryPath = ReadBoostRoot::GetBoostRoot();
    const G4String GeometryPath = m_dataPath + "/dat/BesTof.txt";

    std::ifstream fin;
    fin.open(GeometryPath);

    const int maxCharOfOneLine=255;
    char temp[maxCharOfOneLine],*p;
    int lineNo=0,inputNo=0;
    while (fin.peek()!=EOF)  //Peek: Reads and returns the next character without extracting it, i.e. leaving it as the next character to be extracted from the stream.
    {
        fin.getline(temp,maxCharOfOneLine);
        p=temp;
        lineNo++;
        while (*p!='\0')
        {
            if (*p=='#')
            {
                *p='\0';  //delete the comments.
                break;
            }
            p++;
        }
        p=temp; //reset the pointer to the beginning of the string.
        while (*p==' '||*p=='\t')p++;
        if (*p=='\0')continue;
        inputNo++;
        switch (inputNo)
        {
            case 1:
                std::istrstream(p)>>nScinBr>>br1L>>br1TrapW1>>br1TrapW2>>br1TrapH>>br1R1>>AlThickness>>PVFThickness;
                break;
            case 2:
                std::istrstream(p)>>br2L>>br2TrapW1>>br2TrapW2>>br2TrapH>>br2R1;
                break;
            case 3:
                std::istrstream(p)>>bucketDBr>>bucketLBr;
                break;
            case 4:
                std::istrstream(p)>>nScinEc>>ecL>>ecTrapW1>>ecTrapW2>>ecTrapH>>ecTrapH1
                    >>zPosEastEc>>zPosWestEc>>ecR1>>ecR2;
                break;
            case 5:
                std::istrstream(p)>>bucketDEc>>bucketLEc>>bucketPosR;
                break;
            case 6:
                std::istrstream(p)>>m_tau1>>m_tau2>>m_tau3>>m_tauRatio>>m_refIndex>>m_phNConst>>m_Cpe2pmt>>m_rAngle>>m_QE>>m_CE>>m_peCorFac;
            case 7:
                std::istrstream(p)>>m_ttsMean>>m_ttsSigma>>m_PMTgain>>m_Ce>>m_riseTime>>m_LLthresh>>m_HLthresh>>m_preGain>>m_noiseSigma;
            case 8:
                std::istrstream(p)>>m_tau1Ec>>m_tau2Ec>>m_tau3Ec>>m_tauRatioEc>>m_refIndexEc>>m_phNConstEc>>m_Cpe2pmtEc>>m_rAngleEc>>m_QEEc>>m_CEEc>>m_peCorFacEc>>m_attenEc;
            case 9:
                std::istrstream(p)>>m_ttsMeanEc>>m_ttsSigmaEc>>m_PMTgainEc>>m_CeEc>>m_riseTimeEc>>m_LLthreshEc>>m_HLthreshEc>>m_preGainEc>>m_noiseSigmaEc;
        }
    }

    fin.close();
}



void BesTofGeoParameter::ReadMrpcData()
{
    //G4String GeometryPath = ReadBoostRoot::GetBoostRoot(); 
    G4String MrpcGeometryPath = m_dataPath + "/dat/BesTofMrpc.txt";
    const int maxCharOfOneLine=255;
    char temp[maxCharOfOneLine],*p;
    container.clear();

    std::string key;
    double val;
    std::string unit;
    std::ifstream fin;
    fin.open(MrpcGeometryPath);

    while (fin.peek()!=EOF)  //Peek: Reads and returns the next character without extracting it, i.e. leaving it as the next character to be extracted from the stream.
    {
        fin.getline(temp,maxCharOfOneLine);
        p=temp;
        while (*p!='\0')
        {
            if (*p=='#')
            {
                *p='\0';  //delete the comments.                                                                                                                                                                
                break;
            }
            p++;
        }
        p=temp; //reset the pointer to the beginning of the string.                                                                                                                                             
        while (*p==' '||*p=='\t')p++;
        if (*p=='\0')continue;

        std::stringstream ss(temp);
        ss >> key >> val>>unit;
        container[key]=val*UNIT[unit];
    }
    fin.close();


    //Adjusting parameters
    container2.clear();
    double rOffset, angle, angleOffset;
    VEC v_rOffset, v_angle, v_angleOffset;
    int lineNo = 0;
    G4String MrpcGeometryPath1 = m_dataPath + "/dat/Mrpc_adjust.txt";
    fin.open(MrpcGeometryPath1);

    while (fin.peek()!=EOF)
    {
        fin.getline(temp,maxCharOfOneLine);
        p=temp;
        while (*p!='\0')
        {
            if (*p=='#')
            {
                *p='\0';  //delete the comments.                                                                                                                                                                
                break;
            }
            p++;
        }
        p=temp; //reset the pointer to the beginning of the string.                                                                                                                                             
        while (*p==' '||*p=='\t')p++;
        if (*p=='\0')continue;
        lineNo++;

        std::stringstream ss(temp);
        std::stringstream name;
        ss >> rOffset >> angle >> angleOffset;
        if(lineNo==37)
        {
            container2["rOffset_east"] = v_rOffset;
            container2["angle_east"] = v_angle;
            container2["angleOffset_east"] = v_angleOffset;

            v_rOffset.clear();
            v_angle.clear();
            v_angleOffset.clear();
        }

        v_rOffset.push_back(rOffset);
        v_angle.push_back(angle);
        v_angleOffset.push_back(angleOffset);
    }
    container2["rOffset_west"] = v_rOffset;
    container2["angle_west"] = v_angle;
    container2["angleOffset_west"] = v_angleOffset;

    fin.close();


    ////Print out the map
    //for(CONTAINER::iterator it=container.begin(); it!=container.end(); ++it)
    //{
    //    std::cout<<it->first<<"   "<<it->second<<std::endl;
    //}

    //for(CONTAINER2::iterator it=container2.begin(); it!=container2.end(); ++it)
    //{
    //    std::cout<<it->first<<": "<<std::endl;
    //    for(VEC::iterator it2 = (it->second).begin(); it2!=(it->second).end(); it2++)
    //    {
    //        std::cout << *it2 << std::endl;
    //    }
    //}

    int tofid, strip, end;
    for(int i=0; i<72; i++)
    {
        for(int j=0; j<12; j++)
        {
            m_deadChannel[i][j] = -999;
        }
    }

    G4String MrpcGeometryPath2 = m_dataPath + "/dat/BesTofMrpc_dead.txt";
    fin.open(MrpcGeometryPath2);

    while (fin.peek()!=EOF)
    {
        fin.getline(temp,maxCharOfOneLine);
        p=temp;
        while (*p!='\0')
        {
            if (*p=='#')
            {
                *p='\0';  //delete the comments.                                                                                                                                                                
                break;
            }
            p++;
        }
        p=temp; //reset the pointer to the beginning of the string.                                                                                                                                             
        while (*p==' '||*p=='\t')p++;
        if (*p=='\0')continue;

        std::stringstream ss(temp);
        ss >> tofid >> strip >> end;
        m_deadChannel[tofid][strip] = end;
        std::cout<<"deadC["<<tofid<<"]["<<strip<<"]= "<<m_deadChannel[tofid][strip]<<std::endl;
    }

    fin.close();


}

double BesTofGeoParameter::Get(std::string key)
{
    for(CONTAINER::iterator it=container.begin(); it!=container.end(); ++it)
    {
        if((it->first)==key)
        {
            double val = container[key];
            return val;
        }
    }

    std::cout<<"!!!! Wrong! Please check the value name !!!!"<<std::endl;
    return -999.9;
}

VEC BesTofGeoParameter::GetVec(std::string key)
{
    VEC vec;
    for(CONTAINER2::iterator it=container2.begin(); it!=container2.end(); ++it)
    {
        if((it->first)==key)
        {
            vec = container2[key];
            return vec;
        }
    }

    std::cout<<"!!!! Wrong! Please check the value name !!!!"<<std::endl;
    return vec;
}

void BesTofGeoParameter::Get_deadChannel(int deadChannel[72][12])
{
    for(int i=0; i<72; i++)
    {
        for(int j=0; j<12; j++)
        {
            deadChannel[i][j] = m_deadChannel[i][j];
        }
    }
}

