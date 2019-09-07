#include <fstream>

#include <math.h>
//#include <string.h>

#include "TruthExamples/HistSample.h"
// #include "GeneratorObject/McEventCollection.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"

#include "GaudiKernel/IHistogramSvc.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ObjectList.h"

// #include "StoreGate/StoreGateSvc.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
//  #include "CLHEP/HepMC/GenEvent.h"
//  #include "CLHEP/HepMC/GenParticle.h"
//  #include "CLHEP/HepMC/GenVertex.h"
//  #include "CLHEP/HepMC/ParticleDataTableConfig.h"

// #include "PartPropSvc/PartPropSvc.h"
#include "HepPDT/ParticleData.hh"

#include "GeneratorModule/GeneratorName.h"

#
//static const AlgFactory<HistSample>    Factory;
//const IAlgFactory& HistSampleFactory = Factory;

HistSample::HistSample(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_hgenerated(0), m_hfinal(0), m_ncharged(0),
  m_hChargedPt(0), m_hChargedEta(0),
  m_hZPtall(0), m_hZPt(0), m_hZPte(0), m_hZPtm(0), m_hZPtt(0),
  m_massZall(0), m_massZ(0), m_massZe(0), m_massZm(0), m_massZt(0),
  m_hPtPaire(0), m_hPtPairm(0), m_hPtPairt(0),
  m_massPaire(0), m_massPairm(0), m_massPairt(0),
  m_rapidity(0), m_pseudorapidity(0), m_hpte()
{
//Declare the algorithm's properties
  declareProperty("HistogramFlag", m_produceHistogram = true );
//  declareProperty("HistogramFlag", m_produceHistogram = false );

}

StatusCode HistSample::initialize(){

  StatusCode result = StatusCode::SUCCESS;
  MsgStream msglog(messageService(), name());
  msglog << MSG::INFO << ">>> HistSample from Initialize" << endreq;
  // cout << "----- HistSample World From initialize" << endl;

  /*
  StatusCode sc = service("StoreGateSvc", m_sgSvc);
  if (sc.isFailure()) {
    msglog << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return sc;
  }
  */

  /*
  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc;
  static const bool CREATEIFNOTTHERE(true);
  StatusCode PartPropStatus = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  if (!PartPropStatus.isSuccess() || 0 == p_PartPropSvc) {
    msglog << MSG::ERROR << " Could not initialize Particle Properties Service" << endreq;
    return PartPropStatus;
  }      

  m_particleTable = p_PartPropSvc->PDT();
  */


  // Register the histograms
//        m_hgenerated = histoSvc()->book("/stat/1Dhist/1","Generated",100,0,1000);
//        if( 0 == m_hgenerated ) {
//          msglog << MSG::ERROR << "Cannot register histo Generated" << endreq;
//        }

        m_hgenerated = histoSvc()->book("/stat/1Dhist/1","Generated",100,0,1200);
        if (0 == m_hgenerated) {
          msglog << MSG::ERROR << " ERROR booking histogram" << endreq;
          result = StatusCode::FAILURE;
        }
        m_hfinal = histoSvc()->book("/stat/1Dhist/2","Final state",100,0,800);
        m_ncharged = histoSvc()->book("/stat/1Dhist/3","number of charged tracks",100,0,500);
        m_hChargedPt = histoSvc()->book("/stat/1Dhist/4","Pt of charged tracks",100,0,100);
        m_hChargedEta = histoSvc()->book("/stat/1Dhist/5","Pseudorapidity of charged tracks",100,-12,12);
        m_hZPtall = histoSvc()->book("/stat/1Dhist/6","ZPt, all",100,0,200);
        m_hZPt = histoSvc()->book("/stat/1Dhist/7","ZPt, charged leptons",100,0,200);
        m_hZPte = histoSvc()->book("/stat/1Dhist/8","ZPt electrons",100,0,200);
        m_hZPtm = histoSvc()->book("/stat/1Dhist/9","ZPt muons",100,0,200);
        m_hZPtt = histoSvc()->book("/stat/1Dhist/10","ZPt taus",100,0,200);
        m_massZall = histoSvc()->book("/stat/1Dhist/11","Z mass, all",40,70,110);
        m_massZ = histoSvc()->book("/stat/1Dhist/12","Z mass, charged leptons",40,70,110);
        m_massZe = histoSvc()->book("/stat/1Dhist/13","Z mass, electrons",40,70,110);
        m_massZm = histoSvc()->book("/stat/1Dhist/14","Z mass, muons",40,70,110);
        m_massZt = histoSvc()->book("/stat/1Dhist/15","Z mass, taus",40,70,110);
        m_hPtPaire = histoSvc()->book("/stat/1Dhist/16","Pt electron pairs",100,0,200);
        m_hPtPairm = histoSvc()->book("/stat/1Dhist/17","Pt muon pairs",100,0,200);
        m_hPtPairt = histoSvc()->book("/stat/1Dhist/18","Pt tau pairs",100,0,200);
        m_massPaire = histoSvc()->book("/stat/1Dhist/19","mass, electron pairs",40,70,110);
        m_massPairm = histoSvc()->book("/stat/1Dhist/20","mass, muon pairs",40,70,110);
        m_massPairt = histoSvc()->book("/stat/1Dhist/21","mass, tau pairs",40,70,110);
        m_rapidity = histoSvc()->book("/stat/1Dhist/22","Rapidity of charged tracks",100,-12,12);
        m_pseudorapidity = histoSvc()->book("/stat/1Dhist/23","Pseudorapidity of charged tracks",100,-12,12);
        m_hpte = histoSvc()->book("/stat/1Dhist/24","electon pt",100,0,100);

// Initialization terminated
//        return StatusCode::SUCCESS;
        return result;
}
StatusCode HistSample::execute() {


         MsgStream msglog(messageService(), name());
         msglog << MSG::INFO << ">>> HistSample from execute" << endreq;
//         cout << "----- HistSample World From execute" << endl;

// Read Data from Transient Store
/*
         const McEventCollection* mcCollptr;
         if ( m_sgSvc->retrieve(mcCollptr).isFailure() ) {
           msglog << MSG::ERROR << "Could not retrieve McEventCollection"
 	      << endreq;
           return StatusCode::FAILURE;
         }

//         cout << " ---- Begin Iterating Over McEventCollection --- " << endl;
         McEventCollection::const_iterator it;
         for(it=mcCollptr->begin(); it!=mcCollptr->end(); it++) {
//           cout << "     Generator: " << (*it)->generatorName() << endl;
//// Dump on screen
//           (*it)->pGenEvt()->print();

// fix the STDHEP mess for the Z status
           int properStatus = 2;
//            switch ( (*it)->generatorName()[0] ) {
//            case 'P':
//              properStatus = 2;
//              break;
//            case 'I':
//              properStatus = 12;
//              break;
//            case 'H':
//              properStatus = 195;
//              break;
//            default:
//              properStatus = 2;
//              break;
//            }
           HepMC::GenEvent* genEvt = (*it);
	   int g_id = genEvt->signal_process_id();
           switch ( first_generator(g_id) ) {
           case PYTHIA:
             properStatus = 2;
             break;
           case ISAJET:
             properStatus = 12;
             break;
           case HERWIG:
             properStatus = 195;
             break;
           default:
             properStatus = 2;
             break;
           }


           int number_particles = 0;
           int final_state = 0;
           int number_charged = 0;
// Iterate over MC particles
           for(HepMC::GenEvent::particle_iterator pitr=genEvt->particles_begin();
             pitr!=genEvt->particles_end(); ++pitr ){
             ++number_particles;
//             cout << "particle " << ((*pitr)->pdg_id()) << " status " << ((*pitr)->status()) << endl;

// Z decays
             if( ((*pitr)->pdg_id()) == 23 && ((*pitr)->status()) == properStatus ){
               HepMC::GenVertex::particle_iterator firstChild =
                  (*pitr)->end_vertex()->particles_begin(HepMC::children);
               HepMC::GenVertex::particle_iterator endChild =
                  (*pitr)->end_vertex()->particles_end(HepMC::children);
// plot Pt and mass of the Z (generator values)
               if( ((*firstChild)->pdg_id()) != 23 ){
                 double ZPt = (*pitr)->momentum().perp();
                 m_hZPtall->fill( ZPt, 1.);
                 double Zmass = (*pitr)->momentum().m();
                 m_massZall->fill(Zmass, 1.);
               }
// Z decays to l+l-
               double sumx = 0.0;
               double sumy = 0.0;
               double sumz = 0.0;
               double sume = 0.0;
               int nelds = 0;
               int nmuds = 0;
               int ntauds = 0;
               int Zchild = 0;
               HepMC::GenVertex::particle_iterator thisChild = firstChild;
               for(; thisChild != endChild++; ++thisChild){
                  if( ((*thisChild)->pdg_id()) != 23 ){
//                    cout << "Zchild id " << (*thisChild)->pdg_id() << endl;
                    ++Zchild;
                    if( abs((*thisChild)->pdg_id()) == 11 || abs((*thisChild)->pdg_id()) == 13 || 
                        abs((*thisChild)->pdg_id()) == 15 ){
                      if( abs((*thisChild)->pdg_id()) == 11 )++nelds;
                      if( abs((*thisChild)->pdg_id()) == 13 )++nmuds;
                      if( abs((*thisChild)->pdg_id()) == 15 )++ntauds;
                      sumx += (*thisChild)->momentum().x();
                      sumy += (*thisChild)->momentum().y();
                      sumz += (*thisChild)->momentum().z();
                      sume += (*thisChild)->momentum().e();
                    }
                  }
               }
//               cout << "Zchildren " << Zchild << " nelds " << nelds << " nmuds " << nmuds  << " ntauds " << ntauds << endl;
               if( Zchild != 0 ){
                 double PtZ2 = sumx*sumx + sumy*sumy;
                 double PtZ = 0.;
                 if(PtZ2 > 0.) PtZ = sqrt(PtZ2);
                 if(PtZ != 0.)m_hZPt->fill( PtZ, 1.);
                 double massZ2 = sume*sume - sumx*sumx - sumy*sumy - sumz*sumz;
                 double massZ = 0.;
                 if(massZ2 > 0.) massZ = sqrt(massZ2);
                 if(massZ != 0.)m_massZ->fill( massZ, 1.);
                 if(nelds == 2){
                   m_hZPte->fill( PtZ, 1.);
                   m_massZe->fill( massZ, 1.);
                 }
                 if(nmuds == 2){
                   m_hZPtm->fill( PtZ, 1.);
                   m_massZm->fill( massZ, 1.);
                 }
                 if(ntauds == 2){
                   m_hZPtt->fill( PtZ, 1.);
                   m_massZt->fill( massZ, 1.);
                 }
               }
             }
// all decays to l+l- pairs in the event
             if( (*pitr)->end_vertex() ){
               HepMC::GenVertex::particle_iterator fstChild =
                  (*pitr)->end_vertex()->particles_begin(HepMC::children);
               HepMC::GenVertex::particle_iterator lstChild =
                  (*pitr)->end_vertex()->particles_end(HepMC::children);
               double sumpx = 0.0;
               double sumpy = 0.0;
               double sumpz = 0.0;
               double sumpe = 0.0;
               int nel = 0;
               int nmu = 0;
               int ntau = 0;
               HepMC::GenVertex::particle_iterator aChild = fstChild;
               for(; aChild != lstChild++; ++aChild){
                  if( abs((*aChild)->pdg_id()) == 11 || abs((*aChild)->pdg_id()) == 13 ||
                      abs((*aChild)->pdg_id()) == 15 ){
                      if( abs((*aChild)->pdg_id()) == 11 )++nel;
                      if( abs((*aChild)->pdg_id()) == 13 )++nmu;
                      if( abs((*aChild)->pdg_id()) == 15 )++ntau;
                      sumpx += (*aChild)->momentum().x();
                      sumpy += (*aChild)->momentum().y();
                      sumpz += (*aChild)->momentum().z();
                      sumpe += (*aChild)->momentum().e();
                  }
               }
               if(nel == 2 || nmu == 2 || ntau == 2 ){
                 double PtPair2 = sumpx*sumpx + sumpy*sumpy;
                 double PtPair = 0.;
                 if(PtPair2 > 0.) PtPair = sqrt(PtPair2);
                 double massPair2 = sumpe*sumpe - sumpx*sumpx - sumpy*sumpy - sumpz*sumpz;
                 double massPair = 0.;
                 if(massPair2 > 0.) massPair = sqrt(massPair2);
                 if(nel == 2){
                   m_hPtPaire->fill( PtPair, 1.);
                   m_massPaire->fill( massPair, 1.);
                 }
                 if(nmu == 2){
                   m_hPtPairm->fill( PtPair, 1.);
                   m_massPairm->fill( massPair, 1.);
                 }
                 if(ntau == 2){
                   m_hPtPairt->fill( PtPair, 1.);
                   m_massPairt->fill( massPair, 1.);
                 }
               }
             }
// charged tracks
             double c = 0.;
	     HepPDT::ParticleData* ap = m_particleTable->particle( abs( (*pitr)->pdg_id() ) );

             if(!ap){
//               std::cout << "ChargeService WARNING: abs(id) "
//                    << abs((*pitr)->pdg_id())
//                    << " is not in particle data table" << std::endl;
             }
             else
             {
                c = ap->charge();
             }

             if( ((*pitr)->status() == 1) && ( ! (*pitr)->end_vertex()) ) {
               ++final_state;
               if( ((*pitr)->pdg_id()) == 11 ){
                 double ePt = (*pitr)->momentum().perp();
                 m_hpte->fill( ePt, 1.);
               }
               if(c!=0.){
                 ++number_charged;
                 double chargedPt = (*pitr)->momentum().perp();
                 double chargedEta = (*pitr)->momentum().pseudoRapidity();
                 m_hChargedPt->fill( chargedPt, 1.);
                 m_hChargedEta->fill( chargedEta, 1.);

                 double px = (*pitr)->momentum().x();
                 double py = (*pitr)->momentum().y();
                 double pz = (*pitr)->momentum().z();
                 double pe = (*pitr)->momentum().e();
                 double pp2 = px*px + py*py + pz*pz;
                 double pp3 = 0.;
                 if(pp2 > 0.) pp3 = sqrt(pp2);
                 double y = -999.;
                 if( (pe-pz) != 0. && (pe+pz)/(pe-pz) > 0. ) y = (1./2.)*log((pe+pz)/(pe-pz));
                 double eta = -999.;
                 if( (pp3-pz) != 0. && (pp3+pz)/(pp3-pz) > 0. ) eta = (1./2.)*log((pp3+pz)/(pp3-pz));
                 m_rapidity->fill( y, 1.);
                 m_pseudorapidity->fill( eta, 1.);
               }
             }
           }
           m_hgenerated->fill( number_particles, 1.);
           m_hfinal->fill( final_state, 1.);
           m_ncharged->fill( number_charged, 1.);

         }
*/
// End of execution for each event
         return StatusCode::SUCCESS;
}
StatusCode HistSample::finalize() {

  MsgStream msglog(messageService(), name());
        msglog << MSG::INFO << ">>> HistSample from finalize" << endreq;
//        cout << "----- HistSample World From finalize" << endl;

  // End of finalization step
        return StatusCode::SUCCESS;
}


