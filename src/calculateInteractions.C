
#include "ParsingTgtExit.h"
#include "dk2nu/tree/dkmeta.h"
#include "dk2nu/tree/dk2nu.h"

#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TChain.h"

using namespace NeutrinoTgtAna;

const int Nhel     = 4;
const std::string hel[Nhel] = {"numu","anumu","nue","anue"};
const int Nebin     = 40;
const double emin   = 0;
const double emax   = 40;
const int Nptbin    = 40;
const double ptmin  = 0;
const double ptmax  = 1;

int idx_hel(int pdg);

void calculateInteractions();
  
void calculateInteractions(const char* inputFile, const char* cdet, const char* outputFile){
  
  std::cout<<"starting()"<<std::endl;
  std::cout<<"inputFile : "<<inputFile <<std::endl;
  std::cout<<"cdet      : "<<cdet      <<std::endl;
  std::cout<<"outputFile: "<<outputFile<<std::endl;

  int idet = atoi(cdet);
  bool is_single_file = (std::string(inputFile).find(".root") < 10000);
  bool is_many_files  = (std::string(inputFile).find(".txt")  < 10000);
  if(!is_single_file && !is_many_files){
    std::cout<<"Ont 1 single root file or a text file with a list of root files are allowed"<<std::endl;
    exit (1);
  }
  
  ParsingTgtExit* texit = ParsingTgtExit::getInstance();
  
  TFile* fOut = new TFile(outputFile,"recreate");
  std::cout<<"File name: "<<fOut->GetName()<<std::endl;
  TH1D* hflux[Nhel];
  TH1D* hinter[Nhel];
  TH1D* hcover_cpion[Nhel];
  TH1D* hcover_ckaon[Nhel];
  TH1D* have_inter[Nhel];
  TH1D* have_cover_cpion[Nhel];
  TH1D* have_cover_ckaon[Nhel];
  //
  TH2D* h2D_cpion[Nhel];
  TH2D* h2D_ckaon[Nhel];
  TH2D* h2D_cover_cpion[Nhel];
  TH2D* h2D_cover_ckaon[Nhel];
  TH2D* h2Dave_cover_cpion[Nhel];
  TH2D* h2Dave_cover_ckaon[Nhel];
  for(int i=0;i<Nhel;i++){
    hflux[i]           = new TH1D(Form("hflux_%s"          , hel[i].c_str()),"",Nebin,emin,emax);
    hinter[i]          = new TH1D(Form("hinter_%s"         , hel[i].c_str()),"",Nebin,emin,emax);
    hcover_cpion[i]    = new TH1D(Form("hcover_cpion_%s"   , hel[i].c_str()),"",Nebin,emin,emax);
    hcover_ckaon[i]    = new TH1D(Form("hcover_ckaon_%s"   , hel[i].c_str()),"",Nebin,emin,emax);
    h2D_cpion[i]       = new TH2D(Form("h2D_cpion_%s"      , hel[i].c_str()),"",Nebin,emin,emax,Nptbin,ptmin,ptmax);
    h2D_ckaon[i]       = new TH2D(Form("h2D_ckaon_%s"      , hel[i].c_str()),"",Nebin,emin,emax,Nptbin,ptmin,ptmax);
    h2D_cover_cpion[i] = new TH2D(Form("h2D_cover_cpion_%s", hel[i].c_str()),"",Nebin,emin,emax,Nptbin,ptmin,ptmax);
    h2D_cover_ckaon[i] = new TH2D(Form("h2D_cover_ckaon_%s", hel[i].c_str()),"",Nebin,emin,emax,Nptbin,ptmin,ptmax);
  }
  
  //Loading ntuples:
  TChain* chain_evts   = new TChain("dk2nuTree");  
  TChain* chain_meta   = new TChain("dkmetaTree");  
  bsim::Dk2Nu*  dk2nu  = new bsim::Dk2Nu;  
  bsim::DkMeta* dkmeta = new bsim::DkMeta;

  if(is_single_file){
    std::cout<<" Adding ntuple at "<<inputFile<<std::endl;
    chain_evts->Add(inputFile);
    chain_meta->Add(inputFile);
  }
  if(is_many_files){
    std::ifstream ifs;
    ifs.open(inputFile);
    std::string line;
    int counter = 0;
    while (ifs.good()) {
       getline(ifs,line);
       if(line.find(".root")>10000)continue;
       chain_evts->Add(line.c_str());
       if(counter==0)chain_meta->Add(line.c_str());
       std::cout<<"Adding ntuple at : "<<line<<std::endl;
       counter++;
    }
    ifs.close();  
  }
  
  chain_evts->SetBranchAddress("dk2nu",&dk2nu);
  chain_meta->SetBranchAddress("dkmeta",&dkmeta);
  
  chain_meta->GetEntry(0); 
  std::cout<<"=> Detector: "<< (dkmeta->location)[idet].name <<std::endl;
  int nentries  = chain_evts->GetEntries();
  std::cout<<"N of entries: "<<nentries<<std::endl;

  for(int ii=0;ii<nentries;ii++){  
    if(ii%30000==0)std::cout<<ii/1000<<" k evts"<<std::endl;
    chain_evts->GetEntry(ii);
    texit->CalculateValues(dk2nu);
    
    double fluxWGT  = ( (dk2nu->nuray)[idet].wgt )*(dk2nu->decay.nimpwt)/3.1416;
    double nuenergy = (dk2nu->nuray)[idet].E; 
    
    int nanc  = dk2nu->ancestor.size();
    int tntot = texit->NTotalInter();
    int tncov = texit->NCoveredInter();
    int tniintgt = texit->NInterInTgt();
    
    int ihel = idx_hel(dk2nu->decay.ntype);  
    if(ihel<0){
      std::cout<<"something wrong, exiting: (ihel) = ("<<ihel<<")"<<std::endl;
      exit (1);
    }

    //Filling
    bool is_cpi = abs(texit->Pdg()) == 211;
    bool is_cka = abs(texit->Pdg()) == 321;
    //1D neutrino energy
    hflux[ihel]->Fill(nuenergy,fluxWGT);
    for(int j=0;j<tntot;j++)hinter[ihel]->Fill(nuenergy,fluxWGT);
    for(int j=0;j<tncov;j++){
      if(is_cpi)hcover_cpion[ihel]->Fill(nuenergy,fluxWGT);
      if(is_cka)hcover_ckaon[ihel]->Fill(nuenergy,fluxWGT);
    }
    //2D Pz vs Pt
    if(texit->Pt()>-998. && tniintgt>0){
      if(is_cpi)h2D_cpion[ihel]->Fill(texit->Pz(),texit->Pt(),fluxWGT);
      if(is_cka)h2D_ckaon[ihel]->Fill(texit->Pz(),texit->Pt(),fluxWGT);
      for(int j=0;j<tniintgt;j++){
	if(is_cpi)h2D_cover_cpion[ihel]->Fill(texit->Pz(),texit->Pt(),fluxWGT);
	if(is_cka)h2D_cover_ckaon[ihel]->Fill(texit->Pz(),texit->Pt(),fluxWGT);
      }
    }
  }
  
  fOut->cd();
  for(int i=0;i<Nhel;i++){
    fOut->mkdir(hel[i].c_str());
    fOut->cd(hel[i].c_str());
    //
    hflux[i]->Write();
    hinter[i]->Write();
    hcover_cpion[i]->Write();
    hcover_ckaon[i]->Write();
    h2D_cpion[i]->Write();
    h2D_ckaon[i]->Write();
    h2D_cover_cpion[i]->Write();
    h2D_cover_ckaon[i]->Write();
    //
    have_inter[i]         = (TH1D*)hinter[i]->Clone(         Form("have_inter_%s"        , hel[i].c_str()));
    have_cover_cpion[i]   = (TH1D*)hcover_cpion[i]->Clone(   Form("have_cover_cpion_%s"  , hel[i].c_str()));
    have_cover_ckaon[i]   = (TH1D*)hcover_ckaon[i]->Clone(   Form("have_cover_ckaon_%s"  , hel[i].c_str()));
    h2Dave_cover_cpion[i] = (TH2D*)h2D_cover_cpion[i]->Clone(Form("h2Dave_cover_cpion_%s", hel[i].c_str()));
    h2Dave_cover_ckaon[i] = (TH2D*)h2D_cover_ckaon[i]->Clone(Form("h2Dave_cover_ckaon_%s", hel[i].c_str()));
    //
    have_inter[i]->Divide(hflux[i]);
    have_cover_cpion[i]->Divide(hflux[i]);
    have_cover_ckaon[i]->Divide(hflux[i]);
    h2Dave_cover_cpion[i]->Divide(h2D_cpion[i]);
    h2Dave_cover_ckaon[i]->Divide(h2D_ckaon[i]);
    //
    have_inter[i]->Write();
    have_cover_cpion[i]->Write();
    have_cover_ckaon[i]->Write();
    h2D_cpion[i]->Write();
    h2D_ckaon[i]->Write();
    h2D_cover_cpion[i]->Write();
    h2D_cover_ckaon[i]->Write();
    h2Dave_cover_cpion[i]->Write();
    h2Dave_cover_ckaon[i]->Write();
  }
  
  std::cout<<"ending()"<<std::endl;
  
}
int idx_hel(int pdg){
  int iout = -1;
  if(pdg == 14) iout = 0;
  if(pdg ==-14) iout = 1;
  if(pdg == 12) iout = 2;
  if(pdg ==-12) iout = 3;
  return iout;
}
void calculateInteractions(){
  std::cout<<"Run code:"<<std::endl;
  std::cout<<"bin/calculateInteractions [input file] [detector index] [output file]"<<std::endl;
  std::cout<<"input file     : ROOT file or TXT file with the list of root files "   <<std::endl;
  std::cout<<"detector index : 1=MINERVA, 2:MINOS, 3: NOvA ND (see dkmeta for more indices)"   <<std::endl;
  std::cout<<"output file    : root output file"<<std::endl;
}

////////////////////////////////
#ifndef __CINT__
int main(int argc, const char* argv[]){  
  if(argc==1){
    calculateInteractions();
    exit (1);
  }
  calculateInteractions(argv[1],argv[2],argv[3]);
  return 0;
}
#endif
