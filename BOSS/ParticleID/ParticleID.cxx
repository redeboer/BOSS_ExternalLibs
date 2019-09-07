#include <iostream>
// #include <cmath>
#include <cstdlib>

#include "ParticleID/ParticleID.h"

#ifndef BEAN
#include "EvtRecEvent/EvtRecTrack.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "EventModel/EventHeader.h"
#endif

//
// Author: K.L. He & L. L. Wang & Gang.Qin, 01/07/2007 created

ParticleID * ParticleID::m_pointer = 0;

ParticleID * ParticleID::instance() {
   if(!m_pointer) m_pointer = new ParticleID();
   return m_pointer;
}

void ParticleID::init() {

   if(IsDedxInfoUsed()) {
      if(!m_dedxpid) m_dedxpid = DedxPID::instance();
      m_dedxpid->init();
   }

   if(IsTofInfoUsed()|IsTof1InfoUsed()|IsTof2InfoUsed()) {
      if(!m_tofpid) m_tofpid = TofPID::instance();
      m_tofpid->init();
   }

   if(IsTofCorrInfoUsed()) {
     if(!m_tofcorrpid) m_tofcorrpid = TofCorrPID::instance();
     //      m_tofcorrpid->init();
   }

   if(IsEmcInfoUsed()) {
      if(!m_emcpid) m_emcpid = EmcPID::instance();
      m_emcpid->init();
   }

   if(IsMucInfoUsed()) {
      if(!m_mucpid) m_mucpid = MucPID::instance();
      m_mucpid->init();
   }

   // global info.
   m_pidsys = 0;
   m_pidcase = 0;
   m_method = 0;
   m_TotalLikelihood =0;
   m_discard =1;
   // probability
   m_ndof = 0;
   m_nhitcut=5;
   // chi cut
   setChiMinCut( 4.0 );
   setChiMaxCut( 6.0 );
   for(int i = 0; i < 4; i++) {
      m_chisq[i] = 9999.;
      m_prob[i] = -1.0;
   }
}


ParticleID::ParticleID() : ParticleIDBase() {
   m_dedxpid = 0;
   m_tofpid = 0;
   m_tofepid = 0;
   m_tofqpid = 0;
   m_tofcpid = 0;
   m_tofcorrpid = 0;
   m_emcpid = 0;
   m_mucpid = 0;



}


ParticleID::~ParticleID() {
   //  if(m_dedxpid) delete m_dedxpid;
   //  if(m_tof1pid) delete m_tof1pid;
   //  if(m_tof2pid) delete m_tof2pid;
   //  if(m_tofepid) delete m_tofepid;
   //  if(m_tofqpid) delete m_tofqpid;
   //  if(m_emcpid) delete m_emcpid;
}

void ParticleID::calculate() {
#ifdef BEAN
   cout << " please use ParticleID::calculate(run) ! " << endl;
   exit(1);
}


void ParticleID::calculate(int run)
{
#endif
   int nhitcutpid=getNhitCut();

#ifndef BEAN
   IDataProviderSvc* m_eventSvc;
   Gaudi::svcLocator()->service("EventDataSvc", m_eventSvc, true);

   SmartDataPtr<Event::EventHeader> eventHeaderpid(m_eventSvc,"/Event/EventHeader");
   int runpid=eventHeaderpid->runNumber();
   int eventpid=eventHeaderpid->eventNumber();
   //   cout<<"runpid="<<runpid<<endl;
   //   cout<<"eventpid="<<eventpid<<endl;
#else
   int runpid=run;
#endif

   EvtRecTrack* recTrk = PidTrk();
   //  int runnum=getRunNo();
   //  cout<<"runnum="<<runnum<<endl;
   // if user did not specify sub sys, sepcify the default value
   if(m_pidsys == 0) {
      m_pidsys = useDedx() | useTof() | useTofE() | useEmc() | useMuc() | useTofQ() | useTofC() | useTofCorr();
   }
   // if user did not set the seperate case, set the default value

   if(m_pidcase == 0 ) {
      m_pidcase = all();
   }
   //dedx sys
   if(IsDedxInfoUsed()) {
      if(!m_dedxpid) m_dedxpid = DedxPID::instance();
      m_dedxpid->init();
      m_dedxpid->setRunNo(runpid);
      m_dedxpid->setNhitCutDx(nhitcutpid);
      m_dedxpid->setRecTrack(recTrk);
      m_dedxpid->setChiMinCut(chiMinCut());
      m_dedxpid->setPdfMinSigmaCut(pdfMinSigmaCut());
      m_dedxpid->calculate();
   }

   // tof1 and tof2 sys
   if(IsTofInfoUsed()|IsTof1InfoUsed()|IsTof2InfoUsed()|IsTofCInfoUsed())
   {
      if(IsTofCInfoUsed())
      {
         if(!m_tofcpid) m_tofcpid = TofCPID::instance();
         m_tofcpid->init();
         m_tofcpid->setRunNo(runpid);
         m_tofcpid->setRecTrack(recTrk);
         m_tofcpid->setChiMinCut(chiMinCut());
         m_tofcpid->setPdfMinSigmaCut(pdfMinSigmaCut());
         m_tofcpid->calculate();
      }
      else
      {
         if(!m_tofpid) m_tofpid = TofPID::instance();
         m_tofpid->init();
         m_tofpid->setRecTrack(recTrk);
         m_tofpid->setChiMinCut(chiMinCut());
         m_tofpid->setPdfMinSigmaCut(pdfMinSigmaCut());
         m_tofpid->calculate();
      }

   }


   // tof secondary correction sys
   if(IsTofCorrInfoUsed()) {
     if(!m_tofcorrpid) m_tofcorrpid = TofCorrPID::instance();
     m_tofcorrpid->setRunNo(runpid);
     m_tofcorrpid->init();
     m_tofcorrpid->setRecTrack(recTrk);
     m_tofcorrpid->setChiMinCut(chiMinCut());
     m_tofcorrpid->setChiMaxCut(chiMaxCut());
     m_tofcorrpid->setPdfMinSigmaCut(pdfMinSigmaCut());
     m_tofcorrpid->calculate();
   }

   /*
     // tof1 sys
     if(IsTof1InfoUsed()){
       if(!m_tof1pid) m_tof1pid = Tof1PID::instance();
       m_tof1pid->init();
       m_tof1pid->setRecTrack(recTrk);
       m_tof1pid->setChiMinCut(4);
       m_tof1pid->setPdfMinSigmaCut(4);
       m_tof1pid->calculate();
     }

     // tof2 sys
     if(IsTof2InfoUsed()){
       if(!m_tof2pid) m_tof2pid = Tof2PID::instance();
       m_tof2pid->init();
       m_tof2pid->setRecTrack(recTrk);
       m_tof2pid->setChiMinCut(4);
       m_tof2pid->setPdfMinSigmaCut(4);
       m_tof2pid->calculate();
     }

   */
   // tofq sys
   if(IsTofQInfoUsed()) {
      if(!m_tofqpid) m_tofqpid = TofQPID::instance();
      m_tofqpid->init();
      m_tofqpid->setRecTrack(recTrk);
      m_tofqpid->setChiMinCut(chiMinCut());
      m_tofqpid->calculate();
   }

   // endcap tof sys
   if(IsTofEInfoUsed()&&(!IsTofCorrInfoUsed())) {
      if(!m_tofepid) m_tofepid = TofEPID::instance();
      m_tofepid->init();
      m_tofepid->setRecTrack(recTrk);
      m_tofepid->setChiMinCut(chiMinCut());
      m_tofepid->setPdfMinSigmaCut(pdfMinSigmaCut());
      m_tofepid->calculate();
   }
   // emc sys
   if(IsEmcInfoUsed()) {
      if(!m_emcpid) m_emcpid = EmcPID::instance();
      m_emcpid->init();
      m_emcpid->setRecTrack(recTrk);
      m_emcpid->setChiMinCut(chiMinCut());
      m_emcpid->calculate();
   }

   // muc sys
   if(IsMucInfoUsed()) {
      if(!m_mucpid) m_mucpid = MucPID::instance();
      m_mucpid->init();
      m_mucpid->setRecTrack(recTrk);
      m_mucpid->setChiMinCut(chiMinCut());
      m_mucpid->calculate();
   }
   // probability method
   if((m_method & methodProbability()) == methodProbability())
      if(particleIDCalculation() < 0) m_ndof = 0;
   //    std::cout<<"m_ndof="<<m_ndof<<std::endl;

   //likelihood method
   if((m_method & methodLikelihood()) == methodLikelihood())
      if(LikelihoodCalculation() < 0) m_discard =0;
   // neuron network
   if((m_method & methodNeuronNetwork()) == methodNeuronNetwork())
      //    m_neuronPid = neuronPIDCalculation();
      if(LikelihoodCalculation() < 0) m_discard =0;

}

int ParticleID ::particleIDCalculation() {
   int irc = -1;
   bool valid = IsDedxInfoValid() || IsTofInfoValid()||IsTofEInfoValid()
                || IsTofQInfoValid() || IsEmcInfoValid() || IsMucInfoValid()
                || IsTofCInfoValid() || IsTofCorrInfoValid();

   if(!valid) return irc;

   double chisq[5];
   bool pidcase[5];
   for(int i = 0; i < 5; i++) {
      chisq[i] = 0;
      pidcase[i] = false;
   }

   if((m_pidcase & onlyElectron()) == onlyElectron()) pidcase[0] = true;
   if((m_pidcase & onlyMuon()) == onlyMuon()) pidcase[1] = true;
   if((m_pidcase & onlyPion()) == onlyPion()) pidcase[2] = true;
   if((m_pidcase & onlyKaon()) == onlyKaon()) pidcase[3] = true;
   if((m_pidcase & onlyProton()) == onlyProton()) pidcase[4] = true;

   //
   //  dEdx PID
   //
   if(IsDedxInfoUsed()) {
      if(IsDedxInfoValid()) {
         bool okpid = false;
         for(int i = 0; i < 5; i++) {
            if(pidcase[i] && (fabs(chiDedx(i)) < m_dedxpid->chiMinCut()))
               if(!okpid) okpid = true;
         }
         if(okpid) {
            m_ndof++;
            for(int i = 0; i < 5; i++) chisq[i] += chiDedx(i)*chiDedx(i);


         }
      } // dE/dx
   }
   //
   //  Barrel TOF
   //

   if(IsTofInfoUsed()|IsTof1InfoUsed()|IsTof2InfoUsed() | IsTofCInfoUsed())
   {  if(IsTofCInfoUsed())
      {
         if(IsTofCInfoValid()) {
            bool okpid = false;
            for(int i = 0; i < 5; i++) {
               if(pidcase[i] && (fabs(chiTofC(i)) < m_tofcpid->chiMinCut()))
                  if(!okpid) okpid = true;
            }
            if(okpid) {
               m_ndof++;
               for(int i = 0; i < 5; i++) chisq[i] += chiTofC(i)*chiTofC(i);
            }
         }  // TOF1
      }
      else {
         if(IsTofInfoValid()) {
            bool okpid = false;
            for(int i = 0; i < 5; i++) {
               if(pidcase[i] && (fabs(chiTof(i)) < m_tofpid->chiMinCut()))
                  if(!okpid) okpid = true;
            }
            if(okpid) {
               m_ndof++;
               for(int i = 0; i < 5; i++) chisq[i] += chiTof(i)*chiTof(i);
            }
         }  // TOF1


         //
         //  EndCap Tof
         //

         if(IsTofEInfoUsed()) {
            if(IsTofEInfoValid()) {
               bool okpid = false;
               for(int i = 0; i < 5; i++) {
                  if(pidcase[i] && (fabs(chiTofE(i)) < m_tofepid->chiMinCut()))
                     if(!okpid) okpid = true;
               }
               if(okpid) {
                  m_ndof++;
                  for(int i = 0; i < 5; i++) chisq[i] += chiTofE(i)*chiTofE(i);
               }
            } // EndCap TOF
         }

      }
   }

   // Secondary TOF correction
   if(IsTofCorrInfoUsed()) {
     if(IsTofCorrInfoValid()) {
       bool okpid = false;
       for(int i = 0; i < 5; i++) {
	 if(pidcase[i] && ( chiTofCorr(i) < m_tofcorrpid->chiMaxCut() ) && ( chiTofCorr(i) > ( 0.0 - m_tofcorrpid->chiMinCut() ) ) ) 
	    //	 if(pidcase[i] && ( chiTofCorr(i) < 6.0 && chiTofCorr(i) > -4.0 ) )
	   if(!okpid) okpid = true;
       }
       if(okpid) {
	 m_ndof++;
	 for(int i = 0; i < 5; i++) chisq[i] += chiTofCorr(i)*chiTofCorr(i);
       }
     }
   }

   //
   //  Barrel TOF Q
   //

   if(IsTofQInfoUsed()) {
      if(IsTofQInfoValid()) {
         bool okpid = false;
         for(int i = 0; i < 5; i++) {
            if(pidcase[i] && (fabs(chiTofQ(i)) < m_tofqpid->chiMinCut()))
               if(!okpid) okpid = true;
         }
         if(okpid) {
            m_ndof++;
            for(int i = 0; i < 5; i++) chisq[i] += chiTofQ(i)*chiTofQ(i);
         }
      } // TofQ
   }

   // Muc Pid
   if(IsMucInfoUsed()) {
      if(IsMucInfoValid()) {
         bool okpid = false;
         for(int i = 0; i < 5; i++) {
            if(pidcase[i] && (fabs(chiMuc(i)) < m_mucpid->chiMinCut()))
               if(!okpid) okpid = true;
         }
         if(okpid) {
            m_ndof++;
            for(int i = 0; i < 5; i++) chisq[i] += chiMuc(i)*chiMuc(i);
         }
      } // Muc Pid
   }


   //  Emc PID
   if(IsEmcInfoUsed()) {
      if(IsEmcInfoValid()) {
         bool okpid = false;
         for(int i = 0; i < 5; i++) {
            if(pidcase[i] && (fabs(chiEmc(i)) < m_emcpid->chiMinCut()))
               if(!okpid) okpid = true;
         }
         if(okpid) {
            m_ndof++;
            for(int i = 0; i < 5; i++) chisq[i] += chiEmc(i)*chiEmc(i);
         }
      } // Emc Pid
   }


   if(m_ndof <= 0) return irc;


   for(int i = 0; i < 5; i++) {
      m_chisq[i] = chisq[i];
      m_prob[i] = probCalculate(chisq[i], m_ndof);
   }


   irc = 0;
   return irc;
}





int ParticleID::LikelihoodCalculation() {
   int irc = -1;

   bool valid = IsDedxInfoValid() || IsTofInfoValid() || IsTofEInfoValid() || IsTofQInfoValid() || IsTofCInfoValid() || IsTofCorrInfoValid() || IsEmcInfoValid()||IsMucInfoValid();
   if(!valid) return irc;
   double pdf[5];
   bool pidcase[5];
   for(int i = 0; i < 5; i++) {
      pdf[i] = 1;
      pidcase[i] = false;
   }

   if((m_pidcase & onlyElectron()) == onlyElectron()) pidcase[0] = true;
   if((m_pidcase & onlyMuon()) == onlyMuon()) pidcase[1] = true;
   if((m_pidcase & onlyPion()) == onlyPion()) pidcase[2] = true;
   if((m_pidcase & onlyKaon()) == onlyKaon()) pidcase[3] = true;
   if((m_pidcase & onlyProton()) == onlyProton()) pidcase[4] = true;

   for(int i = 0; i < 5; i++) {
      if(pidcase[i]==0)
         pdf[i]=0;
   }

   //
   //  dEdx PID
   //
   if(IsDedxInfoUsed()) {
      if(IsDedxInfoValid()) {
         bool okpid = false;
         for(int i = 0; i < 5; i++) {
            if(pidcase[i] && pdfCalculate(chiDedx(i),1) > pdfCalculate(m_dedxpid->pdfMinSigmaCut(),1.5))
               if(!okpid) okpid = true;
         }
         if(okpid) {
            m_ndof++;
            for(int i = 0; i < 5; i++) {
               pdf[i] *= pdfDedx(i);
            }
         }
      } // dE/dx
   }


   //
   //  Barrel TOF
   //
   if(IsTofInfoUsed()|IsTof1InfoUsed()|IsTof2InfoUsed()|IsTofCInfoUsed())
   {  if(IsTofCInfoUsed())
      {

         if(IsTofCInfoValid()) {
            bool okpid = false;
            for(int i = 0; i < 5; i++) {
               if(pidcase[i] && pdfCalculate(chiTof(i),1) > pdfCalculate(m_tofcpid->pdfMinSigmaCut(),1.5))
                  if(!okpid) okpid = true;
            }
            if(okpid) {
               m_ndof++;
               for(int i = 0; i < 5; i++) {
                  pdf[i] *= pdfTofC(i);
               }
            }
         }  // TOF
      }

      else {
         if(IsTofInfoValid()) {
            bool okpid = false;
            for(int i = 0; i < 5; i++) {
               if(pidcase[i] && pdfCalculate(chiTof(i),1) > pdfCalculate(m_tofpid->pdfMinSigmaCut(),1.5))
                  if(!okpid) okpid = true;
            }
            if(okpid) {
               m_ndof++;
               for(int i = 0; i < 5; i++) {
                  pdf[i] *= pdfTof(i);
               }
            }
         }  // TOF



         //
         //  EndCap Tof
         //

         if(IsTofEInfoUsed()) {
            if(IsTofEInfoValid()) {
               bool okpid = false;
               for(int i = 0; i < 5; i++) {
                  if(pidcase[i]&& pdfCalculate(chiTofE(i),1) > pdfCalculate(m_tofepid->pdfMinSigmaCut(),1.5))
                     if(!okpid) okpid = true;
               }
               if(okpid) {
                  //      m_ndof++;
                  //      for(int i = 0; i < 5; i++) pdf[i] *= pdfTofE(i);
               }
            } // EndCap TOF
         }
      }

   }

   // Secondary TOF correction
   if(IsTofCorrInfoUsed()) {
     if(IsTofCorrInfoValid()) {
       bool okpid = false;
       for(int i = 0; i < 5; i++) {
	 if(pidcase[i] && pdfCalculate(chiTofCorr(i),1) > pdfCalculate(m_tofcorrpid->pdfMinSigmaCut(),1.5))
	   if(!okpid) okpid = true;
       }
       if(okpid) {
	 m_ndof++;
	 for(int i = 0; i < 5; i++) {
	   pdf[i] *= pdfTofCorr(i);
	 }
       }
     }
   }


   //
   //  Barrel TOF Q
   //

   if(IsTofQInfoValid()) {
      bool okpid = false;
      for(int i = 0; i < 5; i++) {
         if(pidcase[i])
            if(!okpid) okpid = true;
      }
      if(okpid) {
         //    m_ndof++;
         for(int i = 0; i < 5; i++) pdf[i] *= pdfTofQ(i);
      }
   } // TofQ

   //
   //  Emc PID
   //
   if(IsEmcInfoUsed()) {
      if(IsEmcInfoValid()) {
         bool okpid = false;
         for(int i = 0; i < 5; i++) {
            if(pidcase[i]&&pdfEmc(i)>0)
               if(!okpid) okpid = true;
         }
         if(okpid) {
            m_ndof++;
            for(int i = 0; i < 5; i++) {
               pdf[i] *= pdfEmc(i);
            }
         } // Emc Pid
      }
   }
   if(IsMucInfoUsed()) {
      if(IsMucInfoValid()) {
         bool okpid = false;
         for(int i = 0; i < 5; i++) {
            if(pidcase[i]&&pdfMuc(i)>0)
               if(!okpid) okpid = true;
         }
         if(okpid) {
            m_ndof++;
            for(int i = 0; i < 5; i++) {
               pdf[i] *= pdfMuc(i);
            }
         }
      } // Emc Pid
   }



   if(m_ndof <= 0) return irc;
   for(int i = 0; i < 5; i++) {
      m_pdf[i] = pdf[i];
      m_TotalLikelihood += pdf[i];
   }
   for(int i = 0; i < 5; i++) {
      m_likelihoodfraction[i] = pdf[i]/m_TotalLikelihood;
   }


   irc = 0;
   return irc;
}

