#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <algorithm>

#include "EventTag/EventTagSvc.h"

#ifndef BEAN
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#endif

using namespace std;

#ifndef BEAN
bool mcPartPrtLess( Event::McParticle* p1, Event::McParticle* p2){
return p1->particleProperty()<p2->particleProperty();}
 
EventTagSvc::EventTagSvc(const std::string& name, ISvcLocator* svc ) :
  Service(name,svc)
{

  declareProperty("pdgFile", m_pdtFile="pdt.table");
  declareProperty("decayCodesFile", m_decayTabsFile="decay.codes");
  //  declareProperty("userDecayCodesFile", 
  //                            m_userDecayTabsFile="userdecay.codes");
  declareProperty("userDecayCodesFile", m_userDecayTabsFile="");
  declareProperty("buildAntiPartTable", m_buildAntiTabs=true);
  declareProperty("ignorePhotons", m_ignorePhotons=true);
  declareProperty("ignoreFSR", m_ignoreFSR=true);
  declareProperty("chainTrigParticles", userChainTrig);
  declareProperty("digK0", m_digK0=true);
}
#else  // BEAN
EventTagSvc* EventTagSvc::m_evtag = 0;

bool mcPartPrtLess (TMcParticle * p1, TMcParticle * p2) {
   return p1->getParticleID () < p2->getParticleID ();
}

EventTagSvc::EventTagSvc () {
   m_pdtFile = "Analysis/EventTag/share/pdt.table";
   m_decayTabsFile = "Analysis/EventTag/share/decay.codes";

   m_userDecayTabsFile = "";
   m_buildAntiTabs = true;
   m_ignorePhotons = true;
   m_ignoreFSR = true;
   m_digK0 = true;
   verbose = 0;
}
#endif


StatusCode EventTagSvc::initialize(){
#ifndef BEAN
  Service::initialize();
  MsgStream log(msgSvc(), name());

  setProperties();
  log << MSG::INFO << "EventTagSvc initialize()" << endreq;
  log << MSG::INFO << "         pdgFile="<<m_pdtFile<<endreq;
  log << MSG::INFO << "         decayCodesFile="<<m_decayTabsFile<<endreq;
  log << MSG::INFO << "         userDecayTabsFile="<<m_userDecayTabsFile
      <<endreq;
  log << MSG::INFO << "         buildAntiPartTable="<<m_buildAntiTabs<<endreq;
  log << MSG::INFO << "         Ignore Photons="<<m_ignorePhotons<<endreq;
  log << MSG::INFO << " Chain trigger particles:"<<endreq; 
  for(std::vector<string>::iterator it=userChainTrig.begin();
   it!=userChainTrig.end();it++){
    log << MSG::INFO<<"                         " <<(*it)<<endreq;
  }
#else
   if( verbose ) {
      cout << "EventTagSvc initialize()" << endl;
      cout << "         pdgFile=" << m_pdtFile << endl;
      cout << "         decayCodesFile=" << m_decayTabsFile << endl;
      cout << "         userDecayTabsFile=" << m_userDecayTabsFile << endl;
      cout << "         buildAntiPartTable=" << m_buildAntiTabs << endl;
      cout << "         Ignore Photons=" << m_ignorePhotons << endl;
      cout << " Chain trigger particles:" << endl;

      for (std::vector < string >::iterator it = userChainTrig.begin ();
        it != userChainTrig.end (); it++) {
         cout << "                         " << (*it) << endl;
      }
   }
#endif

  //  static const bool CREATEIFNOTTHERE(true);
  if(readPdtFile(m_pdtFile.c_str())){
#ifndef BEAN
    log << MSG::ERROR<<"Can not read pdgFile "<< m_pdtFile<<endreq;
    return StatusCode::StatusCode::FAILURE;
#else
    cerr << "Can not read pdgFile " << m_pdtFile << endl;
    return false;
#endif
  }

  if(readDecayTabFile(m_decayTabsFile)){
#ifndef BEAN
    log << MSG::ERROR<<"Can not read decayTabsFile "<< m_decayTabsFile<<endreq;
    return StatusCode::StatusCode::FAILURE;
#else
    cerr << "Can not read decayTabsFile " << m_decayTabsFile << endl;
    return false;
#endif
  }

  if(m_userDecayTabsFile.size()){
    if(readDecayTabFile(m_userDecayTabsFile)){
#ifndef BEAN
      log << MSG::ERROR<<"Can not read userDecayTabsFile "<<m_userDecayTabsFile
          <<endreq;
      return StatusCode::StatusCode::FAILURE;
#else
      cerr << "Can not read userDecayTabsFile " << m_userDecayTabsFile << endl;
      return false;
#endif
    }
  }

  if(m_buildAntiTabs){
    if(buildAntiPartTabs()){
#ifndef BEAN
      log << MSG::ERROR<<"fail to build decay code table for anti-particles"
          <<endreq;
      return StatusCode::StatusCode::FAILURE;
#else
      cerr << "fail to build decay code table for anti-particles" << endl;
      return false;
#endif
    }
  }
  
  // fill the set with trigger chain particles
  for(std::vector<string>::iterator it=userChainTrig.begin();
   it!=userChainTrig.end();it++){
    long int pdg=labs(name2pdg(*it));
    if(pdg==0){
#ifndef BEAN
      log << MSG::ERROR<<"Unknown ChainTrigger particle "<< (*it)
          <<" ..ignoring"<<endreq;
#else
      cerr << "Unknown ChainTrigger particle " << (*it) 
           <<  " ..ignoring" << endl;
#endif
      continue;
    }
    chainTrigParticles.insert(pdg);
  }

#ifndef BEAN
  log << MSG::DEBUG << "sorted chain trigger particles:"<<endreq; 
  for(set<long int>::iterator it=chainTrigParticles.begin();
   it!=chainTrigParticles.end();it++){
    log << MSG::DEBUG<<"                         "<<(*it)<<"  " 
        <<pdg2name(*it)<<endreq;
  }
#else
   if (verbose) {
      cout << "sorted chain trigger particles:" << endl;
      for (set < long int >::iterator it = chainTrigParticles.begin ();
        it != chainTrigParticles.end (); it++) {
         cout << "                         " << (*it) << "  " 
              << pdg2name(*it) << endl;
      }
   }
#endif


#ifndef BEAN
  return StatusCode::SUCCESS;
#else
  return true;
#endif
}
  
#ifndef BEAN
StatusCode EventTagSvc::finalize() {
  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "EventTagSvc finalize()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode 
EventTagSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( IEventTagSvc::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IEventTagSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
#endif



string EventTagSvc::pdg2name(long int pdg){
  map<long int,string>::iterator it=pdg2nameTab.find(pdg);
  if(it!=pdg2nameTab.end())return it->second;
  return string("");
}

long int EventTagSvc::name2pdg(string pname){
  map<string,long int>::iterator it=name2pdgTab.find(pname);
  if(it!=name2pdgTab.end())return it->second;
  return 0;
}


int EventTagSvc::readPdtFile(string fname){
#ifndef BEAN
  MsgStream log(msgSvc(), name());
#endif
  ifstream infile;
  infile.open(fname.c_str());
  if(!infile.is_open()){
#ifndef BEAN
    log<<MSG::ERROR<< "Can not open pdg-code file :" <<fname<<endreq;
#else
    cerr << "Can not open pdg-code file :" << fname << endl;
#endif
    return 1;
  }
#ifndef BEAN
  log<<MSG::INFO<<"reading pdg codes table from "<<fname<<endreq; 
#else
  if (verbose)
      cout << "reading pdg codes table from " << fname << endl;
#endif
  string ln;
  int nstr=0;
  long int pcode;
  string cmd,dummy,pname,pcode_str;
  stringstream* lns;
  while (! infile.eof() ){
    getline (infile,ln);
    nstr++;
#ifndef BEAN
    log<<MSG::DEBUG<<"parsing line:"<<ln<<endreq;
#else
    if (verbose) 
      cout << "parsing line:" << ln << endl;
#endif
    if(ln.empty())continue;
    if(ln[0]=='*')continue;
    lns=new stringstream;
    (*lns)<<ln;
    cmd="";
    (*lns)>>cmd;
    if(!strcmp(cmd.c_str(),"add")){
      pcode=0;
      pname="";
      pcode_str="";
      (*lns)>>dummy>>dummy>>pname>>pcode_str;
      pcode=atol(pcode_str.c_str());
      if(pcode==0||pcode==LONG_MAX||pcode==LONG_MIN){
#ifndef BEAN
        log<<MSG::ERROR<<"Unrecognized particle code in line "
           <<nstr<<"  of file" <<fname<<endreq;
#else
        cerr << "Unrecognized particle code in line "
             << nstr << "  of file" << fname << endl;
#endif
        infile.close();
        return 1;
      }
#ifndef BEAN
      log<<MSG::DEBUG<<" find particle: " <<pname<<" pgd:"<<pcode <<endreq;
#else
      if (verbose)
        cout << " find particle: " << pname << " pgd:" << pcode << endl;
#endif
      if(pdg2nameTab.find(pcode)!=pdg2nameTab.end()){
#ifndef BEAN
        log<<MSG::ERROR<< "pdg code "<<pcode
           <<" is repeated in line "<<nstr<<". Aborting"<<endreq;
#else
        cerr << "pdg code " << pcode
             << " is repeated in line " << nstr << ". Aborting" << endl;
#endif
        infile.close();
        return 1;
      }
      if(name2pdgTab.find(pname)!=name2pdgTab.end()){
#ifndef BEAN
        log<<MSG::ERROR<< "particle name "<<pcode
           <<" is repeated in line "<<nstr<<". Aborting"<<endreq;
#else
        cerr << "particle name " << pcode
             << " is repeated in line " << nstr << ". Aborting" << endl;
#endif
        infile.close();
        return 1;
      }
      pdg2nameTab[pcode]=pname;
      name2pdgTab[pname]=pcode;
    }
    delete lns;
  }
  infile.close();
  return 0;
}


int EventTagSvc::readDecayTabFile(string fname){
#ifndef BEAN
  MsgStream log(msgSvc(), name());
#endif
  ifstream infile;
  infile.open(fname.c_str());
  if(!infile.is_open()){
#ifndef BEAN
    log<<MSG::ERROR<< "Can not open decay table file :" <<fname<<endreq;
#else
    cerr << "Can not open decay table file :" << fname << endl;
#endif
    infile.close();
    return 1;
  }
#ifndef BEAN
  log<<MSG::INFO<<"Reading decay codes from file " << fname<<endreq;  
#else
  if (verbose)
    cout << "Reading decay codes from file " << fname << endl;
#endif
  string ln;
  long int pdg,mo_pdg;
  int dcode;
  int nstr=0;
  char* pch;
  char str[256],mother[256];
  char whitespaces[]=" \t\f\v\n\r";
  while (! infile.eof() ){
    getline (infile,ln);
    nstr++;
    if(ln.empty())continue;
    if(ln[0]=='#'||ln[0]=='*')continue;
    if(ln.size()>255){
#ifndef BEAN
      log<<MSG::ERROR<<"line "<< nstr <<"in " 
         <<fname<<" is too big" <<endreq;
#else
      cerr << "line " << nstr << "in " << fname << " is too big" << endl;
#endif
      infile.close();
      return 1;
    }
    strcpy(str,ln.c_str());
    pch = strtok (str,whitespaces);
    if(pch==NULL)continue;
    if(strcmp(pch,"Decay")!=0){
#ifndef BEAN
      log<<MSG::ERROR<<"No open  decay, unrecognized token in line "
         <<nstr <<" of " <<fname<<endreq;
#else
      cerr << "No open  decay, unrecognized token in line "
           << nstr << " of " << fname << endl;
#endif
      infile.close();
      return 1;
    }
    pch = strtok (NULL, whitespaces);
    if(pch==NULL) {
#ifndef BEAN
      log<<MSG::ERROR<< "Can not find  particles in line "
         << nstr <<" of " <<fname<<endreq;
#else
      cerr << "Can not find  particles in line "
           << nstr << " of " << fname << endl;
#endif
      infile.close();
      return 1;
    }
    mo_pdg=name2pdg(string(pch));
    strcpy(mother,pch);
    if(mo_pdg==0){
#ifndef BEAN
      log<<MSG::ERROR<< "Unrecognized  particle "<<pch<<" in line " << nstr
         <<" of " <<fname<<endreq;
#else
      cerr << "Unrecognized  particle " << pch << " in line " << nstr
           << " of " << fname << endl;
#endif
      infile.close();
      return 1;
    }
#ifndef BEAN
    log<<MSG::INFO<<"START DECAY TABLE FOR " << mother<<", pdg_code:"
       <<mo_pdg<<endl; 
#else
    cout << "START DECAY TABLE FOR " << mother << ", pdg_code:" 
         << mo_pdg << endl;
#endif
    decayModeTab* dTab;
    map<int,decayModeTab*>::iterator itlist=m_decayTabList.find(mo_pdg);
    if(itlist!=m_decayTabList.end()){
#ifndef BEAN
      log<<MSG::WARNING<< "add new info into existing decay tab for "
         <<mother<<endreq;
#else
      if (verbose)
        cout << "add new info into existing decay tab for " << mother << endl;
#endif
      dTab=itlist->second;
    }
    else{
#ifndef BEAN
      log<<MSG::INFO<< "Create new decay code table for "<<mother<<endreq;
#else
      if (verbose)
        cout << "Create new decay code table for " << mother << endl;
#endif
      dTab=new decayModeTab;
      m_decayTabList[mo_pdg]=dTab;
    }

    while(true){
      getline (infile,ln);
      if( infile.eof() ){
#ifndef BEAN
        log<<MSG::ERROR << "Enexpected EOF for dacay code file "
           <<fname <<endreq;
#else
        cerr << "Enexpected EOF for dacay code file " << fname << endl;
#endif
        infile.close();
        return 1;
      }
      nstr++;
      if(ln.empty())continue;
      if(ln[0]=='#')continue;
      if(ln.size()>255){
#ifndef BEAN
        log<<MSG::ERROR<<"line "<< nstr <<"in decay table file " 
           <<fname<<" is too big" <<endreq;
#else
        cerr << "line " << nstr << "in decay table file "
             << fname << " is too big" << endl;
#endif
        infile.close();
        return 1;
      }
      strcpy(str,ln.c_str());
      pch = strtok (str,whitespaces);
      if(pch==NULL)continue;
      if(strcmp(pch,"Enddecay")==0){
#ifndef BEAN
        log<<MSG::INFO <<" End of "<< mother <<" decay table" <<endreq;
#else
        if (verbose)
          cout << " End of " << mother << " decay table" << endl;
#endif
        break;
      }
#ifndef BEAN
      log<<MSG::DEBUG<<"parsing line "<<ln<<endreq;
#else
      if (verbose)
        cout << "parsing line " << ln << endl;
#endif
      dcode=atoi(pch);
      if(dcode<=0||dcode>255){
#ifndef BEAN
        log<<MSG::ERROR<< " wrong decay code  in line "<<nstr
           <<", file "<<fname<< endreq; 
#else
        cerr << " wrong decay code  in line " << nstr
             << ", file " << fname << endl;
#endif
        infile.close();
        return 1;
      }
      //      decayModeTab::iterator itTab=dTab->find(code);
      //if( itTab != dTab->end()){
        
#ifndef BEAN
      log<<MSG::DEBUG<<"   dcode:"<<dcode<<",  particles: ";
#else
      if (verbose)
        cout << "   dcode:" << dcode << ",  particles: ";
#endif
      pch = strtok (NULL, whitespaces);
      if(pch==NULL){
        //        if(
#ifndef BEAN
        log<<MSG::ERROR<< " no decay found in line "<<nstr
           <<", file "<<fname<< endreq;
#else
        cerr << " no decay found in line " << nstr
             << ", file " << fname << endl;
#endif
        infile.close();
        return 1;
      }
      //      keyVector* pKey=new keyVector;
      keyVector key;
      key.clear();
      //      keyVector* pKey=&key;
      while (pch != NULL){
#ifndef BEAN
        log<<MSG::DEBUG <<pch<<" ";        
#else
        if (verbose)
          cout << pch << " ";
#endif
        pdg=name2pdg(string(pch));
        if(pdg==0){
#ifndef BEAN
           log<<MSG::ERROR<<endl<< "Unrecognized  particle " <<pch 
              <<" in line " << nstr <<" of " <<fname<<endreq;
#else
            cerr << endl << "Unrecognized  particle " << pch 
                 << " in line " << nstr << " of " << fname << endl;
#endif
          infile.close();
           return 1;
         }
        //        pKey->push_back(pdg);
        key.push_back(pdg);
        pch = strtok (NULL, whitespaces);
      }
 
      //      sort(pKey->begin(),pKey->end());
      sort(key.begin(),key.end());
#ifndef BEAN
      log<<MSG::DEBUG<<"    sorted pdg" ;
#else
      if (verbose) {
        cout << "    sorted pdg";
#endif
      //      for(keyVector::iterator it=pKey->begin(); it!=pKey->end(); it++)
      for(keyVector::iterator it=key.begin(); it!=key.end(); it++)
#ifndef BEAN
        log<<MSG::DEBUG <<"  "<<(*it);
      log<<MSG::DEBUG<<endreq;
#else
        cout << "  " << (*it);
      cout << endl;
      } // end of if (verbose)
#endif
      //      decayModeTab::iterator itdec=dTab->find(*pKey);
      decayModeTab::iterator itdec=dTab->find(key);
      if(itdec!=dTab->end()){
        if(itdec->second !=dcode){
#ifndef BEAN
          log<<MSG::ERROR << "line "<<nstr<<" of file " <<fname
             <<"has an error : "<<endreq ;
          log<<MSG::ERROR<<"decay of "<<mother<< " to";
#else
          cerr << "line " << nstr << " of file " << fname 
               << "has an error : " << endl;
          cerr << "decay of " << mother << " to";
#endif
          //          for(keyVector::iterator it=pKey->begin(); it!=pKey->end(); it++)
          for(keyVector::iterator it=key.begin(); it!=key.end(); it++)
#ifndef BEAN
            log<<MSG::ERROR<<" "<<pdg2name(*it);
          log<<MSG::ERROR<<" is already in table. Aborting."<<endreq;
          log<<MSG::ERROR<<"check line "<<nstr<<" of " <<fname<<endreq;
#else
            cerr << " " << pdg2name (*it);
          cerr << " is already in table. Aborting." << endl;
          cerr << "check line " << nstr << " of " << fname << endl;
#endif
          infile.close();
          return 1;
        }
      }
      //      (*dTab)[*pKey]=dcode;
      (*dTab)[key]=dcode;
    }
  } // end if file read loop
  infile.close();
  return 0;
}

#ifndef BEAN
int EventTagSvc::getDecayCode(Event::McParticle* part){
  MsgStream log(msgSvc(), name());
#else
int EventTagSvc::getDecayCode (TMcParticle * part){
   if (!m_TMcEvent) {
      cerr << "(ERROR) EventTag: you must set TMcEvent first!" << endl;
      return -1;
   }
#endif
  keyVector key;

#ifndef BEAN
  long int mother_pdg=part->particleProperty();
#else
  long int mother_pdg = part->getParticleID ();
#endif

  map<int,decayModeTab*>::iterator itlist=m_decayTabList.find(mother_pdg);
  if(itlist==m_decayTabList.end())return 0;
  decayModeTab* dTab=itlist->second;

#ifndef BEAN
  SmartRefVector<Event::McParticle> dref=part->daughterList();
#else
  vector < int >dref = part->getDaughters ();
#endif
  if(dref.size()==0)return 0;
  key.clear();

#ifndef BEAN
  for(SmartRefVector<Event::McParticle>::iterator it=dref.begin();
   it!=dref.end();it++){
    int pdg=(*it)->particleProperty();
#else
   for (unsigned int i = 0; i < dref.size (); i++) {
      const TMcParticle *daughter = m_TMcEvent->getMcParticle (dref[i]);

      int pdg = daughter->getParticleID ();
#endif
    if(m_digK0&&(abs(pdg)==311)){
#ifndef BEAN
      SmartRefVector<Event::McParticle> dref1=(*it)->daughterList();
#else
      vector < int >dref1 = daughter->getDaughters ();
#endif
      if(dref1.size()!=1){
#ifndef BEAN
        log<< MSG::WARNING<<"unknown decay of K0, aborting"<<endreq;
#else
        cout << "unknown decay of K0, aborting" << endl;
#endif
        return 0;
      }
#ifndef BEAN
      pdg=dref1[0]->particleProperty();
#else
      pdg = m_TMcEvent->getMcParticle (dref1[0])->getParticleID ();
#endif
    }
    if((abs(pdg)==22)&&m_ignorePhotons)continue;
    if((pdg==-22)&&m_ignoreFSR)continue;
    key.push_back(pdg);
  }

  sort(key.begin(),key.end());
  if(m_ignorePhotons&&(key.size()==0))key.push_back(22);
  decayModeTab::iterator ittab=dTab->find(key);
  if(ittab==dTab->end())return 0;
  return ittab->second;
}

int EventTagSvc::buildAntiPartTabs(){
#ifndef BEAN
  MsgStream log(msgSvc(), name());
#endif
  decayModeTab* dTab;
  decayModeTab* adTab;
  keyVector key,akey;
  int mother,amother,code;
  for(std::map<int,decayModeTab*>::iterator itl=m_decayTabList.begin(); 
      itl!=m_decayTabList.end();itl++){
    mother=itl->first;
    amother=-mother;
    if(m_decayTabList.find(amother)!=m_decayTabList.end())
      continue;// aparticle tab already exist
    if(pdg2name(mother).size()==0){
#ifndef BEAN
      log<<MSG::ERROR
      <<" buildAntiPartTabs :: Unknown pdg code for mother particle "<<endreq;
#else
      cerr << " buildAntiPartTabs :: Unknown pdg code for mother particle " 
           << endl;
#endif
      return 1;
    }
    if(pdg2name(amother).size()==0)continue; //Aparticle==particle??
#ifndef BEAN
    log<<MSG::INFO<<" buildAntiPartTabs :: create new table for particle "
       << amother<<"("<<pdg2name(amother)<<")"<<endreq;
#else
    if (verbose)
      cout << " buildAntiPartTabs :: create new table for particle "
           << amother << "(" << pdg2name (amother) << ")" << endl;
#endif
    dTab=itl->second;
    adTab=new decayModeTab;
    m_decayTabList[amother]=adTab;
    key.clear();
    for(decayModeTab::iterator it=dTab->begin();it!=dTab->end();it++){
      key=it->first;
      code=it->second;
      for(keyVector::iterator it=key.begin();it!=key.end();it++){
        if(pdg2name(*it).size()==0){
#ifndef BEAN
          log<<MSG::ERROR<<" buildAntiPartTabs :: Unknown pdg code"
             << "for  particle in keyVector"<<endreq;
#else
          cerr << " buildAntiPartTabs :: Unknown pdg code"
               << "for  particle in keyVector" << endl;
#endif
          return 1;
        }
        if(pdg2name(-(*it)).size()==0)continue ; // true neutral ??
        (*it) *= -1;
      }
      sort(key.begin(),key.end());
      (*adTab)[key]=code;
    }
  }
  return 0;
}

#ifndef BEAN
unsigned  int EventTagSvc::getCharmDecayType(Event::McParticle* part){
#else
unsigned int EventTagSvc::getCharmDecayType (TMcParticle * part){
   if (!m_TMcEvent) {
      cerr << "(ERROR) EventTag: you must set TMcEvent first!" << endl;
      return -1;
   }
#endif
  unsigned int a=0;
  //  int type=0;
  if(! part) return 0xF;
#ifndef BEAN
  SmartRefVector<Event::McParticle> dref=part->daughterList();
#else
  vector < int >dref = part->getDaughters ();
#endif
  if(dref.size()==0) return 0;
  int nhad=0;
  long int pdg1=0;
  bool dd=false;
#ifndef BEAN
  for(SmartRefVector<Event::McParticle>::iterator it=dref.begin(); 
   it!=dref.end();it++){
    int pdg=abs((*it)->particleProperty());
#else
  for (unsigned int i = 0; i < dref.size (); i++) {
    const TMcParticle *daughter = m_TMcEvent->getMcParticle (dref[i]);

    int pdg = abs (daughter->getParticleID ());
#endif
    if((pdg==22)&&m_ignorePhotons)continue;
    if(pdg==22){a|=1;}
    else if( (pdg>10)&&(pdg<19) ){a |=(1<<1);}
    else if( pdg>110) {
      if( ((pdg%1000)/100==4)&&((pdg%100)/10==4)){
        a |= 1<<3;
      }
      else{
        a |= 1<<2;
        nhad+=1;
        if(nhad==1)pdg1=pdg;
        dd=(pdg1==pdg);
      }
    }
    else a |=1<<4;
  }
  if(a>0xF)return 0xf;        //unknown particle
  if(a==4){                   //  hadronic
    if((nhad!=2)||(!dd))return 6;
    //    long int pdg1=dref[0]->particleProperty();
    //    long int pdg2=dref[1]->particleProperty();
    //    if(abs(pdg1)!=abs(pdg2)) return 6;
    switch (abs(pdg1)){
    case 411:
      return 7;
      break;
    case 421:
      return 8;
      break;
    case 413:
      return 9;
      break;
    case 423:
      return 10;
      break;
    case 431:
      return 11;
      break;
    case 433:
      return 12;
      break;
    default:
      return 6;
      break;
    }
  }
  if((a&0xE)==2)return 1;     // leptonic 
  if((a&0x6)==0x6)return 2;   // semileptonic 
  if((a&0xE)==0xC)return 4;   //  hadronic charmonium transition
  if((a&0xE)==0x8)return 3;   //  radiative charmonium transition   !!use 0XE mask for correct work with "ignore photons" options; 
  if((a&1)==1)return 5;       //  radiative
  return 0xf;                 //unknown
}
 

#ifndef BEAN
unsigned long long int EventTagSvc::getChainCode(Event::McParticle* part){
#else
unsigned long long int EventTagSvc::getChainCode (TMcParticle * part){
   if (!m_TMcEvent) {
      cerr << "(ERROR) EventTag: you must set TMcEvent first!" << endl;
      return -1;
   }
#endif
  unsigned long long int code=0;
  int shift=0;
#ifndef BEAN
  vector<Event::McParticle*> chainVect;
#else
  vector < TMcParticle * >chainVect;
#endif
  while(shift<57){
    code |=  (getDecayCode(part)&0xFF)<<shift;  
#ifndef BEAN
    SmartRefVector<Event::McParticle> dref=part->daughterList();
#else
    vector < int >dref = part->getDaughters ();
#endif
    chainVect.clear();
#ifndef BEAN
    for(SmartRefVector<Event::McParticle>::iterator it=dref.begin();
     it!=dref.end();it++){
      int abspdg=abs((*it)->particleProperty());
#else
    for (unsigned int i = 0; i < dref.size (); i++) {
      const TMcParticle *daughter = m_TMcEvent->getMcParticle (dref[i]);

      int abspdg = abs (daughter->getParticleID ());
#endif
      if(((((abspdg%1000)/10)<45) && (((abspdg%1000)/10)>40)) // select particles with *4?? pdg code
         ||(chainTrigParticles.find(abspdg)!=chainTrigParticles.end())) { 
#ifndef BEAN
        chainVect.push_back(*it);
#else
        chainVect.push_back (const_cast < TMcParticle * >(daughter));
#endif
      }
    }
    if(chainVect.size()==0 ||chainVect.size()>2)break;
    if(chainVect.size()==1){part=chainVect[0];}
    else{
      sort(chainVect.begin(),chainVect.end(),mcPartPrtLess);
      code |= ((getDecayCode(chainVect[0])&0xFF)<<(shift+8));
      code |= ((getDecayCode(chainVect[1])&0xFF)<<(shift+16));
      break;
    }
    shift+=8;
  }
  return code;
}

#ifdef BEAN
unsigned int EventTagSvc::getEventTag (unsigned int initialEventTag){
   if (!m_TMcEvent) {
      cerr << "(ERROR) EventTag: you must set TMcEvent first!" << endl;
      return -1;
   }

   // Get McParticle Collections
   unsigned int m_EventTag = initialEventTag;


   const TObjArray *mcParticles = m_TMcEvent->getMcParticleCol ();



   if (!mcParticles) {
      cerr << "Can not open McParticleCollection" << endl;
      return true;
   }

   if (m_EventTag == 0) {	//try autodetect general event type: check for ccbar resonance;
      TIterator *mcParticlesIter = mcParticles->MakeIterator ();
      while (TMcParticle * particle =
	     (TMcParticle *) (mcParticlesIter->Next ())) {

	 //      if((*it)->itdecayInFlight())!!((*it)->primaryParticle())continue;
	 long int pdg = abs (particle->getParticleID ());
	 if (((pdg % 1000) / 10) == 44) {
	    m_EventTag =
	       ((int) (pdg == 443)) * 4 +
	       ((int) (pdg == 100443)) * 5 +
	       ((int) (pdg == 30443)) * 6 +
	       ((int) (pdg == 9000443)) * 7 +
	       ((int) (pdg == 9010443)) * 8 + ((int) (pdg == 9020443)) * 9;
	    break;
	 }
      }
   }

   if (m_EventTag == 0) {	//try autodetect general event type;
      m_EventTag = 2;		//off-resonance data
      TIterator *mcParticlesIter2 = mcParticles->MakeIterator ();

      while (TMcParticle * particle =
	     (TMcParticle *) (mcParticlesIter2->Next ())) {
	 if (particle->decayInFlight () || particle->primaryParticle ())
	    continue;
	 long int pdg = abs (particle->getParticleID ());
	 if (pdg == 11) {
	    m_EventTag |= 0X20;
	    break;
	 }
	 else if (pdg == 13) {
	    m_EventTag |= 0X30;
	    break;
	 }
	 else if (pdg == 15) {
	    m_EventTag |= 0X40;
	    break;
	 }
	 else if ((pdg > 0) && (pdg < 9)) {
	    m_EventTag |= 0X50;
	    break;
	 }
      }
   }


   if (((m_EventTag & 0xf) > 3) && ((m_EventTag & 0xf) < 9)) {	/// charmonium resonance
      m_EventTag &= 0xf;
      // find initial charmonium resonance;
      TIterator *mcParticlesIter3 = mcParticles->MakeIterator ();
      while (TMcParticle * particle =
	     (TMcParticle *) (mcParticlesIter3->Next ())) {
	 long int pdg = particle->getParticleID ();
	 if (((pdg % 1000) / 10) != 44)
	    continue;
	 //      part=(*it);
	 m_EventTag |= (getCharmDecayType (particle)) << 4;
	 m_EventTag |= (getChainCode (particle)) << 8;
	 //      printf("Chain code %20llX \n",(m_EventTagSvc->getChainCode(*it)));
	 break;
      }
   }
   else if ((m_EventTag & 0xf) == 0x2) {	//off-resonance
      //save topology info:
      int ncha = 0;
      int nneu = 0;
      TIterator *mcParticlesIter4 = mcParticles->MakeIterator ();
      while (TMcParticle * particle =
	     (TMcParticle *) (mcParticlesIter4->Next ())) {
	 long int pdg = abs (particle->getParticleID ());
	 bool good = (!particle->decayInFlight ())
	    && (!particle->primaryParticle ());
	 ncha += ((int)
		  (good && ((pdg == 11) || (pdg == 13) || (pdg == 211)
			    || (pdg == 321)
			    || (pdg == 2212) || (pdg == 1011))));
	 nneu += ((int)
		  (good && ((pdg == 111) || (pdg == 310) || (pdg == 130)
			    || (pdg == 2112))));
//       printf("%2i %6i  %2i %2i  Nneu:%3i    Ncha:%3i \n",
//           (*it)->trackIndex(),pdg,
//           ((pdg==11)||(pdg==13)||(pdg==211)||(pdg==321)||(pdg==2212)||(pdg==1011)),
//           ((pdg==111)||(pdg==310)||(pdg==130)||(pdg==2112)),
//           nneu,ncha);  
      }
      m_EventTag |= ((ncha & 0xF) << 8);
      m_EventTag |= ((nneu & 0xF) << 12);
      if ((m_EventTag & 0xf0) == 0x40) {	//tau-tau event
	 //cout<< "XXXXXXXXX" <<endl;
	 TIterator *mcParticlesIter5 = mcParticles->MakeIterator ();
	 while (TMcParticle * particle =
		(TMcParticle *) (mcParticlesIter5->Next ())) {
	    long int pdg = particle->getParticleID ();
	    //      cout<<"    YYY: "<<pdg<<endl;
	    if (pdg == -15) {
	       m_EventTag |= (getDecayCode (particle)) << 16;
	       // cout <<"DEcay cod for a-tau:" <<(m_EventTagSvc->getDecayCode(*it))<<endl;
	    }
	    else if (pdg == 15) {
	       m_EventTag |= (getDecayCode (particle)) << 24;
	       //cout <<"DEcay cod for tau:" <<(m_EventTagSvc->getDecayCode(*it))<<endl;
	    }
	 }

      }
   }


   return m_EventTag;
}
#endif
