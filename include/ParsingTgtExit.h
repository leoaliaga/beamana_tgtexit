
#ifndef PARSINGTGTEXIT_H
#define PARSINGTGTEXIT_H

#include "dk2nu/tree/dkmeta.h"
#include "dk2nu/tree/dk2nu.h"

#include <string>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <math.h>

namespace NeutrinoTgtAna{
  
  /*! \class ParsingTgtExit
   *  \brief Parse information of the particles exiting the target
   *
   */
  class ParsingTgtExit{
  public:
    
    //! default constructor
    ParsingTgtExit();
    void CalculateValues(bsim::Dk2Nu* nu);
    //! Destructor
    ~ParsingTgtExit();
    //Number of total interactions
    int NTotalInter();
    //Number of interactions before leaving the target
    int NCoveredInter();
    //Number of interactions in the target
    int NInterInTgt();
    //Interaction vertices and momentums in the target (value is -999 is not definied)
    double Vx();
    double Vy();
    double Vz();
    double Px();
    double Py();
    double Pz();
    double Pt();
    //Pdg code of the particle leaving the targetx
    int Pdg();
    
    static ParsingTgtExit* getInstance();
    static void resetInstance();
    
  private:
    static ParsingTgtExit* instance;
    double tvx,tvy,tvz,tpx,tpy,tpz,tpt;
    int ntot,ncov,tpdg,niintgt;
  };
  
};

#endif 
