from CRABClient.UserUtilities import config, getUsernameFromSiteDB
import sys

# this will use CRAB client API
from CRABAPI.RawCommand import crabCommand

# talk to DBS to get list of files in this dataset
from dbs.apis.dbsClient import DbsApi
dbs = DbsApi('https://cmsweb.cern.ch/dbs/prod/global/DBSReader')

dataset = '/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIISpring18MiniAOD-100X_upgrade2018_realistic_v10-v2/MINIAODSIM'
fileDictList=dbs.listFiles(dataset=dataset)

print ("dataset %s has %d files" % (dataset, len(fileDictList)))

# DBS client returns a list of dictionaries, but we want a list of Logical File Names
lfnList = [ dic['logical_file_name'] for dic in fileDictList ]

# this now standard CRAB configuration

from WMCore.Configuration import Configuration

config = config()

submitVersion ="Run2018_Moriond19JEC_TreeV3"
doEleTree = 'doEleID=True'
doPhoTree = 'doPhoID=False'
#doHLTTree = 'doTrigger=False'
#calibEn   = 'useCalibEn=False'

mainOutputDir = '/store/user/vhegde/EGamma_ntuples/%s' % submitVersion

config.General.transferLogs = False

config.JobType.pluginName  = 'Analysis'

# Name of the CMSSW configuration file
config.JobType.psetName  = '/afs/cern.ch/work/v/vhegde/public/EGamma_v5_2018/CMSSW_10_2_5/src/EgammaAnalysis/TnPTreeProducer/python/TnPTreeProducer_cfg.py'
#config.Data.allowNonValidInputDataset = False
config.JobType.sendExternalFolder     = True

config.Data.inputDBS = 'global'
config.Data.publication = False
config.Data.allowNonValidInputDataset = True
#config.Data.publishDataName = 

config.Site.storageSite = 'T3_US_FNALLPC'


 
if __name__ == '__main__':

    from CRABAPI.RawCommand import crabCommand
    from CRABClient.ClientExceptions import ClientException
    from httplib import HTTPException

    # We want to put all the CRAB project directories from the tasks we submit here into one common directory.
    # That's why we need to set this parameter (here or above in the configuration file, it does not matter, we will not overwrite it).
    config.General.workArea = 'crab_%s' % submitVersion

    def submit(config):
        try:
            crabCommand('submit', config = config)
        except HTTPException as hte:
            print "Failed submitting task: %s" % (hte.headers)
        except ClientException as cle:
            print "Failed submitting task: %s" % (cle)


    ##### submit MC

    config.Data.splitting     = 'FileBased'
    config.Data.unitsPerJob   = 20
    config.Data.outLFNDirBase = '%s/%s/' % (mainOutputDir,'mc')
    config.JobType.pyCfgParams  = ['isMC=True',doEleTree,doPhoTree,'GT=102X_upgrade2018_realistic_v12']
    config.JobType.inputFiles   = ['/afs/cern.ch/work/v/vhegde/public/EGamma_v5_2018/CMSSW_10_2_5/src/EgammaAnalysis/TnPTreeProducer/python/Autumn18_V8_MC.db']

    config.General.requestName  = 'DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8'
    config.Data.inputDataset    = '/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
    submit(config)

    config.General.requestName  = 'DYToEE_M-50_NNPDF31_TuneCP5_13TeV-powheg-pythia8'
    config.Data.inputDataset    = '/DYToEE_M-50_NNPDF31_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM'
    submit(config)


    ##### now submit DATA
    config.Data.outLFNDirBase = '%s/%s/' % (mainOutputDir,'data')
    config.Data.splitting     = 'LumiBased'
    config.Data.lumiMask      = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions18/13TeV/PromptReco/Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON.txt'
    config.Data.unitsPerJob   = 600
    config.JobType.pyCfgParams  = ['isMC=False',doEleTree,doPhoTree,'GT=102X_dataRun2_Sep2018Rereco_v1']
 
    config.General.requestName  = 'Run2018A_17Sep18'
    config.Data.inputDataset    = '/EGamma/Run2018A-17Sep2018-v2/MINIAOD'
    config.JobType.inputFiles   = ['/afs/cern.ch/work/v/vhegde/public/EGamma_v5_2018/CMSSW_10_2_5/src/EgammaAnalysis/TnPTreeProducer/python/Autumn18_RunABCD_V8_DATA.db']
#    submit(config)    

    config.General.requestName  = 'Run2018B_17Sep18'
    config.Data.inputDataset    = '/EGamma/Run2018B-17Sep2018-v1/MINIAOD'
#    submit(config)    

    config.General.requestName  = 'Run2018C_17Sep18'
    config.Data.inputDataset    = '/EGamma/Run2018C-17Sep2018-v1/MINIAOD'
#    submit(config)    

#    config.General.requestName  = 'Run2018D_PromptReco-v1'
#    config.Data.inputDataset    = '/EGamma/Run2018D-PromptReco-v1/MINIAOD'
##    submit(config)    

    config.JobType.pyCfgParams  = ['isMC=False',doEleTree,doPhoTree,'GT=102X_dataRun2_Prompt_v11']
    config.General.requestName  = 'Run2018D_PromptReco-v2'
    config.Data.inputDataset    = '/EGamma/Run2018D-PromptReco-v2/MINIAOD'
#    submit(config)    
