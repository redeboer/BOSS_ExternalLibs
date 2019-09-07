#include <cmath>
#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/KinematicConstraints.h"
#include "VertexFit/HTrackParameter.h"
#include "TStopwatch.h"
#include "TGraph2D.h"

const int KalmanKinematicFit::NTRKPAR = 4;
const int KalmanKinematicFit::Resonance = 1;
const int KalmanKinematicFit::TotalEnergy = 2;
const int KalmanKinematicFit::TotalMomentum = 4;
const int KalmanKinematicFit::Position = 8;
const int KalmanKinematicFit::ThreeMomentum = 16;
const int KalmanKinematicFit::FourMomentum = 32;
const int KalmanKinematicFit::EqualMass = 64;
//TGraph2D *g = new TGraph2D("/ihepbatch/bes/yanl/6.5.0//TestRelease/TestRelease-00-00-62/run/gamma/new/graph.dat");

KalmanKinematicFit * KalmanKinematicFit::m_pointer = 0;

KalmanKinematicFit * KalmanKinematicFit::instance() {
    if(m_pointer) return m_pointer;
    m_pointer = new KalmanKinematicFit();
    return m_pointer;
}

KalmanKinematicFit::KalmanKinematicFit(){;}

KalmanKinematicFit::~KalmanKinematicFit() {
    //       if(m_pointer) delete m_pointer;
    delete m_pointer;
}


void KalmanKinematicFit::init() {
    clearWTrackOrigin();
    clearWTrackInfit();
    clearWTrackList();
    //gamma shape
    clearGammaShape();
    clearGammaShapeList();  
    clearMapkinematic();
    clearMappositionA();
    clearMappositionB();
    clearone();
    cleartwo();
    setBeamPosition(HepPoint3D(0.0,0.0,0.0));
    setVBeamPosition(HepSymMatrix(3,0));
    //=============  
    m_kc.clear();
    m_chisq.clear();
    m_chi = 9999.;
    m_niter = 10;
    m_chicut = 200.;
    m_chiter = 0.005;
    m_espread = 0.0;
    m_collideangle = 11e-3;
    m_flag = 0;
    m_dynamicerror = 0; 
    m_nc = 0;
    m_cpu = HepVector(10, 0);
    m_virtual_wtrk.clear(); 
    m_graph2d = 0;
    //  m_graph2d = TGraph2D("/ihepbatch/bes/yanl/6.5.0//TestRelease/TestRelease-00-00-62/run/gamma/new/graph.dat"); 
}


//
//  Add Resonance
//

void KalmanKinematicFit::AddResonance(int number, double mres, int n1) {
    std::vector<int> tlis = AddList(n1);
    AddResonance(number, mres, tlis);
}

void KalmanKinematicFit::AddResonance(int number, double mres, int n1, int n2) {
    std::vector<int> tlis = AddList(n1, n2);
    AddResonance(number, mres, tlis);
}

void KalmanKinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3) {
    std::vector<int> tlis = AddList(n1, n2, n3);
    AddResonance(number, mres, tlis);
}

void KalmanKinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4);
    AddResonance(number, mres, tlis);
}

void KalmanKinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5);
    AddResonance(number, mres, tlis);
}

void KalmanKinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6);
    AddResonance(number, mres, tlis);
}

void KalmanKinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7);
    AddResonance(number, mres, tlis);
}

void KalmanKinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8);
    AddResonance(number, mres, tlis);
}

void KalmanKinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9);
    AddResonance(number, mres, tlis);
}

void KalmanKinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9, int n10) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);
    AddResonance(number, mres, tlis);
}


void KalmanKinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
    AddResonance(number, mres, tlis);
}

void KalmanKinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11, int n12) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12);
    AddResonance(number, mres, tlis);
}

void KalmanKinematicFit::AddResonance(int number, double mres, std::vector<int> tlis) {
    KinematicConstraints kc;
    HepSymMatrix Vre = HepSymMatrix(1,0);
    kc.ResonanceConstraints(mres, tlis, Vre);
    m_kc.push_back(kc);
    if((unsigned int) number != m_kc.size()-1) 
        std::cout << "wrong kinematic constraints index" << std::endl;
}


//
//  Add TotalMomentum
//

void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1) {
    std::vector<int> tlis = AddList(n1);
    AddTotalMomentum(number, ptot, tlis);
}
void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2) {
    std::vector<int> tlis = AddList(n1, n2);
    AddTotalMomentum(number, ptot, tlis);
}

void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3) {
    std::vector<int> tlis = AddList(n1, n2, n3);
    AddTotalMomentum(number, ptot, tlis);
}

void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4);
    AddTotalMomentum(number, ptot, tlis);
}

void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5);
    AddTotalMomentum(number, ptot, tlis);
}

void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6);
    AddTotalMomentum(number, ptot, tlis);
}

void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7);
    AddTotalMomentum(number, ptot, tlis);
}

void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8);
    AddTotalMomentum(number, ptot, tlis);
}

void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9);
    AddTotalMomentum(number, ptot, tlis);
}

void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);
    AddTotalMomentum(number, ptot, tlis);
}


void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
    AddTotalMomentum(number, ptot, tlis);
}

void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11, int n12) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12);
    AddTotalMomentum(number, ptot, tlis);
}

void KalmanKinematicFit::AddTotalMomentum(int number, double ptot, std::vector<int> tlis) {
    KinematicConstraints kc;
    kc.TotalMomentumConstraints(ptot, tlis);
    m_kc.push_back(kc);
    if((unsigned int) number != m_kc.size()-1) 
        std::cout << "wrong kinematic constraints index" << std::endl;
}

//
//  Add TotalEnergy
//

void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1) {
    std::vector<int> tlis = AddList(n1);
    AddTotalEnergy(number, etot, tlis);
}
void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2) {
    std::vector<int> tlis = AddList(n1, n2);
    AddTotalEnergy(number, etot, tlis);
}

void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3) {
    std::vector<int> tlis = AddList(n1, n2, n3);
    AddTotalEnergy(number, etot, tlis);
}

void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4);
    AddTotalEnergy(number, etot, tlis);
}

void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5);
    AddTotalEnergy(number, etot, tlis);
}

void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6);
    AddTotalEnergy(number, etot, tlis);
}

void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7);
    AddTotalEnergy(number, etot, tlis);
}

void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8);
    AddTotalEnergy(number, etot, tlis);
}

void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9);
    AddTotalEnergy(number, etot, tlis);
}

void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);
    AddTotalEnergy(number, etot, tlis);
}


void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
    AddTotalEnergy(number, etot, tlis);
}

void KalmanKinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11, int n12) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12);
    AddTotalEnergy(number, etot, tlis);
}

void KalmanKinematicFit::AddTotalEnergy(int number, double etot, std::vector<int> tlis) {
    KinematicConstraints kc;
    kc.TotalEnergyConstraints(etot, tlis);
    m_kc.push_back(kc);
    if((unsigned int) number != m_kc.size()-1) 
        std::cout << "wrong kinematic constraints index" << std::endl;
}
//
//  Equal Mass constraints
//

void KalmanKinematicFit::AddEqualMass(int number, std::vector<int> tlis1, std::vector<int> tlis2)  {
    KinematicConstraints kc;
    HepSymMatrix Vne = HepSymMatrix(1,0);
    kc.EqualMassConstraints(tlis1, tlis2, Vne);
    m_kc.push_back(kc);
    if((unsigned int) number != m_kc.size()-1) 
        std::cout << "wrong kinematic constraints index" << std::endl;
}

//
//  Total 3-momentum constraints
//

void KalmanKinematicFit::AddThreeMomentum(int number, Hep3Vector p3) {
    std::vector<int>  tlis;
    tlis.clear();
    WTrackParameter wtrk;
    KinematicConstraints kc;

    for(int i = 0; i < numberWTrack(); i++) {
        tlis.push_back(i);
    }
    kc.ThreeMomentumConstraints(p3, tlis);
    m_kc.push_back(kc);
    if((unsigned int) number != m_kc.size()-1) 
        std::cout << "wrong kinematic constraints index" << std::endl;
}

//
//  Total 4-momentum constraints
//

void KalmanKinematicFit::AddFourMomentum(int number, HepLorentzVector p4) {

    std::vector<int> tlis;
    tlis.clear();
    KinematicConstraints kc;

    for(int i = 0; i < numberWTrack(); i++) {
        tlis.push_back(i);
    }

    HepSymMatrix Vme = HepSymMatrix(4,0);
    Vme[0][0] = 2*m_espread*m_espread*sin(m_collideangle)*sin(m_collideangle);
    Vme[0][3] = 2*m_espread*m_espread*sin(m_collideangle);
    Vme[3][3] = 2*m_espread*m_espread;

    kc.FourMomentumConstraints(p4, tlis, Vme);
    m_kc.push_back(kc);
    if((unsigned int) number != m_kc.size()-1) 
        std::cout << "wrong kinematic constraints index" << std::endl;
}

void KalmanKinematicFit::AddFourMomentum(int number, double etot ){

    HepLorentzVector p4(0.0, 0.0, 0.0, etot);
    std::vector<int> tlis;
    tlis.clear();
    KinematicConstraints kc;

    for(int i = 0; i < numberWTrack(); i++) {
        tlis.push_back(i);
    }
    HepSymMatrix Vme = HepSymMatrix (4,0);
    Vme[3][3] = 2*m_espread*m_espread;
    kc.FourMomentumConstraints(p4, tlis, Vme);
    m_kc.push_back(kc);
    if((unsigned int) number != m_kc.size()-1) 
        std::cout << "wrong kinematic constraints index" << std::endl;
}


void KalmanKinematicFit::fit() {


    KinematicConstraints kc;
    int nc = m_nc;
    int ntrk = numberWTrack();

    TStopwatch timer;



    timer.Start();
    //cout<<" =====calculate AC_{0}A^{T}====="<<endl;
    HepSymMatrix AC(m_nc, 0);
    AC = m_C0.similarity(m_A);
    timer.Stop();
    m_cpu[1] += timer.CpuTime();

    timer.Start();
    //cout<<" =====calculate W====="<<endl;
    int ifail;
    m_W = HepSymMatrix(m_nc, 0);
    m_W = (m_Vm + m_C0.similarity(m_A)).inverse(ifail);
    //cout<<" =====calculate D , D^{-1}====="<<endl;
    int ifailD;
    m_Dinv =  m_D0inv + m_W.similarity(m_B.T());   
    m_D = m_Dinv.inverse(ifailD);
    timer.Stop();
    m_cpu[2] += timer.CpuTime();

    timer.Start();
    //cout<<"===== G equals r ====="<<endl;
    HepVector G(m_nc, 0);
    G = m_A * (m_p0 - m_p) + m_B * (m_q0 - m_q)+ m_G;
    //cout<<"===== calculate KQ, Kp======"<<endl;
    m_KQ = HepMatrix(numbertwo(), m_nc, 0);
    m_KQ = m_D * m_BT * m_W;

    m_KP = HepMatrix(numberone(), m_nc, 0);
    m_KP = m_C0 * m_AT * m_W - m_C0 * m_AT * m_W * m_B * m_KQ;
    // ===== update track par =====
    m_p = m_p0 - m_KP * G;
    m_q = m_q0 - m_KQ * G;
    timer.Stop();
    m_cpu[4] += timer.CpuTime();

    timer.Start();   
    //===== caluculate chi2 ===== 
    m_chi = (m_W.similarity(G.T()) - m_Dinv.similarity((m_q - m_q0).T()))[0][0];
    timer.Stop();
    m_cpu[3] += timer.CpuTime();
    timer.Start();
    if(m_dynamicerror ==1) 
        gda(); 

    timer.Stop();
    m_cpu[6] += timer.CpuTime();

}




void KalmanKinematicFit::upCovmtx() {
    HepMatrix E(numberone(), numbertwo(), 0);
    E = -m_C0 * m_A.T() * m_KQ.T();
    m_C = m_C0 - m_W.similarity((m_A*m_C0).T()) + m_Dinv.similarity(E);
}    





void KalmanKinematicFit::fit(int n) {
    if(m_chisq.size() == 0) {
        for(unsigned int i = 0; i < m_kc.size(); i++)
            m_chisq.push_back(9999.0);
    }

    KinematicConstraints kc;
    int nc = m_nc;
    int ntrk = numberWTrack();

    //cout<<" =====calculate AC_{0}A^{T}====="<<endl;
    HepSymMatrix AC(m_nc, 0);
    AC = m_C0.similarity(m_A);

    //cout<<" =====calculate W====="<<endl;
    int ifail;
    m_W = HepSymMatrix(m_nc, 0);
    m_W = (m_Vm + m_C0.similarity(m_A)).inverse(ifail);
    //cout<<" =====calculate D , D^{-1}====="<<endl;
    int ifailD;
    m_Dinv =  m_D0inv + m_W.similarity(m_B.T());   
    m_D = m_Dinv.inverse(ifailD);

    //cout<<"===== G equals r ====="<<endl;
    HepVector G(m_nc, 0);
    G = m_A * (m_p0 - m_p) + m_B * (m_q0 - m_q)+ m_G;
    //cout<<"===== calculate KQ, Kp======"<<endl;
    m_KQ = HepMatrix(numbertwo(), m_nc, 0);
    m_KQ = m_D * m_BT * m_W;

    m_KP = HepMatrix(numberone(), m_nc, 0);
    m_KP = m_C0 * m_AT * m_W - m_C0 * m_AT * m_W * m_B * m_KQ;
    // ===== update track par =====
    m_p = m_p0 - m_KP * G;
    m_q = m_q0 - m_KQ * G;

    //===== caluculate chi2 =====
    m_chisq[n] = (m_W.similarity(G.T()) - m_Dinv.similarity((m_q - m_q0).T()))[0][0];
    if(m_dynamicerror ==1) 
        gda(); 
}








bool KalmanKinematicFit::Fit() {
    bool okfit = false;
    TStopwatch timer;
    m_nktrk = numberWTrack();
    m_p0 = HepVector(numberone(), 0);
    m_p  = HepVector(numberone(), 0);
    m_q0 = HepVector(numbertwo(), 0);
    m_q  = HepVector(numbertwo(), 0);
    m_C0 = HepSymMatrix(numberone(), 0);
    m_C  = HepSymMatrix(numberone(), 0);
    m_D0inv = HepSymMatrix(numbertwo(), 0);
    m_D  = HepSymMatrix(numbertwo(), 0);

    HepVector  m_p_tmp = HepVector(numberone(), 0);
    HepVector  m_q_tmp = HepVector(numbertwo(), 0);
    HepMatrix  m_A_tmp;
    HepSymMatrix m_W_tmp;
    HepMatrix  m_KQ_tmp;
    HepSymMatrix m_Dinv_tmp;
    for(int i = 0; i < numberWTrack(); i++) {
        setWTrackInfit(i, wTrackOrigin(i));
        int pa = mappositionA()[i] + 1;
        int pb = mappositionB()[i] + 1;
        int ptype = mapkinematic()[i];
        switch(ptype) {
            case 0 : {
                         HepVector w1(4, 0);
                         HepSymMatrix Ew1(4, 0);
                         for(int j = 0; j < 3; j++) {
                             w1[j] = wTrackOrigin(i).w()[j];
                         }
                         w1[3] = wTrackOrigin(i).mass();

                         for(int m = 0; m < 3; m++) {
                             for(int n = 0; n < 3; n++) {
                                 Ew1[m][n] = wTrackOrigin(i).Ew()[m][n];
                             }
                         }
                         setPOrigin(pa, w1);
                         setPInfit(pa, w1);
                         setCOrigin(pa, Ew1);
                         break;
                     }
            case 1 : {
                         setPOrigin(pa, (wTrackOrigin(i).plmp()).sub(1, 4));
                         setPInfit(pa, (wTrackOrigin(i).plmp()).sub(1, 4));
                         setCOrigin(pa, (wTrackOrigin(i).Vplm()).sub(1, 4));
                         break;
                     }
            case 2 : {
                         setQOrigin(pb, (wTrackOrigin(i).w()).sub(1, NTRKPAR));
                         setQInfit(pb, (wTrackOrigin(i).w()).sub(1, NTRKPAR));
                         HepSymMatrix Dinv(4,0);
                         setDOriginInv(pb, Dinv);
                         break;
                     }
            case 3 : {
                         setPOrigin(pa, (wTrackOrigin(i).plmp()).sub(3, 3));
                         setPInfit(pa, (wTrackOrigin(i).plmp()).sub(3, 3));
                         setCOrigin(pa, (wTrackOrigin(i).Vplm()).sub(3, 3));
                         setQOrigin(pb, (wTrackOrigin(i).plmp()).sub(1, 2));
                         setQInfit(pb, (wTrackOrigin(i).plmp()).sub(1, 2));
                         setQOrigin(pb+2, (wTrackOrigin(i).plmp()).sub(4, 4));
                         setQInfit(pb+2, (wTrackOrigin(i).plmp()).sub(4, 4));
                         HepSymMatrix Dinv(3,0);
                         setDOriginInv(pb, Dinv);
                         break;
                     }
            case 4 : {
                         setPOrigin(pa, (wTrackOrigin(i).plmp()).sub(1, 2));
                         setPInfit(pa, (wTrackOrigin(i).plmp()).sub(1, 2));
                         setCOrigin(pa, (wTrackOrigin(i).Vplm()).sub(1, 2));
                         setQOrigin(pb, (wTrackOrigin(i).plmp()).sub(3, 4));
                         setQInfit(pb, (wTrackOrigin(i).plmp()).sub(3, 4));
                         HepSymMatrix Dinv(2,0);
                         setDOriginInv(pb, Dinv);
                         break;
                     }
            case 5 : {
                         setPOrigin(pa, (wTrackOrigin(i).plmp()).sub(1, 3));
                         setPInfit(pa, (wTrackOrigin(i).plmp()).sub(1, 3));
                         setCOrigin(pa, (wTrackOrigin(i).Vplm()).sub(1, 3));
                         setQOrigin(pb, (wTrackOrigin(i).plmp()).sub(4, 4));
                         setQInfit(pb, (wTrackOrigin(i).plmp()).sub(4, 4));
                         HepSymMatrix Dinv(1,0);
                         setDOriginInv(pb, Dinv);
                         break;
                     }
            case 6 : {
                         setPOrigin(pa, (wTrackOrigin(i).w()).sub(5, 7));
                         setPOrigin(pa+3, (wTrackOrigin(i).w()).sub(4, 4));
                         setPInfit(pa, (wTrackOrigin(i).w()).sub(5, 7));
                         setPInfit(pa+3, (wTrackOrigin(i).w()).sub(4, 4));
                         setCOrigin(pa, (wTrackOrigin(i).Ew()).sub(5,7));
                         setCOrigin(pa+3, (wTrackOrigin(i).Ew()).sub(4,4));
                         HepVector beam(3,0);
                         beam[0] = getBeamPosition().x();
                         beam[1] = getBeamPosition().y();
                         beam[2] = getBeamPosition().z();
                         setQOrigin(pb, beam);
                         setQInfit(pb, beam);
                         HepSymMatrix Dinv(3, 0);
                         int ifail;
                         Dinv = getVBeamPosition().inverse(ifail);
                         setDOriginInv(pb, Dinv);
                         break;
                     }
            case 7 : {
                         HepVector w1(4, 0);
                         HepSymMatrix Ew1(4, 0);
                         for(int j = 0; j < 3; j++) {
                             w1[j] = wTrackOrigin(i).w()[j];
                         }
                         w1[3] = wTrackOrigin(i).mass();

                         for(int m = 0; m < 3; m++) {
                             for(int n = 0; n < 3; n++) {
                                 Ew1[m][n] = wTrackOrigin(i).Ew()[m][n];
                             }
                         }
                         setPOrigin(pa, w1);
                         setPInfit(pa, w1);
                         setCOrigin(pa, Ew1);
                         break;
                     }

        }
    }


    //
    // iteration
    //

    std::vector<double> chisq;
    chisq.clear();
    int nc = 0;
    for(int i = 0; i < m_kc.size(); i++)
        nc += m_kc[i].nc();

    m_A = HepMatrix(nc, numberone(), 0);
    m_AT = HepMatrix(numberone(), nc, 0);
    m_B = HepMatrix(nc, numbertwo(), 0);
    m_BT = HepMatrix(numbertwo(), nc, 0);
    m_G = HepVector(nc, 0);
    m_Vm = HepSymMatrix(nc, 0);
    int num1 = 0; 
    for(unsigned int i = 0; i < m_kc.size(); i++) {
        KinematicConstraints kc = m_kc[i];
        m_Vm.sub(num1+1,kc.Vmeasure());
        num1 = num1 + kc.nc();
    }

    double tmp_chisq = 999;
    bool flag_break = 0;
    for(int it = 0; it < m_niter; it++) {
        timer.Start();
        m_nc = 0;
        for(unsigned int i = 0; i < m_kc.size(); i++) {
            KinematicConstraints kc = m_kc[i];
            updateConstraints(kc);
        }
        timer.Stop();
        m_cpu[0] += timer.CpuTime();
        fit();
        //              
        //reset origin parameters for virtual particle
        //
        //                if(it == 0){
        //                    for(int i = 0; i < numberWTrack(); i++) {
        //                      //  setWTrackInfit(i, wTrackOrigin(i));
        //                        int pa = mappositionA()[i] + 1;
        //                        int pb = mappositionB()[i] + 1;
        //                        int ptype = mapkinematic()[i];
        //                        switch(ptype) {
        //                           case 3 : {
        //                                         setPOrigin(pa, m_p.sub(pa, pa));
        //                                         setPInfit(pa,  m_p.sub(pa, pa));
        //                                         setQOrigin(pb, m_q.sub(pb, pb+2));
        //                                         setQInfit(pb,  m_q.sub(pb, pb+2));
        //                                         break;
        //                                    }
        //                           case 4 : {
        //                                        setPOrigin(pa, (wTrackOrigin(i).plmp()).sub(1, 2));
        //                                        setPInfit(pa, (wTrackOrigin(i).plmp()).sub(1, 2));
        //                                        setCOrigin(pa, (wTrackOrigin(i).Vplm()).sub(1, 2));
        //                                        setQOrigin(pb, (wTrackOrigin(i).plmp()).sub(3, 4));
        //                                        setQInfit(pb, (wTrackOrigin(i).plmp()).sub(3, 4));
        //                                        HepSymMatrix Dinv(2,0);
        //                                        setDOriginInv(pb, Dinv);
        //                                        break;
        //                                    }
        //                           case 5 : {
        //                                        setPOrigin(pa, (wTrackOrigin(i).plmp()).sub(1, 3));
        //                                        setPInfit(pa, (wTrackOrigin(i).plmp()).sub(1, 3));
        //                                        setCOrigin(pa, (wTrackOrigin(i).Vplm()).sub(1, 3));
        //                                        setQOrigin(pb, (wTrackOrigin(i).plmp()).sub(4, 4));
        //                                        setQInfit(pb, (wTrackOrigin(i).plmp()).sub(4, 4));
        //                                        HepSymMatrix Dinv(1,0);
        //                                        setDOriginInv(pb, Dinv);
        //                                        break;
        //                                    }
        //                           case 6 : {
        //                                        setPOrigin(pa, (wTrackOrigin(i).w()).sub(5, 7));
        //                                        setPOrigin(pa+3, (wTrackOrigin(i).w()).sub(4, 4));
        //                                        setPInfit(pa, (wTrackOrigin(i).w()).sub(5, 7));
        //                                        setPInfit(pa+3, (wTrackOrigin(i).w()).sub(4, 4));
        //                                        setCOrigin(pa, (wTrackOrigin(i).Ew()).sub(5,7));
        //                                        setCOrigin(pa+3, (wTrackOrigin(i).Ew()).sub(4,4));
        //                                        HepVector beam(3,0);
        //                                        beam[0] = getBeamPosition().x();
        //                                        beam[1] = getBeamPosition().y();
        //                                        beam[2] = getBeamPosition().z();
        //                                        setQOrigin(pb, beam);
        //                                        setQInfit(pb, beam);
        //                                        HepSymMatrix Dinv(3, 0);
        //                                        int ifail;
        //                                        Dinv = getVBeamPosition().inverse(ifail);
        //                                        setDOriginInv(pb, Dinv);
        //                                        break;
        //                                    }
        //
        //                        }
        //                    }
        //
        //                }
        //
        //
        //===================reset over=============================
        //
        chisq.push_back(m_chi);
        if(it > 0) {
            double delchi = chisq[it]- chisq[it-1];  
            if(fabs(delchi) < m_chiter){
                flag_break =1; 
                break;
            }
        }
    }
    if(!flag_break) {
        return okfit;
    }
    if(m_chi > m_chicut) return okfit;
    timer.Start();
    timer.Stop();
    m_cpu[5] += timer.CpuTime();
    okfit = true;
    return okfit;

}


bool KalmanKinematicFit::Fit(int n) {
    bool okfit = false;
    if(n < 0 || (unsigned int)n >= m_kc.size()) return okfit;
    m_nktrk = numberWTrack();
    m_p0 = HepVector(numberone(), 0);
    m_p  = HepVector(numberone(), 0);
    m_q0 = HepVector(numbertwo(), 0);
    m_q  = HepVector(numbertwo(), 0);
    m_C0 = HepSymMatrix(numberone(), 0);
    m_C  = HepSymMatrix(numberone(), 0);
    m_D0inv = HepSymMatrix(numbertwo(), 0);
    m_D  = HepSymMatrix(numbertwo(), 0);

    for(int i = 0; i < numberWTrack(); i++) {
        setWTrackInfit(i, wTrackOrigin(i));
        int pa = mappositionA()[i] + 1;
        int pb = mappositionB()[i] + 1;
        int ptype = mapkinematic()[i];
        switch(ptype) {
            case 0 : {
                         HepVector w1(4, 0);
                         HepSymMatrix Ew1(4, 0);
                         for(int j = 0; j < 3; j++) {
                             w1[j] = wTrackOrigin(i).w()[j];
                         }
                         w1[3] = wTrackOrigin(i).mass();

                         for(int m = 0; m < 3; m++) {
                             for(int n = 0; n < 3; n++) {
                                 Ew1[m][n] = wTrackOrigin(i).Ew()[m][n];
                             }
                         }
                         setPOrigin(pa, w1);
                         setPInfit(pa, w1);
                         setCOrigin(pa, Ew1);
                         break;
                     }
            case 1 : {
                         setPOrigin(pa, (wTrackOrigin(i).plmp()).sub(1, 4));
                         setPInfit(pa, (wTrackOrigin(i).plmp()).sub(1, 4));
                         setCOrigin(pa, (wTrackOrigin(i).Vplm()).sub(1, 4));
                         break;
                     }
            case 2 : {
                         setQOrigin(pb, (wTrackOrigin(i).w()).sub(1, NTRKPAR));
                         setQInfit(pb, (wTrackOrigin(i).w()).sub(1, NTRKPAR));
                         HepSymMatrix Dinv(4,0);
                         setDOriginInv(pb, Dinv);
                         break;
                     }
            case 3 : {
                         setPOrigin(pa, (wTrackOrigin(i).plmp()).sub(3, 3));
                         setPInfit(pa, (wTrackOrigin(i).plmp()).sub(3, 3));
                         setCOrigin(pa, (wTrackOrigin(i).Vplm()).sub(3, 3));
                         setQOrigin(pb, (wTrackOrigin(i).w()).sub(1, 3));
                         setQInfit(pb, (wTrackOrigin(i).w()).sub(1, 3));
                         HepSymMatrix Dinv(3,0);
                         setDOriginInv(pb, Dinv);
                         break;
                     }
            case 4 : {
                         setPOrigin(pa, (wTrackOrigin(i).plmp()).sub(1, 2));
                         setPInfit(pa, (wTrackOrigin(i).plmp()).sub(1, 2));
                         setCOrigin(pa, (wTrackOrigin(i).Vplm()).sub(1, 2));
                         setQOrigin(pb, (wTrackOrigin(i).plmp()).sub(3, 4));
                         setQInfit(pb, (wTrackOrigin(i).plmp()).sub(3, 4));
                         HepSymMatrix Dinv(2,0);
                         setDOriginInv(pb, Dinv);
                         break;
                     }
            case 5 : {
                         setPOrigin(pa, (wTrackOrigin(i).plmp()).sub(1, 3));
                         setPInfit(pa, (wTrackOrigin(i).plmp()).sub(1, 3));
                         setCOrigin(pa, (wTrackOrigin(i).Vplm()).sub(1, 3));
                         setQOrigin(pb, (wTrackOrigin(i).plmp()).sub(4, 4));
                         setQInfit(pb, (wTrackOrigin(i).plmp()).sub(4, 4));
                         HepSymMatrix Dinv(1,0);
                         setDOriginInv(pb, Dinv);
                         break;
                     }
            case 6 : {
                         setPOrigin(pa, (wTrackOrigin(i).w()).sub(5, 7));
                         setPOrigin(pa+3, (wTrackOrigin(i).w()).sub(4, 4));
                         setPInfit(pa, (wTrackOrigin(i).w()).sub(5, 7));
                         setPInfit(pa+3, (wTrackOrigin(i).w()).sub(4, 4));
                         setCOrigin(pa, (wTrackOrigin(i).Ew()).sub(5,7));
                         setCOrigin(pa+3, (wTrackOrigin(i).Ew()).sub(4,4));
                         HepVector beam(3,0);
                         beam[0] = getBeamPosition().x();
                         beam[1] = getBeamPosition().y();
                         beam[2] = getBeamPosition().z();
                         setQOrigin(pb, beam);
                         setQInfit(pb, beam);
                         HepSymMatrix Dinv(3, 0);
                         int ifail;
                         Dinv = getVBeamPosition().inverse(ifail);

                         setDOriginInv(pb, Dinv);
                         break;
                     }

            case 7 : {
                         HepVector w1(4, 0);
                         HepSymMatrix Ew1(4, 0);
                         for(int j = 0; j < 3; j++) {
                             w1[j] = wTrackOrigin(i).w()[j];
                         }
                         w1[3] = wTrackOrigin(i).mass();

                         for(int m = 0; m < 3; m++) {
                             for(int n = 0; n < 3; n++) {
                                 Ew1[m][n] = wTrackOrigin(i).Ew()[m][n];
                             }
                         }
                         setPOrigin(pa, w1);
                         setPInfit(pa, w1);
                         setCOrigin(pa, Ew1);
                         break;
                     }
        }
    }



    //
    // iteration
    //

    std::vector<double> chisq;
    chisq.clear();
    int nc = 0;
    //  for(int i = 0; i < m_kc.size(); i++)
    nc += m_kc[n].nc();

    m_A = HepMatrix(nc, numberone(), 0);
    m_AT = HepMatrix(numberone(), nc, 0);
    m_B = HepMatrix(nc, numbertwo(), 0);
    m_BT = HepMatrix(numbertwo(), nc, 0);
    m_G = HepVector(nc, 0);
    m_Vm = HepSymMatrix(nc, 0);
    int num1 = 0; 
    KinematicConstraints kc = m_kc[n];
    m_Vm.sub(num1+1,kc.Vmeasure());
    num1 =  kc.nc();
    for(int it = 0; it < m_niter; it++) {
        m_nc = 0;
        KinematicConstraints kc = m_kc[n];
        updateConstraints(kc);
        fit(n);

        chisq.push_back(m_chisq[n]);

        if(it > 0) {

            double delchi = chisq[it]- chisq[it-1];
            if(fabs(delchi) < m_chiter) 
                break;
        }
    }
    if(m_chisq[n] >= m_chicut) {
        return okfit;
    }
    //update track parameter and its covariance matrix
    //upCovmtx();

    okfit = true;


    return okfit;

}





void KalmanKinematicFit::BuildVirtualParticle( int n) {
    upCovmtx();
    KinematicConstraints kc = m_kc[n];
    int ntrk = (kc.Ltrk()).size();
    int charge = 0;
    HepVector w(7, 0);
    HepSymMatrix ew1(7, 0);
    HepSymMatrix ew2(7, 0);
    HepVector w4(4, 0);              
    HepSymMatrix ew4(4, 0); 
    HepMatrix dwdp(4, 4, 0);
    dwdp[0][0] = 1;
    dwdp[1][1] = 1;
    dwdp[2][2] = 1;
    dwdp[3][3] = 1;
    for (int i = 0; i < ntrk; i++) {
        int itk = (kc.Ltrk())[i];
        charge += wTrackInfit(itk).charge();
        w4 = w4 + dwdp * pInfit(itk);
        //				ew = ew + (getCInfit(itk)).similarity(dwdp);
    }
    HepMatrix I(4, numberone(), 0);
    for(int j2=0; j2<ntrk; j2++){
        I[0][0+j2*4] = 1;
        I[1][1+j2*4] = 1;
        I[2][2+j2*4] = 1;
        I[3][3+j2*4] = 1;
    } 
    ew4 = m_C.similarity(I);
    HepMatrix J(7,4,0);
    double px = w4[0];
    double py = w4[1];
    double pz = w4[2];
    double e = w4[3];
    double pt = sqrt(px*px + py*py);
    double p0 = sqrt(px*px + py*py + pz*pz);
    double m =  sqrt(e*e - p0*p0);
    J[0][0] = -py;
    J[0][1] = -(pz*px*pt)/(p0*p0);
    J[0][2] = -m*px/(p0*p0);
    J[0][3] = e*px/(p0*p0);
    J[1][0] = px;
    J[1][1] = -(pz*py*pt)/(p0*p0);
    J[1][2] = -m*py/(p0*p0);
    J[1][3] = e*py/(p0*p0);
    J[2][0] = 0;
    J[2][1] = pt*pt*pt/(p0*p0);
    J[2][2] = -m*pz/(p0*p0);
    J[2][3] = e*pz/(p0*p0);
    J[3][0] = 0;
    J[3][1] = 0;
    J[3][2] = 0;
    J[3][3] = 1;
    ew1 = ew4.similarity(J);
    ew2[0][0] = ew1[0][0];
    ew2[1][1] = ew1[1][1];
    ew2[2][2] = ew1[2][2];
    ew2[3][3] = ew1[3][3];
    w[0] = w4[0];
    w[1] = w4[1];
    w[2] = w4[2];
    w[3] = w4[3];
    WTrackParameter vwtrk;
    vwtrk.setCharge(charge);
    vwtrk.setW(w);
    vwtrk.setEw(ew2);
    vwtrk.setMass(m);
    m_virtual_wtrk.push_back(vwtrk);
}



void KalmanKinematicFit::gda(){
    //        TGraph2D *g = new TGraph2D("/ihepbatch/bes/yanl/6.5.0//TestRelease/TestRelease-00-00-62/run/gamma/new/graph.dat");
    for(int i = 0; i < numberWTrack(); i++) {
        if ((wTrackOrigin(i)).type() == 2 ){
            int n ;
            for(int j = 0; j < numberGammaShape(); j++) {
                if(i==GammaShapeList(j))  n = j;
            }
            int pa = mappositionA()[i] + 1;
            int pb = mappositionB()[i] + 1;
            int ptype = mapkinematic()[i];
            HepSymMatrix Ew(NTRKPAR, 0);
            HepLorentzVector p1 = p4Infit(i);
            HepLorentzVector p2 = p4Origin(i);
            double eorigin = pOrigin(i)[3];
            HepMatrix dwda1(NTRKPAR, 3, 0);
            dwda1[0][0] = 1;
            dwda1[1][1] = - (p2.e()*p2.e())/(p2.px()*p2.px() + p2.py()*p2.py());
            //            dwda1[1][1] = - (p1.e()*p1.e())/(p1.px()*p1.px() + p1.py()*p1.py());
            dwda1[3][2] = 1;
            HepSymMatrix emcmea1(3, 0);
            double pk = p1[3];
            //						double pk = GammaShapeValue(n).peak(p1[3]);
            emcmea1[0][0] = GammaShapeValue(n).getdphi() * GammaShapeValue(n).getdphi();
            emcmea1[1][1] = GammaShapeValue(n).getdthe() * GammaShapeValue(n).getdthe()*(1/(1 - p2.cosTheta()*p2.cosTheta())) *(1/(1 - p2.cosTheta()*p2.cosTheta()));
            //            cout<<"delta lambda ="<<emcmea1[1][1]<<endl;
            emcmea1[2][2] = GammaShapeValue(n).de(eorigin,pk)*GammaShapeValue(n).de(eorigin,pk);
            //                        double tmp_e22 = m_graph2d->Interpolate(eorigin, pk);
            //                        if(fabs((eorigin/pk)-1)<0.05&&tmp_e22==0)emcmea1[2][2] = 0.025*pk*0.025;
            //                        else  emcmea1[2][2] = (tmp_e22*tmp_e22);
            //                        emcmea1[2][2] = m_graph2d->Interpolate(eorigin, pk)*m_graph2d->Interpolate(eorigin, pk);
            //						emcmea1[2][2] = GammaShapeValue(n).de(eorigin,pk) * GammaShapeValue(n).de(eorigin,pk);
            //         cout<<"dy_e  ="<<GammaShapeValue(n).de(eorigin,pk)<<endl;
            //        Ew = emcmea1.similarity(dwda1);
            Ew[0][0] = emcmea1[0][0];
            Ew[1][1] = emcmea1[1][1];
            Ew[3][3] = emcmea1[2][2];
            //            cout<<"Ew ="<<Ew<<endl;
            if(ptype==6)
                setCOrigin(pa+3, Ew.sub(4,4));
            else setCOrigin(pa,Ew);
        } 
    }
}


HepVector  KalmanKinematicFit::pull(int n){
    upCovmtx();
    int pa = mappositionA()[n] + 1;
    int pb = mappositionB()[n] + 1 ;
    int ptype = mapkinematic()[n];
    switch(ptype) {
        case 0 :{
                    HepVector W(7,0);
                    HepSymMatrix Ew(7,0);
                    HepVector W1(7,0);
                    HepSymMatrix Ew1(7,0);
                    WTrackParameter wtrk = wTrackOrigin(n);
                    W = wTrackOrigin(n).w();
                    Ew = wTrackOrigin(n).Ew();
                    for(int i=0; i<4; i++) {
                        W[i] = pInfit(n)[i];
                    }
                    for(int j=0; j<4; j++) {
                        for(int k=0; k<4; k++) {
                            Ew[j][k] = getCInfit(n)[j][k];
                        }
                    }
                    W1 = W;
                    double px = p4Infit(n).px();
                    double py = p4Infit(n).py();
                    double pz = p4Infit(n).pz();
                    double m =  p4Infit(n).m();
                    double e =  p4Infit(n).e();
                    HepMatrix J(7, 7, 0);
                    J[0][0] = 1;
                    J[1][1] = 1;
                    J[2][2] = 1;
                    J[3][0] = px/e;
                    J[3][1] = py/e;
                    J[3][2] = pz/e;
                    J[3][3] = m/e;
                    J[4][4] = 1;
                    J[5][5] = 1;
                    J[6][6] = 1;
                    Ew1 = Ew.similarity(J);

                    wtrk.setW(W1);
                    wtrk.setEw(Ew1);
                    setWTrackInfit(n, wtrk);

                    HTrackParameter horigin = HTrackParameter(wTrackOrigin(n));
                    HTrackParameter hinfit = HTrackParameter(wTrackInfit(n));
                    HepVector a0 = horigin.hel();
                    HepVector a1 = hinfit.hel();
                    HepSymMatrix v0 = horigin.eHel();
                    HepSymMatrix v1 = hinfit.eHel(); 
                    HepVector pull(9,0);
                    for (int k=0; k<5; k++) {
                        pull[k] = (a0[k]-a1[k])/sqrt(abs(v0[k][k]-v1[k][k]));
                    }
                    for (int l=5; l<9; l++) {
                        pull[l] = (wTrackOrigin(n).w()[l-5] - wTrackInfit(n).w()[l-5])/sqrt(abs(wTrackOrigin(n).Ew()[l-5][l-5] - wTrackInfit(n).Ew()[l-5][l-5]));
                    }
                    return pull;
                    break;        
                }
        case 1 : {
                     HepVector a0(4,0);
                     HepVector a1(4,0);
                     a0 = m_p0.sub(pa, pa+3);
                     a1 = m_p.sub(pa, pa+3);
                     HepSymMatrix v1 = getCInfit(n);
                     HepSymMatrix v0 = getCOrigin(n);
                     HepVector pull(3,0);
                     for (int k=0; k<2; k++) {
                         pull[k] = (a0[k]-a1[k])/sqrt(v0[k][k]-v1[k][k]);
                     }
                     pull[2] = (a0[3]-a1[3])/sqrt(v0[3][3]-v1[3][3]);
                     return pull;
                     break;
                 }
                 //        case 2 : {
                 //        return pull;
                 //        break;
                 //        }
                 //        case 3 : {
                 //        return pull;
                 //        break;
                 //        }
                 //        case 4 : {
                 //        return pull;
                 //        break;
                 //        }
        case 5 : {
                     HepLorentzVector p0 = p4Origin(n);
                     HepLorentzVector p1 = p4Infit(n);
                     HepVector a0(2,0);
                     HepVector a1(2,0);
                     a0[0] = p4Origin(n).phi();
                     a1[0] = p4Infit(n).phi();
                     a0[1] = p4Origin(n).pz()/p4Origin(n).perp();
                     a1[1] = p4Infit(n).pz()/p4Infit(n).perp();
                     HepMatrix Jacobi(2, 4, 0);
                     Jacobi[0][0] = - p4Infit(n).py()/p4Infit(n).perp2();
                     Jacobi[0][1] =   p4Infit(n).px()/p4Infit(n).perp2();
                     Jacobi[1][0] = - (p4Infit(n).px()/p4Infit(n).perp()) * (p4Infit(n).pz()/p4Infit(n).perp2());
                     Jacobi[1][1] = - (p4Infit(n).py()/p4Infit(n).perp()) * (p4Infit(n).pz()/p4Infit(n).perp2());
                     Jacobi[1][2] = 1/p4Infit(n).perp();
                     HepSymMatrix v1 = getCInfit(n).similarity(Jacobi);
                     HepSymMatrix v0 = wTrackOrigin(n).Vplm().sub(1,2);
                     HepVector pull(2,0);
                     for (int k=0; k<2; k++) {
                         pull[k] = (a0[k]-a1[k])/sqrt(v0[k][k]-v1[k][k]);
                     }
                     return pull;
                     break;
                 }
    }        
}



HepVector KalmanKinematicFit::pInfit(int n) const {
    int pa = mappositionA()[n] + 1;
    int pb = mappositionB()[n] + 1;
    int ptype = mapkinematic()[n];
    switch(ptype) {
        case 0 : {  
                     double mass = m_p.sub(pa+3, pa+3)[0];
                     HepVector p4(4,0);
                     p4[0] = m_p.sub(pa,pa)[0];
                     p4[1] = m_p.sub(pa+1, pa+1)[0];
                     p4[2] = m_p.sub(pa+2, pa+2)[0];
                     p4[3] = sqrt(p4[0]*p4[0] + p4[1]*p4[1] + p4[2]*p4[2] + mass * mass);
                     return p4;
                     break;
                 }
        case 1 : {
                     double phi = m_p.sub(pa, pa)[0];
                     double lambda = m_p.sub(pa+1, pa+1)[0];
                     double mass = m_p.sub(pa+2, pa+2)[0];
                     double E = m_p.sub(pa+3, pa+3)[0];
                     double p0 = sqrt(E*E - mass*mass);
                     HepVector p4(4,0);
                     p4[0] = p0*cos(phi)/sqrt(1 + lambda*lambda);
                     p4[1] = p0*sin(phi)/sqrt(1 + lambda*lambda);
                     p4[2] = p0*lambda/sqrt(1 + lambda*lambda);
                     p4[3] = sqrt(p0*p0 + mass*mass);
                     return p4;
                     break;
                 }	
        case 2 : {
                     return m_q.sub(pb, pb+3);
                     break;
                 }
        case 3 : {
                     double mass = (m_p.sub(pa, pa))[0];	
                     double phi = m_q.sub(pb, pb)[0];
                     double lambda = m_q.sub(pb+1, pb+1)[0];
                     double E = m_q.sub(pb+2, pb+2)[0];
                     double p0 = sqrt(E*E - mass*mass);
                     HepVector p4(4,0);
                     p4[0] = p0*cos(phi)/sqrt(1 + lambda*lambda);
                     p4[1] = p0*sin(phi)/sqrt(1 + lambda*lambda);
                     p4[2] = p0*lambda/sqrt(1 + lambda*lambda);
                     p4[3] = sqrt(p0*p0 + mass*mass);
                     return p4;
                     break;
                 }	
        case 4 : {
                     double phi = m_p.sub(pa, pa)[0];
                     double lambda = m_p.sub(pa+1, pa+1)[0];
                     double mass = m_q.sub(pb, pb)[0];
                     double E = m_q.sub(pb+1, pb+1)[0];
                     double p0 = sqrt(E*E - mass*mass);
                     HepVector p4(4,0);
                     p4[0] = p0*cos(phi)/sqrt(1 + lambda*lambda);
                     p4[1] = p0*sin(phi)/sqrt(1 + lambda*lambda);
                     p4[2] = p0*lambda/sqrt(1 + lambda*lambda);
                     p4[3] = sqrt(p0*p0 + mass*mass);
                     return p4;	
                     break;
                 }
        case 5 : {
                     double phi = m_p.sub(pa, pa)[0];
                     double lambda = m_p.sub(pa+1, pa+1)[0];
                     double mass = m_p.sub(pa+2, pa+2)[0];
                     double p0 = m_q.sub(pb, pb)[0];
                     HepVector p4(4,0);
                     p4[0] = p0*cos(phi)/sqrt(1 + lambda*lambda);
                     p4[1] = p0*sin(phi)/sqrt(1 + lambda*lambda);
                     p4[2] = p0*lambda/sqrt(1 + lambda*lambda);
                     p4[3] = sqrt(p0*p0 + mass*mass);
                     return p4;
                     break;
                 }
        case 6 : {
                     double x = m_p.sub(pa, pa)[0] - m_q.sub(pb, pb)[0];
                     double y = m_p.sub(pa+1, pa+1)[0] - m_q.sub(pb+1, pb+1)[0];
                     double z = m_p.sub(pa+2, pa+2)[0] - m_q.sub(pb+2, pb+2)[0];
                     double p0 = m_p.sub(pa+3, pa+3)[0];
                     double R = sqrt(x*x + y*y + z*z);
                     HepVector p4(4,0);
                     p4[0] = p0*x/R;
                     p4[1] = p0*y/R;
                     p4[2] = p0*z/R;
                     p4[3] = p0;
                     return p4;
                     break; 
                 }
        case 7 : {  
                     double mass = m_p.sub(pa+3, pa+3)[0];
                     HepVector p4(4,0);
                     p4[0] = m_p.sub(pa,pa)[0];
                     p4[1] = m_p.sub(pa+1, pa+1)[0];
                     p4[2] = m_p.sub(pa+2, pa+2)[0];
                     p4[3] = sqrt(p4[0]*p4[0] + p4[1]*p4[1] + p4[2]*p4[2] + mass * mass);
                     return p4;
                     break;
                 }
    }
}


HepVector KalmanKinematicFit::pOrigin(int n) const {
    int pa = mappositionA()[n] + 1;
    int pb = mappositionB()[n] + 1;
    int ptype = mapkinematic()[n]; 
    switch(ptype) {
        case 0 : {
                     double mass = m_p0.sub(pa+3, pa+3)[0];
                     HepVector p4(4,0);
                     p4[0] = m_p0.sub(pa,pa)[0];
                     p4[1] = m_p0.sub(pa+1, pa+1)[0];
                     p4[2] = m_p0.sub(pa+2, pa+2)[0];
                     p4[3] = sqrt(p4[0]*p4[0] + p4[1]*p4[1] + p4[2]*p4[2] + mass * mass);
                     return p4;
                     break;
                 }
        case 1 : {
                     double phi = m_p0.sub(pa, pa)[0];
                     double lambda = m_p0.sub(pa+1, pa+1)[0];
                     double mass = m_p0.sub(pa+2, pa+2)[0];
                     double E = m_p0.sub(pa+3, pa+3)[0];
                     double p0 = sqrt(E*E - mass*mass);
                     HepVector p4(4,0);
                     p4[0] = p0*cos(phi)/sqrt(1 + lambda*lambda);
                     p4[1] = p0*sin(phi)/sqrt(1 + lambda*lambda);
                     p4[2] = p0*lambda/sqrt(1 + lambda*lambda);
                     p4[3] = sqrt(p0*p0 + mass*mass);
                     return p4;
                     break;
                 }	
        case 2 : {
                     return m_q0.sub(pb, pb+3);
                     break;
                 }
        case 3 : {
                     double mass = (m_p0.sub(pa, pa))[0];	
                     double phi = m_q0.sub(pb, pb)[0];
                     double lambda = m_q0.sub(pb+1, pb+1)[0];
                     double E = m_q0.sub(pb+2, pb+2)[0];
                     double p0 = sqrt(E*E - mass*mass);
                     HepVector p4(4,0);
                     p4[0] = p0*cos(phi)/sqrt(1 + lambda*lambda);
                     p4[1] = p0*sin(phi)/sqrt(1 + lambda*lambda);
                     p4[2] = p0*lambda/sqrt(1 + lambda*lambda);
                     p4[3] = sqrt(p0*p0 + mass*mass);
                     return p4;
                     break;
                 }	
        case 4 : {
                     double phi = m_p0.sub(pa, pa)[0];
                     double lambda = m_p0.sub(pa+1, pa+1)[0];
                     double mass = m_q0.sub(pb, pb)[0];
                     double E = m_q0.sub(pb+1, pb+1)[0];
                     double p0 = sqrt(E*E - mass*mass);
                     HepVector p4(4,0);
                     p4[0] = p0*cos(phi)/sqrt(1 + lambda*lambda);
                     p4[1] = p0*sin(phi)/sqrt(1 + lambda*lambda);
                     p4[2] = p0*lambda/sqrt(1 + lambda*lambda);
                     p4[3] = sqrt(p0*p0 + mass*mass);
                     return p4;	
                     break;
                 }
        case 5 : {
                     double phi = m_p0.sub(pa, pa)[0];
                     double lambda = m_p0.sub(pa+1, pa+1)[0];
                     double mass = m_p0.sub(pa+2, pa+2)[0];
                     double p0 = m_q0.sub(pb, pb)[0];
                     HepVector p4(4,0);
                     p4[0] = p0*cos(phi)/sqrt(1 + lambda*lambda);
                     p4[1] = p0*sin(phi)/sqrt(1 + lambda*lambda);
                     p4[2] = p0*lambda/sqrt(1 + lambda*lambda);
                     p4[3] = sqrt(p0*p0 + mass*mass);
                     return p4;
                     break;
                 }
        case 6 : {
                     double x = m_p0.sub(pa, pa)[0] - m_q0.sub(pb, pb)[0];
                     double y = m_p0.sub(pa+1, pa+1)[0] - m_q0.sub(pb+1, pb+1)[0];
                     double z = m_p0.sub(pa+2, pa+2)[0] - m_q0.sub(pb+2, pb+2)[0];
                     double p0 = m_p0.sub(pa+3, pa+3)[0];
                     double R = sqrt(x*x + y*y + z*z);
                     HepVector p4(4,0);
                     p4[0] = p0*x/R;
                     p4[1] = p0*y/R;
                     p4[2] = p0*z/R;
                     p4[3] = p0;
                     return p4;
                     break;  
                 }
        case 7 : {
                     double mass = m_p0.sub(pa+3, pa+3)[0];
                     HepVector p4(4,0);
                     p4[0] = m_p0.sub(pa,pa)[0];
                     p4[1] = m_p0.sub(pa+1, pa+1)[0];
                     p4[2] = m_p0.sub(pa+2, pa+2)[0];
                     p4[3] = sqrt(p4[0]*p4[0] + p4[1]*p4[1] + p4[2]*p4[2] + mass * mass);
                     return p4;
                     break;
                 }
    }
}

HepSymMatrix KalmanKinematicFit::getCOrigin(int n) const {
    int pa = mappositionA()[n] + 1;
    int pb = mappositionB()[n] + 1;
    int ptype = mapkinematic()[n];
    switch(ptype) {
        case 0 : {
                     return m_C0.sub(pa, pa+NTRKPAR-1);
                     break;
                 }
        case 1 : {
                     return m_C0.sub(pa, pa+NTRKPAR-1);
                     break;
                 }
        case 3 : {
                     return m_C0.sub(pa, pa);
                     break;
                 }
        case 4 : {
                     return m_C0.sub(pa, pa+1);
                     break;
                 }
        case 5 : {
                     return m_C0.sub(pa, pa+2);
                     break;
                 }
        case 7 : {
                     return m_C0.sub(pa, pa+NTRKPAR-1);
                     break;
                 }

    }
} 


HepSymMatrix KalmanKinematicFit::getCInfit(int n) const {
    int pa = mappositionA()[n] + 1;
    int pb = mappositionB()[n] + 1;
    int ptype = mapkinematic()[n];
    switch(ptype) {
        case 0 : {
                     return m_C.sub(pa, pa+NTRKPAR-1);
                     break;
                 }
        case 1 : {
                     return m_C.sub(pa, pa+NTRKPAR-1);
                     break;
                 }
        case 3 : {
                     return m_C.sub(pa, pa);
                     break;
                 }
        case 4 : {
                     return m_C.sub(pa, pa+1);
                     break;
                 }
        case 5 : {
                     return m_C.sub(pa, pa+2);
                     break;
                 }
        case 7 : {
                     return m_C.sub(pa, pa+NTRKPAR-1);
                     break;
                 }
    }
}




void KalmanKinematicFit::updateConstraints(KinematicConstraints k ) {
    KinematicConstraints kc = k;


    int type = kc.Type();
    switch(type) {
        case  Resonance: {
                             //
                             //  E^2 - px^2 - py^2 - pz^2 = mres^2
                             //
                             double mres = kc.mres();
                             HepLorentzVector pmis;
                             for(unsigned int j = 0; j< (kc.Ltrk()).size(); j++){
                                 int n = (kc.Ltrk())[j];
                                 pmis = pmis + p4Infit(n);
                             }
                             for(unsigned int j = 0; j< (kc.Ltrk()).size(); j++){
                                 int n = (kc.Ltrk())[j];
                                 double lambda = p4Infit(n).pz()/p4Infit(n).perp();
                                 double a1 = 1 + lambda*lambda;
                                 int pa = mappositionA()[n] + 1;
                                 int pb = mappositionB()[n] + 1;
                                 int ptype = mapkinematic()[n];
                                 switch(ptype) {
                                     case 0 :{
                                                 HepMatrix ta(1, NTRKPAR, 0);
                                                 ta[0][0] = -2 * pmis.px() + 2 * pmis.e() * p4Infit(n).px() / p4Infit(n).e();
                                                 ta[0][1] = -2 * pmis.py() + 2 * pmis.e() * p4Infit(n).py() / p4Infit(n).e();
                                                 ta[0][2] = -2 * pmis.pz() + 2 * pmis.e() * p4Infit(n).pz() / p4Infit(n).e();
                                                 ta[0][3] = 2 * pmis.e() * p4Infit(n).m() / p4Infit(n).e();
                                                 setA(m_nc,pa,ta);
                                                 setAT(pa, m_nc, ta.T());
                                                 break;
                                             }
                                     case 1 :  {
                                                   HepMatrix ta(1, NTRKPAR, 0);
                                                   double a1 = lambda * lambda + 1;
                                                   ta[0][0] = 2 * pmis.px() * p4Infit(n).py() - 2 * pmis.py() * p4Infit(n).px();
                                                   ta[0][1] = 2 * pmis.px() * p4Infit(n).px() * lambda/sqrt(a1) + 2 * pmis.py() * (p4Infit(n)).py() * lambda/sqrt(a1) - 2 * pmis.pz() * (p4Infit(n)).pz() /(sqrt(a1) * lambda);
                                                   ta[0][2] =  2 * pmis.px() * (p4Infit(n)).px() * p4Infit(n).m() /((p4Infit(n)).rho() * p4Infit(n).rho()) + 2 * pmis.py() * (p4Infit(n)).py() * p4Infit(n).m()/((p4Infit(n)).rho() * p4Infit(n).rho())+ 2 * pmis.pz() * (p4Infit(n)).pz() * p4Infit(n).m()/((p4Infit(n)).rho() * p4Infit(n).rho());
                                                   ta[0][3] = 2 * pmis.e()  - 2 * pmis.px() * (p4Infit(n)).px() * p4Infit(n).e() /((p4Infit(n)).rho() * p4Infit(n).rho())- 2 * pmis.py() * (p4Infit(n)).py() * p4Infit(n).e()/((p4Infit(n)).rho() * p4Infit(n).rho())- 2 * pmis.pz() * (p4Infit(n)).pz() * p4Infit(n).e()/((p4Infit(n)).rho() * p4Infit(n).rho());
                                                   setA(m_nc,pa,ta);
                                                   setAT(pa, m_nc, ta.T());
                                                   break;
                                               }
                                     case 2 :  {
                                                   HepMatrix tb(1, 4, 0);
                                                   tb[0][0] = -2 * pmis.px();
                                                   tb[0][1] = -2 * pmis.py();
                                                   tb[0][2] = -2 * pmis.pz();
                                                   tb[0][3] = 2 * pmis.e();
                                                   setB(m_nc,pb,tb);
                                                   setBT(pb,m_nc,tb.T());
                                                   break;
                                               }
                                     case 3 :  {
                                                   HepMatrix ta(1, 1, 0);
                                                   double a1 = lambda * lambda + 1;
                                                   ta[0][0] =  2 * pmis.px() * (p4Infit(n)).px() * p4Infit(n).m() /((p4Infit(n)).rho() * p4Infit(n).rho()) + 2 * pmis.py() * (p4Infit(n)).py() * p4Infit(n).m()/((p4Infit(n)).rho() * p4Infit(n).rho())+ 2 * pmis.pz() * (p4Infit(n)).pz() * p4Infit(n).m()/((p4Infit(n)).rho() * p4Infit(n).rho());
                                                   setA(m_nc,pa,ta);
                                                   setAT(pa, m_nc, ta.T());
                                                   HepMatrix tb(1, 3, 0);
                                                   tb[0][0] = 2 * pmis.px() * p4Infit(n).py() - 2 * pmis.py() * p4Infit(n).px();
                                                   tb[0][1] = 2 * pmis.px() * p4Infit(n).px() * lambda/sqrt(a1) + 2 * pmis.py() * (p4Infit(n)).py() * lambda/sqrt(a1) - 2 * pmis.pz() * (p4Infit(n)).pz() /(sqrt(a1) * lambda);
                                                   tb[0][2] = 2 * pmis.e()  - 2 * pmis.px() * (p4Infit(n)).px() * p4Infit(n).e() /((p4Infit(n)).rho() * p4Infit(n).rho())- 2 * pmis.py() * (p4Infit(n)).py() * p4Infit(n).e()/((p4Infit(n)).rho() * p4Infit(n).rho())- 2 * pmis.pz() * (p4Infit(n)).pz() * p4Infit(n).e()/((p4Infit(n)).rho() * p4Infit(n).rho());
                                                   setB(m_nc,pb,tb);
                                                   setBT(pb,m_nc,tb.T());
                                                   break;
                                               }
                                     case 4 :  {
                                                   HepMatrix ta(1, 2, 0);
                                                   double a1 = lambda * lambda + 1;
                                                   ta[0][0] = 2 * pmis.px() * p4Infit(n).py() - 2 * pmis.py() * p4Infit(n).px();
                                                   ta[0][1] = 2 * pmis.px() * p4Infit(n).px() * lambda/sqrt(a1) + 2 * pmis.py() * (p4Infit(n)).py() * lambda/sqrt(a1) - 2 * pmis.pz() * (p4Infit(n)).pz() /(sqrt(a1) * lambda);
                                                   setA(m_nc,pa,ta);
                                                   setAT(pa, m_nc, ta.T());

                                                   HepMatrix tb(1, 2, 0);
                                                   tb[0][0] =  2 * pmis.px() * (p4Infit(n)).px() * p4Infit(n).m() /((p4Infit(n)).rho() * p4Infit(n).rho()) + 2 * pmis.py() * (p4Infit(n)).py() * p4Infit(n).m()/((p4Infit(n)).rho() * p4Infit(n).rho())+ 2 * pmis.pz() * (p4Infit(n)).pz() * p4Infit(n).m()/((p4Infit(n)).rho() * p4Infit(n).rho());
                                                   tb[0][1] = 2 * pmis.e()  - 2 * pmis.px() * (p4Infit(n)).px() * p4Infit(n).e() /((p4Infit(n)).rho() * p4Infit(n).rho())- 2 * pmis.py() * (p4Infit(n)).py() * p4Infit(n).e()/((p4Infit(n)).rho() * p4Infit(n).rho())- 2 * pmis.pz() * (p4Infit(n)).pz() * p4Infit(n).e()/((p4Infit(n)).rho() * p4Infit(n).rho());
                                                   setB(m_nc,pb,tb);
                                                   setBT(pb, m_nc, tb.T());
                                                   break;
                                               }
                                     case 5 :  {
                                                   HepMatrix ta(1, 3, 0);
                                                   ta[0][0] = 2 * pmis.px() * p4Infit(n).py() - 2 * pmis.py() * p4Infit(n).px();
                                                   ta[0][1] = 2 * pmis.px() * p4Infit(n).px() * lambda/sqrt(a1)+ 2 * pmis.py() * (p4Infit(n)).py() * lambda/sqrt(a1) - 2 * pmis.pz() * (p4Infit(n)).pz() /(sqrt(a1) * lambda);
                                                   ta[0][2] = 2 * pmis.e() * (p4Infit(n)).m()/(p4Infit(n)).e();
                                                   setA(m_nc,pa,ta);
                                                   setAT(pa, m_nc, ta.T());
                                                   HepMatrix tb(1, 1, 0);
                                                   tb[0][0] = 2 * pmis.e() * (p4Infit(n)).rho()/(p4Infit(n)).e() - 2 * pmis.px() * (p4Infit(n)).px()/(p4Infit(n)).rho() - 2 * pmis.py() * (p4Infit(n)).py()/(p4Infit(n)).rho() - 2 * pmis.pz() * (p4Infit(n)).pz()/(p4Infit(n)).rho();
                                                   setB(m_nc,pb,tb);
                                                   setBT(pb, m_nc, tb.T());
                                                   break;
                                               }       
                                     case 7 :{
                                                 HepMatrix ta(1, NTRKPAR, 0);
                                                 ta[0][0] = -2 * pmis.px() + 2 * pmis.e() * p4Infit(n).px() / p4Infit(n).e();
                                                 ta[0][1] = -2 * pmis.py() + 2 * pmis.e() * p4Infit(n).py() / p4Infit(n).e();
                                                 ta[0][2] = -2 * pmis.pz() + 2 * pmis.e() * p4Infit(n).pz() / p4Infit(n).e();
                                                 ta[0][3] = 2 * pmis.e() * p4Infit(n).m() / p4Infit(n).e();
                                                 setA(m_nc,pa,ta);
                                                 setAT(pa, m_nc, ta.T());
                                                 break;
                                             }
                                 }   
                             }

                             HepVector dc(1, 0);
                             dc[0] = pmis.m2() - mres * mres;
                             m_G[m_nc] = dc[0];
                             m_nc+=1;

                             break;
                         }
        case TotalEnergy: {
                              //
                              //  E - Etot = 0
                              //
                              double etot = kc.etot();
                              HepLorentzVector pmis;
                              for(unsigned int j = 0; j < (kc.Ltrk()).size(); j++){
                                  int n = (kc.Ltrk())[j];
                                  pmis = pmis + p4Infit(n);
                              }

                              for(unsigned int j = 0; j < (kc.Ltrk()).size(); j++) {
                                  int n = (kc.Ltrk())[j];
                                  int pa = mappositionA()[n] + 1;
                                  int pb = mappositionB()[n] + 1;
                                  int ptype = mapkinematic()[n];
                                  switch(ptype) {
                                      case 0 :{
                                                  HepMatrix ta(1, NTRKPAR, 0);
                                                  ta[0][0] = p4Infit(n).px()/p4Infit(n).e();
                                                  ta[0][1] = p4Infit(n).py()/p4Infit(n).e();
                                                  ta[0][2] = p4Infit(n).pz()/p4Infit(n).e();
                                                  ta[0][3] = p4Infit(n).m()/p4Infit(n).e();
                                                  setA(m_nc,pa,ta);
                                                  setAT(pa, m_nc, ta.T());
                                                  break;
                                              }
                                      case 1: {
                                                  HepMatrix ta(1, NTRKPAR, 0);
                                                  ta[0][3] = 1.0;
                                                  setA(m_nc,pa,ta);
                                                  setAT(pa, m_nc, ta.T());
                                                  break;
                                              }
                                      case 2: {
                                                  HepMatrix tb(1, 4, 0);
                                                  tb[0][3] = 1.0;
                                                  setA(m_nc,pb,tb);
                                                  setAT(pb, m_nc, tb.T());
                                                  break;
                                              }
                                      case 3: {
                                                  HepMatrix ta(1, 1, 0);
                                                  ta[0][0] = p4Infit(n).m()/p4Infit(n).e();
                                                  setA(m_nc,pa,ta);
                                                  setAT(pa, m_nc, ta.T());

                                                  HepMatrix tb(1, 3, 0);
                                                  setB(m_nc,pb,tb);
                                                  setBT(pb, m_nc, tb.T());
                                                  break;
                                              }
                                      case 4: {
                                                  HepMatrix ta(1, 2, 0);
                                                  setA(m_nc,pa,ta);
                                                  setAT(pa, m_nc, ta.T());

                                                  HepMatrix tb(1, 2, 0);
                                                  tb[0][0] = 0.0;
                                                  tb[0][1] = 1.0;
                                                  //   tb[0][0] = p4Infit(n).m()/p4Infit(n).e();
                                                  //   tb[0][1] = p4Infit(n).rho()/p4Infit(n).e();
                                                  setB(m_nc,pb,tb);
                                                  setBT(pb, m_nc, tb.T());
                                                  break;
                                              }
                                      case 5: {
                                                  HepMatrix ta(1, 3, 0);
                                                  ta[0][2] = p4Infit(n).m()/p4Infit(n).e();
                                                  setA(m_nc,pa,ta);
                                                  setAT(pa, m_nc, ta.T());

                                                  HepMatrix tb(1, 1, 0);
                                                  tb[0][0] = p4Infit(n).rho()/p4Infit(n).e();
                                                  setB(m_nc,pb,tb);
                                                  setBT(pb, m_nc, tb.T());
                                                  break;
                                              }	
                                      case 7 :{
                                                  HepMatrix ta(1, NTRKPAR, 0);
                                                  ta[0][0] = p4Infit(n).px()/p4Infit(n).e();
                                                  ta[0][1] = p4Infit(n).py()/p4Infit(n).e();
                                                  ta[0][2] = p4Infit(n).pz()/p4Infit(n).e();
                                                  ta[0][3] = p4Infit(n).m()/p4Infit(n).e();
                                                  setA(m_nc,pa,ta);
                                                  setAT(pa, m_nc, ta.T());
                                                  break;
                                              }
                                  }	

                              }

                              HepVector dc(1, 0);
                              dc[0] = pmis.e() - etot;
                              m_G[m_nc] = dc[0];
                              m_nc+=1;
                              break;
                          }
        case TotalMomentum: {
                                //
                                //  sqrt(px^2+py^2+pz^2) - ptot = 0
                                //
                                double ptot = kc.ptot();
                                HepLorentzVector pmis;
                                for(unsigned int j = 0; j< (kc.Ltrk()).size(); j++){
                                    int n = (kc.Ltrk())[j];
                                    pmis = pmis + p4Infit(n);
                                }

                                for(unsigned int j = 0; j < (kc.Ltrk()).size(); j++) {
                                    int n = (kc.Ltrk())[j];
                                    int pa = mappositionA()[n] + 1;
                                    int pb = mappositionB()[n] + 1;
                                    int ptype = mapkinematic()[n];
                                    double lambda = p4Infit(n).pz()/p4Infit(n).perp();
                                    switch(ptype) {
                                        case 0 : {
                                                     HepMatrix ta(1, NTRKPAR, 0);
                                                     ta[0][0] = pmis.px()/pmis.rho();
                                                     ta[0][1] = pmis.py()/pmis.rho();
                                                     ta[0][2] = pmis.pz()/pmis.rho();
                                                     setA(m_nc,pa,ta);
                                                     setAT(pa, m_nc, ta.T());
                                                     break;
                                                 }
                                        case 1 : {
                                                     HepMatrix ta(1, NTRKPAR, 0);
                                                     ta[0][0] = - (pmis.px()/pmis.rho()) * p4Infit(n).py() + (pmis.px()/pmis.rho())*p4Infit(n).px();
                                                     ta[0][1] = - (pmis.px()/pmis.rho()) * p4Infit(n).px() * (lambda/(1 + lambda*lambda)) - (pmis.py()/pmis.rho()) * p4Infit(n).py() * (lambda/(1 + lambda*lambda)) + (pmis.pz()/pmis.rho()) * p4Infit(n).pz() * (1/(lambda * (1 + lambda*lambda)));
                                                     ta[0][2] =  -((pmis.px()/pmis.rho()) * p4Infit(n).m()/(p4Infit(n).rho()*p4Infit(n).rho())) * (p4Infit(n).px() + p4Infit(n).py() +p4Infit(n).pz());
                                                     ta[0][3] = ((pmis.px()/pmis.rho()) * p4Infit(n).e()/(p4Infit(n).rho()*p4Infit(n).rho())) * (p4Infit(n).px() + p4Infit(n).py() +
                                                             p4Infit(n).pz()); 

                                                     setA(m_nc,pa,ta);
                                                     setAT(pa, m_nc, ta.T());
                                                     break;
                                                 }
                                        case 2 : {
                                                     HepMatrix tb(1, 4, 0);
                                                     tb[0][0] = pmis.px()/pmis.rho();
                                                     tb[0][1] = pmis.py()/pmis.rho();
                                                     tb[0][2] = pmis.pz()/pmis.rho();
                                                     setB(m_nc,pb,tb);
                                                     setBT(pb, m_nc, tb.T());
                                                     break;
                                                 }
                                        case 3 : {
                                                     HepMatrix ta(1, 1, 0); 
                                                     setA(m_nc,pa,ta);
                                                     setAT(pa, m_nc, ta.T());
                                                     HepMatrix tb(1, 3, 0);
                                                     tb[0][0] = pmis.px()/pmis.rho();
                                                     tb[0][1] = pmis.py()/pmis.rho();
                                                     tb[0][2] = pmis.pz()/pmis.rho();
                                                     setB(m_nc,pb,tb);
                                                     setBT(pb, m_nc, tb.T());
                                                     break;
                                                 }
                                        case 4 : {
                                                     HepMatrix ta(1, 2, 0);
                                                     ta[0][0] = - (pmis.px()/pmis.rho()) * p4Infit(n).py() + (pmis.px()/pmis.rho())*p4Infit(n).px();
                                                     ta[0][1] = - (pmis.px()/pmis.rho()) * p4Infit(n).px() * (lambda/(1 + lambda*lambda)) - (pmis.py()/pmis.rho()) * p4Infit(n).py() * (lambda/(1 + lambda*lambda)) + (pmis.pz()/pmis.rho()) * p4Infit(n).pz() * (1/(lambda * (1 + lambda*lambda)));
                                                     setA(m_nc,pa,ta);
                                                     setAT(pa, m_nc, ta.T());

                                                     HepMatrix tb(1, 2, 0);
                                                     tb[0][0] =  -((pmis.px()/pmis.rho()) * p4Infit(n).m()/(p4Infit(n).rho()*p4Infit(n).rho())) * (p4Infit(n).px() + p4Infit(n).py() +p4Infit(n).pz());
                                                     tb[0][1] = ((pmis.px()/pmis.rho()) * p4Infit(n).e()/(p4Infit(n).rho()*p4Infit(n).rho())) * (p4Infit(n).px() + p4Infit(n).py() +
                                                             p4Infit(n).pz());
                                                     //             tb[0][1] =  (pmis.px()/pmis.rho()) * (p4Infit(n).px()/p4Infit(n).rho()) + (pmis.py()/pmis.rho()) * (p4Infit(n).py()/p4Infit(n).rho()) + (pmis.pz()/pmis.rho()) * (p4Infit(n).pz()/p4Infit(n).rho());
                                                     setB(m_nc,pb,tb);
                                                     setBT(pb, m_nc, tb.T());                
                                                     break;
                                                 }
                                        case 5 : {
                                                     HepMatrix ta(1, 3, 0);
                                                     ta[0][0] = - (pmis.px()/pmis.rho()) * p4Infit(n).py() + (pmis.px()/pmis.rho())*p4Infit(n).px();
                                                     ta[0][1] = - (pmis.px()/pmis.rho()) * p4Infit(n).px() * (lambda/(1 + lambda*lambda)) - (pmis.py()/pmis.rho()) * p4Infit(n).py() * (lambda/(1 + lambda*lambda)) + (pmis.pz()/pmis.rho()) * p4Infit(n).pz() * (1/(lambda * (1 + lambda*lambda)));   
                                                     setA(m_nc,pa,ta);
                                                     setAT(pa, m_nc, ta.T());

                                                     HepMatrix tb(1, 1, 0);
                                                     tb[0][0] =  (pmis.px()/pmis.rho()) * (p4Infit(n).px()/p4Infit(n).rho()) + (pmis.py()/pmis.rho()) * (p4Infit(n).py()/p4Infit(n).rho()) + (pmis.pz()/pmis.rho()) * (p4Infit(n).pz()/p4Infit(n).rho());
                                                     setB(m_nc,pb,tb);
                                                     setBT(pb, m_nc, tb.T()); 
                                                     break;
                                                 }
                                        case 7 : {
                                                     HepMatrix ta(1, NTRKPAR, 0);
                                                     ta[0][0] = pmis.px()/pmis.rho();
                                                     ta[0][1] = pmis.py()/pmis.rho();
                                                     ta[0][2] = pmis.pz()/pmis.rho();
                                                     setA(m_nc,pa,ta);
                                                     setAT(pa, m_nc, ta.T());
                                                     break;
                                                 }
                                    }	 
                                }


                                HepVector dc(1, 0);
                                dc[0] = pmis.rho() - ptot;
                                m_G[m_nc] = dc[0];
                                m_nc+=1;     
                                break;
                            }


        case ThreeMomentum: {
                                //
                                //  px - p3x = 0
                                //  py - p3y = 0
                                //  pz - p3z = 0
                                //
                                Hep3Vector p3 = kc.p3();
                                HepLorentzVector pmis;
                                for(unsigned int j = 0; j< (kc.Ltrk()).size(); j++){
                                    int n = (kc.Ltrk())[j];
                                    pmis = pmis + p4Infit(n);
                                }
                                for(unsigned int j = 0; j < (kc.Ltrk()).size(); j++) {
                                    int n = (kc.Ltrk())[j];
                                    int pa = mappositionA()[n] + 1;
                                    int pb = mappositionB()[n] + 1;
                                    int ptype = mapkinematic()[n];
                                    double lambda = p4Infit(n).pz()/p4Infit(n).perp();
                                    switch(ptype) {
                                        case 0 : {
                                                     HepMatrix ta(kc.nc(), NTRKPAR, 0);
                                                     ta[0][0] = 1.0;
                                                     ta[1][1] = 1.0;
                                                     ta[2][2] = 1.0;
                                                     setA(m_nc, pa, ta);
                                                     setAT(pa, m_nc, ta.T());
                                                     break;
                                                 }
                                        case 1 : {
                                                     HepMatrix ta(kc.nc(), NTRKPAR, 0);
                                                     ta[0][0] = -p4Infit(n).py();
                                                     ta[0][1] = -p4Infit(n).px()*(lambda/sqrt(1 + lambda*lambda));
                                                     ta[0][2] = -p4Infit(n).m()*p4Infit(n).px()/(p4Infit(n).rho()*p4Infit(n).rho());
                                                     ta[0][3] = p4Infit(n).e()*p4Infit(n).px()/(p4Infit(n).rho()*p4Infit(n).rho());
                                                     ta[1][0] = p4Infit(n).px();
                                                     ta[1][1] = -p4Infit(n).py()*(lambda/sqrt(1 + lambda*lambda));
                                                     ta[1][2] = -p4Infit(n).m()*p4Infit(n).py()/(p4Infit(n).rho()*p4Infit(n).rho());
                                                     ta[1][3] = p4Infit(n).e()*p4Infit(n).py()/(p4Infit(n).rho()*p4Infit(n).rho());
                                                     ta[2][0] = 0;
                                                     ta[2][1] = p4Infit(n).pz()*(1/(lambda * (1 + lambda*lambda)));
                                                     ta[2][2] = -p4Infit(n).m()*p4Infit(n).pz()/(p4Infit(n).rho()*p4Infit(n).rho());
                                                     ta[2][3] = p4Infit(n).e()*p4Infit(n).pz()/(p4Infit(n).rho()*p4Infit(n).rho());
                                                     //             ta[0][0] = 1.0;
                                                     //             ta[1][1] = 1.0;
                                                     //             ta[2][2] = 1.0;
                                                     setA(m_nc, pa, ta);
                                                     setAT(pa, m_nc, ta.T()); 
                                                     break;
                                                 }
                                        case 2 : {
                                                     HepMatrix tb(kc.nc(), 4, 0);
                                                     tb[0][0] = 1.0;
                                                     tb[1][1] = 1.0;
                                                     tb[2][2] = 1.0;
                                                     setB(m_nc, pb, tb);
                                                     setBT(pb, m_nc, tb.T());
                                                     break;
                                                 }
                                        case 3 : {
                                                     HepMatrix ta(kc.nc(), 1, 0);
                                                     setA(m_nc, pa, ta);
                                                     setAT(pa, m_nc, ta.T()); 

                                                     HepMatrix tb(kc.nc(), 3, 0);
                                                     tb[0][0] = 1.0;
                                                     tb[1][1] = 1.0;
                                                     tb[2][2] = 1.0;
                                                     setB(m_nc, pb, tb);
                                                     setBT(pb, m_nc, tb.T());

                                                     break;
                                                 }
                                        case 4 : {
                                                     HepMatrix ta(kc.nc(), 2, 0);
                                                     ta[0][0] = -p4Infit(n).py(); 
                                                     ta[0][1] = -p4Infit(n).px()*(lambda/sqrt(1 + lambda*lambda));
                                                     ta[1][0] =  p4Infit(n).px(); 
                                                     ta[1][1] = -p4Infit(n).py()*(lambda/sqrt(1 + lambda*lambda));
                                                     ta[2][0] =  0;
                                                     ta[2][1] =  p4Infit(n).pz()*(1/(lambda * (1 + lambda*lambda)));
                                                     setA(m_nc, pa, ta);
                                                     setAT(pa, m_nc, ta.T()); 

                                                     HepMatrix tb(kc.nc(), 2, 0);
                                                     tb[0][1] = -p4Infit(n).m()*p4Infit(n).px()/(p4Infit(n).rho()*p4Infit(n).rho());
                                                     tb[1][1] = -p4Infit(n).m()*p4Infit(n).py()/(p4Infit(n).rho()*p4Infit(n).rho());
                                                     tb[2][1] = -p4Infit(n).m()*p4Infit(n).pz()/(p4Infit(n).rho()*p4Infit(n).rho());
                                                     setB(m_nc, pb, tb);
                                                     setBT(pb, m_nc, tb.T());
                                                     break;
                                                 }
                                        case 5 : {
                                                     HepMatrix ta(kc.nc(), 3, 0);
                                                     ta[0][0] = -p4Infit(n).py(); 
                                                     ta[0][1] = -p4Infit(n).px()*(lambda/sqrt(1 + lambda*lambda));
                                                     ta[1][0] =  p4Infit(n).px(); 
                                                     ta[1][1] = -p4Infit(n).py()*(lambda/sqrt(1 + lambda*lambda));
                                                     ta[2][0] =  0;
                                                     ta[2][1] =  p4Infit(n).pz()*(1/(lambda * (1 + lambda*lambda)));
                                                     setA(m_nc, pa, ta);
                                                     setAT(pa, m_nc, ta.T()); 

                                                     HepMatrix tb(kc.nc(), 1, 0);
                                                     tb[0][0] = p4Infit(n).px()/p4Infit(n).rho();
                                                     tb[1][0] = p4Infit(n).py()/p4Infit(n).rho();
                                                     tb[2][0] = p4Infit(n).pz()/p4Infit(n).rho();
                                                     setB(m_nc, pb, tb);
                                                     setBT(pb, m_nc, tb.T());
                                                     break;
                                                 }
                                        case 7 : {
                                                     HepMatrix ta(kc.nc(), NTRKPAR, 0);
                                                     ta[0][0] = 1.0;
                                                     ta[1][1] = 1.0;
                                                     ta[2][2] = 1.0;
                                                     setA(m_nc, pa, ta);
                                                     setAT(pa, m_nc, ta.T());
                                                     break;
                                                 }
                                    }
                                }    
                                HepVector dc(kc.nc(), 0);
                                dc[0] = pmis.px() - p3.x();
                                dc[1] = pmis.py() - p3.y();
                                dc[2] = pmis.pz() - p3.z();
                                for(int i = 0; i < kc.nc(); i++) {
                                    m_G[m_nc+i] = dc[i];
                                }	 

                                m_nc += 3; 

                                break;
                            }
        case EqualMass: {
                            //
                            //  (E_1 ^2 - Px_1 ^2 - Py_1 ^2 - Pz_1 ^2) - (E_2 ^2 - Px_2 ^2 - Py_2 ^2 - Pz_2 ^2) = 0
                            //

                            int isiz = (kc.numEqual())[0];
                            HepLorentzVector pmis1, pmis2;
                            for(int n = 0; n < isiz; n++) {
                                int n1 = (kc.Ltrk())[n]; 
                                pmis1 = pmis1 + p4Infit(n1);
                            }

                            int jsiz = (kc.numEqual())[1];
                            for(int n = 0; n < jsiz; n++){
                                int n2 = (kc.Ltrk())[n+isiz];
                                pmis2 = pmis2 + p4Infit(n2);
                            }

                            for(int i = 0; i < isiz; i++) {
                                int n1 = (kc.Ltrk())[i];
                                double lambda = p4Infit(n1).pz()/p4Infit(n1).perp();
                                int pa = mappositionA()[n1] + 1;
                                int pb = mappositionB()[n1] + 1;
                                int ptype = mapkinematic()[n1];
                                switch(ptype) {
                                    case 0: {
                                                HepMatrix ta(1, NTRKPAR, 0);
                                                ta[0][0] = -2 * pmis1.px() + 2 * pmis1.e() * p4Infit(n1).px() / p4Infit(n1).e();
                                                ta[0][1] = -2 * pmis1.py() + 2 * pmis1.e() * p4Infit(n1).py() / p4Infit(n1).e();
                                                ta[0][2] = -2 * pmis1.pz() + 2 * pmis1.e() * p4Infit(n1).pz() / p4Infit(n1).e();
                                                ta[0][3] = 2 * pmis1.e() *  p4Infit(n1).m() / p4Infit(n1).e();
                                                setA(m_nc,pa,ta);
                                                setAT(pa, m_nc, ta.T());
                                                break;
                                            }
                                    case 1 :  {
                                                  HepMatrix ta(1, NTRKPAR, 0);
                                                  double a1 = lambda * lambda + 1;
                                                  ta[0][0] = 2 * pmis1.px() * p4Infit(n1).py() - 2 * pmis1.py() * p4Infit(n1).px();
                                                  ta[0][1] = 2 * pmis1.px() * p4Infit(n1).px() * lambda/sqrt(a1) + 2 * pmis1.py() * (p4Infit(n1)).py() * lambda/sqrt(a1) - 2 * pmis1.pz() * (p4Infit(n1)).pz() /(sqrt(a1) * lambda);
                                                  ta[0][2] = 2 * pmis1.px() * (p4Infit(n1)).px() * p4Infit(n1).m() /((p4Infit(n1)).rho() * p4Infit(n1).rho()) + 2 * pmis1.py() * (p4Infit(n1)).py() * p4Infit(n1).m()/((p4Infit(n1)).rho() * p4Infit(n1).rho())+ 2 * pmis1.pz() * (p4Infit(n1)).pz() * p4Infit(n1).m()/((p4Infit(n1)).rho() * p4Infit(n1).rho());
                                                  ta[0][3] = 2 * pmis1.e()  - 2 * pmis1.px() * (p4Infit(n1)).px() * p4Infit(n1).e() /((p4Infit(n1)).rho() * p4Infit(n1).rho())- 2 * pmis1.py() * (p4Infit(n1)).py() * p4Infit(n1).e()/((p4Infit(n1)).rho() * p4Infit(n1).rho())- 2 * pmis1.pz() * (p4Infit(n1)).pz() * p4Infit(n1).e()/((p4Infit(n1)).rho() * p4Infit(n1).rho());
                                                  setA(m_nc,pa,ta);
                                                  setAT(pa, m_nc, ta.T());
                                                  break;
                                              }
                                    case 2 :  {
                                                  HepMatrix tb(1, 4, 0);
                                                  tb[0][0] = -2 * pmis1.px();
                                                  tb[0][1] = -2 * pmis1.py();
                                                  tb[0][2] = -2 * pmis1.pz();
                                                  tb[0][3] = 2 * pmis1.e();
                                                  setB(m_nc,pb,tb);
                                                  setBT(pb,m_nc,tb.T());
                                                  break;
                                              }
                                    case 3 :  {
                                                  HepMatrix ta(1, 1, 0);
                                                  ta[0][0] = 2 * pmis1.e() * (p4Infit(n1).m()/p4Infit(n1).e());
                                                  setA(m_nc,pa,ta);
                                                  setAT(pa, m_nc, ta.T());
                                                  HepMatrix tb(1, 3, 0);
                                                  tb[0][0] = -2 * pmis1.px();
                                                  tb[0][1] = -2 * pmis1.py();
                                                  tb[0][2] = -2 * pmis1.pz();
                                                  setB(m_nc,pb,tb);
                                                  setBT(pb,m_nc,tb.T());
                                                  break;
                                              }
                                    case 4 :  {
                                                  HepMatrix ta(1, 2, 0);
                                                  double a1 = lambda * lambda + 1;
                                                  ta[0][0] = 2 * pmis1.px() * p4Infit(n1).py() - 2 * pmis1.py() * p4Infit(n1).px();
                                                  ta[0][1] = 2 * pmis1.px() * p4Infit(n1).px() * lambda/sqrt(a1) + 2 * pmis1.py() * (p4Infit(n1)).py() * lambda/sqrt(a1) - 2 * pmis1.pz() * (p4Infit(n1)).pz() /(sqrt(a1) * lambda);
                                                  setA(m_nc,pa,ta);
                                                  setAT(pa, m_nc, ta.T());

                                                  HepMatrix tb(1, 2, 0);
                                                  tb[0][0] =  2 * pmis1.px() * (p4Infit(n1)).px() * p4Infit(n1).m() /((p4Infit(n1)).rho() * p4Infit(n1).rho()) + 2 * pmis1.py() * (p4Infit(n1)).py() * p4Infit(n1).m()/((p4Infit(n1)).rho() * p4Infit(n1).rho())+ 2 * pmis1.pz() * (p4Infit(n1)).pz() * p4Infit(n1).m()/((p4Infit(n1)).rho() * p4Infit(n1).rho());
                                                  tb[0][1] = 2 * pmis1.e()  - 2 * pmis1.px() * (p4Infit(n1)).px() * p4Infit(n1).e() /((p4Infit(n1)).rho() * p4Infit(n1).rho())- 2 * pmis1.py() * (p4Infit(n1)).py() * p4Infit(n1).e()/((p4Infit(n1)).rho() * p4Infit(n1).rho())- 2 * pmis1.pz() * (p4Infit(n1)).pz() * p4Infit(n1).e()/((p4Infit(n1)).rho() * p4Infit(n1).rho());
                                                  setB(m_nc,pb,tb);
                                                  setBT(pb, m_nc, tb.T());
                                                  break;
                                              }
                                    case 5 :  {
                                                  HepMatrix ta(1, 3, 0);
                                                  double a1 = lambda * lambda + 1;
                                                  ta[0][0] = 2 * pmis1.px() * p4Infit(n1).py() - 2 * pmis1.py() * p4Infit(n1).px();
                                                  ta[0][1] = 2 * pmis1.px() * p4Infit(n1).px() * lambda/sqrt(a1)+ 2 * pmis1.py() * (p4Infit(n1)).py() * lambda/sqrt(a1) - 2 * pmis1.pz() * (p4Infit(n1)).pz() /(sqrt(a1) * lambda);
                                                  ta[0][2] = 2 * pmis1.e() * (p4Infit(n1)).m()/(p4Infit(n1)).e();
                                                  setA(m_nc,pa,ta);
                                                  setAT(pa, m_nc, ta.T());
                                                  HepMatrix tb(1, 1, 0);
                                                  tb[0][0] = 2 * pmis1.e() * (p4Infit(n1)).rho()/(p4Infit(n1)).e() - 2 * pmis1.px() * (p4Infit(n1)).px()/(p4Infit(n1)).rho() - 2 * pmis1.py() * (p4Infit(n1)).py()/(p4Infit(n1)).rho() - 2 * pmis1.pz() * (p4Infit(n1)).pz()/(p4Infit(n1)).rho();
                                                  setB(m_nc,pb,tb);
                                                  setBT(pb, m_nc, tb.T());
                                                  break;
                                              }       
                                    case 7: {
                                                HepMatrix ta(1, NTRKPAR, 0);
                                                ta[0][0] = -2 * pmis1.px() + 2 * pmis1.e() * p4Infit(n1).px() / p4Infit(n1).e();
                                                ta[0][1] = -2 * pmis1.py() + 2 * pmis1.e() * p4Infit(n1).py() / p4Infit(n1).e();
                                                ta[0][2] = -2 * pmis1.pz() + 2 * pmis1.e() * p4Infit(n1).pz() / p4Infit(n1).e();
                                                ta[0][3] = 2 * pmis1.e() *  p4Infit(n1).m() / p4Infit(n1).e();
                                                setA(m_nc,pa,ta);
                                                setAT(pa, m_nc, ta.T());
                                                break;
                                            }
                                }   
                            }

                            for(int i = isiz; i < isiz+jsiz; i++) {
                                int n2 = (kc.Ltrk())[i];
                                double lambda = p4Infit(n2).pz()/p4Infit(n2).perp();
                                int pa = mappositionA()[n2] + 1;
                                int pb = mappositionB()[n2] + 1;
                                int ptype = mapkinematic()[n2];
                                switch(ptype) {
                                    case 0 : {
                                                 HepMatrix ta(1, NTRKPAR, 0);
                                                 ta[0][0] = -2 * pmis2.px() + 2 * pmis2.e() * p4Infit(n2).px() / p4Infit(n2).e();
                                                 ta[0][1] = -2 * pmis2.py() + 2 * pmis2.e() * p4Infit(n2).py() / p4Infit(n2).e();
                                                 ta[0][2] = -2 * pmis2.pz() + 2 * pmis2.e() * p4Infit(n2).pz() / p4Infit(n2).e();
                                                 ta[0][3] = 2 * pmis2.e() * p4Infit(n2).m() / p4Infit(n2).e();
                                                 setA(m_nc,pa,-ta);
                                                 setAT(pa, m_nc, -ta.T());
                                                 break;
                                             }
                                    case 1 :  {
                                                  HepMatrix ta(1, NTRKPAR, 0);
                                                  double a1 = lambda * lambda + 1;
                                                  ta[0][0] = 2 * pmis2.px() * p4Infit(n2).py() - 2 * pmis2.py() * p4Infit(n2).px();
                                                  ta[0][1] = 2 * pmis2.px() * p4Infit(n2).px() * lambda/sqrt(a1) + 2 * pmis2.py() * (p4Infit(n2)).py() * lambda/sqrt(a1) - 2 * pmis2.pz() * (p4Infit(n2)).pz() /(sqrt(a1) * lambda);
                                                  ta[0][2] =  2 * pmis2.px() * (p4Infit(n2)).px() * p4Infit(n2).m() /((p4Infit(n2)).rho() * p4Infit(n2).rho()) + 2 * pmis2.py() * (p4Infit(n2)).py() * p4Infit(n2).m()/((p4Infit(n2)).rho() * p4Infit(n2).rho())+ 2 * pmis2.pz() * (p4Infit(n2)).pz() * p4Infit(n2).m()/((p4Infit(n2)).rho() * p4Infit(n2).rho());
                                                  ta[0][3] = 2 * pmis2.e()  - 2 * pmis2.px() * (p4Infit(n2)).px() * p4Infit(n2).e() /((p4Infit(n2)).rho() * p4Infit(n2).rho())- 2 * pmis2.py() * (p4Infit(n2)).py() * p4Infit(n2).e()/((p4Infit(n2)).rho() * p4Infit(n2).rho())- 2 * pmis2.pz() * (p4Infit(n2)).pz() * p4Infit(n2).e()/((p4Infit(n2)).rho() * p4Infit(n2).rho());
                                                  setA(m_nc,pa,-ta);
                                                  setAT(pa, m_nc, -ta.T());
                                                  break;
                                              }
                                    case 2 :  {
                                                  HepMatrix tb(1, 4, 0);
                                                  tb[0][0] = -2 * pmis2.px();
                                                  tb[0][1] = -2 * pmis2.py();
                                                  tb[0][2] = -2 * pmis2.pz();
                                                  tb[0][3] = 2 * pmis2.e();
                                                  setB(m_nc,pb,-tb);
                                                  setBT(pb,m_nc,-tb.T());
                                                  break;
                                              }
                                    case 3 :  {
                                                  HepMatrix ta(1, 1, 0);
                                                  ta[0][0] = 2 * pmis2.e() * (p4Infit(n2).m()/p4Infit(n2).e());
                                                  setA(m_nc,pa,-ta);
                                                  setAT(pa, m_nc, -ta.T());
                                                  HepMatrix tb(1, 3, 0);
                                                  tb[0][0] = -2 * pmis2.px();
                                                  tb[0][1] = -2 * pmis2.py();
                                                  tb[0][2] = -2 * pmis2.pz();
                                                  setB(m_nc,pb,-tb);
                                                  setBT(pb,m_nc,-tb.T());
                                                  break;
                                              }
                                    case 4 :  {
                                                  HepMatrix ta(1, 2, 0);
                                                  double a1 = lambda * lambda + 1;
                                                  ta[0][0] = 2 * pmis2.px() * p4Infit(n2).py() - 2 * pmis2.py() * p4Infit(n2).px();
                                                  ta[0][1] = 2 * pmis2.px() * p4Infit(n2).px() * lambda/sqrt(a1) + 2 * pmis2.py() * (p4Infit(n2)).py() * lambda/sqrt(a1) - 2 * pmis2.pz() * (p4Infit(n2)).pz() /(sqrt(a1) * lambda);
                                                  setA(m_nc,pa,-ta);
                                                  setAT(pa, m_nc, -ta.T());

                                                  HepMatrix tb(1, 2, 0);
                                                  tb[0][0] =  2 * pmis2.px() * (p4Infit(n2)).px() * p4Infit(n2).m() /((p4Infit(n2)).rho() * p4Infit(n2).rho()) + 2 * pmis2.py() * (p4Infit(n2)).py() * p4Infit(n2).m()/((p4Infit(n2)).rho() * p4Infit(n2).rho())+ 2 * pmis2.pz() * (p4Infit(n2)).pz() * p4Infit(n2).m()/((p4Infit(n2)).rho() * p4Infit(n2).rho());
                                                  tb[0][1] = 2 * pmis2.e()  - 2 * pmis2.px() * (p4Infit(n2)).px() * p4Infit(n2).e() /((p4Infit(n2)).rho() * p4Infit(n2).rho())- 2 * pmis2.py() * (p4Infit(n2)).py() * p4Infit(n2).e()/((p4Infit(n2)).rho() * p4Infit(n2).rho())- 2 * pmis2.pz() * (p4Infit(n2)).pz() * p4Infit(n2).e()/((p4Infit(n2)).rho() * p4Infit(n2).rho());
                                                  setB(m_nc,pb,-tb);
                                                  setBT(pb, m_nc, -tb.T());
                                                  break;
                                              }
                                    case 5 :  {
                                                  HepMatrix ta(1, 3, 0);
                                                  double a1 = lambda * lambda + 1;
                                                  ta[0][0] = 2 * pmis2.px() * p4Infit(n2).py() - 2 * pmis2.py() * p4Infit(n2).px();
                                                  ta[0][1] = 2 * pmis2.px() * p4Infit(n2).px() * lambda/sqrt(a1) + 2 * pmis2.py() * (p4Infit(n2)).py() * lambda/sqrt(a1) - 2 * pmis2.pz() * (p4Infit(n2)).pz() /(sqrt(a1) * lambda);
                                                  ta[0][2] = 2 * pmis2.e() * (p4Infit(n2)).m()/(p4Infit(n2)).e();
                                                  setA(m_nc,pa,-ta);
                                                  setAT(pa, m_nc, -ta.T());
                                                  HepMatrix tb(1, 1, 0);
                                                  tb[0][0] = 2 * pmis2.e() * (p4Infit(n2)).rho()/(p4Infit(n2)).e() - 2 * pmis2.px() * (p4Infit(n2)).px()/(p4Infit(n2)).rho() - 2 * pmis2.py() * (p4Infit(n2)).py()/(p4Infit(n2)).rho() - 2 * pmis2.pz() * (p4Infit(n2)).pz()/(p4Infit(n2)).rho();
                                                  setB(m_nc,pb,-tb);
                                                  setBT(pb, m_nc, -tb.T());
                                                  break;
                                              }       
                                    case 7 : {
                                                 HepMatrix ta(1, NTRKPAR, 0);
                                                 ta[0][0] = -2 * pmis2.px() + 2 * pmis2.e() * p4Infit(n2).px() / p4Infit(n2).e();
                                                 ta[0][1] = -2 * pmis2.py() + 2 * pmis2.e() * p4Infit(n2).py() / p4Infit(n2).e();
                                                 ta[0][2] = -2 * pmis2.pz() + 2 * pmis2.e() * p4Infit(n2).pz() / p4Infit(n2).e();
                                                 ta[0][3] = 2 * pmis2.e() * p4Infit(n2).m() / p4Infit(n2).e();
                                                 setA(m_nc,pa,-ta);
                                                 setAT(pa, m_nc, -ta.T());
                                                 break;
                                             }
                                }   
                            } 




                            HepVector dc(1, 0);
                            dc[0] = pmis1.m2() - pmis2.m2();
                            m_G[m_nc] = dc[0];

                            m_nc+=1;

                            break;
                        }


        case FourMomentum: 
        default: {
                     //
                     //  px - p4x = 0
                     //  py - p4y = 0
                     //  pz - p4z = 0
                     //  e  - p4e = 0
                     //
                     HepLorentzVector p4 = kc.p4();
                     HepLorentzVector pmis;
                     for(unsigned int j = 0; j< (kc.Ltrk()).size(); j++){
                         int n = (kc.Ltrk())[j];
                         pmis = pmis + p4Infit(n);
                     }
                     for(unsigned int j = 0; j < (kc.Ltrk()).size(); j++) {
                         int n = (kc.Ltrk())[j];
                         double lambda = p4Infit(n).pz()/p4Infit(n).perp();
                         int pa = mappositionA()[n] + 1;
                         int pb = mappositionB()[n] + 1;
                         int ptype = mapkinematic()[n];
                         switch(ptype) {
                             case 0 : {
                                          HepMatrix ta(kc.nc(), NTRKPAR, 0);
                                          ta[0][0] = 1.0;
                                          ta[1][1] = 1.0;
                                          ta[2][2] = 1.0;
                                          ta[3][0] = p4Infit(n).px() / p4Infit(n).e();
                                          ta[3][1] = p4Infit(n).py() / p4Infit(n).e();
                                          ta[3][2] = p4Infit(n).pz() / p4Infit(n).e();
                                          ta[3][3] = p4Infit(n).m() / p4Infit(n).e();
                                          setA(m_nc, pa, ta);
                                          setAT(pa, m_nc, ta.T());
                                          break;
                                      }
                             case 1 : {
                                          HepMatrix ta(kc.nc(), NTRKPAR, 0);
                                          ta[0][0] = -p4Infit(n).py();
                                          ta[0][1] = -p4Infit(n).px()*(lambda/(1 + lambda*lambda));
                                          ta[0][2] = -p4Infit(n).m()*p4Infit(n).px()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          ta[0][3] = p4Infit(n).e()*p4Infit(n).px()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          ta[1][0] = p4Infit(n).px();
                                          ta[1][1] = -p4Infit(n).py()*(lambda/(1 + lambda*lambda));
                                          ta[1][2] = -p4Infit(n).m()*p4Infit(n).py()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          ta[1][3] = p4Infit(n).e()*p4Infit(n).py()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          ta[2][0] = 0;
                                          ta[2][1] = p4Infit(n).pz()*(1/(lambda * (1 + lambda*lambda)));
                                          ta[2][2] = -p4Infit(n).m()*p4Infit(n).pz()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          ta[2][3] = p4Infit(n).e()*p4Infit(n).pz()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          ta[3][0] = 0;
                                          ta[3][1] = 0;
                                          ta[3][2] = 0;
                                          ta[3][3] = 1.0;
                                          setA(m_nc, pa, ta);
                                          setAT(pa, m_nc, ta.T()); 
                                          break;
                                      }
                             case 2 : {
                                          HepMatrix tb(kc.nc(), 4, 0);
                                          tb[0][0] = 1.0;
                                          tb[1][1] = 1.0;
                                          tb[2][2] = 1.0;
                                          tb[3][3] = 1.0;
                                          setB(m_nc, pb, tb);
                                          setBT(pb, m_nc, tb.T());
                                          break;
                                      }
                             case 3 : {
                                          HepMatrix ta(kc.nc(), 1, 0);
                                          ta[0][0] = -p4Infit(n).m()*p4Infit(n).px()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          ta[1][0] = -p4Infit(n).m()*p4Infit(n).py()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          ta[2][0] = -p4Infit(n).m()*p4Infit(n).pz()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          ta[3][0] = 0;
                                          setA(m_nc, pa, ta);
                                          setAT(pa, m_nc, ta.T()); 

                                          HepMatrix tb(kc.nc(), 3, 0);
                                          tb[0][0] = -p4Infit(n).py();
                                          tb[0][1] = -p4Infit(n).px()*(lambda/(1 + lambda*lambda));
                                          tb[0][2] = p4Infit(n).e()*p4Infit(n).px()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          tb[1][0] = p4Infit(n).px();
                                          tb[1][1] = -p4Infit(n).py()*(lambda/(1 + lambda*lambda));
                                          tb[1][2] = p4Infit(n).e()*p4Infit(n).py()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          tb[2][0] = 0;
                                          tb[2][1] = p4Infit(n).pz()*(1/(lambda * (1 + lambda*lambda)));
                                          tb[2][2] = p4Infit(n).e()*p4Infit(n).pz()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          tb[3][0] = 0;
                                          tb[3][1] = 0;
                                          tb[3][2] = 1.0;

                                          setB(m_nc, pb, tb);
                                          setBT(pb, m_nc, tb.T());

                                          break;
                                      }
                             case 4 : {
                                          HepMatrix ta(kc.nc(), 2, 0);
                                          ta[0][0] = -p4Infit(n).py(); 
                                          ta[0][1] = -p4Infit(n).px()*(lambda/sqrt(1 + lambda*lambda));
                                          ta[1][0] =  p4Infit(n).px(); 
                                          ta[1][1] = -p4Infit(n).py()*(lambda/sqrt(1 + lambda*lambda));
                                          ta[2][0] =  0;
                                          ta[2][1] =  p4Infit(n).pz()*(1/(lambda * (1 + lambda*lambda)));

                                          setA(m_nc, pa, ta);
                                          setAT(pa, m_nc, ta.T()); 

                                          HepMatrix tb(kc.nc(), 2, 0);
                                          //  tb[3][0] = p4Infit(n).m()/p4Infit(n).e();
                                          //  tb[0][1] = p4Infit(n).px()/p4Infit(n).rho();
                                          //  tb[1][1] = p4Infit(n).py()/p4Infit(n).rho();
                                          //  tb[2][1] = p4Infit(n).pz()/p4Infit(n).rho();
                                          //  tb[3][1] = p4Infit(n).rho()/p4Infit(n).e();
                                          tb[0][0] = -p4Infit(n).m()*p4Infit(n).px()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          tb[1][0] = -p4Infit(n).m()*p4Infit(n).py()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          tb[2][0] = -p4Infit(n).m()*p4Infit(n).pz()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          tb[0][1] = p4Infit(n).e()*p4Infit(n).px()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          tb[1][1] = p4Infit(n).e()*p4Infit(n).py()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          tb[2][1] = p4Infit(n).e()*p4Infit(n).pz()/(p4Infit(n).rho()*p4Infit(n).rho());
                                          tb[3][1] = 1;
                                          setB(m_nc, pb, tb);
                                          setBT(pb, m_nc, tb.T());
                                          break;
                                      }
                             case 5 : {
                                          HepMatrix ta(kc.nc(), 3, 0);
                                          ta[0][0] = -p4Infit(n).py();
                                          ta[0][1] = -p4Infit(n).px()*(lambda/sqrt(1 + lambda*lambda));
                                          ta[1][0] =  p4Infit(n).px(); 
                                          ta[1][1] = -p4Infit(n).py()*(lambda/sqrt(1 + lambda*lambda));
                                          ta[2][0] =  0;
                                          ta[2][1] =  p4Infit(n).pz()*(1/(lambda * (1 + lambda*lambda)));
                                          ta[3][2] =  p4Infit(n).m()/p4Infit(n).e();
                                          setA(m_nc, pa, ta);
                                          setAT(pa, m_nc, ta.T()); 

                                          HepMatrix tb(kc.nc(), 1, 0);
                                          tb[0][0] = p4Infit(n).px()/p4Infit(n).rho();
                                          tb[1][0] = p4Infit(n).py()/p4Infit(n).rho();
                                          tb[2][0] = p4Infit(n).pz()/p4Infit(n).rho();
                                          tb[3][0] = p4Infit(n).rho()/p4Infit(n).e();
                                          setB(m_nc, pb, tb);
                                          setBT(pb, m_nc, tb.T());
                                          break;
                                      }
                             case 6 : {
                                          double ptrk = m_p.sub(pa+3, pa+3)[0];
                                          double x = m_p.sub(pa, pa)[0] - m_q.sub(pb, pb)[0];
                                          double y = m_p.sub(pa+1, pa+1)[0] - m_q.sub(pb+1, pb+1)[0];
                                          double z = m_p.sub(pa+2, pa+2)[0] - m_q.sub(pb+2, pb+2)[0];
                                          double R = sqrt(x*x + y*y + z*z);
                                          HepMatrix ta(kc.nc(), 4, 0);
                                          ta[0][0] = ptrk*(y*y + z*z)/(R*R*R);
                                          ta[0][1] = -ptrk*x*y/(R*R*R);
                                          ta[0][2] = -ptrk*x*z/(R*R*R);
                                          ta[0][3] = x/R;
                                          ta[1][0] = -ptrk*y*x/(R*R*R);
                                          ta[1][1] = ptrk*(x*x + z*z)/(R*R*R);
                                          ta[1][2] = -ptrk*y*z/(R*R*R);
                                          ta[1][3] = y/R;
                                          ta[2][0] = -ptrk*z*x/(R*R*R);
                                          ta[2][1] = -ptrk*z*y/(R*R*R);
                                          ta[2][2] = ptrk*(x*x + y*y)/(R*R*R);
                                          ta[2][3] = z/R;
                                          ta[3][3] = 1;
                                          setA(m_nc, pa, ta);
                                          setAT(pa, m_nc, ta.T());

                                          HepMatrix tb(kc.nc(), 3, 0);
                                          tb[0][0] = -ptrk*(y*y + z*z)/(R*R*R);
                                          tb[0][1] = ptrk*x*y/(R*R*R);
                                          tb[0][2] = ptrk*x*z/(R*R*R);
                                          tb[1][0] = ptrk*y*x/(R*R*R);
                                          tb[1][1] = -ptrk*(x*x + z*z)/(R*R*R);
                                          tb[1][2] = ptrk*y*z/(R*R*R);
                                          tb[2][0] = ptrk*z*x/(R*R*R);
                                          tb[2][1] = ptrk*z*y/(R*R*R);
                                          tb[2][2] = -ptrk*(x*x + y*y)/(R*R*R);
                                          HepMatrix tbp = m_B.sub(1, 4, 1, 3);
                                          tb = tbp + tb;
                                          setB(m_nc, pb, tb);
                                          setBT(pb, m_nc, tb.T());
                                          break;
                                      }
                             case 7 : {
                                          HepMatrix ta(kc.nc(), NTRKPAR, 0);
                                          ta[0][0] = 1.0;
                                          ta[1][1] = 1.0;
                                          ta[2][2] = 1.0;
                                          ta[3][0] = p4Infit(n).px() / p4Infit(n).e();
                                          ta[3][1] = p4Infit(n).py() / p4Infit(n).e();
                                          ta[3][2] = p4Infit(n).pz() / p4Infit(n).e();
                                          ta[3][3] = p4Infit(n).m() / p4Infit(n).e();
                                          setA(m_nc, pa, ta);
                                          setAT(pa, m_nc, ta.T());
                                          break;
                                      }
                         }
                     }  

                     HepVector dc(kc.nc(), 0);
                     dc[0] = pmis.px()  - p4.px();
                     dc[1] = pmis.py()  - p4.py();
                     dc[2] = pmis.pz()  - p4.pz();
                     dc[3] = pmis.e()  - p4.e();
                     for(int i = 0; i < kc.nc(); i++) {
                         m_G[m_nc+i] = dc[i];
                     }      	 

                     m_nc += 4;

                     break;
                 }
    }
}


