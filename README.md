# EgammaAnalysis-TnPTreeProducer
TnP package for EGM

For regular users
1. install

```
cmsrel CMSSW_9_4_7
cd CMSSW_9_4_7/src/
cmsenv
git cms-init
git cms-merge-topic lsoffi:CMSSW_9_4_0_pre3_TnP
git cms-merge-topic guitargeek:ElectronID_MVA2017_940pre3
scram b
cd $CMSSW_BASE/external
cd slc6_amd64_gcc630/
git clone https://github.com/lsoffi/RecoEgamma-PhotonIdentification.git data/RecoEgamma/PhotonIdentification/data
cd data/RecoEgamma/PhotonIdentification/data
git checkout CMSSW_9_4_0_pre3_TnP
cd $CMSSW_BASE/external
cd slc6_amd64_gcc630/
git clone https://github.com/lsoffi/RecoEgamma-ElectronIdentification.git data/RecoEgamma/ElectronIdentification/data
cd data/RecoEgamma/ElectronIdentification/data
git checkout CMSSW_9_4_0_pre3_TnP
cd $CMSSW_BASE/src
git clone -b SUSYFastSim_on_CMSSW_9_4_7 https://github.com/vhegde91/EgammaAnalysis-TnPTreeProducer.git EgammaAnalysis/TnPTreeProducer
scram b
cd EgammaAnalysis/TnPTreeProducer/

cmsRun python/TnPTreeProducer_cfg.py doEleID=True isMC=True maxEvents=500 doPhoID=False
```



For developpers
1. On github fork the package https://github.com/cms-analysis/EgammaAnalysis-TnPTreeProducer 
2. Add the remote 

git remote add username-push git@github.com:username/EgammaAnalysis-TnPTreeProducer.git

3. push commits to fork and then standard pull request process
git push username-push branchname

4. submit jobs