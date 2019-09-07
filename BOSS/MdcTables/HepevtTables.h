#ifndef _HepevtTables_h_
#define _HepevtTables_h_

#include <string>
#include <iostream>   
#include <vector>           

using namespace std;

//
//*** generated particle information in LUND7 format
//
class Gen_hepevt {
public:
  Gen_hepevt() {};
public:
  int id;
  int isthep;
  int idhep;
  const Gen_hepevt * mother;
  int mo[2];
  int da[2];
  float P[5];
  float V[4];
};

class GenHepevtCol {
public:
   static vector<Gen_hepevt>* getGenHepevtCol(void);
private:
   static vector<Gen_hepevt>* s_col;
};


//
//*** Packed Gen_HEPEVT (main part)
//
class Gen_pakevt {
public:
  Gen_pakevt() {};
public:
  int id;
  int istid;
  int movtx;
  float PX;
  float PY;
  float PZ;
  float E;
};

class GenPakevtCol {
public:
   static vector<Gen_pakevt>* getGenPakevtCol(void);
private:
   static vector<Gen_pakevt>* s_col;
};


//
//*** Packed Gen_HEPEVT (vertex part)
//
class Gen_pakvtx {
public:
  Gen_pakvtx() {};
public:
  int id;
  float VX;
  float VY;
  float VZ;
  float T;
};

class GenPakvtxCol {
public:
   static vector<Gen_pakvtx>* getGenPakvtxCol(void);
private:
   static vector<Gen_pakvtx>* s_col;
};


#endif // _HepevtTables_h_

