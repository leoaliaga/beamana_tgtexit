#!/bin/bash

setup(){

#Workdir:
    export WORKDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
    echo "setting WORKDIR=${WORKDIR}"

#General UPS setup
    . "/nusoft/app/externals/setup"

#ROOT
    setup -q debug:e2 -f Linux64bit+2.6-2.5 root v5_34_05

#BOOST    
    setup -q debug:e5 -f Linux64bit+2.6-2.5 boost v1_55_0
    export BOOSTROOT=${BOOST_DIR}/source/boost_1_55_0

#DK2NU    
    setup -q debug:e5 -f Linux64bit+2.6-2.5 dk2nu v01_01_03
    export DK2NU_INC=${DK2NU}/include/dk2nu/tree
    export DK2NU_LIB=${DK2NU}/lib

#Binaries and lib
    export PATH=${PATH}:${WORKDIR}/bin
    
    export LD_LIBRARY_PATH=${WORKDIR}/lib:$LD_LIBRARY_PATH
    echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
    
}
setup
