# EgammaAnalysis-TnPTreeProducer
TnP package for EGM

# EgammaAnalysis-TnPTreeProducer
TnP package for EGM

```
cmsrel CMSSW_10_2_5
cd CMSSW_10_2_5/src
cmsenv
```

Copy some of the files from lxplus area	of vhegde. This	is not needed, if the EGM recipe (https://twiki.cern.ch/twiki/bin/view/CMSPublic/ElectronTagAndProbe) is working well.
```
cp -r /afs/cern.ch/work/v/vhegde/public/EGamma_FastSim/Run2018/CMSSW_10_2_5/src/* .
```

Clone the repo for 2018 FastSim. If you have copied files using the command above, the you do not need this step.
```
git clone -b MC2018_FastSim https://github.com/vhegde91/EgammaAnalysis-TnPTreeProducer

```
