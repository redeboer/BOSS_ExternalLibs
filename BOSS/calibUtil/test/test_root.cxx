#include <string.h>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"
#include "TFolder.h"
TList *FileList;
TFile *Target;
void MergeRootfile( TDirectory *target, TList *sourcelist );

int main(int argc, char* argv[]) {
//  std::cout <<  << "  " << file1 << std::endl;
//  char* file = "result.root";
  std::cout<<"argv[0]:"<<argv[0]<<std::endl;
   std::cout<<"argv[1]:"<<argv[1]<<std::endl;
   std::cout<<"argv[2]:"<<argv[2]<<std::endl;
   std::cout<<"argv[3]:"<<argv[3]<<std::endl;
  Target = TFile::Open(argv[1], "RECREATE" );
  FileList = new TList();
//  char* file1="/home/bes/huangb/root_test/hist.root";
  for(int i=0;i<10000;i++){
    std::cout<<"argv[i+2]=="<<argv[i+2]<<std::endl;
    if(string(argv[i+2])=="end") break;
    FileList->Add( TFile::Open(argv[i+2]) );
  }
//  FileList->Add( TFile::Open("/home/bes/huangb/root_test/hist1.root") );
  MergeRootfile( Target, FileList );

} /* end of main */
void MergeRootfile( TDirectory *target, TList *sourcelist ) {

  //  cout << "Target path: " << target->GetPath() << endl;
  TString path( (char*)strstr( target->GetPath(), ":" ) );
  path.Remove( 0, 2 );

  TFile *first_source = (TFile*)sourcelist->First();
  first_source->cd( path );
  TDirectory *current_sourcedir = gDirectory;
  //gain time, do not add the objects in the list in memory
  Bool_t status = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  // loop over all keys in this directory
  TChain *globChain = 0;
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key, *oldkey=0;
  while ( (key = (TKey*)nextkey())) {

    //keep only the highest cycle number for each key
    if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;
   //  std::cout<<"key->GetName()=="<<key->GetName()<<std::endl;
    // read object from first source file
    first_source->cd( path );
    TObject *obj = key->ReadObj();

      TFolder* add = dynamic_cast<TFolder*>(obj);
      TCollection *hh = add->GetListOfFolders();
      TFolder* add1=new TFolder(key->GetName(), key->GetTitle());
//     TIter next(hh);

      TFile *nextsource = (TFile*)sourcelist->After( first_source );
      while ( nextsource ) {
        TIter next(hh);
        nextsource->cd( path );
        std::cout<<"nextsource->Name()=="<<std::endl;
        nextsource->Print();
        std::cout<<"firstsource->Name()=="<<std::endl;
        first_source->Print();
        TFolder* kk=(TFolder *)nextsource->Get(key->GetName());
        std::cout<<"key->GetName()=="<<key->GetName()<<std::endl;
         first_source->cd( path );
         while ((obj = next())){
         TH1 *h1 = (TH1*)obj;
         std::cout<<"obj->getName()=="<<obj->GetName()<<std::endl;
         add1->Add(h1);
        nextsource->cd( path );
        TH1 *h2 = (TH1*)kk->FindObjectAny(h1->GetName());
    if(h2){
        h1->Add( h2 );
        std::cout<<"h1->GetEntries()"<<h1->GetEntries()<<std::endl;
        }
        }
        nextsource = (TFile*)sourcelist->After( nextsource );
      }
    target->cd();
   // std::cout<<"add TFolder:"<<add1->GetName()<<std::endl;
   add1->Write();
    delete add1;
    delete hh;
}
  target->SaveSelf(kTRUE);
}

