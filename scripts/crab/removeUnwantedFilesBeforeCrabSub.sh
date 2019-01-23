#!/bin/bash
#cd $CMSSW_BASE/external
cd /afs/cern.ch/work/v/vhegde/public/EGamma_v3/withSUSYids_v3/CMSSW_9_4_7/external
rm slc6_amd64_gcc630/data/RecoEgamma/PhotonIdentification/data/Spring15/*
rm slc6_amd64_gcc630/data/RecoEgamma/PhotonIdentification/data/Spring16/photon_general_MVA_Spring16_E*
rm slc6_amd64_gcc630/data/RecoEgamma/ElectronIdentification/data/Spring15/*
rm slc6_amd64_gcc630/data/RecoEgamma/ElectronIdentification/data/PHYS14/*
cd -
