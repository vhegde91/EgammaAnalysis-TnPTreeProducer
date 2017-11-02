#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"

#include "PhysicsTools/SelectorUtils/interface/CutApplicatorWithEventContentBase.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"

#include <algorithm>

class IsolationSum : public edm::EDProducer{
public:
  explicit IsolationSum(const edm::ParameterSet &pset);
  ~IsolationSum();

private:
  virtual void produce(edm::Event &event, const edm::EventSetup &setup) override;
  template <typename T> void putInEvent(std::string, const edm::Handle<edm::View<reco::Candidate>>&, std::vector<T>&, edm::Event&);
  double getPFIsolation(edm::Handle<pat::PackedCandidateCollection> pfcands,
                        const reco::Candidate* ptcl,
                        double r_iso_min, double r_iso_max, double kt_scale, double rho,
                        bool charged_only);

  EffectiveAreas                                   effectiveAreas_;
  edm::EDGetTokenT<edm::View<reco::Candidate>>     probesToken_;
  edm::EDGetTokenT<double>                         rhoToken_;
  edm::EDGetTokenT<pat::PackedCandidateCollection> candToken_;
  double minRadius_, maxRadius_;
  double ktScale_;
};

IsolationSum::IsolationSum(const edm::ParameterSet &pset):
  effectiveAreas_((pset.getParameter<edm::FileInPath>("effAreasConfigFile")).fullPath()),
  probesToken_(   consumes<edm::View<reco::Candidate>>(    pset.getParameter<edm::InputTag>("probes"))),
  rhoToken_(      consumes<double>(                        pset.getParameter<edm::InputTag>("rho"))),
  candToken_(     consumes<pat::PackedCandidateCollection>(pset.getParameter<edm::InputTag>("candidates"))),
  minRadius_(     pset.existsAs<double>("minRadius") ? pset.getParameter<double>("minRadius") : -1.),
  maxRadius_(     pset.existsAs<double>("maxRadius") ? pset.getParameter<double>("maxRadius") : -1.),
  ktScale_(       pset.existsAs<double>("ktScale")   ? pset.getParameter<double>("ktScale")   : -1.){
  produces<edm::ValueMap<float> >("sum");
  produces<edm::ValueMap<float> >("charged");
  produces<edm::ValueMap<float> >("neutral");
}

IsolationSum::~IsolationSum(){
}


double IsolationSum::getPFIsolation(edm::Handle<pat::PackedCandidateCollection> pfcands,
                        const reco::Candidate* ptcl,
                        double r_iso_min, double r_iso_max, double kt_scale, double rho,
                        bool charged_only){

    if (ptcl->pt()<5.) return 99999.;

    double absEta  = fabs(dynamic_cast<const pat::Electron *>(ptcl)->superCluster()->eta());


    double deadcone_nh(0.), deadcone_ch(0.), deadcone_ph(0.), deadcone_pu(0.);
    if(ptcl->isElectron() and absEta>1.479){ deadcone_ch = 0.015;  deadcone_pu = 0.015; deadcone_ph = 0.08; deadcone_nh = 0;}
    else if(ptcl->isMuon())                { deadcone_ch = 0.0001; deadcone_pu = 0.01;  deadcone_ph = 0.01; deadcone_nh = 0.01;}

    double iso_nh(0.); double iso_ch(0.); 
    double iso_ph(0.); double iso_pu(0.);
    double ptThresh = ptcl->isElectron()? 0. : 0.5;

    double max_pt = kt_scale/r_iso_min;
    double min_pt = kt_scale/r_iso_max;
    double r_iso  = kt_scale/std::max(std::min(ptcl->pt(), max_pt), min_pt);

    for(const pat::PackedCandidate &pfc : *pfcands){
      if(abs(pfc.pdgId())<7) continue;

      double dr = deltaR(pfc, *ptcl);
      if(dr > r_iso) continue;
      
      if(pfc.charge()==0){								// Neutral
        if(pfc.pt()>ptThresh){
          if(abs(pfc.pdgId())==22 and dr > deadcone_ph)        iso_ph += pfc.pt();	// Photons
          else if (abs(pfc.pdgId())==130 and dr > deadcone_nh) iso_nh += pfc.pt();	// Neutral hadrons
        }
      } else if (pfc.fromPV()>1){
        if(abs(pfc.pdgId())==211 and dr > deadcone_ch) iso_ch += pfc.pt();		// Charged from PV
      } else if(pfc.pt()>ptThresh and dr > deadcone_pu) iso_pu += pfc.pt();		// Charged from PU
    }

    double effArea = effectiveAreas_.getEffectiveArea(absEta);

    double iso;
    if(charged_only) iso = iso_ch;
    else             iso = iso_ch + std::max(0., iso_ph + iso_nh - rho*effArea*(r_iso*r_iso)/(0.3*0.3));

    iso = iso/ptcl->pt();

    return iso;
}



void IsolationSum::produce(edm::Event &event, const edm::EventSetup &setup){
  edm::Handle<double>                         rhoHandle;
  edm::Handle<edm::View<reco::Candidate>>     probesHandle;
  edm::Handle<pat::PackedCandidateCollection> candHandle;

  event.getByToken(rhoToken_,    rhoHandle);
  event.getByToken(probesToken_, probesHandle);
  event.getByToken(candToken_,   candHandle);

  std::vector<float> isos(probesHandle->size());
  std::vector<float> chargedIsos(probesHandle->size());
  std::vector<float> neutralIsos(probesHandle->size());

  for(size_t iprobe = 0; iprobe < probesHandle->size(); ++iprobe){
    auto cand = &(probesHandle->at(iprobe));

    isos.at(iprobe)        = getPFIsolation(candHandle, cand, minRadius_, maxRadius_, ktScale_, static_cast<float>(*rhoHandle), false);
    chargedIsos.at(iprobe) = getPFIsolation(candHandle, cand, minRadius_, maxRadius_, ktScale_, static_cast<float>(*rhoHandle), true);
    neutralIsos.at(iprobe) = isos.at(iprobe) - neutralIsos.at(iprobe);
  }

  putInEvent("sum",     probesHandle, isos,        event);
  putInEvent("charged", probesHandle, chargedIsos, event);
  putInEvent("neutral", probesHandle, neutralIsos, event);
}

/// Function to put product into event
template <typename T> void IsolationSum::putInEvent(std::string name, const edm::Handle<edm::View<reco::Candidate>>& probesHandle, std::vector<T>& product, edm::Event& iEvent){
  std::auto_ptr<edm::ValueMap<T>> out(new edm::ValueMap<T>());
  typename edm::ValueMap<T>::Filler filler(*out);
  filler.insert(probesHandle, product.begin(), product.end());
  filler.fill();
  iEvent.put(out, name);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(IsolationSum);
