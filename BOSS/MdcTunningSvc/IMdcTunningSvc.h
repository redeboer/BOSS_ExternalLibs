#ifndef IMDCTUNNINGSVC_H_
#define IMDCTUNNINGSVC_H_

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "BesMdcRes.hh"
#include <string>
/* Declaration of the interface ID */
static const InterfaceID IID_IMdcTunningSvc("IMdcTunningSvc",1,0);

class IMdcTunningSvc: virtual public IInterface{
public:
     static const InterfaceID& interfaceID() { return IID_IMdcTunningSvc; }
     virtual BesMdcRes* getMdcRes() =0;
     virtual void setMdcRes(std::string path)=0;
     virtual double NewSig(int layerId, double driftD)=0;
     virtual double DeldriftD(int layerId,double driftD)=0;
     virtual double Delcostta(int layerId,double costta)=0;
     virtual double GetEff(int layerId,int cellId,double driftD,double cosTheta,int posFlag)=0;
     virtual double GetRes(int layerId,int cellId,double driftD,double cosTheta,int posFlag,double entranceAngle,double& mean,double& sigma)=0;
     virtual double ResvEntr(int layerId,double enterA,int ilr,double driftD)=0;
     virtual double DelEtr_Sig(int lay,double enterA,int ilr,double driftD)=0;
     virtual double get_docaEff(int i,int j)=0; 
     virtual double get_thetaEff(int i,int j)=0;
     virtual double get_cellEff(int i,int j)=0;
};

#endif 
