#include "VertexFit/KinematicFit.h"
#include "VertexFit/KinematicConstraints.h"
#include "VertexFit/HTrackParameter.h"
#include "TStopwatch.h"


const int KinematicFit::NTRKPAR = 3;        //modify by yanl 2010.7.26  4---->3

const int KinematicFit::Resonance = 1;
const int KinematicFit::TotalEnergy = 2;
const int KinematicFit::TotalMomentum = 4;
const int KinematicFit::Position = 8;
const int KinematicFit::ThreeMomentum = 16;
const int KinematicFit::FourMomentum = 32;
const int KinematicFit::EqualMass = 64;


KinematicFit * KinematicFit::m_pointer = 0;

KinematicFit * KinematicFit::instance() {
    if(m_pointer) return m_pointer;
    m_pointer = new KinematicFit();
    return m_pointer;
}

KinematicFit::KinematicFit(){;}

KinematicFit::~KinematicFit() {
    //  if(m_pointer) delete m_pointer;
}


void KinematicFit::init() {
    clearWTrackOrigin();
    clearWTrackInfit();
    clearWTrackList();
    //For Virtual Particles
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
    m_kalman = 0;
    m_collideangle = 11e-3;
    m_flag = 0;
    m_dynamicerror = 0; 
    m_nc = 0;
    m_cpu = HepVector(10, 0);
    m_massvector = HepVector(12,0);
    m_virtual_wtrk.clear(); 
}


//
//  Add Resonance
//

void KinematicFit::AddResonance(int number, double mres, int n1) {
    std::vector<int> tlis = AddList(n1);
    AddResonance(number, mres, tlis);
}

void KinematicFit::AddResonance(int number, double mres, int n1, int n2) {
    std::vector<int> tlis = AddList(n1, n2);
    AddResonance(number, mres, tlis);
}

void KinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3) {
    std::vector<int> tlis = AddList(n1, n2, n3);
    AddResonance(number, mres, tlis);
}

void KinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4);
    AddResonance(number, mres, tlis);
}

void KinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5);
    AddResonance(number, mres, tlis);
}

void KinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6);
    AddResonance(number, mres, tlis);
}

void KinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7);
    AddResonance(number, mres, tlis);
}

void KinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8);
    AddResonance(number, mres, tlis);
}

void KinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9);
    AddResonance(number, mres, tlis);
}

void KinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9, int n10) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);
    AddResonance(number, mres, tlis);
}


void KinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
    AddResonance(number, mres, tlis);
}

void KinematicFit::AddResonance(int number, double mres, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11, int n12) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12);
    AddResonance(number, mres, tlis);
}

void KinematicFit::AddResonance(int number, double mres, std::vector<int> tlis) {
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

void KinematicFit::AddTotalMomentum(int number, double ptot, int n1) {
    std::vector<int> tlis = AddList(n1);
    AddTotalMomentum(number, ptot, tlis);
}
void KinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2) {
    std::vector<int> tlis = AddList(n1, n2);
    AddTotalMomentum(number, ptot, tlis);
}

void KinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3) {
    std::vector<int> tlis = AddList(n1, n2, n3);
    AddTotalMomentum(number, ptot, tlis);
}

void KinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4);
    AddTotalMomentum(number, ptot, tlis);
}

void KinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5);
    AddTotalMomentum(number, ptot, tlis);
}

void KinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6);
    AddTotalMomentum(number, ptot, tlis);
}

void KinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7);
    AddTotalMomentum(number, ptot, tlis);
}

void KinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8);
    AddTotalMomentum(number, ptot, tlis);
}

void KinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9);
    AddTotalMomentum(number, ptot, tlis);
}

void KinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);
    AddTotalMomentum(number, ptot, tlis);
}


void KinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
    AddTotalMomentum(number, ptot, tlis);
}

void KinematicFit::AddTotalMomentum(int number, double ptot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11, int n12) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12);
    AddTotalMomentum(number, ptot, tlis);
}

void KinematicFit::AddTotalMomentum(int number, double ptot, std::vector<int> tlis) {
    KinematicConstraints kc;
    kc.TotalMomentumConstraints(ptot, tlis);
    m_kc.push_back(kc);
    if((unsigned int) number != m_kc.size()-1) 
        std::cout << "wrong kinematic constraints index" << std::endl;
}

//
//  Add TotalEnergy
//

void KinematicFit::AddTotalEnergy(int number, double etot, int n1) {
    std::vector<int> tlis = AddList(n1);
    AddTotalEnergy(number, etot, tlis);
}
void KinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2) {
    std::vector<int> tlis = AddList(n1, n2);
    AddTotalEnergy(number, etot, tlis);
}

void KinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3) {
    std::vector<int> tlis = AddList(n1, n2, n3);
    AddTotalEnergy(number, etot, tlis);
}

void KinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4);
    AddTotalEnergy(number, etot, tlis);
}

void KinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5);
    AddTotalEnergy(number, etot, tlis);
}

void KinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6);
    AddTotalEnergy(number, etot, tlis);
}

void KinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7);
    AddTotalEnergy(number, etot, tlis);
}

void KinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8);
    AddTotalEnergy(number, etot, tlis);
}

void KinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9);
    AddTotalEnergy(number, etot, tlis);
}

void KinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);
    AddTotalEnergy(number, etot, tlis);
}


void KinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
    AddTotalEnergy(number, etot, tlis);
}

void KinematicFit::AddTotalEnergy(int number, double etot, int n1, int n2, int n3, int n4,
        int n5, int n6, int n7, int n8, int n9,
        int n10, int n11, int n12) {
    std::vector<int> tlis = AddList(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12);
    AddTotalEnergy(number, etot, tlis);
}

void KinematicFit::AddTotalEnergy(int number, double etot, std::vector<int> tlis) {
    KinematicConstraints kc;
    kc.TotalEnergyConstraints(etot, tlis);
    m_kc.push_back(kc);
    if((unsigned int) number != m_kc.size()-1) 
        std::cout << "wrong kinematic constraints index" << std::endl;
}
//
//  Equal Mass constraints
//

void KinematicFit::AddEqualMass(int number, std::vector<int> tlis1, std::vector<int> tlis2)  {
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

void KinematicFit::AddThreeMomentum(int number, Hep3Vector p3) {
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

void KinematicFit::AddFourMomentum(int number, HepLorentzVector p4) {

    std::vector<int> tlis;
    tlis.clear();
    KinematicConstraints kc;

    for(int i = 0; i < numberWTrack(); i++) {
        tlis.push_back(i);
    }
    //  for(int i = 0; i < numberWTrack_V(); i++) {
    //    tlis_V.push_back(i);
    //  }

    HepSymMatrix Vme = HepSymMatrix(4,0);
    Vme[0][0] = 2*m_espread*m_espread*sin(m_collideangle)*sin(m_collideangle);
    Vme[0][3] = 2*m_espread*m_espread*sin(m_collideangle);
    Vme[3][3] = 2*m_espread*m_espread;

    //  kc.FourMomentumConstraints(p4, tlis, Vme);
    kc.FourMomentumConstraints(p4, tlis, Vme);
    m_kc.push_back(kc);
    if((unsigned int) number != m_kc.size()-1) 
        std::cout << "wrong kinematic constraints index" << std::endl;
}

void KinematicFit::AddFourMomentum(int number, double etot ){

    HepLorentzVector p4(0.0, 0.0, 0.0, etot);
    std::vector<int> tlis;
    tlis.clear();
    KinematicConstraints kc;

    for(int i = 0; i < numberWTrack(); i++) {
        tlis.push_back(i);
    }
    HepSymMatrix Vme = HepSymMatrix (4,0);
    Vme[3][3] = 2*m_espread*m_espread;
    // kc.FourMomentumConstraints(p4, tlis, Vme);
    kc.FourMomentumConstraints(p4, tlis, Vme);
    m_kc.push_back(kc);
    if((unsigned int) number != m_kc.size()-1) 
        std::cout << "wrong kinematic constraints index" << std::endl;
}
/*
   void KinematicFit::AddPosition(int number, HepPoint3D xorigin, std::vector<int> tlis_V){
   KinematicConstraints kc;
   HepSymMatrix Vpe = HepSymMatrix(2,0);
   HepSymMatrix Vclus = HepSymMatrix (3,0);
   Vclus[0][0] = wTrackOrigin_V(tlis_V[0]).Ew()[4][4];
   Vclus[0][1] = wTrackOrigin_V(tlis_V[0]).Ew()[4][5];
   Vclus[1][1] = wTrackOrigin_V(tlis_V[0]).Ew()[5][5];
   Vclus[2][2] = wTrackOrigin_V(tlis_V[0]).Ew()[6][6];
   HepMatrix p(2,3,0);
   p[0][0] = wTrackOrigin_V(tlis_V[0]).w()[1];
   p[0][1] = -wTrackOrigin_V(tlis_V[0]).w()[0];
   p[1][0] = wTrackOrigin_V(tlis_V[0]).w()[2];
   p[1][2] = -wTrackOrigin_V(tlis_V[0]).w()[0];
   Vpe = Vclus.similarity(p);	

   kc.PositionConstraints(xorigin, tlis_V, Vpe);
   m_kc.push_back(kc);
   if((unsigned int) number != m_kc.size()-1) 
   std::cout << "wrong kinematic constraints index" << std::endl;
   }     
   */



void KinematicFit::fit() {

    KinematicConstraints kc;
    int nc = m_nc;
    int ntrk = numberWTrack();

    TStopwatch timer;
    timer.Start();

    m_VD = HepSymMatrix(m_nc, 0);
    m_VD = m_covOrigin.similarity(m_D);
    timer.Stop();
    m_cpu[1] += timer.CpuTime();
    timer.Start();
    
    int ifail;
    m_VD.invert(ifail);
    
    timer.Stop();
    m_cpu[2] += timer.CpuTime();
    timer.Start();

    HepVector G(m_nc, 0);
    G = m_D * (m_pOrigin - m_pInfit) + m_d; 

    m_KP = HepMatrix(NTRKPAR*m_nktrk, m_nc, 0);
    m_KP = m_covOrigin * m_DT * m_VD;
    m_chi = (m_VD.similarity(G.T()))[0][0];


    timer.Stop();
    m_cpu[3] += timer.CpuTime();
    timer.Start();

    m_pInfit = m_pOrigin - m_KP * G;

    //  ===== gamma dynamic adjustment====

    timer.Stop();
    m_cpu[4] += timer.CpuTime();

    timer.Start();
    if(m_dynamicerror ==1) 
        gda(); 

    timer.Stop();
    m_cpu[6] += timer.CpuTime();
}





void KinematicFit::upCovmtx() {

    int ntrk = numberWTrack();
    HepSymMatrix I(NTRKPAR*m_nktrk, 1);
    m_covInfit = m_covOrigin.similarity(I - m_KP*m_D);
    for(int i = 0; i < m_nktrk; i++) {
        HepSymMatrix ew3 (3, 0);          
        HepSymMatrix ew6 (6, 0);
        HepSymMatrix Ew1 (7, 0);
        ew3 = m_covInfit.sub(i*NTRKPAR + 1, (i+1)*NTRKPAR);
        for(int j = 0; j < NTRKPAR; j++) {
            for(int k = 0; k < NTRKPAR; k++) {
                ew6[j][k] = ew3[j][k];
            }
        }
        for(int m = NTRKPAR; m < 6; m++) {
            for(int n = NTRKPAR; n < 6; n++) {
                ew6[m][n] = 0;//(wTrackOrigin(i).Ew())[m+1][n+1];
            }
        }
        double px = p4Infit(i).px();
        double py = p4Infit(i).py();
        double pz = p4Infit(i).pz();
        double m =  p4Infit(i).m();
        double e =  p4Infit(i).e();

        HepMatrix J(7, 6, 0);
        J[0][0] = 1;
        J[1][1] = 1;
        J[2][2] = 1;
        J[3][0] = px/e;
        J[3][1] = py/e;
        J[3][2] = pz/e;
        J[4][3] = 1;
        J[5][4] = 1;
        J[6][5] = 1;
        Ew1 = ew6.similarity(J);

        HepVector W(7, 0);
        for(int j=0; j<4; j++) {
            W[j] = p4Infit(i)[j];
        }
        W[4] = wTrackOrigin(i).w()[4];
        W[5] = wTrackOrigin(i).w()[5];
        W[6] = wTrackOrigin(i).w()[6];
        

        WTrackParameter wtrk = wTrackInfit(i);
        wtrk.setEw(Ew1);
        wtrk.setW(W);
        setWTrackInfit(i, wtrk);
    }


}    

   

void KinematicFit::fit(int n) {

    if(m_chisq.size() == 0) {
        for(unsigned int i = 0; i < m_kc.size(); i++)
            m_chisq.push_back(9999.0);
    }
    KinematicConstraints kc;
    int nc = m_nc;
    int ntrk = numberWTrack();

    m_VD = HepSymMatrix(m_nc, 0);
    m_VD = m_covOrigin.similarity(m_D);
    int ifail;
    m_VD.invert(ifail);
    HepVector G(m_nc, 0);
    G = m_D * (m_pOrigin - m_pInfit) + m_d; 
    m_KP = HepMatrix(NTRKPAR*m_nktrk, m_nc, 0);
    m_KP = m_covOrigin * m_DT * m_VD;
    m_chisq[n]  = (m_VD.similarity(G.T()))[0][0];
    m_pInfit = m_pOrigin - m_KP * G;

}



void KinematicFit::covMatrix(int n) {
    KinematicConstraints kc = m_kc[n];
    int nc = kc.nc();
    int ntrk = (kc.Ltrk()).size();
    HepSymMatrix Va0(7*ntrk, 0);
    for (int i = 0; i < ntrk; i++) {
        int itk = (kc.Ltrk())[i];
        for(int j = 0; j < 7; j++) 
            for (int k = 0; k < 7; k++)
                Va0[7*i+j][7*i+k] = (wTrackOrigin(itk).Ew())[j][k];

    }
    HepMatrix D(nc, 7*ntrk, 0);
    int ncc = 0;
    for(int j = 0; j < kc.nc(); j++) {
        for(int l = 0; l < ntrk; l++) { 
            for(int k = 0; k < 7; k++)
                D[ncc][7*l+k] = (kc.Dc()[l])[j][k];
        }
        ncc++;
    }


    HepSymMatrix VD(nc, 0);
    VD = kc.VD()[0];
    HepSymMatrix Va(7*ntrk, 0);
    Va = Va0 - (VD.similarity(D.T())).similarity(Va0);
    for(int i = 0; i < ntrk; i++) {
        int itk = (kc.Ltrk())[i];
        HepVector w(7, 0);
        HepSymMatrix Ew(7, 0);
        for(int j = 0; j < 7; j++) {
            for(int k = 0; k < 7; k++) 
                Ew[j][k] = Va[7*i + j] [7*i + k];
        }
        WTrackParameter wtrk = wTrackInfit(itk);
        wtrk.setEw(Ew);
        setWTrackInfit(itk, wtrk);
    }
    m_kc[n] = kc;

}




bool KinematicFit::Fit() {
    bool okfit = false;
    TStopwatch timer;
    m_nktrk = numberWTrack();
    m_pOrigin = HepVector(m_nktrk * NTRKPAR, 0);
    m_pInfit = HepVector(m_nktrk * NTRKPAR, 0);
    m_covOrigin = HepSymMatrix(m_nktrk * NTRKPAR, 0);
    m_covInfit = HepSymMatrix(m_nktrk * NTRKPAR, 0);
    m_massvector = HepVector(m_nktrk, 0);
    for(int i = 0; i < numberWTrack(); i++) {
        setWTrackInfit(i, wTrackOrigin(i));
        setPOrigin(i, (wTrackOrigin(i).w()).sub(1, NTRKPAR));
        setPInfit(i, (wTrackOrigin(i).w()).sub(1, NTRKPAR));
        setCovOrigin(i, (wTrackOrigin(i).Ew()).sub(1, NTRKPAR));
        setMassvector(i, wTrackOrigin(i).mass());
    }
    
    //
    // iteration
    //
    //    cout<<"m_pInfit ="<<m_pInfit<<endl;
    //    cout<<"m_covOrigin="<<m_covOrigin<<endl;
    //    cout<<"m_massvector ="<<m_massvector<<endl;

    std::vector<double> chisq;
    chisq.clear();
    int nc = 0;
    for(int i = 0; i < m_kc.size(); i++)
        nc += m_kc[i].nc();

    m_D = HepMatrix(nc, m_nktrk * NTRKPAR, 0);
    m_DT = HepMatrix(m_nktrk * NTRKPAR, nc, 0);
    m_d = HepVector(nc, 0);

    for(int it = 0; it < m_niter; it++) {

        timer.Start();
        m_nc = 0;
        for(unsigned int i = 0; i < m_kc.size(); i++) {
            KinematicConstraints kc = m_kc[i];
            //      std::vector<WTrackParameter> wlis;
            //      std::vector<WTrackParameter> wlis_V;
            //      wlis.clear();
            //      wlis_V.clear();
            //      for(unsigned int j = 0; j < (kc.Ltrk()).size(); j++) {
            //	int n = (kc.Ltrk())[j];
            //	WTrackParameter wtrk = wTrackInfit(n);
            //       if(m_espread!=0)	 wtrk = wTrackOrigin(n);
            //	wlis.push_back(wtrk);
            //      } 
            //      for(unsigned int j = 0; j < (kc.Ltrk_V()).size(); j++) {
            //	int n = (kc.Ltrk_V())[j];
            //	WTrackParameter wtrk = wTrackInfit_V(n);
            //	wlis_V.push_back(wtrk);
            //      } 
            //      kc.UpdateConstraints(wlis, wlis_V);
            //      m_kc[i] = kc;
            //cout<<"wlis_V ="<<(wlis_V[0].w())[0]<<endl;     
            updateConstraints(kc);
            //      std::cout << "updata OK " << m_d << std::endl;
        }
        timer.Stop();
        m_cpu[0] += timer.CpuTime();

        fit();  
        chisq.push_back(m_chi);
        if(it > 0) {
            double delchi = chisq[it]- chisq[it-1];
            if(fabs(delchi) < m_chiter) 
                break;
        }
    }
    if(m_chi >= m_chicut) {

        return okfit;
    }
    // update track parameter and its covariance matrix
    //    upTrkpar();
    //    covMatrix();
    timer.Start();
    //upCovmtx();
    timer.Stop();
    m_cpu[5] += timer.CpuTime();

    okfit = true;

    /*
       for (int i = 0; i<numberWTrack(); i++){
       if (wTrackOrigin(i).charge()==0) continue ;
       HTrackParameter horigin = HTrackParameter(wTrackOrigin(i));
       HTrackParameter hinfit = HTrackParameter(wTrackInfit(i));

       HepVector a0 = horigin.hel();
       HepVector a1 = hinfit.hel();
       HepSymMatrix v0 = horigin.eHel();
       HepSymMatrix v1 = hinfit.eHel(); 
       HepVector pull(5,0);
       for (int k=0; k<5; k++) {
       pull[k] = (a0[k]-a1[k])/sqrt(abs(v0[k][k]-v1[k][k]));
       }

       WTrackParameter wtrk2 = wTrackInfit(i);
       wtrk2.setPull(pull);
    //    for (int l=0;l<5; l++) {
    //(wTrackInfit(i).pull())[l]=(wtrk2.pull())[l];
    //   }
    setWTrackInfit(i, wtrk2);
    }
    */
    /*/

      for (int i = 0; i<numberWTrack_V(); i++){ 
    //if (wTrackOrigin(i).charge()==0) continue ;
    HTrackParameter horigin_V = HTrackParameter(wTrackOrigin_V(i));
    HTrackParameter hinfit_V = HTrackParameter(wTrackInfit_V(i));

    HepVector a0 = horigin.hel();
    HepVector a1 = hinfit.hel();
    HepSymMatrix v0 = horigin.eHel();
    HepSymMatrix v1 = hinfit.eHel();
    HepVector pull(5,0);
    for (int k=0; k<5; k++) {
    pull[k] = (a0[k]-a1[k])/sqrt(abs(v0[k][k]-v1[k][k]));
    }

    WTrackParameter wtrk2 = wTrackInfit(i);
    wtrk2.setPull(pull);
    //    for (int l=0;l<5; l++) {
    //(wTrackInfit(i).pull())[l]=(wtrk2.pull())[l];
    //   }
    setWTrackInfit(i, wtrk2);
    }  
    */

    return okfit;
}

bool KinematicFit::Fit(int n) {
    bool okfit = false;
    if(n < 0 || (unsigned int)n >= m_kc.size()) return okfit;

    m_nktrk = numberWTrack();
    m_pOrigin = HepVector(m_nktrk * NTRKPAR, 0);
    m_pInfit = HepVector(m_nktrk * NTRKPAR, 0);
    m_covOrigin = HepSymMatrix(m_nktrk * NTRKPAR, 0);
    m_covInfit = HepSymMatrix(m_nktrk * NTRKPAR, 0);
    m_massvector = HepVector(m_nktrk * NTRKPAR, 0);
    for(int i = 0; i < numberWTrack(); i++) {
        setWTrackInfit(i, wTrackOrigin(i));
        setPOrigin(i, (wTrackOrigin(i).w()).sub(1, NTRKPAR));
        setPInfit(i, (wTrackOrigin(i).w()).sub(1, NTRKPAR));
        setCovOrigin(i, (wTrackOrigin(i).Ew()).sub(1, NTRKPAR));
        setMassvector(i, wTrackOrigin(i).mass());
    }

    //
    // iteration loop
    //

    std::vector<double> chisq;
    chisq.clear();

    m_D = HepMatrix(m_kc[n].nc(), m_nktrk * NTRKPAR, 0);
    m_DT = HepMatrix(m_nktrk * NTRKPAR, m_kc[n].nc(), 0);
    m_d = HepVector(m_kc[n].nc(), 0);

    for(int it = 0; it < m_niter; it++) {
        m_nc = 0;
        KinematicConstraints kc = m_kc[n];
        updateConstraints(kc);
        //     m_kc[n] = kc;
        fit(n);

        chisq.push_back(m_chisq[n]);
        if(it > 0) {
            double delchi = chisq[it]- chisq[it-1];
            if(fabs(delchi) < m_chiter) 
                break;
        }
    }

    if(m_chisq[n] >= m_chicut) return okfit;
    //    ====update cov====
    //   upCovmtx();
    okfit = true;
    return okfit;
}


void KinematicFit::BuildVirtualParticle( int n) {
//
//      q = p1 + p2 + ... + pn
//
	upCovmtx();
	KinematicConstraints kc = m_kc[n];
	int ntrk = (kc.Ltrk()).size();
	int charge = 0;
	HepVector w(7, 0);
	HepSymMatrix ew(7, 0);
	HepMatrix dwdp(7, 7, 0);
	dwdp[0][0] = 1;
	dwdp[1][1] = 1;
	dwdp[2][2] = 1;
	dwdp[3][3] = 1;
	dwdp[4][4] = 1;
	dwdp[5][5] = 1;
	dwdp[6][6] = 1;
	for (int i = 0; i < ntrk; i++) {
		int itk = (kc.Ltrk())[i];
		charge += wTrackInfit(itk).charge();
		w[0] = w[0] + wTrackInfit(itk).w()[0];
		w[1] = w[1] + wTrackInfit(itk).w()[1];
		w[2] = w[2] + wTrackInfit(itk).w()[2];
		w[3] = w[3] + wTrackInfit(itk).w()[3];
		w[4] = 0.0;//
		w[5] = 0.0;// set virtual particle's vertex at (0,0,0)
		w[6] = 0.0;//
		ew = ew + (wTrackInfit(itk).Ew()).similarity(dwdp);  // the  vertex matrix  of this particles is not correct, because we do not use vertex information in kinematicfit, so ...
	}
	double m = sqrt(w[3]*w[3] - w[0]*w[0] - w[1]*w[1] - w[2]*w[2]);
	WTrackParameter vwtrk;
	vwtrk.setCharge(charge);
	vwtrk.setW(w);
	vwtrk.setEw(ew);
	vwtrk.setMass(m);
	m_virtual_wtrk.push_back(vwtrk);
}



void KinematicFit::gda(){
	for(int i = 0; i < numberWTrack(); i++) {

		if ((wTrackOrigin(i)).type() == 2 ){
			int n ;
			for(int j = 0; j < numberGammaShape(); j++) {
				if(i==GammaShapeList(j))  n = j;
			}
			HepSymMatrix Ew(NTRKPAR, 0);
			HepLorentzVector p1 = p4Infit(i);
			double eorigin = pOrigin(i)[3];
			//cout<<"p1 ="<<p1<<endl;
			HepMatrix dwda1(NTRKPAR, 3, 0);
			dwda1[0][0] = -p1.py();
			dwda1[1][0] = p1.px();
			dwda1[0][1] = p1.px()*p1.pz()/p1.perp();
			dwda1[1][1] = p1.py()*p1.pz()/p1.perp();
			dwda1[2][1] = -p1.perp();
			dwda1[0][2] = p1.px()/p1.rho();
			dwda1[1][2] = p1.py()/p1.rho();
			dwda1[2][2] = p1.pz()/p1.rho();
			dwda1[3][2] = p1.rho()/p1.e();
			HepSymMatrix emcmea1(3, 0);
			double pk = p1[3];
			emcmea1[0][0] = GammaShapeValue(n).getdphi() * GammaShapeValue(n).getdphi();
			emcmea1[1][1] = GammaShapeValue(n).getdthe() * GammaShapeValue(n).getdthe();
			emcmea1[2][2] = GammaShapeValue(n).de(eorigin,pk) * GammaShapeValue(n).de(eorigin,pk);
			Ew = emcmea1.similarity(dwda1);
			//cout<<"emcmea1 ="<<emcmea1<<endl;
			//cout<<"Ew ="<<Ew<<endl;
			setCovOrigin(i, Ew); 
		} 
	}
}


HepVector KinematicFit::pull(int n){
	upCovmtx();

	if (wTrackOrigin(n).charge()!=0){
		HepVector W(6,0);
		HepSymMatrix Ew(6,0);
		HepVector W1(7,0);
		HepSymMatrix Ew1(7,0);
		WTrackParameter wtrk = wTrackOrigin(n);
		//                W = wTrackOrigin(n).w();
		//                Ew = wTrackOrigin(n).Ew();
		//cout<<"===Origin status==="<<endl;
		//cout<<"W = "<<W<<endl;
		//cout<<"Ew ="<<Ew<<endl; 
		for(int i=0; i<3; i++) {
			W[i] = pInfit(n)[i];
		}
		W[3] = wTrackOrigin(n).w()[4];
		W[4] = wTrackOrigin(n).w()[5];
		W[5] = wTrackOrigin(n).w()[6];
		for(int j=0; j<3; j++) {
			for(int k=0; k<3; k++) {
				Ew[j][k] = covInfit(n)[j][k];
			}
		}

		for(int j=3; j<6; j++) {
			for(int k=3; k<6; k++) {
				Ew[j][k] = wTrackOrigin(n).Ew()[j+1][k+1];
			}
		}
		//
		// define J matrix to transfer 3 parameters to 4 parameters
		// 
		double px = p4Infit(n).px();
		double py = p4Infit(n).py();
		double pz = p4Infit(n).pz();
		double e  = p4Infit(n).e(); 
		HepMatrix J(7, 6, 0);
		J[0][0] = 1;
		J[1][1] = 1;
		J[2][2] = 1;
		J[3][0] = px/e;
		J[3][1] = py/e;
		J[3][2] = pz/e;
		J[4][3] = 1;
		J[5][4] = 1;
		J[6][5] = 1;
		W1 = J * W;
		Ew1 = Ew.similarity(J) ;



		// cout<<"===Infiddt status==="<<endl;
		// cout<<"p4 ="<<p4Infit(n)<<endl;
		// cout<<"W ="<<wTrackOrigin(n).w()<<endl;
		// cout<<"W1 ="<<W1<<endl;
		// cout<<"Ew ="<<wTrackOrigin(n).Ew()<<endl;
		// cout<<"Ew1 ="<<Ew1<<endl;

		wtrk.setW(W1);
		wtrk.setEw(Ew1);
		setWTrackInfit(n, wtrk); 
		HTrackParameter horigin = HTrackParameter(wTrackOrigin(n));
		HTrackParameter hinfit = HTrackParameter(wTrackInfit(n));

		HepVector a0 = horigin.hel();
		HepVector a1 = hinfit.hel();
		HepSymMatrix v0 = horigin.eHel();
		HepSymMatrix v1 = hinfit.eHel(); 
		HepVector pull(11,0);
		for (int k=0; k<5; k++) {
			pull[k] = (a0[k]-a1[k])/sqrt(abs(v0[k][k]-v1[k][k]));
		//	cout<<"pull ["<<k<<"] ="<<pull[k]<<endl;
		}
		for (int l=5; l<9; l++) {
			pull[l] = (wTrackOrigin(n).w()[l-5] - wTrackInfit(n).w()[l-5])/sqrt(abs(wTrackOrigin(n).Ew()[l-5][l-5] - wTrackInfit(n).Ew()[l-5][l-5]));
			//        cout<<"pull ["<<l<<"] ="<<pull[l]<<endl;
		}
	        
		//                pull[9] = wTrackOrigin(n).w()[3] - wTrackInfit(n).w()[3];
		//                pull[10] =1/sqrt(abs(wTrackOrigin(n).Ew()[3][3] - wTrackInfit(n).Ew()[3][3]));  
		return pull;  
	}else {
		HepVector pull(3,0);
		for (int m=0; m<3; m++) {
		        pull[m] = (wTrackOrigin(n).w()[m] - wTrackInfit(n).w()[m])/sqrt(abs(wTrackOrigin(n).Ew()[m][m] - wTrackInfit(n).Ew()[m][m]));
		}
		return pull;
	}
}


void KinematicFit::updateConstraints(KinematicConstraints k ) {
	KinematicConstraints kc = k;

	//   std::vector<HepLorentzVector> wlis;
	//   std::vector<WTrackParameter> wlis_V;
	//   wlis.clear();
	//   wlis_V.clear();
	//   for(unsigned int j = 0; j < (kc.Ltrk()).size(); j++) {
	//      int n = (kc.Ltrk())[j];
	//        HepVec wtrk = wTrackInfit(n);
	//       if(m_espread!=0)        wtrk = wTrackOrigin(n);

	//       wlis.push_back(p4Infit(n));
	//   }
	//    for(unsigned int j = 0; j < (kc.Ltrk_V()).size(); j++) {
	//      int n = (kc.Ltrk_V())[j];
	//      WTrackParameter wtrk = wTrackInfit_V(n);
	//       wlis_V.push_back(wtrk);
	//  }


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
					 //    for(unsigned int i = 0; i < wlis_V.size(); i++)
					 //      pmis = pmis + wlis_V[i].p();
					 for(unsigned int j = 0; j< (kc.Ltrk()).size(); j++){
						 int n = (kc.Ltrk())[j];
						 HepMatrix Dc(1, NTRKPAR, 0);
						 Dc[0][0] = -2 * pmis.px() + 2 * pmis.e() * p4Infit(n).px() / p4Infit(n).e();
						 Dc[0][1] = -2 * pmis.py() + 2 * pmis.e() * p4Infit(n).py() / p4Infit(n).e();
						 Dc[0][2] = -2 * pmis.pz() + 2 * pmis.e() * p4Infit(n).pz() / p4Infit(n).e();
						 //						 Dc[0][3] = 2 * pmis.e();
						 setD(m_nc,n,Dc);
						 setDT(n, m_nc, Dc.T());
					 }	

					 // for(unsigned int i = 0; i < wlis_V.size(); i++) {
					 //      HepMatrix Dc_V(1, 7, 0);
					 //      Dc_V[0][0] = -2 * pmis.px();
					 //      Dc_V[0][1] = -2 * pmis.py();
					 //      Dc_V[0][2] = -2 * pmis.pz();
					 //     Dc_V[0][3] = 2 * pmis.e();
					 //      m_Dc_V.push_back(Dc_V);
					 //    }


					 HepVector dc(1, 0);
					 dc[0] = pmis.m2() - mres * mres;
					 m_d[m_nc] = dc[0];
					 m_nc+=1;
					 //    std::cout << "etot = " << dc[0] <<" , " << mres<< std::endl;
					 //    m_dc.push_back(dc);
					 //    HepVector lambda(1, 0);
					 //    m_lambda.push_back(lambda);
					 //    HepSymMatrix vd(1, 0);
					 //    m_VD.push_back(vd);
					 //    HepSymMatrix V6 = m_Vre;
					 //    m_Vm.push_back(V6);

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
					  //    for(unsigned int i = 0; i < wlis.size(); i++)
					  //      pmis = pmis + wlis[i].p();
					  for(unsigned int j = 0; j < (kc.Ltrk()).size(); j++) {
						  int n = (kc.Ltrk())[j];
						  HepMatrix Dc(1, NTRKPAR, 0);
						  Dc[0][0] = p4Infit(n).px()/p4Infit(n).e();
						  Dc[0][1] = p4Infit(n).py()/p4Infit(n).e();
						  Dc[0][2] = p4Infit(n).pz()/p4Infit(n).e();
						  //	  Dc[0][3] = 1.0;
						  setD(m_nc,n,Dc);
						  setDT(n, m_nc, Dc.T());
					  }
					  HepVector dc(1, 0);
					  dc[0] = pmis.e() - etot;
					  m_d[m_nc] = dc[0];
					  m_nc+=1;
					  //    m_dc.push_back(dc);
					  //    HepVector lambda(1, 0);
					  //    m_lambda.push_back(lambda);
					  //    HepSymMatrix vd(1, 0);
					  //    m_VD.push_back(vd);
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

					    //    for(unsigned int i = 0; i < wlis.size(); i++)
					    //      pmis = pmis + wlis[i].p();
					    for(unsigned int j = 0; j < (kc.Ltrk()).size(); j++) {
						    int n = (kc.Ltrk())[j];
						    HepMatrix Dc(1, NTRKPAR, 0);
						    Dc[0][0] = pmis.px()/pmis.rho();
						    Dc[0][1] = pmis.py()/pmis.rho();
						    Dc[0][2] = pmis.pz()/pmis.rho();
						    setD(m_nc,n,Dc);
						    setDT(n, m_nc, Dc.T());
						    //      m_Dc.push_back(Dc);
					    }
					    HepVector dc(1, 0);
					    dc[0] = pmis.rho() - ptot;
					    m_d[m_nc] = dc[0];
					    m_nc+=1;     
					    //    m_dc.push_back(dc);
					    //    HepVector lambda(1, 0);
					    //    m_lambda.push_back(lambda);
					    //    HepSymMatrix vd(1, 0);
					    //    m_VD.push_back(vd);
					    break;
				    }
				    /*
				       case kc.typePoint(): {
				       HepPoint3D point = kc.point();
				       double flightpath;
				       HepMatrix p(2,3,0);

				       HepSymMatrix Vp(2,0);
				       for(unsigned int i = 0; i < wlis.size(); i++) {
				       HepMatrix Dc(2, 7, 0);
				       m_Dc.push_back(Dc);
				       }
				    // wlis_V.size() should be 1
				    for(unsigned int i = 0; i < wlis_V.size(); i++) {
				    //      HepMatrix Dc(3, 7, 0);
				    //      m_Dc.push_back(Dc);
				    HepMatrix Dc_V(2, 7, 0);
				    HepSymMatrix Vclus(3, 0); 
				    for(unsigned int j = 0; j < 3; j++){
				    for(unsigned int k = j; k < 3; k++){
				    Vclus[j][k] = wlis_V[i].Ew()[j+4][k+4];
				    }
				    }
				    cout<<"Vclus ="<<Vclus<<endl;	
				    p[0][0] = wlis_V[i].w()[1];
				    p[0][1] = -wlis_V[i].w()[0];
				    p[1][0] = wlis_V[i].w()[2];
				    p[1][2] = -wlis_V[i].w()[0];
				    Vp = Vclus.similarity(p);	
				    cout<<"Vp ="<<Vp<<endl;
				    Dc_V[0][0] = -(wlis_V[i].w()[5] - point[1]);
				    Dc_V[0][1] = wlis_V[i].w()[4] - point[0];
				    Dc_V[0][4] = wlis_V[i].w()[1];
				    Dc_V[0][5] = -wlis_V[i].w()[0];

				    Dc_V[1][0] = -(wlis_V[i].w()[6] - point[2]);
				    Dc_V[1][2] = wlis_V[i].w()[4] - point[0];
				    Dc_V[1][4] = wlis_V[i].w()[2];
				    Dc_V[1][6] = -wlis_V[i].w()[0];

				    //      HepMatrix q_x(3,1,0) ;
				    //      q_x[0][0] = 1;
				    //      HepMatrix deltaX_x(3,1,0) ;
				    //      deltaX_x[0][0] = 1;
				    //      HepMatrix p1_x = -(q_x.T()*Vclus.inverse(iver)*deltaX)*p2 + p1*(q_x.T()*Vclus.inverse(iver)*q + q.T()*Vclus.inverse(iver)*q_x);
				    //      HepMatrix p2_x = p2*p2;

				    //      HepMatrix q_y(3,1,0) ;
				    //      q_y[1][0] = 1;
				    //      HepMatrix deltaX_y(3,1,0);
				    //      deltaX_y[1][0] = 1;
				    //      HepMatrix p1_y = -(q_y.T()*Vclus.inverse(iver)*deltaX)*p2 + p1*(q_y.T()*Vclus.inverse(iver)*q + q.T()*Vclus.inverse(iver)*q_y);
				    //      HepMatrix p2_y = p2*p2;

				    //      HepMatrix q_z(3,1,0);
				    //      q_z[2][0] = 1;
				    //      HepMatrix deltaX_z(3,1,0);
				    //      deltaX_z[2][0] = 1;
				    //      Hw()[0]<<endl;
				    //cout<<"dc[0] ="<<dc[0]<<endl; 
				    dc[1] = (wlis_V[i].w()[4] - point[0])*wlis_V[i].w()[2] - (wlis_V[i].w()[6] - point[2])*wlis_V[i].w()[0];
				    m_dc.push_back(dc);
				    }
				    HepSymMatrix V3 = Vp;
				    m_Vm.push_back(V3);
				    break;
				    } 
				    */

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
					    //    for(unsigned int i = 0; i < wlis.size(); i++)
					    //      pmis = pmis + wlis[i].p();
					    for(unsigned int j = 0; j < (kc.Ltrk()).size(); j++) {
						    int n = (kc.Ltrk())[j];

						    HepMatrix Dc(kc.nc(), NTRKPAR, 0);
						    Dc[0][0] = 1.0;
						    Dc[1][1] = 1.0;
						    Dc[2][2] = 1.0;
						    HepVector dc(kc.nc(), 0);
						    dc[0] = pmis.px() - p3.x();
						    dc[1] = pmis.py() - p3.y();
						    dc[2] = pmis.pz() - p3.z();
						    for(int i = 0; i < kc.nc(); i++) {
							    setD(m_nc+i, n, Dc.sub(i+1, i+1, 1, NTRKPAR));
							    setDT(n, m_nc+i, (Dc.sub(i+1, i+1, 1, NTRKPAR)).T());
							    m_d[m_nc+i] = dc[i];
						    }
						    //    m_Dc.push_back(Dc);
					    }
					    m_nc += 3; 

					    //    HepVector dc(3, 0);
					    //    dc[0] = pmis.px() - p3.x();
					    //    dc[1] = pmis.py() - p3.y();
					    //    dc[2] = pmis.pz() - p3.z();
					    //    m_dc.push_back(dc);
					    //    HepVector lambda(3, 0);
					    //    m_lambda.push_back(lambda);
					    //    HepSymMatrix vd(3, 0);
					    //    m_VD.push_back(vd);
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
						HepMatrix Dc(1, NTRKPAR, 0);
						Dc[0][0] = -2 * pmis1.px() + 2 * pmis1.e() * p4Infit(n1).px() / p4Infit(n1).e();
						Dc[0][1] = -2 * pmis1.py() + 2 * pmis1.e() * p4Infit(n1).py() / p4Infit(n1).e();
						Dc[0][2] = -2 * pmis1.pz() + 2 * pmis1.e() * p4Infit(n1).pz() / p4Infit(n1).e();
						//	Dc[0][3] = 2 * pmis1.e();
						setD(m_nc,n1,Dc);
						setDT(n1,m_nc,Dc.T());
					}
					for(int i = 0; i < jsiz; i++) {
						int n2 = (kc.Ltrk())[i+isiz];
						HepMatrix Dc(1, NTRKPAR, 0);
						Dc[0][0] = 2 * pmis2.px() - 2 * pmis2.e() * p4Infit(n2).px() / p4Infit(n2).e();
						Dc[0][1] = 2 * pmis2.py() - 2 * pmis2.e() * p4Infit(n2).py() / p4Infit(n2).e();
						Dc[0][2] = 2 * pmis2.pz() - 2 * pmis2.e() * p4Infit(n2).pz() / p4Infit(n2).e();
						Dc[0][3] = -2 * pmis2.e();
						setD(m_nc,n2,Dc);
						setDT(n2,m_nc,Dc.T());
					}
					//    int isiz_V = m_nequal_V[0];
					//    HepLorentzVector pmis1_V, pmis2_V;
					//    if(isiz_V > 0){
					//    for(int n = 0; n < isiz_V; n++) {
					//      pmis1_V = pmis1_V + wlis_V[n].p();
					//    } 
					//    } 
					//    int jsiz_V = m_nequal_V[1];
					//    if(jsiz_V > 0) {
					//    for(int n = 0; n < jsiz_V; n++) 
					//      pmis2_V = pmis2_V + wlis_V[isiz_V+n].p();
					//    }

					//    for(int i = 0; i < isiz_V; i++) {
					//      HepMatrix Dc_V(1, 7, 0);
					//      Dc_V[0][0] = -2 * pmis1_V.px();
					//      Dc_V[0][1] = -2 * pmis1_V.py();
					//      Dc_V[0][2] = -2 * pmis1_V.pz();
					//      Dc_V[0][3] = 2 * pmis1_V.e();
					//      m_Dc_V.push_back(Dc_V);
					//    }
					//    for(int i = isiz_V; i < isiz_V+jsiz_V; i++) {
					//      HepMatrix Dc_V(1, 7, 0);
					//      Dc_V[0][0] = 2 * pmis2_V.px();
					//      Dc_V[0][1] = 2 * pmis2_V.py();
					//      Dc_V[0][2] = 2 * pmis2_V.pz();
					//      Dc_V[0][3] = -2 * pmis2_V.e();
					//      m_Dc_V.push_back(Dc_V);
					//    }
					HepVector dc(1, 0);
					dc[0] = pmis1.m2() - pmis2.m2();// + pmis1_V.m2() - pmis2_V.m2();
					m_d[m_nc] = dc[0];

					m_nc+=1;
					//    m_dc.push_back(dc);
					//    HepVector lambda(1, 0);
					//    m_lambda.push_back(lambda);
					//    HepSymMatrix vd(1, 0);
					//    m_VD.push_back(vd);
					//    HepSymMatrix V2 = m_Vne;
					//    m_Vm.push_back(V2);
					//    std::cout <<"m_Vm[0] ="<<m_Vm[0]<<std::endl;
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
				 //    HepLorentzVector pmis_V;
				 for(unsigned int j = 0; j < (kc.Ltrk()).size(); j++) {
					 int n = (kc.Ltrk())[j];
					 HepMatrix Dc(kc.nc(), NTRKPAR, 0);
					 Dc[0][0] = 1.0;
					 Dc[1][1] = 1.0;
					 Dc[2][2] = 1.0;
					 Dc[3][0] = p4Infit(n).px() / p4Infit(n).e();
					 Dc[3][1] = p4Infit(n).py() / p4Infit(n).e();
					 Dc[3][2] = p4Infit(n).pz() / p4Infit(n).e();
					 //					 Dc[3][3] = 1.0;

					 //      m_Dc.push_back(Dc);
					 HepVector dc(kc.nc(), 0);
					 dc[0] = pmis.px()  - p4.px();
					 dc[1] = pmis.py()  - p4.py();
					 dc[2] = pmis.pz()  - p4.pz();
					 dc[3] = pmis.e()  - p4.e();
					 for(int i = 0; i < kc.nc(); i++) {
						 setD(m_nc+i, n, Dc.sub(i+1, i+1, 1, NTRKPAR));
						 setDT(n, m_nc+i, (Dc.sub(i+1, i+1, 1, NTRKPAR)).T());
						 m_d[m_nc+i] = dc[i];
					 }
				 }
				 m_nc += 4;
				 //    for(unsigned int i = 0; i < wlis_V.size(); i++)
				 //     pmis_V = pmis_V + wlis_V[i].p();
				 //    for(unsigned int i = 0; i < wlis_V.size(); i++) {
				 //      HepMatrix Dc_V(4, 7, 0);
				 //      Dc_V[0][0] = 1.0;
				 //      Dc_V[1][1] = 1.0;
				 //      Dc_V[2][2] = 1.0;
				 //      Dc_V[3][3] = 1.0;
				 //      m_Dc_V.push_back(Dc_V);
				 //    }

				 //    HepVector dc(4, 0);
				 //   dc[0] = pmis.px() + pmis_V.px() - p4.px();
				 //    dc[1] = pmis.py() + pmis_V.py() - p4.py();
				 //    dc[2] = pmis.pz() + pmis_V.pz() - p4.pz();
				 //    dc[3] = pmis.e() + pmis_V.e() - p4.e();
				 //    m_dc.push_back(dc);

				 //    HepSymMatrix V1 = m_Vme;
				 //    m_Vm.push_back(V1);
				 //    HepVector lambda(4, 0);
				 //    m_lambda.push_back(lambda);
				 //   HepSymMatrix vd(4, 0);
				 //    m_VD.push_back(vd);

				 break;
			 }
	}
}

