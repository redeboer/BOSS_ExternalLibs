#ifndef VertexFit_TrackPool_H
#define VertexFit_TrackPool_H

//
//  Add Track Parameters to VertexFit/KinematicFit
//  Author: K.L. He     date: 11/09/2005, created
//

// #include <map>
// #include <string>
// #include <utility>

#include <vector>
#include "VertexFit/WTrackParameter.h"
#include "VertexFit/GammaShape.h"

#ifndef BEAN
//#include "DstEvent/DstMdcTrack.h"
//#include "DstEvent/DstMdcKalTrack.h"
//#include "DstEvent/DstEmcTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "EmcRecEventModel/RecEmcShower.h"
#else
#include "DstEvtRecTracks.h"
#endif

class TrackPool {

    public:

        // constructor & deconstructor
        TrackPool();
        ~TrackPool(){;}

        //
        //  Constructor Function
        //
        //                   event data model dependent
        void AddTrack(const int number, const double mass, const RecMdcTrack *trk);
        void AddTrack(const int number, const double mass, const RecMdcKalTrack *trk);
        void AddTrack(const int number, const double mass, const RecEmcShower *trk);
        //                   event data model independent
        void AddTrack(const int number, const WTrackParameter wtrk);
        //                   virtual track with missing parameters
        void AddMissTrack(const int number, const double mass);
        void AddMissTrack(const int number, const double mass, const HepLorentzVector p4);
        void AddMissTrack(const int number, const double mass, const RecEmcShower *trk);
        void AddMissTrack(const int number, const RecEmcShower *trk);
        void AddMissTrack(const int number, const HepLorentzVector p4); 
        void AddTrackVertex(const int number, const double mass, const RecEmcShower *trk);

        std::vector<int> AddList(int n1);
        std::vector<int> AddList(int n1, int n2);
        std::vector<int> AddList(int n1, int n2, int n3);
        std::vector<int> AddList(int n1, int n2, int n3, int n4);
        std::vector<int> AddList(int n1, int n2, int n3, int n4, int n5);
        std::vector<int> AddList(int n1, int n2, int n3, int n4, int n5, int n6);
        std::vector<int> AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7);
        std::vector<int> AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8);
        std::vector<int> AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8,
                int n9);
        std::vector<int> AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8,
                int n9, int n10);
        std::vector<int> AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8,
                int n9, int n10, int n11);
        std::vector<int> AddList(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8,
                int n9, int n10, int n11, int n12);

        //                       Get Function

        std::vector<WTrackParameter> wTrackOrigin() const {return m_wtrk_origin;}
        std::vector<WTrackParameter> wTrackInfit() const {return m_wtrk_infit;}
        std::vector<int>    wTrackList() const {return m_lwtrk;}
        WTrackParameter wTrackOrigin(int n) const {return m_wtrk_origin[n];}
        WTrackParameter wTrackInfit(int n) const {return m_wtrk_infit[n];}
        int     wTrackList(int n) const {return m_lwtrk[n];}
        //                       number of WTrack  
        int numberWTrack() const { return ((int)(m_lwtrk.size()));}
        //
        // For Virtual Particles
        //
        //  std::vector<WTrackParameter> wTrackOrigin_V() const {return m_wtrk_origin_V;}
        //  std::vector<WTrackParameter> wTrackInfit_V() const {return m_wtrk_infit_V;}
        //  std::vector<int>    wTrackList_V() const {return m_lwtrk_V;}
        //  WTrackParameter wTrackOrigin_V(int n) const {return m_wtrk_origin_V[n];}
        //  WTrackParameter wTrackInfit_V(int n) const {return m_wtrk_infit_V[n];}
        //  int     wTrackList_V(int n) const {return m_lwtrk_V[n];}
        //                       number of WTrack  
        //  int numberWTrack_V() const { return ((int)(m_lwtrk_V.size()));}

        //
        // gammashape get function
        //
        std::vector<GammaShape> GammaShapeValue() const {return m_gammashape;}
        std::vector<int>  GammaShapeList() const {return m_lgammashape;}
        GammaShape GammaShapeValue(int n) const {return m_gammashape[n];}
        int   GammaShapeList(int n) const {return m_lgammashape[n];}
        int numberGammaShape() const { return ((int)(m_lgammashape.size()) );} 
        //
        // modifier
        //

        //                set Function
        void setWTrackOrigin(const int n, const WTrackParameter wtrk) {m_wtrk_origin[n] = wtrk;}
        void setWTrackInfit(const int n, const WTrackParameter wtrk) {m_wtrk_infit[n] = wtrk;}
        void setWTrackOrigin(const WTrackParameter wtrk) {m_wtrk_origin.push_back(wtrk);}
        void setWTrackInfit(const WTrackParameter wtrk) {m_wtrk_infit.push_back(wtrk);}
        void setWTrackList(const int n) {m_lwtrk.push_back(n);}
        //                clear Function
        void clearWTrackOrigin() {m_wtrk_origin.clear();}
        void clearWTrackInfit() {m_wtrk_infit.clear();}
        void clearWTrackList() {m_lwtrk.clear();}

        void clearone() {m_numberone = 0;}
        void cleartwo() {m_numbertwo = 0;}
        int numberone() const {return m_numberone;}
        int numbertwo() const {return m_numbertwo;}

        vector<int> mappositionA() const {return m_mappositionA;}
        vector<int> mappositionB() const {return m_mappositionB;}
        vector<int> mapkinematic() const {return m_mapkinematic;}

        void clearMapkinematic() {m_mapkinematic.clear();}
        void clearMappositionA() {m_mappositionA.clear();}
        void clearMappositionB() {m_mappositionB.clear();}

        void setMapkinematic(const int n) {m_mapkinematic.push_back(n);}
        void setMappositionA(const int n) {m_mappositionA.push_back(n);}
        void setMappositionB(const int n) {m_mappositionB.push_back(n);}

        //
        // set GammaShape 
        //
        void setGammaShape(const int n, const GammaShape gammashape) {m_gammashape[n] = gammashape;}
        void setGammaShape(const GammaShape gammashape) {m_gammashape.push_back(gammashape);}
        void setGammaShapeList(const int n) {m_lgammashape.push_back(n);}
        //       clear gammashape
        void clearGammaShape() {m_gammashape.clear();}
        void clearGammaShapeList() {m_lgammashape.clear();}

        //=== set beam information===
        void setBeamPosition(const HepPoint3D BeamPosition) {m_BeamPosition = BeamPosition;}
        void setVBeamPosition(const HepSymMatrix VBeamPosition) {m_VBeamPosition = VBeamPosition;} 

        //=== get beam information===
        HepPoint3D getBeamPosition() const{return m_BeamPosition;}
        HepSymMatrix getVBeamPosition() const {return m_VBeamPosition;}


    protected:


    private:
        std::vector<WTrackParameter> m_wtrk_origin;   // WTrack parameter before fit
        std::vector<WTrackParameter> m_wtrk_infit;    // WTrack Parameter in fit
        std::vector<int>    m_lwtrk;         // List of the WTrack number ( != DST tracklist ID)
        //
        // For Virtual Particles
        //
        //  std::vector<WTrackParameter> m_wtrk_origin_V;  
        //  std::vector<WTrackParameter> m_wtrk_infit_V;  
        //  std::vector<int>    m_lwtrk_V; 

        //  std::multimap< int, int >  m_mapkinematic;        // index origin track and new track  
        //  std::multimap< int, int >  m_mappositionA;
        //  std::multimap< int, int >  m_mappositionB;
        int m_numberone;             //recording the number of common particlss
        int m_numbertwo;             //recording the number of virtual particles
        //    Gamma dynamic error information
        //
        std::vector<GammaShape> m_gammashape;
        std::vector<int> m_lgammashape;          // maybe can share with the m_lwtrk

        std::vector<int>  m_mapkinematic;        // index origin track and new track  
        std::vector<int>  m_mappositionA;
        std::vector<int>  m_mappositionB;

    private:
        HepPoint3D m_BeamPosition;
        HepSymMatrix m_VBeamPosition; 

};
#endif
