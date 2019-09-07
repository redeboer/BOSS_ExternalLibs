#include "GaudiKernel/Algorithm.h"
#include "MdcGeomSvc/MdcGeomSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CalibData/CalibModel.h"
#include "CalibData/Mdc/MdcAlignData.h"
#include "GaudiKernel/MsgStream.h"
#include "EventModel/EventHeader.h"
#include "EventModel/Event.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include <fstream>
#include <float.h>



// initialize static members
bool MdcGeomSvc::m_doSag = true;
bool MdcGeomSvc::m_readAlignParDataBase = true;
bool MdcGeomSvc::m_nomcalignment=true;

MdcGeomSvc::MdcGeomSvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc) {
    if(getenv("MDCGEOMSVCROOT")){
        m_alignFilePath = std::string(getenv("MDCGEOMSVCROOT"))+std::string("/share/MdcAlignPar.dat");
        //std::cout<<" the MDC alignment file: "<<m_alignFilePath<<std::endl;

        m_wirePosFilePath = std::string(getenv("MDCGEOMSVCROOT"))+std::string("/share/WirePosCalib.dat");
        //std::cout<<" the MDC wire position  file: "<<m_wirePosFilePath<<std::endl;

        m_wireTensionFilePath = std::string(getenv("MDCGEOMSVCROOT"))+std::string("/share/mdcWireTension.dat");
        //std::cout<<" the MDC wire tension file: "<<m_wireTensionFilePath<<std::endl;

    }
    else {
        std::cout<<"A fatal error, contact wangjk..."<<std::endl;
    }

    declareProperty("doSag",      m_doSag = true);
    declareProperty("readAlignParDataBase", m_readAlignParDataBase = true);
    declareProperty("mcnoalignment",m_nomcalignment=true);
    declareProperty("wholeShiftX",m_wholeShiftX = 0.);
    declareProperty("wholeShiftY",m_wholeShiftY = 0.);
    declareProperty("wholeShiftZ",m_wholeShiftZ = 0.);
    declareProperty("wholeRotatX",m_wholeRotatX = 0.);
    declareProperty("wholeRotatY",m_wholeRotatY = 0.);
    declareProperty("wholeRotatZ",m_wholeRotatZ = 0.);
    declareProperty("alignFilePath",m_alignFilePath);
    declareProperty("wirePosFilePath",m_wirePosFilePath);
    declareProperty("wireTensionFilePath",m_wireTensionFilePath);
    

}

StatusCode MdcGeomSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ){

    if ( IID_IMdcGeomSvc.versionMatch(riid) ) { 
        *ppvInterface = static_cast<IMdcGeomSvc*> (this); 
    } else { 
        return Service::queryInterface(riid, ppvInterface) ; 
    }
    return StatusCode::SUCCESS;
}

StatusCode MdcGeomSvc::initialize ( ) {
    MsgStream log(messageService(), name());
    log << MSG::INFO << name() << ": Start of run initialisation" << endreq;

    StatusCode sc = Service::initialize();
    if ( sc.isFailure() ) return sc;
    m_mindex=0; 
    m_updataalign = false;
    IIncidentSvc* incsvc;
    sc = service("IncidentSvc", incsvc);
    int priority = 100;
    if( sc.isSuccess() ){
        incsvc -> addListener(this, "NewRun", priority);
    }

    //    ReadFilePar(); //  get geometry data from file SimUtil/dat/Mdc.txt
    // Fill();  //  get geometry data from Database

    sc = service("CalibDataSvc", m_pCalibDataSvc, true);

    if ( !sc.isSuccess() ) {
        log << MSG::ERROR 
            << "Could not get IDataProviderSvc interface of CalibXmlCnvSvc" 
            << endreq;
        return sc;
    } else {
        log << MSG::DEBUG 
            << "Retrieved IDataProviderSvc interface of CalibXmlCnvSvc" 
            << endreq;
    }
     ReadFilePar(); 
    return StatusCode::SUCCESS;
}

StatusCode MdcGeomSvc::finalize ( ) {
    MsgStream log(messageService(), name());
    log << MSG::INFO << name() << ": End of Run" << endreq;
    return StatusCode::SUCCESS;
}

MdcGeomSvc::~MdcGeomSvc(){
    for(vector<MdcGeoLayer*>::iterator it1 = fLayers.begin(); it1 != fLayers.end(); it1++) delete *it1;
    for(vector<MdcGeoSuper*>::iterator it2 = fSupers.begin(); it2 != fSupers.end(); it2++) delete *it2;
    for(vector<MdcGeoWire*>::iterator it3 = fWires.begin(); it3 != fWires.end(); it3++) delete *it3;
    for(vector<MdcGeoEnd*>::iterator it4 = fEnd.begin(); it4 != fEnd.end(); it4++) delete *it4;  
    fGenerals.clear();
    fWires.clear();
    fLayers.clear();
    fSupers.clear();
    fEnd.clear();  
}

void MdcGeomSvc::clean(){
    for(vector<MdcGeoLayer*>::iterator it1 = fLayers.begin(); it1 != fLayers.end(); it1++) delete *it1;
    for(vector<MdcGeoSuper*>::iterator it2 = fSupers.begin(); it2 != fSupers.end(); it2++) delete *it2;
    for(vector<MdcGeoWire*>::iterator it3 = fWires.begin(); it3 != fWires.end(); it3++) delete *it3;
    for(vector<MdcGeoEnd*>::iterator it4 = fEnd.begin(); it4 != fEnd.end(); it4++) delete *it4;
    fGenerals.clear();
    fWires.clear();
    fLayers.clear();
    fSupers.clear();
    fEnd.clear();
}


void MdcGeomSvc::ReadFilePar(){
    std::string geometryFilePath = getenv("MDCSIMROOT");
    geometryFilePath += "/dat/Mdc.txt";
    std::ifstream inFile(geometryFilePath.c_str() );

    if(!inFile.good()){
        std::cout<<"Error, mdc parameters file not exist"<<std::endl; 
        return;
    }
    std::string alignFilePath;
    std::string wirePosFilePath;
    std::string wireTensionFilePath;
    if (!m_updataalign) {
        alignFilePath = std::string(getenv("MDCGEOMSVCROOT"))+std::string("/share/MdcAlignPar.dat");
        wirePosFilePath = std::string(getenv("MDCGEOMSVCROOT"))+std::string("/share/WirePosCalib.dat");
        wireTensionFilePath = std::string(getenv("MDCGEOMSVCROOT"))+std::string("/share/mdcWireTension.dat");
    } else {
        alignFilePath = m_alignFilePath;
        wirePosFilePath = m_wirePosFilePath;
        wireTensionFilePath = m_wireTensionFilePath;
    }
    std::ifstream alignFile(alignFilePath.c_str()); 
    std::ifstream wirePosFile(wirePosFilePath.c_str());
    std::ifstream wireTensionFile(wireTensionFilePath.c_str());

    if(!wirePosFile){
        std::cout<<"Error, mdc wire position file not exist..."<<std::endl;
    }

    if(!wireTensionFile){
        std::cout<<"Error, mdc wire tension file not exist..."<<std::endl;
    }

      std::vector<double> wireTensionVec;
    //cout << __FILE__ << "    " << __LINE__ << "  readdb = " << m_readAlignParDataBase
    //    << "   update align = " << m_updataalign << endl;
    if (m_readAlignParDataBase && m_updataalign) {
        ReadTensionDataBase(wireTensionVec);
    }else {
        cout << "Read wireTension from file:" << wireTensionFilePath.c_str() << endl;
        int idd;
        double tension;
        for(int ii=0; ii<6796; ii++){
            wireTensionFile>>idd>>tension;

            if(getSagFlag()){
                wireTensionVec.push_back(tension);
            }
            else { 
                tension = DBL_MAX;
                wireTensionVec.push_back(tension);
            }
        }
    }

    std::vector<vector<double> > wirePosVec(6796);
    if (m_readAlignParDataBase && m_updataalign) {
        ReadWirePosDataBase(wirePosVec);
    } else {
        cout << "Read wirePosition from file:" << wirePosFilePath.c_str() << endl;
        double dxe,dye,dze,dxw,dyw,dzw;
        int wid;
        std::string title; 
        getline(wirePosFile, title);
        wirePosFile.seekg(1,ios::cur);
        for(int j=0; j<6796; j++){
            wirePosFile>>wid>>dxe>>dye>>dze>>dxw>>dyw>>dzw; 
            wirePosVec[j].push_back(dxe);
            wirePosVec[j].push_back(dye);
            wirePosVec[j].push_back(dze);
            wirePosVec[j].push_back(dxw);
            wirePosVec[j].push_back(dyw);
            wirePosVec[j].push_back(dzw);
            /// test
            /*
               for(int jj=0;jj<6;jj++){
               std::cout<<" wid: "<<wid<<" dxe: "<<wirePosVec[j][0]<<" dye: "<<wirePosVec[j][1]<<" dze: "<<wirePosVec[j][2]<<" dxw: "<<wirePosVec[j][3]<<" dyw: "<<wirePosVec[j][4]<<" dzw: "<<wirePosVec[j][5]<<std::endl;
               }
               */
        }
    }

    double signalWireR, fieldWireR;
    int TLayerNo, TWireNo,TSignalLayerNo,fSignalLayer[50];  
    int i,wireNo, firstWire,segmentNo,signalLayer;
    double length, nomphi, phi, r,nomaadiv2, aadiv2,
           nomaa, aa, nomrotateCell, rotateCell, wlength, innerR, outR, z;
    std::string layer, name, line;

    vector<int> WireNo, fSegmentNo;
    vector<double> wLength, R, nomPhi, Phi,nomadiv2, adiv2, First, 
        noma, a, nomebusing, ebusing, nomsigma, sigma, nomdeltaz, deltaz,
        nomRotate, Rotate, fLength, fInnerR, fOutR, fZ;
    vector<string> LayerName, fName;  
    vector<double> Sx,Sy,Sz,Rx,Ry,Rz;
    double tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;

    /// read alignment file

    if (m_readAlignParDataBase && m_updataalign) {
        ReadAliParDataBase(Sx, Sy, Sz, Rx, Ry, Rz);
    } else {
        cout << "Read alignment parameters from file:" << alignFilePath.c_str() << endl;
        getline(alignFile, line);
        alignFile.seekg(1,ios::cur);
        for(int k=0;k<16;k++){
            alignFile>>line>>tmp1>>tmp2>>tmp3>>tmp4>>tmp5>>tmp6;   
            //changed by luot 10.03.12
            //if (!m_updataalign) {
            //    tmp1 = 0.0;
            //    tmp2 = 0.0;
            //    tmp3 = 0.0;
            //    tmp4 = 0.0;
            //    tmp5 = 0.0;
            //    tmp6 = 0.0;
            //}
            Sx.push_back(m_wholeShiftX+tmp1);
            Sy.push_back(m_wholeShiftY+tmp2);
            Sz.push_back(m_wholeShiftZ+tmp3);
            Rx.push_back(m_wholeRotatX+tmp4);
            Ry.push_back(m_wholeRotatY+tmp5);
            Rz.push_back(m_wholeRotatZ+tmp6);
        }
        /// test the service 
        /// for(int m=0;m<16;m++)  std::cout<<" Rz["<<m<<"] is: "<<Rz[m]<<std::endl;
    }

    getline(inFile, line);
    inFile>>TLayerNo>>TWireNo>>TSignalLayerNo>>signalWireR>>fieldWireR;
    inFile.seekg(1,ios::cur);
    getline(inFile, line);
    for( i=0; i<TSignalLayerNo; i++){
        inFile>>signalLayer;
        /// signalLayer is the index of this signalLayer in all the layers   
        fSignalLayer[i]=signalLayer-1;
    }

    inFile.seekg(1,ios::cur);
    getline(inFile, line);
    getline(inFile, line);

    int i_east,i_west;   
    double delta_rotateCell;
    /// in this loop we read the file and store them into according vectors 
    for( i=0; i<TLayerNo; i++){
        i_east=getAlignParIndexEast(i);
        i_west=getAlignParIndexWest(i); 

        inFile>>layer>>wireNo>>wlength>>r>>nomphi>>firstWire>>nomrotateCell;
        getline(inFile, line);

        /// wireNo is the total wire number of this layer, including both signal
        /// wire& field wire
        nomaadiv2=2*pi*nomrotateCell/wireNo; 
        //aadiv2=2*pi*rotateCell/wireNo+0.5*(Rz[i_west]-Rz[i_east]);
        nomaa=2*r*sin(nomaadiv2);
        delta_rotateCell=(Rz[i_west]-Rz[i_east])*wireNo/(4*pi);
        rotateCell=nomrotateCell+delta_rotateCell;
        aadiv2=2*pi*rotateCell/wireNo;
        aa=2*r*sin(aadiv2);

        double shiftPhi=twopi/wireNo;
        nomphi*=deg;
        if(nomphi<0) nomphi += shiftPhi; //By definition, phi of first wire must >=0
        phi=nomphi+Rz[i_east];
        LayerName.push_back(layer);
        WireNo.push_back(wireNo);
        wLength.push_back(wlength);
        R.push_back(r);
        nomPhi.push_back(nomphi);
        Phi.push_back(phi);
        //nomadiv2.push_back(nomaadiv2);
        //adiv2.push_back(aadiv2);
        First.push_back(firstWire);
        //noma.push_back(nomaa);
        a.push_back(aa);
        /// CLHEP says that:  
        /// static const double radian      = 1.;
        /// static const double degree = (3.14159265358979323846/180.0)*radian
        /// static const double deg  = degree;

        nomebusing.push_back(atan(nomaa/wlength));
        ebusing.push_back(atan(aa/wlength));

        //if(aa==0.0) sigma.push_back(0.0);
        //else  sigma.push_back(0.13*wlength/aa);
        //nomdeltaz.push_back(r*(1.0-cos(nomaadiv2)));
        //deltaz.push_back(r*(1.0-cos(aadiv2)));
        nomRotate.push_back(nomrotateCell);
        Rotate.push_back(rotateCell);
    }

    getline(inFile, line);
    inFile>>segmentNo;  
    inFile.seekg(1,ios::cur);
    getline(inFile, line);
    getline(inFile, line);

    for(i=0; i<segmentNo; i++){
        inFile>>length>>innerR>>outR>>z>>name;
        getline(inFile,line);
        fLength.push_back(length);
        fInnerR.push_back(innerR);
        fOutR.push_back(outR);
        fZ.push_back(z);
        fName.push_back(name);
    }

    /// Variables used here are as follows,
    /// MdcGeoGeneral hlh; 
    /// MdcGeoSuper* suph;
    /// MdcGeoLayer* lh;
    /// MdcGeoWire* wh;
    /// MdcGeoEnd* end;

    //MdcGeoGeneral include signal wire and field wire
    MdcGeoGeneral hlh;
    double a1,a2,a3,nomphi0,phi0,cellphi;
    double noma1,noma2,noma3;
    for(i=0;i<TLayerNo;i++){
        i_east=getAlignParIndexEast(i);
        i_west=getAlignParIndexWest(i);
        hlh.Id(i);
        hlh.LayerName(LayerName[i]);    
        hlh.Radius(R[i]);
        hlh.Length(wLength[i]);
        hlh.NCell(WireNo[i]/2);
        //east endcap rotates away from designed position by Phi considering misalignment,
        //nomPhi without considering misalignment 
        hlh.Offset(Phi[i]);
        hlh.nomOffset(nomPhi[i]);
        hlh.Phi(Phi[i]);    
        hlh.nomPhi(nomPhi[i]);
        hlh.First((int)First[i]); 
        HepPoint3D offF(Sx[i_west],Sy[i_west],Sz[i_west]); 
        HepPoint3D offB(Sx[i_east],Sy[i_east],Sz[i_east]);
        hlh.OffF(offF);
        hlh.OffB(offB);
        hlh.TwistF(Rz[i_west]);
        hlh.TwistB(Rz[i_east]);
        //west endcap rotates away from designed position by Rotate considering misalignment,
        //nomRotate without considering misalignment 
        hlh.Shift(Rotate[i]);
        hlh.nomShift(nomRotate[i]);
        hlh.SxEast(Sx[i_east]);
        hlh.SxWest(Sx[i_west]);
        hlh.SyEast(Sy[i_east]);
        hlh.SyWest(Sy[i_west]);
        hlh.SzEast(Sz[i_east]);
        hlh.SzWest(Sz[i_west]);

        hlh.RxEast(Rx[i_east]);
        hlh.RxWest(Rx[i_west]);
        hlh.RyEast(Ry[i_east]);
        hlh.RyWest(Ry[i_west]);
        hlh.RzEast(Rz[i_east]);
        hlh.RzWest(Rz[i_west]);
        fGenerals.push_back(hlh);
    }

    MdcGeoSuper *suph;
    for(i=0;i<TSignalLayerNo;i++){
        //MdcGeoLayer only represent signal wires
        MdcGeoLayer *lh = new MdcGeoLayer();
        // lh is a signalwire layer 
        int lyr=fSignalLayer[i];
        // the value of lyr starts from 1,because the first layer of MDC is field
        // wire
        i_east=getAlignParIndexEast(lyr);
        i_west=getAlignParIndexWest(lyr);
        lh->SLayer(lyr+1);  
        if(i%4==0){
            suph = new MdcGeoSuper();
            suph->LowerR(R[lyr-1]);
            if(i==40){
                suph->UpperR(R[lyr+5]);
            }
            else {
                suph->UpperR(R[lyr+7]);
            }
            switch(i/4){
                case 0: suph->Type(-1); break;
                case 1: suph->Type( 1); break;
                case 2: suph->Type( 0); break;
                case 3: suph->Type( 0); break;
                case 4: suph->Type( 0); break;
                case 5: suph->Type(-1); break;
                case 6: suph->Type( 1); break;
                case 7: suph->Type(-1); break;
                case 8: suph->Type( 1); break;
                case 9: suph->Type( 0); break;
                case 10: suph->Type(0); break;
                default: break;
            }
            suph->Id(i/4);
            fSupers.push_back(suph);
        }
        /// attention, it is 2*twopi means 4*pi
        cellphi=2*twopi/WireNo[lyr];
        /// calculate the phi angle of the sense wire of the first cell of the layer 
        phi0=Phi[lyr]+abs(First[lyr]-1)*(cellphi/2);
        nomphi0=nomPhi[lyr]+abs(First[lyr]-1)*(cellphi/2);
        lh->Id(i);
        int wircount=0;
        for(int wk=0; wk<i; wk++){
            int lyrwk=fSignalLayer[wk];
            wircount=wircount + WireNo[lyrwk]/2;
        };
        /// Wirst is the the global number of the first signal wire of this layer
        /// in all the signal wire
        lh->Wirst(wircount);
        lh->Slant(ebusing[lyr]);
        lh->nomSlant(nomebusing[lyr]);
        lh->Radius(R[lyr]);
        lh->Length(wLength[lyr]);
        lh->RCSiz1(R[lyr]-R[lyr-1]);
        lh->RCSiz2(R[lyr+1]-R[lyr]);
        lh->PCSiz(R[lyr]*cellphi);
        lh->NCell(WireNo[lyr]/2);
        lh->Offset(phi0);
        lh->Shift(Rotate[lyr]);
        lh->nomOffset(nomphi0);
        lh->nomShift(nomRotate[lyr]);

        vector<MdcGeoSuper*>::iterator it1 = fSupers.end()-1;
        lh->Sup(* it1);
        fLayers.push_back(lh);
        int geo = -1;
        if (i < 8) {
            geo = i * 2 + 1;
        } else if (i < 20) {
            geo = i * 2 + 2;
        } else if (i < 36) {
            geo = i * 2 + 3;
        } else {
            geo = i * 2 + 4;
        } 
        lh->Gen(geo);

        for(int j=0;j<WireNo[lyr]/2;j++){
            /// wh is signal wire
            MdcGeoWire *wh = new MdcGeoWire();
            const int wireId = wircount+j;
            wh->Id(wireId);

            /// caluculate misaligned wire position
            a1 = R[lyr]*cos(phi0+j*cellphi)+Sx[i_east]+wirePosVec[wireId][0];
            a2 = R[lyr]*sin(phi0+j*cellphi)+Sy[i_east]+wirePosVec[wireId][1];
            a3 = wLength[lyr]/2+wirePosVec[wireId][2]+Sz[i_east];
            //a3 = wLength[lyr]/2+wirePosVec[wireId][2];
            //MDC rotate around x firstly;
            double xb = a1;
            double yb = a2 * cos(Rx[i_east]) + a3 * sin(Rx[i_east]);
            double zb = a3 * cos(Rx[i_east]) - a2 * sin(Rx[i_east]);

            //then MDC rotate around y;
            double xbnew = xb * cos(Ry[i_east]) - zb * sin(Ry[i_east]);
            double ybnew = yb;
            double zbnew = xb * sin(Ry[i_east]) + zb * cos(Ry[i_east]);
            /// 
            noma1 = R[lyr]*cos(nomphi0+j*cellphi);
            noma2 = R[lyr]*sin(nomphi0+j*cellphi);
            /// attention wLength[lyr] not the true length of the wire
            noma3 = wLength[lyr]/2;
            //HepPoint3D pb(a1,a2,a3);
            HepPoint3D pb(xbnew,ybnew,zbnew);
            HepPoint3D nompb(noma1,noma2,noma3);
            HepPoint3D wireposb(wirePosVec[wireId][0],wirePosVec[wireId][1],wirePosVec[wireId][2]);
            wh->Backward(pb);
            wh->nomBackward(nompb);
            wh->BWirePos(wireposb);

            //double g1 = noma1*cos(Rz[i_east]) - noma2*sin(Rz[i_east]) + Sx[i_east] + wirePosVec[wireId][0];
            //double g2 = noma2*cos(Rz[i_east]) + noma1*sin(Rz[i_east]) + Sy[i_east] + wirePosVec[wireId][1];
            //double g3 = noma3 + wirePosVec[wireId][2];
            //HepPoint3D backward_test(g1,g2,g3);

            a1 = R[lyr]*cos(phi0+(j+lh->Shift())*cellphi)+Sx[i_west]+wirePosVec[wireId][3];
            a2 = R[lyr]*sin(phi0+(j+lh->Shift())*cellphi)+Sy[i_west]+wirePosVec[wireId][4];
            //a3 = -wLength[lyr]/2+wirePosVec[wireId][5];
            a3 = -wLength[lyr]/2+wirePosVec[wireId][5]+Sz[i_west];
            //MDC rotate around x firstly;
            double xf = a1;
            double yf = a2 * cos(Rx[i_west]) + a3 * sin(Rx[i_west]);
            double zf = a3 * cos(Rx[i_west]) - a2 * sin(Rx[i_west]);

            //then MDC rotate around y;
            double xfnew = xf * cos(Ry[i_west]) - zf * sin(Ry[i_west]);
            double yfnew = yf;
            double zfnew = xf * sin(Ry[i_west]) + zf * cos(Ry[i_west]);

            noma1 = R[lyr]*cos(nomphi0+(j+lh->nomShift())*cellphi);
            noma2 = R[lyr]*sin(nomphi0+(j+lh->nomShift())*cellphi);
            noma3 = -wLength[lyr]/2;
            //HepPoint3D pf(a1,a2,a3);
            HepPoint3D pf(xfnew, yfnew, zfnew);
            HepPoint3D nompf(noma1,noma2,noma3);
            HepPoint3D wireposf(wirePosVec[wireId][3],wirePosVec[wireId][4],wirePosVec[wireId][5]);
            wh->Forward(pf);
            wh->nomForward(nompf);
            wh->FWirePos(wireposf);

            //double f1 = noma1*cos(Rz[i_west]) - noma2*sin(Rz[i_west]) + Sx[i_west] + wirePosVec[wireId][3];
            //double f2 = noma2*cos(Rz[i_west]) + noma1*sin(Rz[i_west]) + Sy[i_west] + wirePosVec[wireId][4];
            //double f3 = noma3 + wirePosVec[wireId][5];
            //HepPoint3D forward_test(f1,f2,f3);
            /// test
            /// std::cout<<" wireId: "<<wireId<<" backward position:"<< wh->Backward()<<" forward position: "<<wh->Forward()<<std::endl;
            wh->Layer(i);
            wh->Cell(j);
            wh->Stat(0);
            wh->Slant(ebusing[lyr]);
            wh->nomSlant(nomebusing[lyr]);

            /// set the tension and wire sag
            wh->Tension(wireTensionVec[wireId]);
            //std::cout<<"  wh->Tension: "<< wh->Tension()<<std::endl;
            //std::cout<<" wire sag: "<<wh->Sag(wireId)<<std::endl;

            /// because of just before have fLayers.push_back(lh); 
            /// so use it2 = fLayers.end()-1 to get the MdcGeoLayer
            /// which have just push_back into the stack
            vector<MdcGeoLayer*>::iterator it2 = fLayers.end()-1;
            wh->Lyr(* it2);
            fWires.push_back(wh);
        }
    }

    fMisc.OuterR(fOutR[1]);
    fMisc.InnerR(fInnerR[2]);
    fMisc.OuterTk(fOutR[1]-fInnerR[1]);
    fMisc.InnerTk(fOutR[2]-fInnerR[2]);
    fMisc.NSWire(fWires.size());
    fMisc.NFWire(TWireNo-fWires.size());

    fMisc.LayerNo(TLayerNo);
    fMisc.WireNo(TWireNo);
    fMisc.SLayerNo(TSignalLayerNo);
    fMisc.SWireR(signalWireR);
    fMisc.FWireR(fieldWireR);

    for(i=0;i<segmentNo;i++){
        MdcGeoEnd * end=new MdcGeoEnd();
        end->Id(i);
        end->Length(fLength[i]);
        end->InnerR(fInnerR[i]);
        end->OutR(fOutR[i]);
        end->Z(fZ[i]);
        end->Name(fName[i]);
        fEnd.push_back(end);
    }
}

void MdcGeomSvc::ReadTensionDataBase(std::vector<double> & wireTensionVec){
    std::string fullPath = "/Calib/MdcAlign";
    MsgStream log(messageService(), name());
    log << MSG::INFO<<"ReadTensionDataBase() wireTensionPath = "<<fullPath<< endreq;
    cout << "Read wireTension from Calibration Database!" << endl;

    SmartDataPtr<CalibData::MdcAlignData> tension(m_pCalibDataSvc, fullPath);
    if( ! tension){
        log << MSG::ERROR << "can not get MdcAlignConst via SmartPtr" 
            << endreq;
        return;
    }
    for(int i=0;i<6796;i++){
        double tens = tension->gettension(i);
        wireTensionVec.push_back(tens);
    }
}
void MdcGeomSvc::ReadWirePosDataBase(std::vector<vector<double> > & wirePosVec){
    std::string fullPath = "/Calib/MdcAlign";
    MsgStream log(messageService(), name());
    log << MSG::INFO<<"ReadWirePosDataBase() wirePositionPath = "<<fullPath<< endreq;

    cout << "Read wirePosition from Calibration Database!" << endl;
    SmartDataPtr<CalibData::MdcAlignData> wirepos(m_pCalibDataSvc, fullPath);
    if( ! wirepos){
        log << MSG::ERROR << "can not get MdcAlignConst via SmartPtr" 
            << endreq;
        return;
    }
    double dxe,dye,dze,dxw,dyw,dzw;
    for(int j=0; j<6796; j++){

        dxe = wirepos->getdxWireEast(j);
        dye = wirepos->getdyWireEast(j);
        dze = wirepos->getdzWireEast(j);
        dxw = wirepos->getdxWireWest(j);
        dyw = wirepos->getdyWireWest(j);
        dzw = wirepos->getdzWireWest(j);

        wirePosVec[j].push_back(dxe);
        wirePosVec[j].push_back(dye);
        wirePosVec[j].push_back(dze);
        wirePosVec[j].push_back(dxw);
        wirePosVec[j].push_back(dyw);
        wirePosVec[j].push_back(dzw);
    }

}
void MdcGeomSvc::ReadAliParDataBase(vector<double>& Sx, vector<double>& Sy, vector<double>& Sz,
        vector<double>& Rx, vector<double>& Ry, vector<double>& Rz){
    MsgStream log(messageService(), name());
    std::string fullPath = "/Calib/MdcAlign";
    log << MSG::INFO<<"ReadAliParDataBase() alignParPath = "<<fullPath<< endreq;
    cout << "Read alignment parameters from Calibration Database!" << endl;

    SmartDataPtr<CalibData::MdcAlignData> alignpar(m_pCalibDataSvc, fullPath);
    if( ! alignpar){
        log << MSG::ERROR << "can not get MdcAlignConst via SmartPtr" 
            << endreq;
        return;
    }
    double tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;
    for(int k=0;k<16;k++){
        tmp1 = alignpar->getdxEP(k);
        tmp2 = alignpar->getdyEP(k);
        tmp3 = alignpar->getdzEP(k);
        tmp4 = alignpar->getrxEP(k);
        tmp5 = alignpar->getryEP(k);
        tmp6 = alignpar->getrzEP(k);

        Sx.push_back(m_wholeShiftX+tmp1);
        Sy.push_back(m_wholeShiftY+tmp2);
        Sz.push_back(m_wholeShiftZ+tmp3);
        Rx.push_back(m_wholeRotatX+tmp4);
        Ry.push_back(m_wholeRotatY+tmp5);
        Rz.push_back(m_wholeRotatZ+tmp6);
    }

}
const int MdcGeomSvc::getWireSize()
{
    return fWires.size();
}

const int MdcGeomSvc::getLayerSize()
{
    return fLayers.size();
}

const int MdcGeomSvc::getSuperLayerSize()
{
    return fSupers.size();
}

const int MdcGeomSvc::getGeneralLayerSize()
{
    return fGenerals.size();
}

const int MdcGeomSvc::getSegmentNo()
{
    return fEnd.size();
}   

void MdcGeomSvc::Dump(){}

const int MdcGeomSvc::getAlignParIndexEast(int lyr) const{
    /// the Mdc small endplate east
    if((lyr>=0) && (lyr<=16))  return 0;  
    /// the 1th ring east   
    else if((lyr>=17) && (lyr<=20)) return 1;
    /// the 2th ring east
    else if((lyr>=21) && (lyr<=24)) return 2;
    /// the 3th ring east
    else if((lyr>=25) && (lyr<=28)) return 3;
    /// the 4th ring east
    else if((lyr>=29) && (lyr<=32)) return 4;
    /// the 5th ring east
    else if((lyr>=33) && (lyr<=36)) return 5;
    /// the 6th ring east
    else if((lyr>=37) && (lyr<=41)) return 6;
    /// the Mdc big endplate east
    else if(lyr>=42) return 7; 
    else std::cout<<" Hi, ERROR OCCUR !!!"<<std::endl;
    return -1;
}


const int MdcGeomSvc::getAlignParIndexWest(int lyr) const{
    /// the Mdc small endplate west
    if((lyr>=0) && (lyr<=16))  return 8;
    /// the 1th ring west
    else if((lyr>=17) && (lyr<=20)) return 9;
    /// the 2th ring west
    else if((lyr>=21) && (lyr<=24)) return 10;
    /// the 3th ring west
    else if((lyr>=25) && (lyr<=28)) return 11;
    /// the 4th ring west
    else if((lyr>=29) && (lyr<=32)) return 12;
    /// the 5th ring west
    else if((lyr>=33) && (lyr<=36)) return 13;
    /// the 6th ring west
    else if((lyr>=37) && (lyr<=41)) return 14;
    /// the Mdc big endplate west
    else if(lyr>=42) return 15;
    else std::cout<<" Hi, ERROR OCCUR !!!"<<std::endl;
    return -1;
}


/// this handle function is prepared for special use
void MdcGeomSvc::handle(const Incident& inc){
    MsgStream log( messageService(), name() );
    log << MSG::DEBUG << "handle: " << inc.type() << endreq;
    IDataProviderSvc* m_eventSvc;
    Gaudi::svcLocator()->service("EventDataSvc", m_eventSvc, true);
    SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc,"/Event/EventHeader");
    if (!eventHeader) { 
    log << MSG::FATAL << "Could not find Event Header" << endreq;
      }
    if (m_updataalign) return;
    if (inc.type() == "NewRun" ){
        log << MSG::DEBUG << "Begin Event" << endreq;
        clean();
        m_updataalign = true;
    if(m_nomcalignment&&m_mindex==0) {  
        int   RunNo=eventHeader->runNumber();
        if(RunNo<0)  m_readAlignParDataBase=false ;
           else   m_readAlignParDataBase=true;
        m_mindex+=1;
    cout<<"m__RunNo="<<RunNo<<"m_mindex="<<m_mindex<<endl;
        }
   //std::cout<<"############"<<m_readAlignParDataBase<<std::endl;
        ReadFilePar();          
    }
}

const MdcGeoWire * const 
    MdcGeomSvc::Wire(unsigned id){
        if (id < fWires.size())
            return  fWires[id];

        return 0;
    }

const MdcGeoWire * const
    MdcGeomSvc::Wire(unsigned lyrid, unsigned wirid){
        if ((lyrid <fLayers.size()) && ((int) wirid < Layer(lyrid)->NCell()))
            return fWires[Layer(lyrid)->Wirst() + wirid];

        return 0;
    }

const MdcGeoLayer * const 
    MdcGeomSvc::Layer(unsigned id){
        if (id < fLayers.size())
            return  fLayers[id];

        return 0;
    }

const MdcGeoSuper * const 
    MdcGeomSvc::SuperLayer(unsigned id){
        if (id < fSupers.size())
            return  fSupers[id];

        return 0;
    }

const MdcGeoGeneral * const
    MdcGeomSvc::GeneralLayer(unsigned id){
        if (id < fGenerals.size()) 
            return & fGenerals[id];

        return 0;
    }

const MdcGeoMisc * const
MdcGeomSvc::Misc(void){
    return & fMisc;
}

const MdcGeoEnd * const  
    MdcGeomSvc::End(unsigned id){
        if (id < fEnd.size())     
            return  fEnd[id];

        return 0;
    }  


bool MdcGeomSvc::getSagFlag(void) {

    return m_doSag;
}  


