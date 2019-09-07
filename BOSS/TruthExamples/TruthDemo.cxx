// Example algoritm to demonstrate how to use 
// the predicates in Genarators/GenAnalysisTools/TruthHelper
// Ian Hinchliffe
// January 2003#include <math.h>
//
#include "TruthExamples/TruthDemo.h"
#include "GeneratorObject/McEventCollection.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"

#include "GaudiKernel/IHistogramSvc.h"

#include "TruthHelper/IsGenStable.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ObjectList.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
// #include "HepMC/ParticleData.h"
// #include "HepMC/ParticleDataTable.h"
// #include "HepMC/IO_PDG_ParticleDataTable.h"
#include "HepMC/GenVertex.h"

typedef std::vector<const HepMC::GenParticle*>  MCparticleCollection ;


static const AlgFactory<TruthDemo>    Factory;
const IAlgFactory& TruthDemoFactory = Factory;

TruthDemo::TruthDemo(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
//Declare the algorithm's properties
  declareProperty("HistogramFlag", m_produceHistogram = true );
//  declareProperty("HistogramFlag", m_produceHistogram = false );
}

StatusCode TruthDemo::initialize(){
  StatusCode result = StatusCode::SUCCESS;
  MsgStream msglog(messageService(), name());
  msglog << MSG::INFO << ">>> Truthdemo from Initialize" << endreq;
  m_hgenerated = histoSvc()->book("/stat/1Dhist/1","Generated",100,0,1200);
  if (0 == m_hgenerated) {
    msglog << MSG::ERROR << " ERROR booking histogram" << endreq;
    result = StatusCode::FAILURE;
  }
  m_pxBalance = histoSvc()->book("/stat/1Dhist/25","px balance",50,-10.,10.);
  m_pyBalance = histoSvc()->book("/stat/1Dhist/26","py balance",50,-10.,10.);
  m_totEnergy = histoSvc()->book("/stat/1Dhist/27","total energy",50,10000.,20000.);
  m_tesIO = new GenAccessIO();

//        return StatusCode::SUCCESS;
  return result;
}
StatusCode TruthDemo::execute() {
//   HepMC::IO_PDG_ParticleDataTable pdg_io("PDGTABLE");
//   static HepMC::ParticleDataTable *pp = NULL;
//   if (pp == NULL) pp = pdg_io.read_particle_data_table();
  MsgStream msglog(messageService(), name());
  msglog << MSG::INFO << ">>> TruthDemo from execute" << endreq;
    //
  // Px/Py Balance and total energy
  //
  float totenergy = 0.;
  float pxbalance = 0.;
  float pybalance = 0.;
    // Iterate over MC particles  We are using the IsGenStable predicate from
    IsGenStable ifs;
  std::vector<const HepMC::GenParticle*> particles;
  StatusCode stat = m_tesIO->getMC(particles, &ifs);
  for (std::vector<const HepMC::GenParticle*>::iterator pitr = particles.begin();
       pitr != particles.end(); pitr++) {	   
    pxbalance += (*pitr)->momentum().x();
    pybalance += (*pitr)->momentum().y();
    totenergy += (*pitr)->momentum().e();
  }
  m_pxBalance->fill(pxbalance, 1.);
  m_pyBalance->fill(pybalance, 1.);
  m_totEnergy->fill(totenergy, 1.);
  // End of execution for each event
  return StatusCode::SUCCESS;
}

StatusCode TruthDemo::finalize() {
  MsgStream msglog(messageService(), name());
        msglog << MSG::INFO << ">>> TruthDemo from finalize" << endreq;
        return StatusCode::SUCCESS;
}


