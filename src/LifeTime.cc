// -*- C++ -*-
//
// Package:    LifeTime
// Class:      LifeTime
// 
/**\class LifeTime LifeTime.cc NWU/LifeTime/src/LifeTime.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andrey Pozdnyakov
//         Created:  Thu Apr 11 14:54:41 CDT 2013
// $Id$
//
//


// system include files
#include <memory>
#include <iostream>
#include <fstream>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"

#include "MyHiggsEvent.h"
#include "HistManager.h"


using namespace std;
using namespace edm;
using namespace reco;

class LifeTime : public edm::EDAnalyzer {
   public:
      explicit LifeTime(const edm::ParameterSet&);
      ~LifeTime();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

      // ----------member data ---------------------------


  vector<unsigned> runv,evtv; // for removing duplicate events


  vector<MyHiggsEvent> HiggsEvents;

  HistManager *hists;
  TFile *outFile;

};

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

LifeTime::LifeTime(const edm::ParameterSet& iConfig)
{
}


LifeTime::~LifeTime()
{
  delete hists;
  delete outFile;
}


void LifeTime::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   
   //cout<<"event"<<endl;


   if(iEvent.isRealData()) { 

     bool isDuplic=false;
     bool isSameRun=false;
     for(unsigned ievt=0; ievt<evtv.size(); ievt++) {
       if((iEvent.id().run() == runv[ievt]) && (iEvent.id().event() == evtv[ievt])) {
         isDuplic = true;
         break;
       }
       else if (iEvent.id().run() == runv[ievt])
         isSameRun = true;
     }

     if(isDuplic) {
       cout << "   skipping duplicate event: " << iEvent.id().run() << " " << iEvent.id().event() << endl;
       return;
     }
     else{
       if (isSameRun)
         cout<<"Events in the same run  but different ev number.  Run = "<<iEvent.id().run()<< "   ev = "<<iEvent.id().event()<<endl;
     
       runv.push_back(iEvent.id().run());
       evtv.push_back(iEvent.id().event());
       hists->fill1DHist(iEvent.id().run(),"run","run", 300000,0,300000,  1, "");
     }
   }

   //hists->fill1DHist(1,"run","run", 3,0,3,  1, "");


   Handle<vector<reco::Muon> > muons;
   iEvent.getByLabel("muons", muons);

   //cout<<"we are here"<<endl;

   UInt_t nMuons7=0;
   for (vector<reco::Muon>::const_iterator iMuon = muons->begin(); iMuon != muons->end(); ++iMuon) {
     if(!iMuon->isGlobalMuon() || !iMuon->isTrackerMuon()) continue;
     if(iMuon->pt()<7) continue;
     
     nMuons7++;
     //cout<<"Muon"<<endl;
   }

   hists->fill1DHist(muons->size(),"nMuons","Number of muons", 5,0,5,  1, "");
   hists->fill1DHist(nMuons7,"nMuons7","Number of muons with pt>7", 5,0,5,  1, "");

   Handle<reco::ConversionCollection> hConversions;
   iEvent.getByLabel("allConversions", hConversions);

   UInt_t nElectrons7 = 0;
   Handle<reco::GsfElectronCollection > electrons;
   iEvent.getByLabel("gsfElectrons", electrons);

   for (vector<reco::GsfElectron>::const_iterator iElectron = electrons->begin(); iElectron != electrons->end(); ++iElectron) {
     if (iElectron->pt() < 7) continue;
     nElectrons7++;
   }

   hists->fill1DHist(electrons->size(),"nElectrons","Number of electrons", 5,0,5,  1, "");
   hists->fill1DHist(nElectrons7,"nElectrons7","Number of electrons with pt>7", 5,0,5,  1, "");

   UInt_t  nLeptons7 = nMuons7+nElectrons7;
   hists->fill1DHist(electrons->size() + muons->size(),"nLeptons","Number of leptons", 5,0,5,  1, "");
   hists->fill1DHist(nLeptons7,"nLeptons7","Number of leptons with pt>7", 5,0,5,  1, "");


}

void LifeTime::beginJob()
{
  outFile = new TFile("output.root","recreate");
  hists   = new HistManager(outFile);
  outFile->cd();


  //AP. Since I don't want to perform whole HZZ4l analysis, here is a hack.
  //(Such ass FSR corrections, finding the right leptons, arranging the Z candidates etc)
  //I took all this information from the Mit's ntuples and dumped in a txt file.
  //Here  I read that file and store the objects as a  vector of MyHiggsEvent() class.
  //I only interested in the run/event number and 4 leptons that I will later match to the ones in my skimmed sample.


  MyHiggsEvent *hEv = new MyHiggsEvent(); 
  string STRING;
  ifstream infile;
  infile.open ("events.txt");

  while(infile>>STRING) // To get you all the lines.
    {
      
      std::vector<std::string> x = split(STRING, ':');      

      if (x.size()==0) continue;  
      if (x[0]=="#run") continue; //Ignore the first line

      hEv->SetRun((UInt_t)atoi(x[0].c_str()));
      hEv->SetLs( (UInt_t)atoi(x[1].c_str()));
      hEv->SetEvt((UInt_t)atoi(x[2].c_str()));

      TLorentzVector l[4];
      for(int n = 0; n<=3; n++)
        {
          int t =  6+4*n;
          //cout<<t<<"   "<<x[3+4*n]<<" "<<x[4+4*n]<<" "<<x[5+4*n]<<endl;
          if (x[t]=="11") //electron!
            l[n].SetPtEtaPhiM(atof(x[3+4*n].c_str()), atof(x[4+4*n].c_str()),atof(x[5+4*n].c_str()), 0.511e-3);
          else if(x[t]=="13") //muon
            l[n].SetPtEtaPhiM(atof(x[3+4*n].c_str()), atof(x[4+4*n].c_str()),atof(x[5+4*n].c_str()), 105.66e-3);
          else
            cout<<"  !!! !!!Something crazy!!  --->> "<<x[t]<<endl;
          
        }
      hEv->SetLep1(l[0]);
      hEv->SetLep2(l[1]);
      hEv->SetLep3(l[2]);
      hEv->SetLep4(l[3]);

      cout<<hEv->run()<<endl;
      HiggsEvents.push_back(*hEv);
    }

  cout<<"Higgs Events size = "<<HiggsEvents.size()<<endl;
  //cout<<HiggsEvents.at(10).lep1().Pt()<<endl;

  infile.close();
}

void LifeTime::endJob() 
{
  outFile->cd();
  hists->writeHists(outFile);
  //outFile->Write();
  outFile->Close();
}

void LifeTime::beginRun(edm::Run const&, edm::EventSetup const&)
{ }

void  LifeTime::endRun(edm::Run const&, edm::EventSetup const&)
{ }

void LifeTime::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{}

void LifeTime::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{}

void LifeTime::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}




DEFINE_FWK_MODULE(LifeTime);
