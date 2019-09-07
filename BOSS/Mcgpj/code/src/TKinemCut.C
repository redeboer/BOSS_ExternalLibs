#include "TKinemCut.h"
#include <stdexcept>

TKinemCut::TKinemCut(){
  using namespace std;
  fDeltaTheta = -1.;
  fDeltaPhi   = -1.;
  fPAverage   = -1.;
  fPCross     = -1.;
  fEMin       = -1.;
  fPAverageAbs= -1.;
  fPCrossAbs  = -1.;
  fEMinAbs    = -1.;
  fCosPsi     = -2.;
  fAverageThetaMin = -1.;
  fAverageThetaMax = gConst->Pi() + 1.;
  fThetaMinMinus = -1.;
  fThetaMinPlus = -1.;
  //  cout<<fPAverage<<" "<<fPAverageAbs<<endl;
}

void TKinemCut::Default(){
  fDeltaTheta  = 0.25;
  fDeltaPhi    = 0.15;
  fPAverageAbs = 90.;
  fPCrossAbs   = 90.;
  fEMinAbs     = 50.;
  fCosPsi      = 1.;
  fAverageThetaMin = 1.1;
  fAverageThetaMax = gConst->Pi() - 1.1;
}

void TKinemCut::Init(){
  double E = gGlobal->Get_E();
  if(0>E)
    throw std::logic_error("Energy is not defined!");

  if(0>fDeltaTheta)
    throw std::logic_error("Delta Theta is not defined!");

  if(0>fDeltaPhi)
    throw std::logic_error("Delta Phi is not defined!");

  if(0>fPAverage)
    if(0>fPAverageAbs)
      throw std::logic_error("Average momentum of final particle is not defined!");
    else
    fPAverage = fPAverageAbs/E;
  
  if(0>fPCross)
    if(0>fPCrossAbs)
      throw std::logic_error("Cross momentum for each final particle is not defined!");
    else
    fPCross = fPCrossAbs/E;
  
  if(0>fEMin)
    if(0>fEMinAbs)
      throw std::logic_error("Minumum final particle energy is not defined!");
    else
    fEMin = fEMinAbs/E;
  
  if(-1>fCosPsi)
    throw std::logic_error("Space angle between final particles is not defined!");

  if(0>fAverageThetaMin || gConst->Pi() < fAverageThetaMax )
    throw std::logic_error("Average angle of final particles is not defined!");
    
  fThetaMin = gGlobal->Get_ThetaMin();
  fThetaMax = gConst->Pi() - gGlobal->Get_ThetaMin();
  if(0>fThetaMin || gConst->Pi() < fThetaMax )
    throw std::logic_error("ThetaMin is not defined!");

  if(fThetaMinMinus<0)
    ThetaMinM(fThetaMin);

  if(fThetaMinPlus<0)
    ThetaMinP(fThetaMin);
}

void TKinemCut::Print(){
  using namespace std;
  cout
    <<"Delta Theta      = "<<fDeltaTheta<<" rad"<<endl
    <<"Delta Phi        = "<<fDeltaPhi<<" rad"<<endl
    <<"Average Momentum = "<<fPAverage*gGlobal->Get_E()<<" MeV"<<endl
    <<"Cross Momentum   = "<<fPCross*gGlobal->Get_E()<<" MeV"<<endl
    <<"Minimum Energy   = "<<fEMin*gGlobal->Get_E()<<" MeV"<<endl
    <<"Cosine of Psi    = "<<fCosPsi<<endl
    <<"Average Theta    = "<<fAverageThetaMin<<" rad"<<endl
    <<"ThetaRangeMinus  = from "<<fThetaMinMinus<<" to "<<fThetaMaxMinus<<" rad"<<endl
    <<"ThetaRangePlus   = from "<<fThetaMinPlus<<" to "<<fThetaMaxPlus<<" rad"<<endl
    <<flush;
}
