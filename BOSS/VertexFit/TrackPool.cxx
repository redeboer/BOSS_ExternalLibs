#include "VertexFit/TrackPool.h"
#include "VertexFit/WTrackParameter.h"
#include "VertexFit/GammaShape.h"
#include <string>

TrackPool::TrackPool() {
    clearWTrackOrigin();
    clearWTrackInfit();
    clearWTrackList();
    clearGammaShape();
    clearGammaShapeList();
    clearMapkinematic();
    clearMappositionA();
    clearMappositionB();
    setBeamPosition(HepPoint3D(0.0,0.0,0.0));
    setVBeamPosition(HepSymMatrix(3,0));
    m_numberone = 0;
    m_numbertwo = 0;
}


void TrackPool::AddTrack(const int number, const double mass,
        const RecMdcTrack *trk) {
    HepVector helix(5,0);
    double error[15];
    for(int i = 0; i < 5; i++)
        helix[i] = trk->helix(i);
    for(int i = 0; i < 15; i++)
        error[i] = trk->err(i);
    WTrackParameter wtrk(mass, helix, error);
    setWTrackOrigin(wtrk);
    setWTrackInfit(wtrk);
    setWTrackList(number);
    if(number != numberWTrack()-1) {
        std::cout << "TrackPool: wrong track index" <<" " 
            <<number<<" , " <<numberWTrack()<< std::endl;
    }
    setMapkinematic(0);
    setMappositionA(m_numberone);
    setMappositionB(m_numbertwo);
    m_numberone = m_numberone + 4;
}

void TrackPool::AddTrack(const int number, const double mass,
        const RecEmcShower *trk) {
    //
    //parameters: phi lambda mass energy
    //  
    double ptrk = trk->energy();
    double e = sqrt(ptrk*ptrk + mass * mass);
    double the = trk->theta();
    double phi = trk->phi();
    HepLorentzVector p4(ptrk * sin(the) * cos(phi), 
            ptrk * sin(the) * sin(phi),
            ptrk * cos(the),
            e);
    double dphi = trk->dphi();
    double dthe = trk->dtheta();
    double de   = trk->dE();
    double x = trk->x();
    double y = trk->y();
    double z = trk->z();
    HepPoint3D x3 (x, y ,z);
    WTrackParameter wtrk(x3, p4 ,dphi ,dthe, de);
    HepSymMatrix Vpl = HepSymMatrix(2,0);
    //=== get Vclus===
    HepSymMatrix Vclus = HepSymMatrix (3,0);
    Vclus = (wtrk.Ew()).sub(5,7);
    double xpr = x - m_BeamPosition[0];
    double ypr = y - m_BeamPosition[1];
    double zpr = z - m_BeamPosition[2];
    double Rpr = sqrt(xpr*xpr + ypr*ypr);
    // === get jacobi ===   
    HepMatrix J(2,3,0);
    J[0][0] = -ypr/(Rpr*Rpr);
    J[0][1] = xpr/(Rpr*Rpr);
    J[1][0] = -xpr * zpr/(Rpr*Rpr*Rpr);
    J[1][1] = -ypr * zpr/(Rpr*Rpr*Rpr);
    J[1][2] = 1/Rpr;
    Vpl = Vclus.similarity(J) + m_VBeamPosition.similarity(J);
    Vpl[0][1]=0;
    // === get phipre, lambda===

    double phipre = atan(ypr/xpr);

    if(xpr<0){
        phipre = atan(ypr/xpr) + 3.1415926;
    }
    double lambdapre = zpr/Rpr;


    // === set p4 ===
    double p0x = ptrk*cos(phipre)/sqrt(1 + lambdapre*lambdapre);
    double p0y = ptrk*sin(phipre)/sqrt(1 + lambdapre*lambdapre);
    double p0z = ptrk*lambdapre/sqrt(1 + lambdapre*lambdapre);
    double p0e = e;


    double p0ver = sqrt(p0x*p0x + p0y*p0y);


    HepMatrix B(4,3,0);
    B[0][0] = -p0y;
    B[0][1] = -p0z * p0x * p0ver/(p0e * p0e);
    B[0][2] = p0x/p0e;
    B[1][0] = p0x;
    B[1][1] = -p0z * p0y * p0ver/(p0e * p0e);
    B[1][2] = p0y/p0e;
    B[2][1] = p0ver * p0ver * p0ver/(p0e * p0e);
    B[2][2] = p0z/p0e;
    B[3][2] = 1;

    HepSymMatrix Vple(3,0);
    Vple[0][0] = Vpl[0][0];
    Vple[1][1] = Vpl[1][1];
    Vple[2][2] = de * de;

    HepSymMatrix Vpxyze(4,0);
    Vpxyze = Vple.similarity(B);

    wtrk.setW(0,p0x);
    wtrk.setW(1,p0y);
    wtrk.setW(2,p0z);
    wtrk.setW(3,p0e);

    wtrk.setEw(Vpxyze);

    HepSymMatrix Vplme(4,0);
    Vplme[0][0] = Vpl[0][0];
    Vplme[1][1] = Vpl[1][1];
    Vplme[3][3] = de * de;
    wtrk.setVplm(Vplme); 

    HepVector plmp(4 , 0);
    plmp[0] = phipre;
    plmp[1] = lambdapre;
    plmp[2] = mass;
    plmp[3] = e;
    wtrk.setPlmp(plmp);


    setWTrackOrigin(wtrk);
    setWTrackInfit(wtrk);
    setWTrackList(number);
    if(number != numberWTrack()-1) {
        std::cout << "TrackPool: wrong track index" <<" " 
            <<number<<" , " <<numberWTrack()<< std::endl;
    }
    GammaShape gtrk(p4,dphi,dthe,de);
    setGammaShape(gtrk);
    setGammaShapeList(number);
    setMapkinematic(1);
    setMappositionA(m_numberone);
    setMappositionB(m_numbertwo);
    m_numberone = m_numberone + 4;
}


void TrackPool::AddMissTrack(const int number, const double mass,
        const RecEmcShower *trk) {
    //
    //parameters: phi lambda mass ptrk
    //
    double ptrk = trk->energy();
    double e = sqrt(ptrk*ptrk + mass * mass);
    double the = trk->theta();
    double phi = trk->phi();
    HepLorentzVector p4( e* sin(the) * cos(phi), 
            e * sin(the) * sin(phi),
            e * cos(the),
            e);
    double dphi = trk->dphi();
    double dthe = trk->dtheta();
    double de   = 1E+6;
    double x = trk->x();
    double y = trk->y();
    double z = trk->z();

    HepPoint3D x3 (x, y ,z);
    WTrackParameter wtrk(x3, p4 ,dphi ,dthe, de);
    HepSymMatrix Vpe = HepSymMatrix(2,0);
    //=== get Vclus===
    HepSymMatrix Vclus = HepSymMatrix (3,0);
    Vclus = (wtrk.Ew()).sub(5,7);
    double xpr = x - m_BeamPosition[0];
    double ypr = y - m_BeamPosition[1];
    double zpr = z - m_BeamPosition[2];
    double Rpr = sqrt(xpr*xpr + ypr*ypr);
    // === get jacobi ===   
    HepMatrix J(2,3,0);
    J[0][0] = -ypr/(Rpr*Rpr);
    J[0][1] = xpr/(Rpr*Rpr);
    J[1][0] = -xpr * zpr/(Rpr*Rpr*Rpr);
    J[1][1] = -ypr * zpr/(Rpr*Rpr*Rpr);
    J[1][2] = 1/Rpr;
    Vpe = Vclus.similarity(J) + m_VBeamPosition.similarity(J);
    Vpe[0][1]=0;

    double phipre = atan(ypr/xpr);

    if(xpr<0){
        phipre = atan(ypr/xpr) + 3.1415926;
    } 
    double lambdapre = zpr/Rpr;  


    HepVector plmp(4 , 0);
    plmp[0] = phipre;
    plmp[1] = lambdapre;
    plmp[2] = mass;
    plmp[3] = ptrk;
    wtrk.setPlmp(plmp);

    HepSymMatrix Vplm(3,0);
    Vplm[0][0] = Vpe[0][0];
    Vplm[1][1] = Vpe[1][1];
    wtrk.setVplm(Vplm);



    // === set p4 ===
    double p0x = ptrk*cos(phipre)/sqrt(1 + lambdapre*lambdapre);
    double p0y = ptrk*sin(phipre)/sqrt(1 + lambdapre*lambdapre);  
    double p0z = ptrk*lambdapre/sqrt(1 + lambdapre*lambdapre);
    double p0e = e;

    wtrk.setW(0,p0x);
    wtrk.setW(1,p0y);
    wtrk.setW(2,p0z);
    wtrk.setW(3,p0e);

    wtrk.setType(1);
    setWTrackOrigin(wtrk);
    setWTrackInfit(wtrk);
    setWTrackList(number);
    GammaShape gtrk(p4,dphi,dthe,de);
    setGammaShape(gtrk);
    setGammaShapeList(number);
    setMapkinematic(5);
    setMappositionA(m_numberone);
    setMappositionB(m_numbertwo);

    m_numberone = m_numberone + 3;
    m_numbertwo = m_numbertwo + 1;
}




void TrackPool::AddMissTrack(const int number, const RecEmcShower *trk) {
    //
    //parameters: phi lambda mass E
    //

    double mass = 0;
    double ptrk = trk->energy();
    double e = sqrt(ptrk*ptrk + mass * mass);
    double the = trk->theta();
    double phi = trk->phi();
    HepLorentzVector p4( e* sin(the) * cos(phi),
            e * sin(the) * sin(phi),
            e * cos(the),
            e);
    double dphi = trk->dphi();
    double dthe = trk->dtheta();
    double de   = 1E+6;
    double x = trk->x();
    double y = trk->y();
    double z = trk->z();

    HepPoint3D x3 (x, y ,z);
    WTrackParameter wtrk(x3, p4 ,dphi ,dthe, de);
    HepSymMatrix Vpe = HepSymMatrix(2,0);
    //=== get Vclus===
    HepSymMatrix Vclus = HepSymMatrix (3,0);
    Vclus = (wtrk.Ew()).sub(5,7);
    double xpr = x - m_BeamPosition[0];
    double ypr = y - m_BeamPosition[1];
    double zpr = z - m_BeamPosition[2];
    double Rpr = sqrt(xpr*xpr + ypr*ypr);
    // === get jacobi ===   
    HepMatrix J(2,3,0);
    J[0][0] = -ypr/(Rpr*Rpr);
    J[0][1] = xpr/(Rpr*Rpr);
    J[1][0] = -xpr * zpr/(Rpr*Rpr*Rpr);
    J[1][1] = -ypr * zpr/(Rpr*Rpr*Rpr);
    J[1][2] = 1/Rpr;
    Vpe = Vclus.similarity(J) + m_VBeamPosition.similarity(J);
    Vpe[0][1]=0;
    double phipre = atan(ypr/xpr);

    if(xpr<0){
        phipre = atan(ypr/xpr) + 3.1415926;
    }
    double lambdapre = zpr/Rpr;


    HepVector plmp(4 , 0);
    plmp[0] = phipre;
    plmp[1] = lambdapre;
    plmp[2] = mass;
    plmp[3] = e;
    wtrk.setPlmp(plmp);

    HepSymMatrix Vplm(2,0);
    Vplm[0][0] = Vpe[0][0];
    Vplm[1][1] = Vpe[1][1];
    wtrk.setVplm(Vplm);



    // === set p4 ===
    double p0x = ptrk*cos(phipre)/sqrt(1 + lambdapre*lambdapre);
    double p0y = ptrk*sin(phipre)/sqrt(1 + lambdapre*lambdapre);
    double p0z = ptrk*lambdapre/sqrt(1 + lambdapre*lambdapre);
    double p0e = e;

    wtrk.setW(0,p0x);
    wtrk.setW(1,p0y);
    wtrk.setW(2,p0z);
    wtrk.setW(3,p0e);

    wtrk.setType(1);
    setWTrackOrigin(wtrk);
    setWTrackInfit(wtrk);
    setWTrackList(number);
    GammaShape gtrk(p4,dphi,dthe,de);
    setGammaShape(gtrk);
    setGammaShapeList(number);
    setMapkinematic(4);
    setMappositionA(m_numberone);
    setMappositionB(m_numbertwo);

    m_numberone = m_numberone + 2;
    m_numbertwo = m_numbertwo + 2;
}




void TrackPool::AddMissTrack(const int number, const double mass, const HepLorentzVector p4) {
    //
    //parameters: mass px py pz
    //  

    double dphi = 1E+6;
    double dthe = 1E+6;
    double dE = 1E+6;
    WTrackParameter wtrk(p4, dphi, dthe, dE);
    HepVector plmp(4, 0);
    double phipre = atan(p4[1]/p4[0]);

    if(p4[0]<0){
        phipre = atan(p4[1]/p4[0]) + 3.1415926;
    }
    plmp[0] = phipre;
    plmp[1] = wtrk.Lambda();
    plmp[2] = mass;
    plmp[3] = p4[3];
    HepSymMatrix Vplm(3, 0);
    wtrk.setPlmp(plmp);
    wtrk.setVplm(Vplm);
    wtrk.setType(1);
    setWTrackOrigin(wtrk);
    setWTrackInfit(wtrk); 
    setWTrackList(number);
    setMapkinematic(3);
    setMappositionA(m_numberone);
    setMappositionB(m_numbertwo);
    m_numberone = m_numberone + 1;
    m_numbertwo = m_numbertwo + 3;
}


void TrackPool::AddMissTrack(const int number, const double mass) {
    //
    //parameters: mass px py pz, but px,py,pz 's error matrix is set as 10e^6
    //
    WTrackParameter wtrk;
    wtrk.setMass(mass);
    HepVector w(7,0);
    HepSymMatrix Ew(7,0);
    w[0] = 0.2;
    w[1] = 0.2;
    w[2] = 0.2;
    w[3] = sqrt(0.2*0.2*3 + mass*mass);
    Ew[0][0] = 1E+6;
    Ew[1][1] = 1E+6;
    Ew[2][2] = 1E+6;
    wtrk.setW(w);
    wtrk.setEw(Ew);
    setWTrackOrigin(wtrk);
    setWTrackInfit(wtrk);
    setWTrackList(number);
    setMapkinematic(7);
    setMappositionA(m_numberone);
    setMappositionB(m_numbertwo);
    m_numberone = m_numberone + 4;

}


void TrackPool::AddMissTrack(const int number, HepLorentzVector p4) {
    double dphi = 1E+3;
    double dthe = 1E+3;
    double dE = 1E+3;
    WTrackParameter wtrk(p4, dphi, dthe, dE);
    HepSymMatrix Ew = HepSymMatrix(7,0);
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            if(i==j)    Ew[i][j] = 1E+6;
        }
    }
    wtrk.setType(1);
    wtrk.setEw(Ew);
    setWTrackOrigin(wtrk);
    setWTrackInfit(wtrk);
    setWTrackList(number);
    setMapkinematic(2);
    setMappositionA(m_numberone);
    setMappositionB(m_numbertwo);
    m_numbertwo = m_numbertwo + 4;     
}





void TrackPool::AddTrack(const int number, WTrackParameter wtrk) {
    setWTrackOrigin(wtrk);
    setWTrackInfit(wtrk);
    setWTrackList(number);
    if(number != numberWTrack()-1) {
        std::cout << "TrackPool: wrong track index" <<" " 
            <<number<<" , " <<numberWTrack()<< std::endl;
    }
    setMapkinematic(0);
    setMappositionA(m_numberone); 
    setMappositionB(m_numbertwo);
    m_numberone = m_numberone + 4;
}


void TrackPool::AddTrackVertex(const int number, const double mass, const RecEmcShower *trk) {
    double ptrk = trk->energy();
    double e = sqrt(ptrk*ptrk + mass * mass);
    double the = trk->theta();
    double phi = trk->phi();
    HepLorentzVector p4(ptrk * sin(the) * cos(phi), 
            ptrk * sin(the) * sin(phi),
            ptrk * cos(the),
            e);
    double dphi = trk->dphi();
    double dthe = trk->dtheta();
    double de   = trk->dE();
    double x = trk->x();
    double y = trk->y();
    double z = trk->z();
    HepPoint3D x3 (x, y ,z);
    WTrackParameter wtrk(x3, p4 ,dphi ,dthe, de);
    setWTrackOrigin(wtrk);
    setWTrackInfit(wtrk);
    setWTrackList(number);
    if(number != numberWTrack()-1) {
        std::cout << "TrackPool: wrong track index" <<" " 
            <<number<<" , " <<numberWTrack()<< std::endl;
    }
    GammaShape gtrk(p4,dphi,dthe,de);
    setGammaShape(gtrk);
    setGammaShapeList(number);
    setMapkinematic(6);
    m_numbertwo = 0;
    setMappositionA(m_numberone);
    setMappositionB(m_numbertwo);

    m_numberone = m_numberone + 4;
    m_numbertwo = m_numbertwo + 3;
}




std::vector<int> TrackPool::AddList(int n1) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    return lis;
}

std::vector<int> TrackPool::AddList(int n1, int n2) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    lis.push_back(n2);
    return lis;
}

std::vector<int> TrackPool::AddList(int n1, int n2, int n3) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    lis.push_back(n2);
    lis.push_back(n3);
    return lis;
}

std::vector<int> TrackPool::AddList(int n1, int n2, int n3, int n4) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    lis.push_back(n2);
    lis.push_back(n3);
    lis.push_back(n4);
    return lis;
}

std::vector<int> TrackPool::AddList(int n1, int n2, int n3, int n4, int n5) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    lis.push_back(n2);
    lis.push_back(n3);
    lis.push_back(n4);
    lis.push_back(n5);
    return lis;
}

std::vector<int> TrackPool::AddList(int n1, int n2, int n3, int n4, int n5, int n6) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    lis.push_back(n2);
    lis.push_back(n3);
    lis.push_back(n4);
    lis.push_back(n5);
    lis.push_back(n6);
    return lis;
}

std::vector<int> TrackPool::AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    lis.push_back(n2);
    lis.push_back(n3);
    lis.push_back(n4);
    lis.push_back(n5);
    lis.push_back(n6);
    lis.push_back(n7);
    return lis;
}

std::vector<int> TrackPool::AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    lis.push_back(n2);
    lis.push_back(n3);
    lis.push_back(n4);
    lis.push_back(n5);
    lis.push_back(n6);
    lis.push_back(n7);
    lis.push_back(n8);
    return lis;
}

std::vector<int> TrackPool::AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8,
        int n9) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    lis.push_back(n2);
    lis.push_back(n3);
    lis.push_back(n4);
    lis.push_back(n5);
    lis.push_back(n6);
    lis.push_back(n7);
    lis.push_back(n8);
    lis.push_back(n9);
    return lis;
}

std::vector<int> TrackPool::AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8,
        int n9, int n10) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    lis.push_back(n2);
    lis.push_back(n3);
    lis.push_back(n4);
    lis.push_back(n5);
    lis.push_back(n6);
    lis.push_back(n7);
    lis.push_back(n8);
    lis.push_back(n9);
    lis.push_back(n10);
    return lis;
}

std::vector<int> TrackPool::AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8,
        int n9, int n10, int n11) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    lis.push_back(n2);
    lis.push_back(n3);
    lis.push_back(n4);
    lis.push_back(n5);
    lis.push_back(n6);
    lis.push_back(n7);
    lis.push_back(n8);
    lis.push_back(n9);
    lis.push_back(n10);
    lis.push_back(n11);
    return lis;
}

std::vector<int> TrackPool::AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8,
        int n9, int n10, int n11, int n12) {
    std::vector<int> lis;
    lis.clear();
    lis.push_back(n1);
    lis.push_back(n2);
    lis.push_back(n3);
    lis.push_back(n4);
    lis.push_back(n5);
    lis.push_back(n6);
    lis.push_back(n7);
    lis.push_back(n8);
    lis.push_back(n9);
    lis.push_back(n10);
    lis.push_back(n11);
    lis.push_back(n12);
    return lis;
}


