#ifndef TOFSHOWER_H
#define TOFSHOWER_H

#include "TofRecEvent/RecTofTrack.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/PropertyMgr.h"
#include <vector>
#include <string>

using namespace std;
class Identifier;
class TofData;

class TofShower {
  public:
    TofShower();
    ~TofShower() {;}
  public:
    // Get Neighbors of a scintillator
    vector<Identifier> getNeighbors(const Identifier& id);
    vector<Identifier> getNextNeighbors(const Identifier& id);


    void energyCalib(vector<TofData*>& tofDataVec, RecTofTrackCol* recTofTrackCol);
    void findSeed(vector<TofData*>& tofDataVec);
    void findShower(vector<TofData*>& tofDataVec, RecTofTrackCol* recTofTrackCol,double);
    void BookNtuple(NTuple::Tuple*& tuple, NTuple::Tuple*& tuple1, NTuple::Tuple*& tuple2);


  public:
    void readCalibPar();
    double ecalib(const int nsci) const;
    void setEcalib(const int nsci, const double ecalib);

    double calib(const int n, const int m) const;
    void setCalib(const int n, const int m, const double ecalib);

    inline double calibConst() const { return m_calibConst; }
    inline void setCalibConst(const double cal) { m_calibConst = cal; }

    inline bool isData() const { return m_isData; }
    inline void setIsData(const bool isData) { m_isData = isData; }

  private:
    vector<Identifier> m_seedVec;
    double m_ecalib[176];
    double m_calib[176][4];
    double m_calibConst;
    bool m_isData;

  private:
    PropertyMgr   m_propMgr;
    bool m_output;
    NTuple::Tuple*  m_tuple;
    NTuple::Item<long>  m_part;
    NTuple::Item<long>  m_layer;
    NTuple::Item<long>  m_im;
    NTuple::Item<long>  m_end;
    NTuple::Item<double>  m_zpos;
    NTuple::Item<double>  m_adc1;
    NTuple::Item<double>  m_adc2;
    NTuple::Item<double>  m_tdc1;
    NTuple::Item<double>  m_tdc2;
    NTuple::Item<double>  m_energy;

    NTuple::Tuple*  m_tuple1;
    NTuple::Item<long>  m_shower_part;
    NTuple::Item<long>  m_shower_layer;
    NTuple::Item<long>  m_shower_im;
    NTuple::Item<double>  m_shower_zpos;
    NTuple::Item<double>  m_shower_energy;

    NTuple::Tuple*  m_tuple2;
    NTuple::Item<double>  m_seed_dist;
};
#endif
