#ifndef  HOUGHHITLIST_H
#define  HOUGHHITLIST_H

#include "MdcHoughFinder/HoughHit.h"
#include "MdcRawEvent/MdcDigi.h"
#include "MdcHoughFinder/HoughGlobal.h"
#include <vector>
#include "CLHEP/Matrix/Vector.h"

using namespace Event;
typedef std::vector<MdcDigi*> MdcDigiVec;

class HoughHitList{
  public:
    //constructors
    HoughHitList();
    HoughHitList(MdcDigiVec mdcDigiVec);
    ~HoughHitList();
    void clearHoughHitList();

    //**********************************
    // Accessor
    //**********************************
    int nHit() const {return _houghHitList.size();}
    int getHitNum(int) const ;
    int getHitNumA(int) const ;
    int getHitNumS(int) const ;
    //int nMdcHit() const;
    //int nCgemHit() const;
    const std::vector<HoughHit>& getList() const;
    std::vector<HoughHit>& getList() ;
    const HoughHit& getHoughHit(int i) const{return _houghHitList[i];}
    HoughHit& getHoughHit(int i) {return _houghHitList[i];}
    HoughHitType type() const{return _type;}


    //**********************************
    //  Modify
    //**********************************
    void addHit(HoughHit& a);//add hit into hitlist
    int addMdcDigiList(MdcDigiVec mdcDigiVec );
    int addTruthInfo(std::map<int, const HepVector > mcTkPars);
    void setType(HoughHitType t){ _type=t; }
	int  outerHit();
	int  continousHit();
	void remove(const HoughHit& hit);


    void print();
    void printAll();
    void printAll(int);
  private:
    std::vector<HoughHit> _houghHitList;
    HoughHitType _type;
};
#endif
