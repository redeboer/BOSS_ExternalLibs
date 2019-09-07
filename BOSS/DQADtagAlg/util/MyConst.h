#ifndef QIN_MY_CONST
#define QIN_MY_CONST
const double mpi = 0.13957;
const double mk  = 0.493677;
const double mD0  = 1.86484;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
//const double velc = 29.9792458;  tof_path unit in cm.
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<double> Vdouble;
typedef std::vector<HepLorentzVector> Vp4;
#endif
