#include "EventNavigator/NavigationTestAlg.h"

#include "EventNavigator/EventNavigator.h"

#include <cmath>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "PartPropSvc/PartPropSvc.h"

// Monte-Carlo data
#include "McTruth/McParticle.h"
#include "McTruth/MdcMcHit.h"

// MDC reconstructed data
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"

// EMC reconstructed data
#include "EmcRecEventModel/RecEmcShower.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"

using namespace std;
using namespace Event;

NavigationTestAlg::NavigationTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{
  declareProperty("OutputFileName", m_fout="histo.root");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode NavigationTestAlg::initialize(){
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << " Creating histograms" << endreq;

  // create histograms
  m_histo.push_back(new TH1F("deltap","#delta p, true momentum - rec momentum, MeV/c",600,-100,500)); //0
  
  m_histo.push_back(new TH1F("n_parents","Number of parents for reconstructed track",20,0,20)); // 1
  m_histo.push_back(new TH1F("n_tracks","Number of tracks for one MC particle",20,0,20)); // 2
  
  m_histo.push_back(new TH1F("hit_parent","Number of parent MC hits for reconstructed hit",20,-10,10)); // 3
  m_histo.push_back(new TH1F("hit_track","Number of reconstructed hits for MC hit",20,-10,10)); // 4

  m_histo.push_back(new TH1F("true_p","true momentum, MeV/c",1500,0,1500)); // 5
  m_histo.push_back(new TH1F("rec_p","reconstructed momentum, MeV/c",1500,0,1500)); // 6

  m_histo.push_back(new TH1F("n_emcparents","# of parents for reconstructed shower",20,0,20)); // 7
  m_histo.push_back(new TH1F("n_showers","# of showers for one true particle",20,0,20)); // 8

  m_histo.push_back(new TH1F("pdg_miss","Pdg of particles with # trk==0 and #showers > 0",6000,-3000,3000)); //9

  m_histo.push_back(new TH1F("dE_true","True energy - shower energy",40,-2000,2000)); //10
  m_histo.push_back(new TH1F("dE_rec","shower energy - sqrt(Prec**2+m_true**2)",40,-2000,2000)); //11

  m_histo.push_back(new TH1F("E_true","True energy, MeV",100,0,1000)); //12
  m_histo.push_back(new TH1F("E_rec","Rec energy, MeV",100,0,1000)); //13

  m_histo2.push_back(new TH2F("deltap_p","#delta p/p vs. p, MeV",100,0,1000,20,0,2));

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode NavigationTestAlg::execute() {
  MsgStream log(msgSvc(), name());
  
  // Get EventNavigator from the TDS
  SmartDataPtr<EventNavigator> navigator (eventSvc(),"/Event/Navigator");
  if( ! navigator )
    {
      log << MSG::ERROR << " Unable to retrieve EventNavigator" << endreq;
      return StatusCode::FAILURE;
    }

  log << MSG::INFO << "EventNavigator object" << endreq;
  navigator->Print();


  // ======== Analyse Monte-Carlo information using navigator
  log << MSG::INFO << "=======================================================================" << endreq;
  log << MSG::INFO << "MC Particles ===============================================================" << endreq;

  // Get McParticle collection
  SmartDataPtr<McParticleCol> mcParticles(eventSvc(),"/Event/MC/McParticleCol");
  if( ! mcParticles )
    {
      log << MSG::ERROR << " Unable to retrieve McParticleCol" << endreq;
      return StatusCode::FAILURE;
    }
  
  // For each McParticle       
  for( McParticleCol::iterator it = mcParticles->begin();  it != mcParticles->end(); it++ )
    {     
      // get charge of McParticles
      int pdg_code = (*it)->particleProperty();

      // get true momentum value
      double true_mom = (*it)->initialFourMomentum().vect().mag();
      
      // fill true momentum
      m_histo[5]->Fill(true_mom);
      
      log<<MSG::INFO<<"Retrieved McParticle # "<<(*it)->trackIndex() 
	 << " PDG " << pdg_code << " of true momentum " 
	 << true_mom <<" GeV/c " <<endreq;
      
      // get list of reconstructed tracks, which correspond to this particle, using EventNavigator
      RecMdcTrackVector tracks = navigator->getMdcTracks(*it);
      RecMdcKalTrackVector ktracks = navigator->getMdcKalTracks(*it);
      
      // fill the distribution of number of rec. tracks corresponding to single particle
      m_histo[2]->Fill(tracks.size());
      
      log << MSG::INFO << "  Found " << tracks.size() << " tracks and " << ktracks.size() << " Kalman tracks" << endreq;
      
      // for each track
      for(unsigned int i=0; i<ktracks.size(); i++)
	{      
	  // Hep3Vector p = momentum(tracks[i]);
	  double momentum = ktracks[i]->p(); 	     
	  
	  log << MSG::INFO << "\t Track # " << i 
	      << " id = " << ktracks[i]->trackId()
	      << " Pid " << ktracks[i]->getPidType() 
	      <<  " Ptot " << momentum << " GeV/c" << endreq;
	  
	  // fill difference of true momentum and rec momentum for the _same_ track
	  m_histo[0]->Fill(true_mom-momentum);
	  
	  // fill delta_p/p wrt p for the _same_ track
	  m_histo2[0]->Fill(true_mom, fabs(true_mom-momentum)/true_mom);
	}
      
      // get list of reconstructed showers, which correspond to this particle, using EventNavigator
      RecEmcShowerVector showers = navigator->getEmcRecShowers(*it);
      
      m_histo[8]->Fill(showers.size());
      
      log << MSG::INFO << "  Found " << showers.size() << " showers" << endreq;
      
      for(unsigned int i=0; i<showers.size(); i++)
	{
	  double true_energy = (*it)->initialFourMomentum().e();
	  double rec_energy = showers[i]->energy()*1000; // MeV
	  
	  log <<  MSG::INFO << "\t Shower # " << i 
	      << " Id = " << showers[i]->getShowerId().get_value() 
	      << " E = " << showers[i]->energy()*1000 << " MeV " << endreq;
	  
	  m_histo[12]->Fill(true_energy);
	  m_histo[13]->Fill(rec_energy);
	}
    }

  log << MSG::INFO << "MDC Tracks ==============================================================" << endreq;
  // ======== Analyse reconstructed track information using navigator
  
  SmartDataPtr<RecMdcKalTrackCol> mdcKalTracks(eventSvc(),"/Event/Recon/RecMdcKalTrackCol");
  if( ! mdcKalTracks )
    {
      log << MSG::ERROR << " Unable to retrieve MdcKalTrackCol" << endreq;
      return StatusCode::SUCCESS;
    }
    
  for( RecMdcKalTrackCol::iterator it = mdcKalTracks->begin(); it != mdcKalTracks->end(); it++ )
    {   
      McParticleVector particles = navigator->getMcParticles(*it);
 
      log << MSG::INFO << "Retrieved " << particles.size() << " McParticles for for MdcKalTrack # " 
          << (*it)->trackId() << " of reconstructed momentum " << (*it)->p() << " GeV/c (PID=" 
	  << (*it)->getPidType() << ")" << endreq;
      
      // fill rec momentum
      m_histo[6]->Fill((*it)->p());
      
      // fill the distribution of number of parent particles corresponding to this track
      m_histo[1]->Fill(particles.size());
      
      // for each parent particle
      for(unsigned int i=0; i<particles.size(); i++)
	{
	  int pdg_code = particles[i]->particleProperty();
	  
	  // get true momentum value
	  double true_mom = particles[i]->initialFourMomentum().vect().mag();
	
	  // get relevance
	  int relevance = navigator->getMcParticleRelevance(*it, particles[i]);  

	  // dump particle names, momenta and their relevance
	  log << MSG::INFO << "\t" << pdg_code << " momentum " 
	      << true_mom << " relevance " << relevance << endreq; 
	}
    }

  // ======== Analyse reconstructed shower information using navigator
  log << MSG::INFO << "EMC Showers ==============================================================" << endreq;
 SmartDataPtr<RecEmcShowerCol> emcShowers(eventSvc(),"/Event/Recon/RecEmcShowerCol");
  if( ! emcShowers )
    {
      log << MSG::ERROR << " Unable to retrieve EmcRecShowerCol" << endreq;
      return StatusCode::SUCCESS;
    }
  
  int ij = 0;
  for( RecEmcShowerCol::iterator it = emcShowers->begin(); it != emcShowers->end(); it++ )
    {
      McParticleVector particles = navigator->getMcParticles(*it);
      log << MSG::INFO << "Retrieved McParticles for EmcShower # " << ij++ 
	  << " Id " << (*it)->getShowerId().get_value() 
	  << " Energy = " << (*it)->energy() << endreq;

      for(unsigned int i=0; i<particles.size(); i++)
        {
          int pdg_code = particles[i]->particleProperty();

          // get true energy
          double true_e = particles[i]->initialFourMomentum().e();

	  // get relevance
          int relevance = navigator->getMcParticleRelevance(*it, particles[i]);

	  log << "\t Particle " << i << " PDG " << pdg_code << " E=" << true_e 
	      << " Relevance " << relevance << endreq;  
        }
      
      // fill the distribution of number of parent particles corresponding to this track
      m_histo[7]->Fill(particles.size());
    }

  log << MSG::INFO << "=======================================================================" << endreq;
    
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode NavigationTestAlg::finalize() {

  // Save histograms to ROOT file
  TFile of(m_fout.c_str(),"RECREATE");
  of.cd();
  for(vector<TH1F*>::iterator it=m_histo.begin(); it!=m_histo.end(); it++)
    (*it)->Write();
  for(vector<TH2F*>::iterator it=m_histo2.begin(); it!=m_histo2.end(); it++)
    (*it)->Write();
  of.Close();
  
  return StatusCode::SUCCESS;
}
