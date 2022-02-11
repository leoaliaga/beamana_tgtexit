#include "ParsingTgtExit.h"

namespace NeutrinoTgtAna{

  ParsingTgtExit* ParsingTgtExit::instance = 0;
  
  ParsingTgtExit::ParsingTgtExit(){    
    tvx   = -1000.;
    tvy   = -1000.;
    tvz   = -1000.;
    tpx   = -1000.;
    tpy   = -1000.;
    tpz   = -1000.;
    tpt   = -1000.;
    tpdg  = 0;
    ntot  = 0;
    ncov  = 0;
    niintgt = 0;
  }

  void ParsingTgtExit::CalculateValues(bsim::Dk2Nu* nu){    
    tvx   = nu->tgtexit.tvx;
    tvy   = nu->tgtexit.tvy;
    tvz   = nu->tgtexit.tvz;
    tpx   = nu->tgtexit.tpx;
    tpy   = nu->tgtexit.tpy;
    tpz   = nu->tgtexit.tpz;
    if( (tvx < -998.) &&(tvy < -998.) &&(tvz < -998.) ){
      tpt   = -999.999;
    }
    else{
      tpt   = sqrt(pow(tpx,2)+pow(tpy,2));
    }
    
    tpdg  = nu->tgtexit.tptype;

    ntot  = 0;
    ncov  = 0;
    niintgt = 0;
    
    int last_tgt = 0;
    int Nanc     = int(nu->ancestor.size());
    for(unsigned int ianc=0;ianc<Nanc;ianc++){
      
      bool isinel = (nu->ancestor[ianc].proc).find("Inelastic") < 1000;
      
      bool istgt = (nu->ancestor[ianc].ivol == "TGT1") ||
	(nu->ancestor[ianc].ivol == "Budal_VFHS") ||
	(nu->ancestor[ianc].ivol == "Budal_HFVS");

      if(!isinel)continue;
      
      if( isinel )ntot++;
      if( isinel && istgt ){
	niintgt++;
	last_tgt = ntot;      
      }
    }    
    ncov=last_tgt;

    if( tpt < -998. )ncov=0;
    if( tpt > -998. && last_tgt==0 )ncov=0;

  }
  
  ParsingTgtExit::~ParsingTgtExit(){ }

  double ParsingTgtExit::Vx(){return tvx;}
  double ParsingTgtExit::Vy(){return tvy;}
  double ParsingTgtExit::Vz(){return tvz;}
  double ParsingTgtExit::Px(){return tpx;}
  double ParsingTgtExit::Py(){return tpy;}
  double ParsingTgtExit::Pz(){return tpz;}
  double ParsingTgtExit::Pt(){return tpt;}
  int    ParsingTgtExit::Pdg(){return tpdg;}
  int    ParsingTgtExit::NTotalInter()  {return ntot;}
  int    ParsingTgtExit::NCoveredInter(){return ncov;}
  int    ParsingTgtExit::NInterInTgt()  {return niintgt;}
  
  ParsingTgtExit* ParsingTgtExit::getInstance(){
    if (instance == 0) instance = new ParsingTgtExit();
    return instance;
  }
  ////
  void ParsingTgtExit::resetInstance(){
    delete instance; 
    instance = 0; 
  }
  
};
