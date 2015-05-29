// -----------------------------------------------------
// bprimeKit.cc -- b' analysis kit
// -----------------------------------------------------
// For CMSSW_3_6_1
// Lastest updates:
// Jul 05, 2010 - Add Electron ID by Jacky, Add PFMet by Chiyi
// May 19, 2010 - Add Muon Iso variables, MCTag, and PF Jet  - by Dmitry Hits & Yeong-jyi Kei
// Apr 15, 2010 - Update to CMSSW_3_5_6, fix bTag bug  - by Yeong-jyi Lei
// Sep 24, 2009 - Updates label name, and triggerbook. Add PhotonInfo - by Yeong-jyi Lei
// Sep 11, 2009 - Updates with CMSSW_3_1_2 - by Kai-Feng Chen
// May 29, 2009 - Include GenInfo, electron classification, bug fix on vtx - by Yeong-jyi Lei
// Mar 10, 2009 - Change the trigger table
// Feb 20, 2009 - Protection added for missing InnerTrack() from muons, adding MC top mass, bug fix on JetInfo.
// Jan 03, 2009 - Updates according to CMSSW_2_2_3 release (clean 2_2_3 + PhysicsTools/PatAlgos V04-14-15)

#include "MyAna/bprimeKit/interface/bprimeKit.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TObject.h>
#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <map>

#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"

#include "DataFormats/Candidate/interface/Particle.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include "DataFormats/PatCandidates/interface/Particle.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/METReco/interface/PFMET.h"

#include "DataFormats/Math/interface/angle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "DataFormats/METReco/interface/CaloMETCollection.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

//For electron convertion flag
#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "DataFormats/TrackReco/interface/Track.h"
//#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Scalers/interface/DcsStatus.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "EgammaAnalysis/ElectronTools/interface/EGammaCutBasedEleId.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectronCore.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronCoreFwd.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PileUpPFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PileUpPFCandidateFwd.h"
#include "EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h"
#include "UserCode/sixie/Muon/MuonAnalysisTools/interface/MuonEffectiveArea.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"


// For JEC
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
//#include "JetMETCorrections/Objects/interface/JetCorrector.h"

// PileupSummaryInfo
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include <string>

//
// DM: Addiign header for IVF and double b tagging
//
#include "RecoBTag/SecondaryVertex/interface/SecondaryVertex.h"
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"

#include "QuarkGluonTagger/EightTeV/interface/QGTagger.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"


// Including tool from ggNTuplizer for isolation calculation 


#define W_MASS          80.403
#define Z_MASS          91.1876
#define MUON_MASS       0.105658
#define ELECTRON_MASS   0.0005109989
#define TurnOffInCMSSW73x true
#define TurnOnInCMSSW_7_4_1 false
// uncomment the following line for filling the di-jet pairs
//#define FILL_DIJET_PAIRS 1

using namespace edm;
using namespace reco;
using namespace std;
using namespace pat;
using namespace math;
using namespace ROOT;


bprimeKit::bprimeKit( const edm::ParameterSet& iConfig )
{
   edm::Service<TFileService> fs;
   TFileDirectory results = TFileDirectory( fs->mkdir( "results" ) );

   //MCtag                = iConfig.getUntrackedParameter<bool>("MCtag",true);
   muonlabel_           = iConfig.getParameter<std::vector<InputTag> >( "muonlabel" ); //"cleanPatMuons"
   eleclabel_           = iConfig.getParameter<std::vector<InputTag> >( "eleclabel" ); // "cleanPatElectrons"
   taulabel_            = iConfig.getParameter<std::vector<InputTag> >( "taulabel" ); // "selectedPatTausPFlow"
   pholabel_            = iConfig.getParameter<std::vector<InputTag> >( "pholabel" );
   // jetlabel_            = iConfig.getParameter<std::vector<InputTag> >( "jetlabel" ); // "cleanPatJets"
   metlabel_            = iConfig.getParameter<std::vector<InputTag> >( "metlabel" ); //"patMETs"
   pfmetlabel_          = iConfig.getParameter<std::vector<InputTag> >( "pfmetlabel" ); //"pfpatMETs"
   genlabel_            = iConfig.getParameter<std::vector<InputTag> >( "genlabel" ); // "genParticles"
   hltlabel_            = iConfig.getParameter<std::vector<InputTag> >( "hltlabel" ); // "TriggerResults::HLT"
   pathltlabel_         = iConfig.getParameter<std::vector<InputTag> >( "pathltlabel" ); // patTriggerEvent
   offlinePVlabel_      = iConfig.getParameter<std::vector<InputTag> >( "offlinePVlabel" ); //offlinePrimaryVertices
   offlinePVBSlabel_    = iConfig.getParameter<std::vector<InputTag> >( "offlinePVBSlabel" ); //offlinePrimaryVerticesWithBS
   offlineBSlabel_      = iConfig.getParameter<std::vector<InputTag> >( "offlineBSlabel" ); //offlineBeamSpot
   tracklabel_          = iConfig.getParameter<std::vector<InputTag> >( "tracklabel" ); //generalTracks
   dcslabel_            = iConfig.getParameter<std::vector<InputTag> >( "dcslabel" ); //scalersRawToDigi
   genevtlabel_         = iConfig.getParameter<std::vector<InputTag> >( "genevtlabel" ); //generator
   gtdigilabel_         = iConfig.getParameter<std::vector<InputTag> >( "gtdigilabel" ); //gtDigis
   rhocorrectionlabel_  = iConfig.getParameter<std::vector<InputTag> >( "rhocorrectionlabel" ); // For PU correction
   sigmaLabel_          = iConfig.getParameter<std::vector<InputTag> >( "sigmaLabel" ); // For PU correction
   puInfoLabel_         = iConfig.getParameter<std::vector<InputTag> >( "puInfoLabel" );

   pfToken_             = ( consumes<pat::PackedCandidateCollection>( iConfig.getParameter<edm::InputTag>( "pfCands" ) ) );

   // Add 2012 EID simple-cut-based
   conversionsInputTag_    = iConfig.getParameter<edm::InputTag>( "conversionsInputTag" );
   rhoIsoInputTag          = iConfig.getParameter<edm::InputTag>( "rhoIsoInputTag" );
   isoValInputTags_        = iConfig.getParameter<std::vector<edm::InputTag> >( "isoValInputTags" );
   EIDMVAInputTags_        = iConfig.getParameter<std::vector<std::string> >( "EIDMVAInputTags" );

   lepcollections_      = iConfig.getParameter<std::vector<std::string> >( "LepCollections" ); //branch names
   phocollections_      = iConfig.getParameter<std::vector<std::string> >( "PhoCollections" ); //branch names
   jetcollections_      = iConfig.getParameter<std::vector<std::string> >( "JetCollections" ); //branch names
   jettype_             = iConfig.getParameter<std::vector<int> >( "JetType" );

   pairColl_            = iConfig.getUntrackedParameter<int>( "PairCollection", 0 );
   getElectronID_       = iConfig.getUntrackedParameter<bool>( "ElectronID", true );
   skipGenInfo_         = iConfig.getUntrackedParameter<bool>( "SkipGenInfo", false );
   includeL7_           = iConfig.getUntrackedParameter<bool>( "IncludeL7", true );

   SelectionParameters_ = iConfig.getParameter<edm::ParameterSet>( "SelectionParameters" );

   // update for CMSSW_7_2_0
   reducedEBRecHitCollectionToken_ = consumes<EcalRecHitCollection>( iConfig.getParameter<edm::InputTag>( "reducedEBRecHitCollection" ) );
   reducedEERecHitCollectionToken_ = consumes<EcalRecHitCollection>( iConfig.getParameter<edm::InputTag>( "reducedEERecHitCollection" ) );
   // update for CMSSW_7_3_1
   // reducedEBRecHitCollectionToken_ = consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("reducedEgamma","reducedEBRecHits"));
   // reducedEERecHitCollectionToken_ = consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("reducedEgamma","reducedEERecHits"));
   
   debug_ = iConfig.getUntrackedParameter<int>( "Debug", 0 );

   isolatorR03.initializeElectronIsolation( kTRUE ); //NOTE: this automatically set all the correct defaul veto values
   isolatorR04.initializeElectronIsolation( kTRUE ); //NOTE: this automatically set all the correct defaul veto values
   isolatorR03.setConeSize( 0.3 );
   isolatorR04.setConeSize( 0.4 );

   PhotonisolatorR03.initializePhotonIsolation( kTRUE ); //NOTE: this automatically set all the correct defaul veto values
   PhotonisolatorR04.initializePhotonIsolation( kTRUE ); //NOTE: this automatically set all the correct defaul veto values
   PhotonisolatorR03.setConeSize( 0.3 );
   PhotonisolatorR04.setConeSize( 0.4 );
   // NOTE: It is safer and crab-compliant to get the files locally, i.e in EgammaAnalysis/ElectronTools/data
   // (see the downloard.url file in that directory)
   // Alternatively (for tests), they can be read from AFS:
   //*
   myMVANonTrig = new EGammaMvaEleEstimator();
   std::vector<std::string> myManualCatWeigths;
   if( EIDMVAInputTags_.size() != 12 ) { cout << "EIDMVAInputTags array size (12) is not correct" << endl; }
   for( int ie = 0; ie < 6; ie++ ) { myManualCatWeigths.push_back( EIDMVAInputTags_[ie].c_str() ); }

   Bool_t manualCat = true;

   myMVANonTrig->initialize( "BDT",
                             EGammaMvaEleEstimator::kNonTrig,
                             manualCat,
                             myManualCatWeigths );

   // NOTE: it is better if you copy the MVA weight files locally. See the previous remark
   std::vector<std::string> myManualCatWeigthsTrig;
   for( int ie = 0; ie < 6; ie++ ) { myManualCatWeigthsTrig.push_back( EIDMVAInputTags_[ie + 6].c_str() ); }

   myMVATrig = new EGammaMvaEleEstimator();
   myMVATrig->initialize( "BDT",
                          EGammaMvaEleEstimator::kTrig,
                          manualCat,
                          myManualCatWeigthsTrig );

   for( int i = 0; i < N_TRIGGER_BOOKINGS; i++ ) { HLTmaplist.insert( pair< std::string, int > ( TriggerBooking[i], i ) ); }

}


bprimeKit::~bprimeKit()
{
}

void bprimeKit::beginJob()
{
   root = new TTree( "root", "root" );
   EvtInfo.RegisterTree( root );
   if( !skipGenInfo_ ) { GenInfo.RegisterTree( root ); }
   if( lepcollections_.size() > MAX_LEPCOLLECTIONS ) { cout << "WARNING: Too many lep collections, using first " << MAX_LEPCOLLECTIONS << endl; }
   for( unsigned i = 0; i < lepcollections_.size(); i++ ) {
      if( i >= MAX_LEPCOLLECTIONS ) { break; }
      LepInfo[i].RegisterTree( root, lepcollections_[i] );
   }
   for( unsigned i = 0; i < phocollections_.size(); i++ ) {
      if( i >= MAX_PHOCOLLECTIONS ) { break; }
      PhotonInfo[i].RegisterTree( root, phocollections_[i] );
   }
   VertexInfo.RegisterTree( root );
   if( jetcollections_.size() > MAX_JETCOLLECTIONS ) { cout << "WARNING: Too many jet collections, using first " << MAX_JETCOLLECTIONS << endl; }
   for( unsigned i = 0; i < jetcollections_.size(); i++ ) {
      if( i >= MAX_JETCOLLECTIONS ) { break; }
      JetInfo[i].RegisterTree( root, jetcollections_[i] );
   }
   if( pairColl_ >= 0 ) { PairInfo.RegisterTree( root ); }

}

void bprimeKit::endJob()
{
}


//-----------------  Method called when starting to process a run  ------------------
void bprimeKit::beginRun
( edm::Run const& iRun, edm::EventSetup const& iSetup )
{
   std::string processName_ = "HLT";
   bool changed( false );
   hltConfig_.init( iRun, iSetup, processName_, changed );
}

//-----------------------------  When ending a process  -----------------------------
void bprimeKit::endRun
( edm::Run const&, edm::EventSetup const& )
{
}


void bprimeKit::analyze
( const edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   edm::ESHandle<ParticleDataTable> pdt_;
   iSetup.getData( pdt_ );
   if( debug_ > 0 ) { cout << "Begin Analyze" << endl; }
   isData = iEvent.isRealData();   // Add by Jacky

   std::vector<edm::Handle<std::vector<pat::Muon    >>> MuonHandle;
   std::vector<edm::Handle<std::vector<pat::Electron>>> ElectronHandle;
   std::vector<edm::Handle<std::vector<pat::Tau     >>> TauHandle;
   edm::Handle<std::vector<pat::MET> >          METHandle;
   edm::Handle<std::vector<pat::MET> >          pfMETHandle;


   edm::Handle<reco::GenParticleCollection>     GenHandle;
   //edm::Handle<reco::VertexCollection>          VertexHandlePixel; //Dmitry
   edm::Handle<edm::View<reco::Track> >         TrackHandle; //Dmitry
   edm::Handle<reco::TrackCollection>           tracks_h; // Jacky
   edm::Handle<DcsStatusCollection>             dcsHandle;   // Jacky

   std::vector<edm::Handle<double> > rhoH;
   std::vector<edm::Handle<double> > sigmaHandle;

   if( !TurnOffInCMSSW73x )
      for( unsigned il = 0; il < rhocorrectionlabel_.size(); il++ ) {
         rhoH.push_back( edm::Handle<double> () );
         iEvent.getByLabel( rhocorrectionlabel_[il], rhoH[il] );
         sigmaHandle.push_back( edm::Handle<double> () );
         iEvent.getByLabel( sigmaLabel_[il], sigmaHandle[il] );
      }

   for( unsigned il = 0; il < muonlabel_.size(); il++ ) {
      MuonHandle.push_back( edm::Handle<std::vector<pat::Muon> >() );
      iEvent.getByLabel( muonlabel_[il], MuonHandle[il] );
      if( debug_ > 10 ) { cout << "leps " << il << " muonlabel " << muonlabel_[il] << " with " << MuonHandle[il]->size() << " entries\n"; }
   }
   for( unsigned il = 0; il < eleclabel_.size(); il++ ) {
      ElectronHandle.push_back( edm::Handle<std::vector<pat::Electron> >() );
      iEvent.getByLabel( eleclabel_[il], ElectronHandle[il] );
      if( debug_ > 10 ) { cout << "leps " << il << " electronlabel " << eleclabel_[il] << " with " << ElectronHandle[il]->size() << " entries\n"; }
   }

   string NonLabel = "";
   for( unsigned il = 0; il < taulabel_.size(); il++ ) {
      if( NonLabel.compare( taulabel_[il].label() ) == 0 ) { continue; }
      TauHandle.push_back( edm::Handle<std::vector<pat::Tau> >() );
      iEvent.getByLabel( taulabel_[il], TauHandle[il] );
      if( debug_ > 10 ) { cout << "leps " << il << " taulabel " << taulabel_[il] << " with " << TauHandle[il]->size() << " entries\n"; }
   }

   //  if(metlabel_.size() > 0) iEvent.getByLabel( metlabel_[0],    METHandle);
   if( pfmetlabel_.size() > 0 ) { iEvent.getByLabel( pfmetlabel_[0],  pfMETHandle ); }

   if( !isData && genlabel_.size() > 0 ) { iEvent.getByLabel( genlabel_[0], GenHandle ); }

   if( offlinePVlabel_.size() > 0 ) { iEvent.getByLabel( offlinePVlabel_[0], VertexHandle ); }
   //const reco::VertexCollection* pvCol = VertexHandle.product();

   if( offlinePVBSlabel_.size() > 0 ) { iEvent.getByLabel( offlinePVBSlabel_[0], VertexHandleBS ); } //Offline primary vertices with Beam Spot constraint //Dmitry
   if( !TurnOffInCMSSW73x )
      if( tracklabel_.size() > 0 ) { iEvent.getByLabel( tracklabel_[0], TrackHandle ); }         //get tracks for calculating dRmin (Dmitry)
   if( dcslabel_.size() > 0 ) { iEvent.getByLabel( dcslabel_[0], dcsHandle ); }            //refer to ElectroWeakAnalysis/MultiBosons/VgAnalyzerKit.cc (Jacky)
   if( !TurnOffInCMSSW73x )
      if( tracklabel_.size() > 0 ) { iEvent.getByLabel( tracklabel_[0], tracks_h ); }            //Add by Jacky

   printf( "[Test] tracklabel is ok?\n" );

   edm::Handle< pat::TriggerEvent > triggerEvent;
   if( pathltlabel_.size() > 0 ) { iEvent.getByLabel( pathltlabel_[0], triggerEvent ); }
   const pat::helper::TriggerMatchHelper matchHelper;

   const edm::View<reco::Track>& tracks = *TrackHandle;

   // electrons
   edm::Handle<reco::GsfElectronCollection> els_h;
   iEvent.getByLabel( "reducedEgamma", "reducedGedGsfElectronCores", els_h ); //  for CMSSW73x
   //iEvent.getByLabel("gsfElectrons", els_h);

   // conversions
   edm::Handle<reco::ConversionCollection> conversions_h;
   iEvent.getByLabel( conversionsInputTag_, conversions_h );

   // rho for isolation
   cout << "Attempting to read rhoiso..." << endl;
   edm::Handle<double> rhoIso_h;
   if( !TurnOffInCMSSW73x )
   iEvent.getByLabel( rhoIsoInputTag, rhoIso_h ); 
   double rhoIso = 0;
   if( !TurnOffInCMSSW73x )
   rhoIso = *( rhoIso_h.product() );

   // All PF Candidate for alternate isolation
   edm::Handle<reco::PFCandidateCollection> pfCandidatesH;
   if( !TurnOffInCMSSW73x )
   { iEvent.getByLabel( "particleFlow", pfCandidatesH ); }
   if( !TurnOffInCMSSW73x )
   { thePfColl = *( pfCandidatesH.product() ); }

   // development for CMSSW_73X
   //edm::Handle<pat::PackedCandidateCollection> thePfColl;
   //iEvent.getByToken(pfToken_, thePfColl);
   //const  PackedCandidateCollection thePfColl = pfCandidatesH;


   // update for CMSSW_7_2_0
   // reference to (https://cmssdt.cern.ch/SDT/lxr/source//EgammaAnalysis/ElectronTools/plugins/ElectronIdMVAProducer.cc)
   EcalClusterLazyTools lazyTools( iEvent, iSetup, reducedEBRecHitCollectionToken_, reducedEERecHitCollectionToken_ );

   if( TurnOnInCMSSW_7_4_1 ){
   //    edm::ESHandle<TransientTrackBuilder> builder;
   //    iSetup.get<TransientTrackRecord>().get( "TransientTrackBuilder", builder );
   //    TransientTrackBuilder thebuilder = *( builder.product() );
   //    const TransientTrackBuilder* transientTrackBuilder = builder.product();
   }

   //  get ConditionsInRunBlock (Only for RECO)
   /*
   edm::Handle<edm::ConditionsInRunBlock> condInRunBlock;
   iEvent.getByLabel("conditionsInEdm", condInRunBlock);

   const boost::uint16_t beamMomentumVal = condInRunBlock->beamMomentum;
   const boost::uint32_t lhcFillNumberVal = condInRunBlock->lhcFillNumber;

   std::cout <<"  Beam Momentum = " << beamMomentumVal << " GeV" << std::endl;
   std::cout <<"  LHC Fill Number = " << lhcFillNumberVal<< std::endl;
   */

   //  iEvent.getByLabel("electronGsfTracks",      gsfTrackHandle);  //get gsftracks (Dmitry)
   //  const edm::View<reco::GsfTrack> & gsfTracks = *gsfTrackHandle;

   // Start to fill the main root branches

   //************************************************************************************
   // The magnetic field (modified by Jacky,
   // https://twiki.cern.ch/twiki/bin/view/CMS/ConversionBackgroundRejection)
   //************************************************************************************
   // need the magnetic field
   //
   // if isRealData then derive bfield using the
   // magnet current from DcsStatus
   // otherwise take it from the IdealMagneticFieldRecord
   double evt_bField;
   if ( isData ) {
      // scale factor = 3.801/18166.0 which are
      // average values taken over a stable two
      // week period
      if( ( *dcsHandle ).size() != 0 ) {
         float currentToBFieldScaleFactor = 2.09237036221512717e-04;
         float current = ( *dcsHandle )[0].magnetCurrent();
         evt_bField = current * currentToBFieldScaleFactor;
      } else {
         evt_bField = 3.80;
      }
   } else {
      ESHandle<MagneticField> magneticField;
      iSetup.get<IdealMagneticFieldRecord>().get( magneticField );

      evt_bField = magneticField->inTesla( Surface::GlobalPoint( 0., 0., 0. ) ).z();
   }

   //------------------------------------------------------------------------------ 
   //   VertexInfo added by Dmitry to help filter out pile up and bad events
   //------------------------------------------------------------------------------ 
   fillVertex( iEvent , iSetup ) ;

   //=================================================================================
   // Leptons
   //=================================================================================
   for( unsigned icoll = 0; icoll < lepcollections_.size(); icoll++ ) { //loop over collections
      if( icoll >= MAX_LEPCOLLECTIONS ) { break; }
      if( debug_ > 5 ) { cout << "Fill lepton info, collection " << icoll << " with name " << lepcollections_[icoll] << endl; }
      memset( &LepInfo[icoll], 0x00, sizeof( LepInfo[icoll] ) );
      //Muons
      if( MuonHandle.size() > icoll ) { //have muon collection
         if( debug_ > 10 ) { cout << " Muon collection size " << MuonHandle[icoll]->size() << endl; }
         for( std::vector<pat::Muon>::const_iterator it_mu = MuonHandle[icoll]->begin();
              it_mu != MuonHandle[icoll]->end(); it_mu++ ) { //loop over muons in collection

            if( debug_ > 11 ) { cout << "  Size " << LepInfo[icoll].Size << " mu pt,eta,phi " << it_mu->pt() << "," << it_mu->eta() << "," << it_mu->phi() << endl; }
            if ( LepInfo[icoll].Size >= MAX_LEPTONS ) {
               fprintf( stderr, "ERROR: number of leptons exceeds the size of array.\n" );
               break;//exit(0);
            }

            LepInfo[icoll].Index      [LepInfo[icoll].Size] = LepInfo[icoll].Size;
            LepInfo[icoll].LeptonType [LepInfo[icoll].Size] = 13;
            LepInfo[icoll].Charge     [LepInfo[icoll].Size] = it_mu->charge();
            LepInfo[icoll].Pt         [LepInfo[icoll].Size] = it_mu->pt();
            LepInfo[icoll].Eta        [LepInfo[icoll].Size] = it_mu->eta();
            LepInfo[icoll].Phi        [LepInfo[icoll].Size] = it_mu->phi();
            LepInfo[icoll].TrackIso   [LepInfo[icoll].Size] = it_mu->trackIso();
            LepInfo[icoll].EcalIso    [LepInfo[icoll].Size] = it_mu->ecalIso();
            LepInfo[icoll].HcalIso    [LepInfo[icoll].Size] = it_mu->hcalIso();

            LepInfo[icoll].Px         [LepInfo[icoll].Size] = it_mu->px(); //Uly 2011-04-04
            LepInfo[icoll].Py         [LepInfo[icoll].Size] = it_mu->py(); //Uly 2011-04-04
            LepInfo[icoll].Pz         [LepInfo[icoll].Size] = it_mu->pz(); //Uly 2011-04-04
            LepInfo[icoll].Energy     [LepInfo[icoll].Size] = it_mu->energy(); //Uly 2011-04-04

            // For Soft Muon selection (https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Soft_Muon)
            if( muon::isGoodMuon( *it_mu, muon::TMOneStationTight ) ) {
               LepInfo[icoll].isGoodMuonTMOneStationTight    [LepInfo[icoll].Size] = true;
            } else {
               LepInfo[icoll].isGoodMuonTMOneStationTight    [LepInfo[icoll].Size] = false;
            }

            //std::cout<<"Muon IP : "<<fabs(it_mu->dB(pat::Muon::PV3D))<<std::endl;
            //std::cout<<"Muon IPError : "<<fabs(it_mu->edB(pat::Muon::PV3D))<<std::endl;
            //std::cout<<"Muon Ip3d : "<<fabs(it_mu->dB(pat::Muon::PV3D))/fabs(it_mu->edB(pat::Muon::PV3D))<<std::endl;
            // Initialize these three variables in case the muon has no track.
            LepInfo[icoll].Ip3dPV[LepInfo[icoll].Size]             = -10000;
            LepInfo[icoll].Ip3dPVErr[LepInfo[icoll].Size]          = -10000;
            LepInfo[icoll].Ip3dPVSignificance[LepInfo[icoll].Size] = -10000;

            //PFIso
            LepInfo[icoll].isPFMuon          [LepInfo[icoll].Size] = it_mu->isPFMuon();
            LepInfo[icoll].ChargedHadronIso  [LepInfo[icoll].Size] = it_mu->chargedHadronIso();
            LepInfo[icoll].NeutralHadronIso  [LepInfo[icoll].Size] = it_mu->neutralHadronIso();
            LepInfo[icoll].PhotonIso         [LepInfo[icoll].Size] = it_mu->photonIso();

            /*
               There are two ways to correct PFIsol by using DeltaBeta or rho correction.
               1). DeltaBeta : I = [sumChargedHadronPt+ max(0.,sumNeutralHadronPt+sumPhotonPt-0.5sumPUPt]/pt
               2). Rho correction in page9 of
                   https://indico.cern.ch/getFile.py/access?contribId=1&resId=0&materialId=slides&confId=188494
              Effective area :
                      http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/sixie/Muon/MuonAnalysisTools/interface/MuonEffectiveArea.h?revision=1.7&view=markup
               */
            if ( it_mu->isPFMuon() && it_mu->isPFIsolationValid() ) {
               LepInfo[icoll].ChargedHadronIsoR03  [LepInfo[icoll].Size] = it_mu->pfIsolationR03().sumChargedHadronPt;
               LepInfo[icoll].NeutralHadronIsoR03  [LepInfo[icoll].Size] = it_mu->pfIsolationR03().sumNeutralHadronEt;
               LepInfo[icoll].PhotonIsoR03         [LepInfo[icoll].Size] = it_mu->pfIsolationR03().sumPhotonEt;
               LepInfo[icoll].sumPUPtR03           [LepInfo[icoll].Size] = it_mu->pfIsolationR03().sumPUPt;
               MuonEffectiveArea::MuonEffectiveAreaTarget EATarget = MuonEffectiveArea::kMuEAFall11MC;
               if( isData ) { EATarget = MuonEffectiveArea::kMuEAData2012; }
               float AEffR03 = MuonEffectiveArea::GetMuonEffectiveArea( MuonEffectiveArea::kMuGammaAndNeutralHadronIso03, LepInfo[icoll].Eta[LepInfo[icoll].Size], EATarget );
               double rhoPrime = 0;

               if( !TurnOffInCMSSW73x )
               rhoPrime = max( ( double ) * ( rhoH[1].product() ), 0.0 );
               
               LepInfo[icoll].IsoRhoCorrR03             [LepInfo[icoll].Size] =
                  LepInfo[icoll].ChargedHadronIsoR03[LepInfo[icoll].Size] +
                  max( LepInfo[icoll].NeutralHadronIsoR03[LepInfo[icoll].Size]
                       + LepInfo[icoll].PhotonIsoR03[LepInfo[icoll].Size] - rhoPrime * AEffR03, 0.0 );
               LepInfo[icoll].ChargedHadronIsoR04  [LepInfo[icoll].Size] = it_mu->pfIsolationR04().sumChargedHadronPt;
               LepInfo[icoll].NeutralHadronIsoR04  [LepInfo[icoll].Size] = it_mu->pfIsolationR04().sumNeutralHadronEt;
               LepInfo[icoll].PhotonIsoR04         [LepInfo[icoll].Size] = it_mu->pfIsolationR04().sumPhotonEt;
               LepInfo[icoll].sumPUPtR04           [LepInfo[icoll].Size] = it_mu->pfIsolationR04().sumPUPt;
               float AEffR04 = MuonEffectiveArea::GetMuonEffectiveArea( MuonEffectiveArea::kMuGammaAndNeutralHadronIso04, LepInfo[icoll].Eta[LepInfo[icoll].Size], EATarget );
               LepInfo[icoll].IsoRhoCorrR04             [LepInfo[icoll].Size] =
                  LepInfo[icoll].ChargedHadronIsoR04[LepInfo[icoll].Size] +
                  max( LepInfo[icoll].NeutralHadronIsoR04[LepInfo[icoll].Size]
                       + LepInfo[icoll].PhotonIsoR04[LepInfo[icoll].Size] - rhoPrime * AEffR04, 0.0 );
            }

            // Timing information for distinguishing cosmic ray and prompt muon
            if( it_mu->isTimeValid() ) {
               // number of muon stations used
               LepInfo[icoll].MuontimenDof             [LepInfo[icoll].Size] = it_mu->time().nDof;
               // time of arrival at the IP for the Beta=1 hypothesis
               // a) particle is moving from inside out
               LepInfo[icoll].MuontimeAtIpInOut        [LepInfo[icoll].Size] = it_mu->time().timeAtIpInOut;
               // b) particle is moving from outside in
               LepInfo[icoll].MuontimeAtIpOutIn        [LepInfo[icoll].Size] = it_mu->time().timeAtIpOutIn;
               // enum Direction { OutsideIn = -1, Undefined = 0, InsideOut = 1 };
               LepInfo[icoll].Muondirection            [LepInfo[icoll].Size] = it_mu->time().direction();
            }

            reco::MuonEnergy muEnergy = it_mu->calEnergy();
            LepInfo[icoll].CaloEnergy [LepInfo[icoll].Size] = muEnergy.em + muEnergy.had + muEnergy.ho;

            LepInfo[icoll].MuIDGlobalMuonPromptTight         [LepInfo[icoll].Size] = it_mu->muonID( "GlobalMuonPromptTight" );

            // InnerTrack() is only valid for GlobalMuon and TrackerMuon
            // See LepInfo[icoll].MuType for it_mu->type() bits
            if ( ( it_mu->type() & 0x02 ) || ( it_mu->type() & 0x04 ) ) {
               LepInfo[icoll].innerTracknormalizedChi2 [LepInfo[icoll].Size] = it_mu->innerTrack()->normalizedChi2();
               LepInfo[icoll].MuInnerPtError         [LepInfo[icoll].Size] = it_mu->innerTrack()->ptError();
               LepInfo[icoll].MuInnerTrackDz         [LepInfo[icoll].Size] = it_mu->innerTrack()->dz( PrimVtx.position() );
               LepInfo[icoll].MuInnerTrackD0         [LepInfo[icoll].Size] = it_mu->innerTrack()->d0();
               LepInfo[icoll].MuInnerTrackDxy_BS     [LepInfo[icoll].Size] = it_mu->innerTrack()->dxy( beamSpot.position() );
               LepInfo[icoll].MuInnerTrackDxy_PV     [LepInfo[icoll].Size] = it_mu->innerTrack()->dxy( PrimVtx.position() );
               LepInfo[icoll].MuInnerTrackDxy_PVBS   [LepInfo[icoll].Size] = it_mu->innerTrack()->dxy( PrimVtx_BS.position() );
               LepInfo[icoll].MuInnerTrackNHits      [LepInfo[icoll].Size] = it_mu->innerTrack()->numberOfValidHits();
               LepInfo[icoll].MuNTrackerHits         [LepInfo[icoll].Size] = it_mu->innerTrack()->hitPattern().numberOfValidTrackerHits();
               LepInfo[icoll].MuNPixelLayers         [LepInfo[icoll].Size] = it_mu->innerTrack()->hitPattern().numberOfValidPixelHits();
               //LepInfo[icoll].MuNLostInnerHits       [LepInfo[icoll].Size] = it_mu->innerTrack()->hitPattern().numberOfLostHits();
               // not valid (https://cmssdt.cern.ch/SDT/lxr/source//DataFormats/TrackReco/interface/HitPattern.h)
               LepInfo[icoll].MuNLostInnerHits       [LepInfo[icoll].Size] = -1;
               LepInfo[icoll].vertexZ                [LepInfo[icoll].Size] = it_mu->vertex().z(); //Uly 2011-04-04
               LepInfo[icoll].MuNPixelLayersWMeasurement[LepInfo[icoll].Size] = it_mu->innerTrack()->hitPattern().pixelLayersWithMeasurement(); //Uly 2011-04-04
               LepInfo[icoll].MuNTrackLayersWMeasurement[LepInfo[icoll].Size] = it_mu->innerTrack()->hitPattern().trackerLayersWithMeasurement();

               // Reference from UserCode/MitProd/TreeFiller/src/FillerMuons.cc
               if( TurnOnInCMSSW_7_4_1 ) {
               //   const reco::TransientTrack& tt_mu = transientTrackBuilder->build( it_mu->track() );
               //   reco::Vertex thevtx = pvCol->at( 0 );
               //   const std::pair<bool, Measurement1D>& ip3dpv =  IPTools::absoluteImpactParameter3D( tt_mu, thevtx );
               //   const double thesign   = ( ( -it_mu->track()->dxy( thevtx.position() ) )   >= 0 ) ? 1. : -1.;
               //   //std::cout<<"Muon Ip3dPVSignificance : "<<thesign*ip3dpv.second.value()/ip3dpv.second.error() <<std::endl;
               //   LepInfo[icoll].Ip3dPV[LepInfo[icoll].Size] = thesign * ip3dpv.second.value();
               //   LepInfo[icoll].Ip3dPVErr[LepInfo[icoll].Size] = ip3dpv.second.error();
               //   LepInfo[icoll].Ip3dPVSignificance[LepInfo[icoll].Size] = thesign * ip3dpv.second.value() / ip3dpv.second.error();
               }
            }

            if ( it_mu->type() & 0x02 ) {
               LepInfo[icoll].MuGlobalPtError        [LepInfo[icoll].Size] = it_mu->globalTrack()->ptError();
               LepInfo[icoll].MuGlobalNormalizedChi2 [LepInfo[icoll].Size] = it_mu->globalTrack()->normalizedChi2();
               LepInfo[icoll].MuNMuonhits            [LepInfo[icoll].Size] = it_mu->globalTrack()->hitPattern().numberOfValidMuonHits();
               LepInfo[icoll].MuDThits               [LepInfo[icoll].Size] = it_mu->globalTrack()->hitPattern().numberOfValidMuonDTHits();
               LepInfo[icoll].MuCSChits              [LepInfo[icoll].Size] = it_mu->globalTrack()->hitPattern().numberOfValidMuonCSCHits();
               LepInfo[icoll].MuRPChits              [LepInfo[icoll].Size] = it_mu->globalTrack()->hitPattern().numberOfValidMuonRPCHits();
            }
            if ( ( it_mu->type() & 0x02 ) || ( it_mu->type() & 0x08 ) ) {
               //LepInfo[icoll].MuNLostOuterHits       [LepInfo[icoll].Size] = it_mu->outerTrack()->hitPattern().numberOfLostHits();
               // not valid (https://cmssdt.cern.ch/SDT/lxr/source//DataFormats/TrackReco/interface/HitPattern.h)
               LepInfo[icoll].MuNLostOuterHits       [LepInfo[icoll].Size] = -1;
            }

            LepInfo[icoll].MuCaloCompat              [LepInfo[icoll].Size] = it_mu->caloCompatibility();
            LepInfo[icoll].MuNChambers               [LepInfo[icoll].Size] = it_mu->numberOfChambers();
            LepInfo[icoll].MuNChambersMatchesSegment [LepInfo[icoll].Size] = it_mu->numberOfMatches();  // At least 2 Chambers matched with segments
            LepInfo[icoll].MuNMatchedStations        [LepInfo[icoll].Size] = it_mu->numberOfMatchedStations();

            // Muon type bits:
            // static const unsigned int GlobalMuon     =  1<<1 (0x02);
            // static const unsigned int TrackerMuon    =  1<<2 (0x04);
            // static const unsigned int StandAloneMuon =  1<<3 (0x08);
            // static const unsigned int CaloMuon       =  1<<4 (0x10);
            // static const unsigned int PFMuon         =  1<<5 (0x20);
            LepInfo[icoll].MuType[LepInfo[icoll].Size] = it_mu->type();

            if ( !isData && !skipGenInfo_ ) {
               const reco::GenParticle* gen = it_mu->genLepton();
               if ( gen != NULL ) {
                  LepInfo[icoll].GenPt    [LepInfo[icoll].Size] = gen->pt();
                  LepInfo[icoll].GenEta   [LepInfo[icoll].Size] = gen->eta();
                  LepInfo[icoll].GenPhi   [LepInfo[icoll].Size] = gen->phi();
                  LepInfo[icoll].GenPdgID [LepInfo[icoll].Size] = gen->pdgId();

                  const reco::Candidate* genCand = gen;

                  int bprime_tag = 0; // 0: not b' or t'; 1: b'; 2:t'
                  while( genCand != NULL && genCand->numberOfMothers() == 1 ) {
                     genCand = genCand->mother( 0 );

                     if ( abs( genCand->pdgId() ) == 7 ) { bprime_tag = 1; } // check if it's bprime
                     if ( abs( genCand->pdgId() ) == 8 ) { bprime_tag = 2; } // check if it's tprime.

                     if ( LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] == 0  ) {
                        if     ( abs( genCand->pdgId() )                    == 23 ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 2; }
                        else if( abs( genCand->pdgId() )                    == 24 ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 1; }
                        else if( abs( genCand->pdgId() )                    == 5  ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 3; } // from a b quark
                        else if( ( abs( genCand->pdgId() ) % 1000 ) / 100   == 5  ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 3; } // from a B meson
                        else if( ( abs( genCand->pdgId() ) % 10000 ) / 1000 == 5  ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 3; } // from a bottom baryon
                        else if( abs( genCand->pdgId() )                    == 4  ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 4; } // from a c quark
                        else if( ( abs( genCand->pdgId() ) % 1000 ) / 100   == 4  ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 4; } // from a D meson
                        else if( ( abs( genCand->pdgId() ) % 10000 ) / 1000 == 4  ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 4; } // from a charm baryon
                        else if( abs( genCand->pdgId() )                    == 15 ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 7; } // from tau
                     }
                  }
                  if ( bprime_tag == 1 ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] += 10; }
                  if ( bprime_tag == 2 ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] += 20; }
               }
            }

            if ( LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] == 0 && !isData && !skipGenInfo_ ) {
               for( std::vector<reco::GenParticle>::const_iterator it_gen = GenHandle->begin();
                    it_gen != GenHandle->end(); it_gen++ ) {
                  double r = deltaR<double>( it_gen->eta(), it_gen->phi(), it_mu->eta(), it_mu->phi() );

                  if ( ( abs( it_gen->pdgId() ) <= 5 || abs( it_gen->pdgId() ) == 21 ) && it_gen->status() == 3 &&
                       r < 0.5 && fabs( it_gen->pt() - it_mu->pt() ) / it_gen->pt() < 0.5 ) {
                     LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 5; // matched to a parton (q,g)
                     break;
                  }
                  if ( abs( it_gen->pdgId() ) == 22 && it_gen->status() == 1 &&
                       r < 0.5 && fabs( it_gen->pt() - it_mu->pt() ) / it_gen->pt() < 0.5 ) {
                     LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 6; // matched to a photon
                     break;
                  }
               }
            }

            LepInfo[icoll].CandRef [LepInfo[icoll].Size] = ( reco::Candidate* ) & ( *it_mu );
            LepInfo[icoll].Size++;
         }//loop over muons in collection
      }//have muon collection

      //Electrons
      if( ElectronHandle.size() > icoll ) { //have electron collection
         if( debug_ > 10 ) { cout << " Electron collection size " << ElectronHandle[icoll]->size() << endl; }
         for( std::vector<pat::Electron>::const_iterator it_el = ElectronHandle[icoll]->begin();
              it_el != ElectronHandle[icoll]->end(); it_el++ ) {//loop over electrons in collection

            if( debug_ > 11 ) cout << "  Size " << LepInfo[icoll].Size << " el et,eta,phi " <<
                                      it_el->et() << "," << it_el->superCluster()->eta() << "," << it_el->superCluster()->phi() << endl;
            if ( LepInfo[icoll].Size >= MAX_LEPTONS ) {
               fprintf( stderr, "ERROR: number of leptons exceeds the size of array.\n" );
               break;//exit(0);
            }

            LepInfo[icoll].Index           [LepInfo[icoll].Size] = LepInfo[icoll].Size;
            LepInfo[icoll].isEcalDriven    [LepInfo[icoll].Size] = it_el->ecalDrivenSeed();
            LepInfo[icoll].isTrackerDriven [LepInfo[icoll].Size] = it_el->trackerDrivenSeed();
            LepInfo[icoll].LeptonType      [LepInfo[icoll].Size] = 11;
            LepInfo[icoll].Charge          [LepInfo[icoll].Size] = it_el->charge();
            LepInfo[icoll].ChargeGsf       [LepInfo[icoll].Size] = it_el->gsfTrack()->charge();

            // turn off on CMSSW73X
            if( !TurnOffInCMSSW73x )
               if( tracks_h.isValid() && it_el->closestCtfTrackRef().isNonnull() ) //tracks_h may not be exactly right, but should work?
               { LepInfo[icoll].ChargeCtf       [LepInfo[icoll].Size] = it_el->closestCtfTrackRef()->charge(); }
            LepInfo[icoll].ChargeScPix     [LepInfo[icoll].Size] = it_el->scPixCharge();
            LepInfo[icoll].Pt              [LepInfo[icoll].Size] = it_el->pt();
            LepInfo[icoll].Et              [LepInfo[icoll].Size] = it_el->et(); //Uly 2011-04-04
            LepInfo[icoll].Eta             [LepInfo[icoll].Size] = it_el->superCluster()->eta(); //Uly 2011-04-04
            LepInfo[icoll].caloEta         [LepInfo[icoll].Size] = it_el->caloPosition().eta();
            LepInfo[icoll].Phi             [LepInfo[icoll].Size] = it_el->superCluster()->phi(); //Uly 2011-04-04

            LepInfo[icoll].Px         [LepInfo[icoll].Size] = it_el->px(); //Uly 2011-04-04
            LepInfo[icoll].Py         [LepInfo[icoll].Size] = it_el->py(); //Uly 2011-04-04
            LepInfo[icoll].Pz         [LepInfo[icoll].Size] = it_el->pz(); //Uly 2011-04-04
            LepInfo[icoll].Energy     [LepInfo[icoll].Size] = it_el->energy(); //Uly 2011-04-04

            LepInfo[icoll].CaloEnergy [LepInfo[icoll].Size] = it_el->caloEnergy();
            LepInfo[icoll].e1x5       [LepInfo[icoll].Size] = it_el->scE1x5();
            LepInfo[icoll].e2x5Max    [LepInfo[icoll].Size] = it_el->scE2x5Max();
            LepInfo[icoll].e5x5       [LepInfo[icoll].Size] = it_el->scE5x5();

            LepInfo[icoll].TrackIso        [LepInfo[icoll].Size] = it_el->dr03TkSumPt();
            LepInfo[icoll].EcalIso         [LepInfo[icoll].Size] = it_el->dr03EcalRecHitSumEt();
            LepInfo[icoll].HcalIso         [LepInfo[icoll].Size] = it_el->dr03HcalTowerSumEt();
            LepInfo[icoll].HcalDepth1Iso   [LepInfo[icoll].Size] = it_el->dr03HcalDepth1TowerSumEt();
            LepInfo[icoll].HcalDepth2Iso   [LepInfo[icoll].Size] = it_el->dr03HcalDepth2TowerSumEt();

            //PFIso
            //LepInfo[icoll].ChargedHadronIso  [LepInfo[icoll].Size] = it_el->chargedHadronIso();
            //LepInfo[icoll].NeutralHadronIso  [LepInfo[icoll].Size] = it_el->neutralHadronIso();
            //LepInfo[icoll].PhotonIso         [LepInfo[icoll].Size] = it_el->photonIso();

            if( getElectronID_ ) {
               // Add 2012 EID (https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification)
               unsigned int nGsfEle = 0;
               if( !TurnOffInCMSSW73x )
                  for( reco::GsfElectronCollection::const_iterator gsfEle = els_h->begin();
                       gsfEle != els_h->end(); ++gsfEle ) {

                     if ( gsfEle->gsfTrack()->pt() == it_el->gsfTrack()->pt() ) {

                        reco::GsfElectronRef ele( els_h, nGsfEle );

                        // MVA-ID  -- start
                        const GsfElectronCollection theEGamma = *( els_h.product() );
                        Vertex dummy;
                        if( TurnOffInCMSSW73x ) {
                        //const Vertex* pv = &dummy;
                        }
                        if ( VertexHandle->size() != 0 ) {
                        //   pv = &*VertexHandle->begin();
                        } else { // create a dummy PV
                           Vertex::Error e;
                           e( 0, 0 ) = 0.0015 * 0.0015;
                           e( 1, 1 ) = 0.0015 * 0.0015;
                           e( 2, 2 ) = 15. * 15.;
                           Vertex::Point p( 0, 0, 0 );
                           dummy = Vertex( p, e, 0, 0, 0 );
                        }

                        //bool debugMVAclass = false;
                        if ( TurnOnInCMSSW_7_4_1 ) {
                        //   float myMVANonTrigMethod =
                        //      myMVANonTrig->mvaValue( ( theEGamma[nGsfEle] ), *pv, thebuilder, lazyTools, debugMVAclass );
                        //   float myMVATrigMethod =
                        //      myMVATrig->mvaValue( ( theEGamma[nGsfEle] ), *pv, thebuilder, lazyTools, debugMVAclass );
                        //   LepInfo[icoll].EgammaMVANonTrig   [LepInfo[icoll].Size] = myMVANonTrigMethod;
                        //   LepInfo[icoll].EgammaMVATrig      [LepInfo[icoll].Size] = myMVATrigMethod;
                        }
                        // MVA-ID  -- end

                        // alternative way to access to PF iso
                        unsigned int ivtx = 0;
                        VertexRef myVtxRef( VertexHandle, ivtx );

                        if( !TurnOffInCMSSW73x ) {
                           isolatorR03.fGetIsolation( &*gsfEle, &thePfColl, myVtxRef, VertexHandle );
                           isolatorR04.fGetIsolation( &*gsfEle, &thePfColl, myVtxRef, VertexHandle );
                        }

                        // get particle flow isolation
                        // For CMSSW_5_3_11
                        //double iso_ch = it_el->pfIsolationVariables().chargedHadronIso;
                        //double iso_em = it_el->pfIsolationVariables().photonIso;
                        //double iso_nh = it_el->pfIsolationVariables().neutralHadronIso;
                        // For CMSSW_7_2_0
                        // Reference to https://cmssdt.cern.ch/SDT/lxr/source//DataFormats/EgammaCandidates/interface/GsfElectron.h
                        double iso_ch = it_el->pfIsolationVariables().sumChargedHadronPt;
                        double iso_em = it_el->pfIsolationVariables().sumPhotonEt;
                        double iso_nh = it_el->pfIsolationVariables().sumNeutralHadronEt;

                        ElectronEffectiveArea::ElectronEffectiveAreaTarget EATarget = ElectronEffectiveArea::kEleEAFall11MC;
                        if( isData ) { EATarget = ElectronEffectiveArea::kEleEAData2012; }

                        // working points
                        // update for CMSSW_7_2_0 (https://cmssdt.cern.ch/SDT/lxr/source//EgammaAnalysis/ElectronTools/src/EGammaCutBasedEleId.cc)
                        bool veto       = EgammaCutBasedEleId::PassWP( EgammaCutBasedEleId::VETO, ele, conversions_h, beamSpot, VertexHandle, iso_ch, iso_em, iso_nh, rhoIso, EATarget );
                        bool loose      = EgammaCutBasedEleId::PassWP( EgammaCutBasedEleId::LOOSE, ele, conversions_h, beamSpot, VertexHandle, iso_ch, iso_em, iso_nh, rhoIso, EATarget );
                        bool medium     = EgammaCutBasedEleId::PassWP( EgammaCutBasedEleId::MEDIUM, ele, conversions_h, beamSpot, VertexHandle, iso_ch, iso_em, iso_nh, rhoIso, EATarget );
                        bool tight      = EgammaCutBasedEleId::PassWP( EgammaCutBasedEleId::TIGHT, ele, conversions_h, beamSpot, VertexHandle, iso_ch, iso_em, iso_nh, rhoIso, EATarget );

                        LepInfo[icoll].EgammaCutBasedEleIdVETO   [LepInfo[icoll].Size] = veto;
                        LepInfo[icoll].EgammaCutBasedEleIdLOOSE  [LepInfo[icoll].Size] = loose;
                        LepInfo[icoll].EgammaCutBasedEleIdMEDIUM [LepInfo[icoll].Size] = medium;
                        LepInfo[icoll].EgammaCutBasedEleIdTIGHT  [LepInfo[icoll].Size] = tight;

                        // cuts to match tight trigger requirements
                        bool trigtight = EgammaCutBasedEleId::PassTriggerCuts( EgammaCutBasedEleId::TRIGGERTIGHT, ele );
                        LepInfo[icoll].EgammaCutBasedEleIdTRIGGERTIGHT  [LepInfo[icoll].Size] = trigtight;

                        // for 2011 WP70 trigger
                        bool trigwp70 = EgammaCutBasedEleId::PassTriggerCuts( EgammaCutBasedEleId::TRIGGERWP70, ele );
                        LepInfo[icoll].EgammaCutBasedEleIdTRIGGERWP70 [LepInfo[icoll].Size] = trigwp70;

                        //PFIso
                        /*
                           iso_correct = iso_ch + max(iso_nh + iso_em - rhoPrime * AEff, 0.0) .
                           Effective area :
                               http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h?revision=1.3&view=markup
                           */
                        LepInfo[icoll].ChargedHadronIso          [LepInfo[icoll].Size] = iso_ch;
                        LepInfo[icoll].NeutralHadronIso          [LepInfo[icoll].Size] = iso_nh;
                        LepInfo[icoll].PhotonIso                 [LepInfo[icoll].Size] = iso_em;
                        LepInfo[icoll].ChargedHadronIsoR03       [LepInfo[icoll].Size] = isolatorR03.getIsolationCharged();
                        LepInfo[icoll].NeutralHadronIsoR03       [LepInfo[icoll].Size] = isolatorR03.getIsolationNeutral();
                        LepInfo[icoll].PhotonIsoR03              [LepInfo[icoll].Size] = isolatorR03.getIsolationPhoton();
                        LepInfo[icoll].sumPUPtR03                [LepInfo[icoll].Size] = isolatorR03.getIsolationChargedAll();
                        float AEffR03 = ElectronEffectiveArea::GetElectronEffectiveArea( ElectronEffectiveArea::kEleGammaAndNeutralHadronIso03, LepInfo[icoll].Eta[LepInfo[icoll].Size], EATarget );
                        double rhoPrime = 0.;
                        if( !TurnOffInCMSSW73x )
                        rhoPrime = max( ( double ) * ( rhoH[0].product() ), 0.0 ); 
                        
                        LepInfo[icoll].IsoRhoCorrR03             [LepInfo[icoll].Size] =
                           LepInfo[icoll].ChargedHadronIsoR03[LepInfo[icoll].Size] +
                           max( LepInfo[icoll].NeutralHadronIsoR03[LepInfo[icoll].Size]
                                + LepInfo[icoll].PhotonIsoR03[LepInfo[icoll].Size] - rhoPrime * AEffR03, 0.0 );

                        LepInfo[icoll].ChargedHadronIsoR04       [LepInfo[icoll].Size] = isolatorR04.getIsolationCharged();
                        LepInfo[icoll].NeutralHadronIsoR04       [LepInfo[icoll].Size] = isolatorR04.getIsolationNeutral();
                        LepInfo[icoll].PhotonIsoR04              [LepInfo[icoll].Size] = isolatorR04.getIsolationPhoton();
                        LepInfo[icoll].sumPUPtR04                [LepInfo[icoll].Size] = isolatorR04.getIsolationChargedAll();

                        float AEffR04 = ElectronEffectiveArea::GetElectronEffectiveArea( ElectronEffectiveArea::kEleGammaAndNeutralHadronIso04, LepInfo[icoll].Eta[LepInfo[icoll].Size], EATarget );
                        LepInfo[icoll].IsoRhoCorrR04             [LepInfo[icoll].Size] =
                           LepInfo[icoll].ChargedHadronIsoR04[LepInfo[icoll].Size] +
                           max( LepInfo[icoll].NeutralHadronIsoR04[LepInfo[icoll].Size]
                                + LepInfo[icoll].PhotonIsoR04[LepInfo[icoll].Size] - rhoPrime * AEffR04, 0.0 );

                        LepInfo[icoll].ElEcalE                   [LepInfo[icoll].Size] = ele->ecalEnergy();
                        LepInfo[icoll].ElhasConv                 [LepInfo[icoll].Size]
                           = ConversionTools::hasMatchedConversion( *ele, conversions_h, beamSpot.position() );
                        /*
                           hOverE2012 = (hcalDepth1 + hcalDepth2)/ele->superCluster()->energy();
                           */
                        LepInfo[icoll].ElhcalOverEcalBc          [LepInfo[icoll].Size] = ele->hcalOverEcalBc();
                        LepInfo[icoll].Eldr03HcalDepth1TowerSumEtBc[LepInfo[icoll].Size] = ele->dr03HcalDepth1TowerSumEtBc();
                        LepInfo[icoll].Eldr03HcalDepth2TowerSumEtBc[LepInfo[icoll].Size] = ele->dr03HcalDepth2TowerSumEtBc();
                        LepInfo[icoll].Eldr04HcalDepth1TowerSumEtBc[LepInfo[icoll].Size] = ele->dr04HcalDepth1TowerSumEtBc();
                        LepInfo[icoll].Eldr04HcalDepth2TowerSumEtBc[LepInfo[icoll].Size] = ele->dr04HcalDepth2TowerSumEtBc();
                     }
                     nGsfEle ++;

                  }



               //simpleEleId status : (Add by Jacky)
               //0: fails
               //1: passes electron ID only
               //2: passes electron Isolation only
               //3: passes electron ID and Isolation only
               //4: passes conversion rejection
               //5: passes conversion rejection and ID
               //6: passes conversion rejection and Isolation
               //7: passes the whole selection
               //                 LepInfo[icoll].simpleEleId95relIso   [LepInfo[icoll].Size] = it_el->electronID("simpleEleId95relIso");
               //                 LepInfo[icoll].simpleEleId90relIso   [LepInfo[icoll].Size] = it_el->electronID("simpleEleId90relIso");
               //                 LepInfo[icoll].simpleEleId85relIso   [LepInfo[icoll].Size] = it_el->electronID("simpleEleId85relIso");
               //                 LepInfo[icoll].simpleEleId80relIso   [LepInfo[icoll].Size] = it_el->electronID("simpleEleId80relIso");
               //                 LepInfo[icoll].simpleEleId70relIso   [LepInfo[icoll].Size] = it_el->electronID("simpleEleId70relIso");
               //                 LepInfo[icoll].simpleEleId60relIso   [LepInfo[icoll].Size] = it_el->electronID("simpleEleId60relIso");
               //                 LepInfo[icoll].simpleEleId95cIso     [LepInfo[icoll].Size] = it_el->electronID("simpleEleId95cIso");
               //                 LepInfo[icoll].simpleEleId90cIso     [LepInfo[icoll].Size] = it_el->electronID("simpleEleId90cIso");
               //                 LepInfo[icoll].simpleEleId85cIso     [LepInfo[icoll].Size] = it_el->electronID("simpleEleId85cIso");
               //                 LepInfo[icoll].simpleEleId80cIso     [LepInfo[icoll].Size] = it_el->electronID("simpleEleId80cIso");
               //                 LepInfo[icoll].simpleEleId70cIso     [LepInfo[icoll].Size] = it_el->electronID("simpleEleId70cIso");
               //                 LepInfo[icoll].simpleEleId60cIso     [LepInfo[icoll].Size] = it_el->electronID("simpleEleId60cIso");

               /*
                  EID -- CIC
                  0 - no cut passed
                  1 - eID cuts passed
                  2 - iso cuts passed
                  4 - conversion rejection
                  8 - ip cut
                  Below three examples to check eId results:
                  eID+Iso+ConversionRejection+IP -> ((eIDmap[electronRef] &15) == 15)
                  Iso only -> ((eIDmap[electronRef] & 2) == 2)
                  eID+ConversionRejection+IP -> ((eIDmap[electronRef] & 13) == 13)
                */
               //CIC without ISO
               //                 LepInfo[icoll].eidVeryLoose[LepInfo[icoll].Size] = it_el->electronID("eidVeryLoose");
               //                 LepInfo[icoll].eidLoose[LepInfo[icoll].Size] = it_el->electronID("eidLoose");
               //                 LepInfo[icoll].eidMedium[LepInfo[icoll].Size] = it_el->electronID("eidMedium");
               //                 LepInfo[icoll].eidTight[LepInfo[icoll].Size] = it_el->electronID("eidTight");
               //                 LepInfo[icoll].eidSuperTight[LepInfo[icoll].Size] = it_el->electronID("eidSuperTight");
               //                 LepInfo[icoll].eidHyperTight1[LepInfo[icoll].Size] = it_el->electronID("eidHyperTight1");
               //                 LepInfo[icoll].eidHyperTight2[LepInfo[icoll].Size] = it_el->electronID("eidHyperTight2");
               //                 LepInfo[icoll].eidHyperTight3[LepInfo[icoll].Size] = it_el->electronID("eidHyperTight3");
               //                 LepInfo[icoll].eidHyperTight4[LepInfo[icoll].Size] = it_el->electronID("eidHyperTight4");

               // CIC with ISO
               //                 LepInfo[icoll].eidVeryLooseMC[LepInfo[icoll].Size] = it_el->electronID("eidVeryLooseMC");
               //                 LepInfo[icoll].eidLooseMC[LepInfo[icoll].Size] = it_el->electronID("eidLooseMC");
               //                 LepInfo[icoll].eidMediumMC[LepInfo[icoll].Size] = it_el->electronID("eidMediumMC");
               //                 LepInfo[icoll].eidTightMC[LepInfo[icoll].Size] = it_el->electronID("eidTightMC");
               //                 LepInfo[icoll].eidSuperTightMC[LepInfo[icoll].Size] = it_el->electronID("eidSuperTightMC");
               //                 LepInfo[icoll].eidHyperTight1MC[LepInfo[icoll].Size] = it_el->electronID("eidHyperTight1MC");
               //                 LepInfo[icoll].eidHyperTight2MC[LepInfo[icoll].Size] = it_el->electronID("eidHyperTight2MC");
               //                 LepInfo[icoll].eidHyperTight3MC[LepInfo[icoll].Size] = it_el->electronID("eidHyperTight3MC");
               //                 LepInfo[icoll].eidHyperTight4MC[LepInfo[icoll].Size] = it_el->electronID("eidHyperTight4MC");
            }

            LepInfo[icoll].ElEoverP              [LepInfo[icoll].Size] = it_el->eSuperClusterOverP();
            LepInfo[icoll].EldeltaEta            [LepInfo[icoll].Size] = it_el->deltaEtaSuperClusterTrackAtVtx();
            LepInfo[icoll].EldeltaPhi            [LepInfo[icoll].Size] = it_el->deltaPhiSuperClusterTrackAtVtx();
            LepInfo[icoll].ElHadoverEm           [LepInfo[icoll].Size] = it_el->hadronicOverEm();
            LepInfo[icoll].ElsigmaIetaIeta       [LepInfo[icoll].Size] = it_el->sigmaIetaIeta();
            LepInfo[icoll].ElscSigmaIetaIeta     [LepInfo[icoll].Size] = it_el->scSigmaIEtaIEta();
            LepInfo[icoll].ElEnergyErr           [LepInfo[icoll].Size] = it_el->ecalEnergyError();
            LepInfo[icoll].ElMomentumErr         [LepInfo[icoll].Size] = it_el->trackMomentumError();
            //      LepInfo[icoll].ElTrackNHits          [LepInfo[icoll].Size] = it_el->gsfTrack()->recHitsSize();   //CANNOT RUN THIS LINE WITH AODSIM
            //LepInfo[icoll].ElTrackNLostHits      [LepInfo[icoll].Size] = it_el->gsfTrack()->trackerExpectedHitsInner().numberOfLostHits();
            // not valid (https://cmssdt.cern.ch/SDT/lxr/source//DataFormats/TrackReco/interface/HitPattern.h)
            LepInfo[icoll].ElTrackNLostHits      [LepInfo[icoll].Size] = -1;
            LepInfo[icoll].ElTrackDz             [LepInfo[icoll].Size] = it_el->gsfTrack()->dz( PrimVtx.position() );
            LepInfo[icoll].ElTrackDz_BS          [LepInfo[icoll].Size] = it_el->gsfTrack()->dz( beamSpot.position() );
            LepInfo[icoll].ElTrackD0             [LepInfo[icoll].Size] = it_el->gsfTrack()->d0();
            LepInfo[icoll].ElTrackDxy_BS         [LepInfo[icoll].Size] = it_el->gsfTrack()->dxy( beamSpot.position() );
            LepInfo[icoll].ElTrackDxy_PV         [LepInfo[icoll].Size] = it_el->gsfTrack()->dxy( PrimVtx.position() );
            LepInfo[icoll].ElTrackDxy_PVBS       [LepInfo[icoll].Size] = it_el->gsfTrack()->dxy( PrimVtx_BS.position() );

            // Reference from UserCode/MitProd/TreeFiller/src/FillerElectrons.cc
            if( TurnOnInCMSSW_7_4_1 ) {
            //   const reco::TransientTrack& tt = transientTrackBuilder->build( it_el->gsfTrack() );
            //   reco::Vertex thevtx = pvCol->at( 0 );
            //   const std::pair<bool, Measurement1D>& ip3dpv =  IPTools::absoluteImpactParameter3D( tt, thevtx );
            //   const double gsfsign   = ( ( -it_el->gsfTrack()->dxy( thevtx.position() ) )   >= 0 ) ? 1. : -1.;
            //   //std::cout<<"Electron Ip3dPVSignificance : "<<gsfsign*ip3dpv.second.value()/ip3dpv.second.error() <<std::endl;
            //   LepInfo[icoll].Ip3dPV[LepInfo[icoll].Size] = gsfsign * ip3dpv.second.value();
            //   LepInfo[icoll].Ip3dPVErr[LepInfo[icoll].Size] = ip3dpv.second.error();
            //   LepInfo[icoll].Ip3dPVSignificance[LepInfo[icoll].Size] = gsfsign * ip3dpv.second.value() / ip3dpv.second.error();
            }

            LepInfo[icoll].ElNClusters           [LepInfo[icoll].Size] = it_el->basicClustersSize();
            LepInfo[icoll].ElClassification     [LepInfo[icoll].Size] = it_el->classification();
            LepInfo[icoll].ElFBrem               [LepInfo[icoll].Size] = it_el->fbrem();
            LepInfo[icoll].ElNumberOfBrems      [LepInfo[icoll].Size] = it_el->numberOfBrems();
            //LepInfo[icoll].NumberOfExpectedInnerHits[LepInfo[icoll].Size] = it_el->gsfTrack()->trackerExpectedHitsInner().numberOfHits();// Add by Jacky
            LepInfo[icoll].NumberOfExpectedInnerHits[LepInfo[icoll].Size] = it_el->gsfTrack()->hitPattern().numberOfHits( reco::HitPattern::MISSING_INNER_HITS ); // Add by Jacky

            LepInfo[icoll].vertexZ               [LepInfo[icoll].Size] = it_el->vertex().z();//Uly 2011-04-04

            //Conversion rejection (Add by Jacky)
            ConversionFinder convFinder;

            // Conversion rejection (Version2).
            if( !TurnOffInCMSSW73x )
               if( tracks_h.isValid() ) { // turn off on CMSSW73X
                  if( debug_ > 15 ) { cout << "   Get conversion info\n"; }
                  ConversionInfo convInfo = convFinder.getConversionInfo( *it_el, tracks_h, evt_bField );
                  LepInfo[icoll].Eldist          [LepInfo[icoll].Size] = convInfo.dist();
                  LepInfo[icoll].Eldcot       [LepInfo[icoll].Size] = convInfo.dcot();
                  LepInfo[icoll].Elconvradius    [LepInfo[icoll].Size] = convInfo.radiusOfConversion();
                  LepInfo[icoll].ElConvPoint_x      [LepInfo[icoll].Size] = convInfo.pointOfConversion().x();
                  LepInfo[icoll].ElConvPoint_y      [LepInfo[icoll].Size] = convInfo.pointOfConversion().y();
                  LepInfo[icoll].ElConvPoint_z      [LepInfo[icoll].Size] = convInfo.pointOfConversion().z();
               }

            //   static bool hasMatchedConversion(const reco::GsfElectron &ele,
            //       const edm::Handle<reco::ConversionCollection> &convCol, const math::XYZPoint &beamspot,
            //       bool allowCkfMatch=true, float lxyMin=2.0, float probMin=1e-6, uint nHitsBeforeVtxMax=0);
            //LepInfo[icoll].ElhasConv[LepInfo[icoll].Size] = ConversionTools::hasMatchedConversion(*it_el, conversions_h, beamSpot.position());
            //   float mHits = it_el->gsfTrack()->trackerExpectedHitsInner().numberOfHits();

            // For CIC (CutBasedElectronID.cc) First check electron categories from CutBasedElectronID.cc
            float dist_ = ( it_el->convDist() == -9999. ? 9999 : it_el->convDist() );
            float dcot_ = ( it_el->convDcot() == -9999. ? 9999 : it_el->convDcot() );
            LepInfo[icoll].dcotdist[LepInfo[icoll].Size] = ( ( 0.04 - std::max( fabs( dist_ ), fabs( dcot_ ) ) ) > 0 ? ( 0.04 - std::max( fabs( dist_ ), fabs( dcot_ ) ) ) : 0 );
            LepInfo[icoll].ElseedEoverP[LepInfo[icoll].Size] = it_el->eSeedClusterOverP();
            LepInfo[icoll].ElHcalIso04[LepInfo[icoll].Size]  = it_el->dr04HcalTowerSumEt();
            // ElTrackDxy_PVBS for ip_gsf
            LepInfo[icoll].ElEcalIso04[LepInfo[icoll].Size]  = it_el->dr04EcalRecHitSumEt();
            // iso_sumoet = iso_sum*(40./scEt);

            if ( !isData && !skipGenInfo_ ) { //MC
               if( debug_ > 15 ) { cout << "   Getting MC information\n"; }
               const reco::GenParticle* gen = it_el->genLepton();

               if ( gen != NULL ) {
                  LepInfo[icoll].GenPt        [LepInfo[icoll].Size] = gen->pt();
                  LepInfo[icoll].GenEta       [LepInfo[icoll].Size] = gen->eta();
                  LepInfo[icoll].GenPhi       [LepInfo[icoll].Size] = gen->phi();
                  LepInfo[icoll].GenPdgID     [LepInfo[icoll].Size] = gen->pdgId();

                  const reco::Candidate* genCand = gen;

                  int bprime_tag = 0;  // 0: not b' or t'; 1: b'; 2:t'
                  while( genCand != NULL &&
                         genCand->numberOfMothers() == 1 ) {

                     genCand = genCand->mother( 0 );

                     if ( abs( genCand->pdgId() ) == 7 ) { bprime_tag = 1; } // check if it's bprime
                     if ( abs( genCand->pdgId() ) == 8 ) { bprime_tag = 2; } // check if it's tprime.

                     if ( LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] == 0 ) {
                        if ( abs( genCand->pdgId() ) == 23 )                    { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 2; }
                        else if ( abs( genCand->pdgId() ) == 24 )               { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 1; }
                        else if ( abs( genCand->pdgId() ) == 5 )               { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 3; } // from a b quark
                        else if ( ( abs( genCand->pdgId() ) % 1000 ) / 100 == 5 )   { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 3; } // from a B meson
                        else if ( ( abs( genCand->pdgId() ) % 10000 ) / 1000 == 5 )   { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 3; } // from a bottom baryon
                        else if ( abs( genCand->pdgId() ) == 4 )               { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 4; } // from a c quark
                        else if ( ( abs( genCand->pdgId() ) % 1000 ) / 100 == 4 )   { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 4; } // from a D meson
                        else if ( ( abs( genCand->pdgId() ) % 10000 ) / 1000 == 4 )   { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 4; } // from a charm baryon
                        else if ( abs( genCand->pdgId() ) == 15 )               { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 7; } // from tau
                     }
                  }
                  if ( bprime_tag == 1 ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] += 10; }
                  if ( bprime_tag == 2 ) { LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] += 20; }

               }


               if( debug_ > 15 ) { cout << "Get GenHandle\n"; }
               if ( LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] == 0 && GenHandle.isValid() ) {
                  for( std::vector<reco::GenParticle>::const_iterator it_gen = GenHandle->begin();
                       it_gen != GenHandle->end(); it_gen++ ) {
                     double r = deltaR<double>( it_gen->eta(), it_gen->phi(), it_el->eta(), it_el->phi() );

                     if ( ( abs( it_gen->pdgId() ) <= 5 || abs( it_gen->pdgId() ) == 21 ) && it_gen->status() == 3 &&
                          r < 0.5 && fabs( it_gen->pt() - it_el->pt() ) / it_gen->pt() < 0.5 ) {
                        LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 5; // matched to a parton (q,g)
                        break;
                     }
                     if ( abs( it_gen->pdgId() ) == 22 && it_gen->status() == 1 &&
                          r < 0.5 && fabs( it_gen->pt() - it_el->pt() ) / it_gen->pt() < 0.5 ) {
                        LepInfo[icoll].GenMCTag[LepInfo[icoll].Size] = 6; // matched to a photon
                        break;
                     }
                  }
               }
               if( debug_ > 15 ) { cout << "   Done getting MC information\n"; }
            }//MC

            LepInfo[icoll].CandRef[LepInfo[icoll].Size] = ( reco::Candidate* ) & ( *it_el );
            LepInfo[icoll].Size++;
         }//loop over electrons in collection
      }//have electron collection

      //Taus
      if( TauHandle.size() > icoll ) { //have tau collection
         if( debug_ > 10 ) { cout << " Tau collection size " << TauHandle[icoll]->size() << endl; }
         for( std::vector<pat::Tau>::const_iterator it_tau = TauHandle[icoll]->begin();
              it_tau != TauHandle[icoll]->end(); it_tau++ ) {//loop over taus in collection

            if( it_tau->pt() < 20 ) { continue; } //Require PT > 20 GeV
            if( debug_ > 11 ) { cout << "  Size " << LepInfo[icoll].Size << " tau pt,eta,phi " << it_tau->pt() << "," << it_tau->eta() << "," << it_tau->phi() << endl; }
            if ( LepInfo[icoll].Size >= MAX_LEPTONS ) {
               fprintf( stderr, "ERROR: number of leptons exceeds the size of array.\n" );
               break;//exit(0);
            }

            LepInfo[icoll].Index      [LepInfo[icoll].Size] = LepInfo[icoll].Size;
            LepInfo[icoll].LeptonType [LepInfo[icoll].Size] = 15;
            LepInfo[icoll].Charge     [LepInfo[icoll].Size] = it_tau->charge();
            LepInfo[icoll].Pt         [LepInfo[icoll].Size] = it_tau->pt();
            LepInfo[icoll].Eta        [LepInfo[icoll].Size] = it_tau->eta();
            LepInfo[icoll].Phi        [LepInfo[icoll].Size] = it_tau->phi();
            LepInfo[icoll].TrackIso   [LepInfo[icoll].Size] = it_tau->trackIso();
            LepInfo[icoll].EcalIso    [LepInfo[icoll].Size] = it_tau->ecalIso();
            LepInfo[icoll].HcalIso    [LepInfo[icoll].Size] = it_tau->hcalIso();

            LepInfo[icoll].Px         [LepInfo[icoll].Size] = it_tau->px(); //Uly 2011-04-04
            LepInfo[icoll].Py         [LepInfo[icoll].Size] = it_tau->py(); //Uly 2011-04-04
            LepInfo[icoll].Pz         [LepInfo[icoll].Size] = it_tau->pz(); //Uly 2011-04-04

            //PFIso
            LepInfo[icoll].ChargedHadronIso  [LepInfo[icoll].Size] = it_tau->chargedHadronIso();
            LepInfo[icoll].NeutralHadronIso  [LepInfo[icoll].Size] = it_tau->neutralHadronIso();
            LepInfo[icoll].PhotonIso         [LepInfo[icoll].Size] = it_tau->photonIso();

            LepInfo[icoll].isPFTau         [LepInfo[icoll].Size] = it_tau->isPFTau();    // YoungKyu 2012-10-16
            //LepInfo[icoll].signalCharge         [LepInfo[icoll].Size] = it_tau->signalCharge();    // YoungKyu 2012-11-08
            //hpsPFTau ID
            LepInfo[icoll].decayModeFinding[LepInfo[icoll].Size] = it_tau->tauID( "decayModeFinding" ); // YoungKyu 2012-10-31
            if( !TurnOffInCMSSW73x )
            { LepInfo[icoll].byVLooseCombinedIsolationDeltaBetaCorr[LepInfo[icoll].Size] = it_tau->tauID( "byVLooseCombinedIsolationDeltaBetaCorr" ); } // YoungKyu 2012-10-31
            LepInfo[icoll].byLooseCombinedIsolationDeltaBetaCorr[LepInfo[icoll].Size] = it_tau->tauID( "byLooseCombinedIsolationDeltaBetaCorr3Hits" ); // YoungKyu 2012-10-31
            LepInfo[icoll].byMediumCombinedIsolationDeltaBetaCorr[LepInfo[icoll].Size] = it_tau->tauID( "byMediumCombinedIsolationDeltaBetaCorr3Hits" ); // YoungKyu 2012-10-31
            LepInfo[icoll].byTightCombinedIsolationDeltaBetaCorr[LepInfo[icoll].Size] = it_tau->tauID( "byTightCombinedIsolationDeltaBetaCorr3Hits" ); // YoungKyu 2012-10-31
            LepInfo[icoll].againstElectronLoose[LepInfo[icoll].Size] = it_tau->tauID( "againstElectronLoose" ); // YoungKyu 2012-10-31
            LepInfo[icoll].againstElectronMedium[LepInfo[icoll].Size] = it_tau->tauID( "againstElectronMedium" ); // YoungKyu 2012-10-31
            LepInfo[icoll].againstElectronTight[LepInfo[icoll].Size] = it_tau->tauID( "againstElectronTight" ); // YoungKyu 2012-10-31
            LepInfo[icoll].againstElectronMVA[LepInfo[icoll].Size] = it_tau->tauID( "againstElectronMVA5category" ); // YoungKyu 2012-10-31
            LepInfo[icoll].againstMuonLoose[LepInfo[icoll].Size] = it_tau->tauID( "againstMuonLoose" ); // YoungKyu 2012-10-31
            LepInfo[icoll].againstMuonMedium[LepInfo[icoll].Size] = it_tau->tauID( "againstMuonMedium" ); // YoungKyu 2012-10-31
            LepInfo[icoll].againstMuonTight[LepInfo[icoll].Size] = it_tau->tauID( "againstMuonTight" ); // YoungKyu 2012-10-31
            //LepInfo[icoll].againstElectronMVA[LepInfo[icoll].Size] = it_tau->tauID("againstElectronMVA"); // YoungKyu 2012-10-31
            //LepInfo[icoll].byLooseCombinedIsolationDeltaBetaCorr[LepInfo[icoll].Size] = it_tau->tauID("byLooseCombinedIsolationDeltaBetaCorr"); // YoungKyu 2012-10-31
            //LepInfo[icoll].byMediumCombinedIsolationDeltaBetaCorr[LepInfo[icoll].Size] = it_tau->tauID("byMediumCombinedIsolationDeltaBetaCorr"); // YoungKyu 2012-10-31
            //LepInfo[icoll].byTightCombinedIsolationDeltaBetaCorr[LepInfo[icoll].Size] = it_tau->tauID("byTightCombinedIsolationDeltaBetaCorr"); // YoungKyu 2012-10-31

            if ( !isData && !skipGenInfo_ ) { //MC                                            // YoungKyu 2012-11-08
               if( debug_ > 15 ) { cout << "   Getting MC information\n"; }                     // YoungKyu 2012-11-08
               const reco::Candidate* gen = it_tau->genLepton();                            // YoungKyu 2012-11-08

               if ( gen != NULL ) {                                                         // YoungKyu 2012-11-08
                  LepInfo[icoll].GenPt        [LepInfo[icoll].Size] = gen->pt();             // YoungKyu 2012-11-08
                  LepInfo[icoll].GenEta       [LepInfo[icoll].Size] = gen->eta();            // YoungKyu 2012-11-08
                  LepInfo[icoll].GenPhi       [LepInfo[icoll].Size] = gen->phi();            // YoungKyu 2012-11-08
                  LepInfo[icoll].GenPdgID     [LepInfo[icoll].Size] = gen->pdgId();          // YoungKyu 2012-11-08
               }

            }




            LepInfo[icoll].CandRef[LepInfo[icoll].Size] = ( reco::Candidate* ) & ( *it_tau );
            LepInfo[icoll].Size++;
         }//loop over taus
      }//have tau collection

   }//loop over collections

   //=================================================================================
   // Photons 
   //=================================================================================
   fillPhoton( iEvent , iSetup );

   //=================================================================================
   // Jets
   //=================================================================================
   fillJets( iEvent , iSetup ) ;

   //=================================================================================
   // Pairs
   //=================================================================================

   if( pairColl_ >= 0 ) { //fill pair info
      if( debug_ > 5 ) { cout << "\tFill Pair Info."; }
      memset( &PairInfo, 0x00, sizeof( PairInfo ) );

      if( pairColl_ >= ( int )lepcollections_.size() ) { pairColl_ = 0; } //set to something that should be valid

      // di-lepton pairs
      for( int index1 = 0; index1 < LepInfo[pairColl_].Size; index1++ ) { //first lepton loop
         for( int index2 = index1 + 1; index2 < LepInfo[pairColl_].Size; index2++ ) { //second lepton loop
            if ( PairInfo.Size >= MAX_PAIRS ) {
               fprintf( stderr, "ERROR: number of lepton pairs exceeds the size of array.\n" );
               index1 = index2 = LepInfo[pairColl_].Size;
               break;
            }

            math::XYZTLorentzVector Sum = LepInfo[pairColl_].CandRef[index1]->p4() + LepInfo[pairColl_].CandRef[index2]->p4();

            if ( Sum.mag() < 200. ) { //mag<200
               PairInfo.Index     [PairInfo.Size] = PairInfo.Size;
               PairInfo.Type      [PairInfo.Size] = 1; // 1: ll
               PairInfo.Obj1Index [PairInfo.Size] = index1;
               PairInfo.Obj2Index [PairInfo.Size] = index2;
               PairInfo.Mass      [PairInfo.Size] = Sum.mag();
               PairInfo.Pt     [PairInfo.Size] = Sum.Pt();
               PairInfo.Eta    [PairInfo.Size] = Sum.Eta();
               PairInfo.Phi    [PairInfo.Size] = Sum.Phi();

               if ( !isData && !skipGenInfo_ ) { //mc
                  const reco::GenParticle* par1 = NULL;
                  const reco::GenParticle* par2 = NULL;

                  if ( LepInfo[pairColl_].LeptonType[index1] == 11 )
                  { par1 = ( ( pat::Electron* )LepInfo[pairColl_].CandRef[index1] )->genLepton(); }
                  if ( LepInfo[pairColl_].LeptonType[index1] == 13 )
                  { par1 = ( ( pat::Muon* )LepInfo[pairColl_].CandRef[index1] )->genLepton(); }
                  if ( LepInfo[pairColl_].LeptonType[index2] == 11 )
                  { par2 = ( ( pat::Electron* )LepInfo[pairColl_].CandRef[index2] )->genLepton(); }
                  if ( LepInfo[pairColl_].LeptonType[index2] == 13 )
                  { par2 = ( ( pat::Muon* )LepInfo[pairColl_].CandRef[index2] )->genLepton(); }

                  if ( par1 != NULL && par2 != NULL ) {
                     const reco::Candidate* gen1 = par1;
                     const reco::Candidate* gen2 = par2;

                     while( gen1 != NULL && par1 != NULL && gen1->numberOfMothers() == 1 &&
                            gen1->mother( 0 )->pdgId() == par1->pdgId() ) {
                        gen1 = gen1->mother( 0 );
                     }
                     while( gen2 != NULL && par2 != NULL && gen2->numberOfMothers() == 1 && par2 != NULL &&
                            gen2->mother( 0 )->pdgId() == par2->pdgId() ) {
                        gen2 = gen2->mother( 0 );
                     }

                     if ( gen1 != NULL && gen1->numberOfMothers() == 1 && gen2 != NULL && gen2->numberOfMothers() == 1 &&
                          gen1->mother( 0 ) == gen2->mother( 0 ) ) {
                        const reco::Candidate* mon = gen1->mother( 0 );

                        PairInfo.GenMass   [PairInfo.Size] = mon->p4().mag();
                        PairInfo.GenPt     [PairInfo.Size] = mon->pt();
                        PairInfo.GenEta    [PairInfo.Size] = mon->eta();
                        PairInfo.GenPhi    [PairInfo.Size] = mon->phi();
                        PairInfo.GenPdgID  [PairInfo.Size] = mon->pdgId();
                     }
                  }
               }//mc
               PairInfo.Size++;
            }//mag<200
         }//second lepton loop
      }//first lepton loop
   }//fill pair info

   // di-jet pairs

#ifdef FILL_DIJET_PAIRS
   for( int index1 = 0; index1 < JetInfo[0].Size; index1++ ) {
      for( int index2 = index1 + 1; index2 < JetInfo[0].Size; index2++ ) {
         if ( PairInfo.Size >= MAX_PAIRS ) {
            fprintf( stderr, "ERROR: number of jet pairs exceeds the size of array.\n" );
            index1 = index2 = JetInfo[0].Size;
            break;
         }

         math::XYZTLorentzVector Sum = JetInfo[0].CandRef[index1]->p4() + JetInfo[0].CandRef[index2]->p4();

         if ( JetInfo[0].Pt[index1] > 25. && JetInfo[0].Pt[index2] > 25. && Sum.mag() < 200. ) {
            PairInfo.Index     [PairInfo.Size] = PairInfo.Size;
            PairInfo.Type      [PairInfo.Size] = 2; // 2: jj
            PairInfo.Obj1Index [PairInfo.Size] = index1;
            PairInfo.Obj2Index [PairInfo.Size] = index2;
            PairInfo.Mass      [PairInfo.Size] = Sum.mag();
            PairInfo.Pt     [PairInfo.Size] = Sum.Pt();
            PairInfo.Eta    [PairInfo.Size] = Sum.Eta();
            PairInfo.Phi    [PairInfo.Size] = Sum.Phi();

            if ( !isData && !skipGenInfo_ ) {
               const reco::GenParticle* par1 = NULL;
               const reco::GenParticle* par2 = NULL;

               par1 = ( ( pat::Jet* )JetInfo[0].CandRef[index1] )->genParton();
               par2 = ( ( pat::Jet* )JetInfo[0].CandRef[index2] )->genParton();

               if ( par1 != NULL && par2 != NULL ) {
                  const reco::Candidate* gen1 = par1;
                  const reco::Candidate* gen2 = par2;

                  while( gen1 != NULL && par1 != NULL && gen1->numberOfMothers() == 1 &&
                         gen1->mother( 0 )->pdgId() == par1->pdgId() ) {
                     gen1 = gen1->mother( 0 );
                  }
                  while( gen2 != NULL && par2 != NULL && gen2->numberOfMothers() == 1 && par2 != NULL &&
                         gen2->mother( 0 )->pdgId() == par2->pdgId() ) {
                     gen2 = gen2->mother( 0 );
                  }

                  if ( gen1 != NULL && gen1->numberOfMothers() == 1 && gen2 != NULL && gen2->numberOfMothers() == 1 &&
                       gen1->mother( 0 ) == gen2->mother( 0 ) ) {
                     const reco::Candidate* mon = gen1->mother( 0 );

                     PairInfo.GenMass  [PairInfo.Size] = mon->p4().mag();
                     PairInfo.GenPt    [PairInfo.Size] = mon->pt();
                     PairInfo.GenEta   [PairInfo.Size] = mon->eta();
                     PairInfo.GenPhi   [PairInfo.Size] = mon->phi();
                     PairInfo.GenPdgID [PairInfo.Size] = mon->pdgId();
                  }
               }

            }
            PairInfo.Size++;
         }
      }
   }
#endif

   vector<const reco::Candidate*> cands;
   vector<const Candidate*>::const_iterator found = cands.begin();
   if( !isData && !skipGenInfo_ ) {
      for( std::vector<reco::GenParticle>::const_iterator it_gen = GenHandle->begin(); it_gen != GenHandle->end(); it_gen++ ) {
         cands.push_back( &*it_gen );
      }
   }

   if( debug_ > 5 ) { cout << "\tFill Gen and Event Info.\n"; }
   memset( &GenInfo, 0x00, sizeof( GenInfo ) );
   memset( &EvtInfo, 0x00, sizeof( EvtInfo ) );

   EvtInfo.RunNo  = iEvent.id().run();
   EvtInfo.EvtNo  = iEvent.id().event();
   EvtInfo.McFlag = iEvent.isRealData() ? 0 : 1;
   EvtInfo.BxNo   = iEvent.bunchCrossing();
   EvtInfo.LumiNo = iEvent.luminosityBlock();
   EvtInfo.Orbit  = iEvent.orbitNumber();
   EvtInfo.nTrgBook = N_TRIGGER_BOOKINGS;

   EvtInfo.ptHat = -1.;
   /*
      // only for RECO, thus used GenProducer instead of HepMC with AOD sample
      // https://cmssdt.cern.ch/SDT/lxr/source/SimDataFormats/GeneratorProducts/src/GenEventInfoProduct.cc
   if(!isData){
       edm::Handle<HepMCProduct> evtpthat;
       iEvent.getByLabel("generator", evtpthat);
       std::cout<<"not data : evtpthat.isValid() : "<<evtpthat.isValid()<<std::endl;
       if(evtpthat.isValid()){
           HepMC::GenEvent * myGenEvent = new HepMC::GenEvent(*(evtpthat->GetEvent()));
           EvtInfo.ptHat = myGenEvent->event_scale();
       }
   }
   */


   if( !TurnOffInCMSSW73x )
      for( unsigned int ri_ = 0; ri_ < 2; ri_++ ) {
         if( rhoH[ri_].isValid() ) { EvtInfo.RhoPU[ri_] = *( rhoH[ri_].product() ); }
         if( sigmaHandle[ri_].isValid() ) { EvtInfo.SigmaPU[ri_] = *( sigmaHandle[ri_].product() ); }
      }

   // MC daughters: 0-1: hard jet from b'bar/t'bar, 2-9: W daughters, 10-13: Z daughters
   const reco::Candidate* MCDaughters[14];
   for( int i = 0; i < 14; i++ ) { MCDaughters[i] = NULL; }
   if( !isData && !skipGenInfo_ ) { //mc info
      if( debug_ > 15 ) { cout << "Getting MC info\n"; }
      // PileupSummaryInfo
      Handle<std::vector< PileupSummaryInfo > >  PUInfo;
      if( puInfoLabel_.size() > 0 ) { iEvent.getByLabel( puInfoLabel_[0], PUInfo ); }
      std::vector<PileupSummaryInfo>::const_iterator PVI;

      for( PVI = PUInfo->begin(); PVI != PUInfo->end(); ++PVI ) {
         EvtInfo.nPU[EvtInfo.nBX] = PVI->getPU_NumInteractions();
         EvtInfo.BXPU[EvtInfo.nBX] = PVI->getBunchCrossing();
         EvtInfo.TrueIT[EvtInfo.nBX] = PVI->getTrueNumInteractions();
         //cout << " Pileup Information: bunchXing, nvtx: " << PVI->getBunchCrossing() << " " << PVI->getPU_NumInteractions() << endl;
         EvtInfo.nBX += 1;
      }

      double evWeight = 1.0 ;
      edm::Handle< GenEventInfoProduct > genEventInfo;
      iEvent.getByLabel( "generator", genEventInfo );
      if ( genEventInfo.isValid() ) {
         evWeight = genEventInfo->weight();
         EvtInfo.ptHat = genEventInfo->qScale();
         //double ptHat_        = (genEventInfo->hasBinningValues() ? genEventInfo->binningValues()[0] : 0.0);
         //std::cout<<"qScale : "<<genEventInfo->qScale()<<" ( "<< ptHat_<<")"<<std::endl;
      }
      GenInfo.Weight = evWeight;

      if( debug_ ){ 
         //printf( " idx  |    ID -       Name |Stat|  Mo1  Mo2  Da1  Da2 |nMo nDa|    pt       eta     phi   |     px         py         pz        m     |\n" ); 
      }
      for( std::vector<reco::GenParticle>::const_iterator it_gen = GenHandle->begin();
           it_gen != GenHandle->end(); it_gen++ ) {
         int pdgId = it_gen->pdgId();
         int numberOfDaughters = it_gen->numberOfDaughters();
         int numberOfMothers = it_gen->numberOfMothers();
         int dauId1 = 0, dauId2 = 0, monId = 0;
         const reco::Candidate* dau1 = NULL;
         const reco::Candidate* dau2 = NULL;

         if( it_gen->status() == 3 ) { //Book GenInfo for DC table
            // Particles Mothers and Daighters
            int iMo1 = -1;
            int iMo2 = -1;
            int iDa1 = -1;
            int iDa2 = -1;
            int iGrandMo1 = -1;
            int iGrandMo2 = -1;
            int NMo = it_gen->numberOfMothers();
            int NDa = it_gen->numberOfDaughters();

            found = find( cands.begin(), cands.end(), it_gen->mother( 0 ) );
            if( found != cands.end() ) { iMo1 = found - cands.begin() ; }
            //if(iMo1>GenInfo.Size) iMo1 = 0;

            found = find( cands.begin(), cands.end(), it_gen->mother( NMo - 1 ) );
            if( found != cands.end() ) { iMo2 = found - cands.begin() ; }

            found = find( cands.begin(), cands.end(), it_gen->daughter( 0 ) );
            if( found != cands.end() ) { iDa1 = found - cands.begin() ; }

            found = find( cands.begin(), cands.end(), it_gen->daughter( NDa - 1 ) );
            if( found != cands.end() ) { iDa2 = found - cands.begin() ; }

            if( iMo1 != -1 ) {
               found = find( cands.begin(), cands.end(), ( GenHandle->begin() + iMo1 )->mother( 0 ) );
               if( found != cands.end() ) { iGrandMo1 = found - cands.begin() ; }
            }

            if( iMo2 != -1 ) {
               found = find( cands.begin(), cands.end(), ( GenHandle->begin() + iMo2 )->mother( 0 ) );
               if( found != cands.end() ) { iGrandMo2 = found - cands.begin() ; }
            }

            GenInfo.Pt[GenInfo.Size]       = it_gen->pt();
            GenInfo.Eta[GenInfo.Size]      = it_gen->eta();
            GenInfo.Phi[GenInfo.Size]      = it_gen->phi();
            GenInfo.Mass[GenInfo.Size]     = it_gen->mass();
            GenInfo.PdgID[GenInfo.Size]    = it_gen->pdgId();

            if( it_gen->pdgId() == 22 ) {
               // -1 : unknown or not photon, 0 : prompt photon, 1 : decay in flight, 2 : ISR, 3 : FSR
               GenInfo.PhotonFlag[GenInfo.Size]    = 0;
            } else {
               GenInfo.PhotonFlag[GenInfo.Size]    = -1;
            }
            GenInfo.Status[GenInfo.Size]   = it_gen->status();
            GenInfo.nMo[GenInfo.Size]      = NMo;
            GenInfo.nDa[GenInfo.Size]      = NDa;
            GenInfo.Mo1[GenInfo.Size]      = iMo1;
            GenInfo.Mo2[GenInfo.Size]      = iMo2;
            GenInfo.Da1[GenInfo.Size]      = iDa1;
            GenInfo.Da2[GenInfo.Size]      = iDa2;

            GenInfo.Mo1PdgID[GenInfo.Size]    = 0;
            GenInfo.Mo2PdgID[GenInfo.Size]    = 0;
            GenInfo.Mo1Status[GenInfo.Size]   = -1;
            GenInfo.Mo2Status[GenInfo.Size]   = -1;
            GenInfo.Da1PdgID[GenInfo.Size]    = 0;
            GenInfo.Da2PdgID[GenInfo.Size]    = 0;

            GenInfo.GrandMo1PdgID[GenInfo.Size] = 0;
            GenInfo.GrandMo2PdgID[GenInfo.Size] = 0;
            GenInfo.GrandMo1Status[GenInfo.Size] = -1;
            GenInfo.GrandMo2Status[GenInfo.Size] = -1;

            if( iMo1 != -1 ) {
               GenInfo.Mo1PdgID[GenInfo.Size]      = ( GenHandle->begin() + iMo1 )->pdgId();
               GenInfo.Mo1Status[GenInfo.Size]  = ( GenHandle->begin() + iMo1 )->status();
            }
            if( iMo2 != -1 ) {
               GenInfo.Mo2PdgID[GenInfo.Size]      = ( GenHandle->begin() + iMo2 )->pdgId();
               GenInfo.Mo2Status[GenInfo.Size]  = ( GenHandle->begin() + iMo2 )->status();
            }
            if( iDa1 != -1 )
            { GenInfo.Da1PdgID[GenInfo.Size]    = ( GenHandle->begin() + iDa1 )->pdgId(); }
            if( iDa2 != -1 )
            { GenInfo.Da2PdgID[GenInfo.Size]    = ( GenHandle->begin() + iDa2 )->pdgId(); }

            if( iGrandMo1 != -1 ) {
               GenInfo.GrandMo1PdgID[GenInfo.Size]    = ( GenHandle->begin() + iGrandMo1 )->pdgId();
               GenInfo.GrandMo1Status[GenInfo.Size]      = ( GenHandle->begin() + iGrandMo1 )->status();
            }
            if( iGrandMo2 != -1 ) {
               GenInfo.GrandMo2PdgID[GenInfo.Size]    = ( GenHandle->begin() + iGrandMo2 )->pdgId();
               GenInfo.GrandMo2Status[GenInfo.Size]      = ( GenHandle->begin() + iGrandMo2 )->status();
            }

            GenInfo.Size++;
         } else if( it_gen->status() == 1 ) {
            if( it_gen->pt() < 20 ) { continue; } // remove lots of soft photon
            // Particles Mothers and Daighters
            int iMo1 = -1;
            int iMo2 = -1;
            int iDa1 = -1;
            int iDa2 = -1;
            int iGrandMo1 = -1;
            int iGrandMo2 = -1;
            int nMo = it_gen->numberOfMothers();
            int nDa = it_gen->numberOfDaughters();

            found = find( cands.begin(), cands.end(), it_gen->mother( 0 ) );
            if( found != cands.end() ) { iMo1 = found - cands.begin() ; }

            found = find( cands.begin(), cands.end(), it_gen->mother( nMo - 1 ) );
            if( found != cands.end() ) { iMo2 = found - cands.begin() ; }

            found = find( cands.begin(), cands.end(), it_gen->daughter( 0 ) );
            if( found != cands.end() ) { iDa1 = found - cands.begin() ; }

            found = find( cands.begin(), cands.end(), it_gen->daughter( nDa - 1 ) );
            if( found != cands.end() ) { iDa2 = found - cands.begin() ; }
            // const ParticleData* pd = pdt_->particle( it_gen->pdgId() );     //Disable to remove compile warning  

            if( iMo1 != -1 ) {
               found = find( cands.begin(), cands.end(), ( GenHandle->begin() + iMo1 )->mother( 0 ) );
               if( found != cands.end() ) { iGrandMo1 = found - cands.begin() ; }
            }

            if( iMo2 != -1 ) {
               found = find( cands.begin(), cands.end(), ( GenHandle->begin() + iMo2 )->mother( 0 ) );
               if( found != cands.end() ) { iGrandMo2 = found - cands.begin() ; }
            }

            GenInfo.Pt[GenInfo.Size]       = it_gen->pt();
            GenInfo.Eta[GenInfo.Size]      = it_gen->eta();
            GenInfo.Phi[GenInfo.Size]      = it_gen->phi();
            GenInfo.Mass[GenInfo.Size]     = it_gen->mass();
            GenInfo.PdgID[GenInfo.Size] = it_gen->pdgId();
            GenInfo.Status[GenInfo.Size]   = it_gen->status();
            GenInfo.nMo[GenInfo.Size]      = nMo;
            GenInfo.nDa[GenInfo.Size]      = nDa;
            GenInfo.Mo1[GenInfo.Size]      = iMo1;
            GenInfo.Mo2[GenInfo.Size]      = iMo2;
            GenInfo.Da1[GenInfo.Size]      = iDa1;
            GenInfo.Da2[GenInfo.Size]      = iDa2;

            GenInfo.Mo1PdgID[GenInfo.Size]    = 0;
            GenInfo.Mo2PdgID[GenInfo.Size]    = 0;
            GenInfo.Mo1Status[GenInfo.Size]   = -1;
            GenInfo.Mo2Status[GenInfo.Size]   = -1;
            GenInfo.Da1PdgID[GenInfo.Size]    = 0;
            GenInfo.Da2PdgID[GenInfo.Size]    = 0;
            GenInfo.GrandMo1PdgID[GenInfo.Size] = 0;
            GenInfo.GrandMo2PdgID[GenInfo.Size] = 0;
            GenInfo.GrandMo1Status[GenInfo.Size] = -1;
            GenInfo.GrandMo2Status[GenInfo.Size] = -1;

            if( iMo1 != -1 ) {
               GenInfo.Mo1PdgID[GenInfo.Size]      = ( GenHandle->begin() + iMo1 )->pdgId();
               GenInfo.Mo1Status[GenInfo.Size]  = ( GenHandle->begin() + iMo1 )->status();
            }
            if( iMo2 != -1 ) {
               GenInfo.Mo2PdgID[GenInfo.Size]      = ( GenHandle->begin() + iMo2 )->pdgId();
               GenInfo.Mo2Status[GenInfo.Size]  = ( GenHandle->begin() + iMo2 )->status();
            }
            if( iDa1 != -1 )
            { GenInfo.Da1PdgID[GenInfo.Size]    = ( GenHandle->begin() + iDa1 )->pdgId(); }
            if( iDa2 != -1 )
            { GenInfo.Da2PdgID[GenInfo.Size]    = ( GenHandle->begin() + iDa2 )->pdgId(); }

            if( iGrandMo1 != -1 ) {
               GenInfo.GrandMo1PdgID[GenInfo.Size]    = ( GenHandle->begin() + iGrandMo1 )->pdgId();
               GenInfo.GrandMo1Status[GenInfo.Size]      = ( GenHandle->begin() + iGrandMo1 )->status();
            }
            if( iGrandMo2 != -1 ) {
               GenInfo.GrandMo2PdgID[GenInfo.Size]    = ( GenHandle->begin() + iGrandMo2 )->pdgId();
               GenInfo.GrandMo2Status[GenInfo.Size]      = ( GenHandle->begin() + iGrandMo2 )->status();
            }
            /*
               Photon Flag Rule :
               -1 : unknown or not photon, 0 : prompt photon, 1 : decay in flight, 2 : ISR, 3 : FSR

               1). prompt photon :
                   {pid(22) && status(3)} or
                   {pid(22) && status(1) && M_pid(22) && M_status(3)}

               2). decay in flight :
                   pid(22) && status(1) && M_status(2)

               3). ISR :
                   pid(22) && status(1) && M_status(3) && M_pid(<6 || =21) && GM_status(3) && GM_pid(2212)

               4). FSR :
                   pid(22) && status(1) && M_status(3) && GM_status(3) && GM_pid(!2212)
               */
            GenInfo.PhotonFlag[GenInfo.Size]     = -1;

            if( iMo1 != -1 && iMo2 != -1 && it_gen->pdgId() == 22 ) {
               if( GenInfo.Mo1PdgID[GenInfo.Size] == 22 && GenInfo.Mo2PdgID[GenInfo.Size] == 22 ) {
                  if( ( GenHandle->begin() + iMo1 )->status() == 3 && ( GenHandle->begin() + iMo2 )->status() == 3 )
                  { GenInfo.PhotonFlag[GenInfo.Size]   = 0; }
               } else if( ( GenHandle->begin() + iMo1 )->status() == 2 && ( GenHandle->begin() + iMo2 )->status() == 2 ) {
                  GenInfo.PhotonFlag[GenInfo.Size]      = 1;
               } else if( ( GenHandle->begin() + iMo1 )->status() == 3 && ( GenHandle->begin() + iMo2 )->status() == 3 &&
                          ( ( abs( ( GenHandle->begin() + iMo1 )->pdgId() ) < 6 || ( GenHandle->begin() + iMo1 )->pdgId() == 21 ) &&
                            ( abs( ( GenHandle->begin() + iMo1 )->pdgId() ) < 6 || ( GenHandle->begin() + iMo1 )->pdgId() == 21 ) )
                        ) {
                  if( iGrandMo1 != -1 && iGrandMo2 != -1 )
                     if( ( GenHandle->begin() + iGrandMo1 )->pdgId() == 2212 )
                        if( ( GenHandle->begin() + iGrandMo2 )->pdgId() == 2212 )
                        { GenInfo.PhotonFlag[GenInfo.Size]      = 2; }
               } else if( ( GenHandle->begin() + iMo1 )->status() == 3 && ( GenHandle->begin() + iMo2 )->status() == 3 ) {
                  if( iGrandMo1 != -1 && iGrandMo2 != -1 )
                     if( ( GenHandle->begin() + iGrandMo1 )->pdgId() != 2212 )
                        if( ( GenHandle->begin() + iGrandMo2 )->pdgId() != 2212 )
                        { GenInfo.PhotonFlag[GenInfo.Size]      = 3; }
               }
            }

            GenInfo.Size++;
            if( debug_ )
               if( !( it_gen->pdgId() == 22 && ( GenHandle->begin() + iMo1 )->status() == 2  ) ){
                  /*
                  printf( " %4d | %5d - %10s | %2d | %4d %4d %4d %4d | %2d %2d | %7.3f %10.3f %6.3f | %10.3f %10.3f %10.3f %8.3f |\n",
                          ( int )( it_gen - GenHandle->begin() ),
                          it_gen->pdgId(),
                          pd->name().c_str(),
                          it_gen->status(),
                          iMo1, iMo2, iDa1, iDa2, nMo, nDa,
                          it_gen->pt(),
                          it_gen->eta(),
                          it_gen->phi(),
                          it_gen->px(),
                          it_gen->py(),
                          it_gen->pz(),
                          it_gen->mass()
                        );
                        */
               }
         }

         if ( numberOfDaughters >= 2 ) {
            dau1    = it_gen->daughter( 0 );
            dau2    = it_gen->daughter( 1 );
            dauId1 = abs( dau1->pdgId() );
            dauId2 = abs( dau2->pdgId() );
            if ( dauId1 > dauId2 ) { // always keep dauId2 > dauId1
               const reco::Candidate* tmp = dau1;
               dau1    = dau2;
               dau2    = tmp;
               dauId1 = abs( dau1->pdgId() );
               dauId2 = abs( dau2->pdgId() );
            }
         }
         if ( numberOfMothers >= 1 )
         { monId   = it_gen->mother( 0 )->pdgId(); }

         if ( pdgId == +7 ) { // find b'
            EvtInfo.McbprimeMass[0] = it_gen->p4().mag();

            // b' decay mode      - 0: others, 1: tW, 2: cW, 3: bZ, 4: bH
            if ( dauId1 == 6 && dauId2 == 24 ) { EvtInfo.McbprimeMode[0] = 1; }
            if ( dauId1 == 4 && dauId2 == 24 ) { EvtInfo.McbprimeMode[0] = 2; }
            if ( dauId1 == 5 && dauId2 == 23 ) { EvtInfo.McbprimeMode[0] = 3; }
            if ( dauId1 == 5 && dauId2 == 25 ) { EvtInfo.McbprimeMode[0] = 4; }
            if ( EvtInfo.McbprimeMode[0] == 0 ) { continue; }

            if ( dauId1 == 6 ) {
               if ( abs( dau1->daughter( 0 )->pdgId() ) == 5 ) { MCDaughters[0] = dau1->daughter( 0 ); }
               if ( abs( dau1->daughter( 1 )->pdgId() ) == 5 ) { MCDaughters[0] = dau1->daughter( 1 ); }
            } else { MCDaughters[0] = dau1; }

         } else if ( pdgId == -7 ) { // find b'bar
            EvtInfo.McbprimeMass[1] = it_gen->p4().mag();

            // b' decay mode      - 0: others, 1: tW, 2: cW, 3: bZ, 4: bH
            if ( dauId1 == 6 && dauId2 == 24 ) { EvtInfo.McbprimeMode[1] = 1; }
            if ( dauId1 == 4 && dauId2 == 24 ) { EvtInfo.McbprimeMode[1] = 2; }
            if ( dauId1 == 5 && dauId2 == 23 ) { EvtInfo.McbprimeMode[1] = 3; }
            if ( dauId1 == 5 && dauId2 == 25 ) { EvtInfo.McbprimeMode[1] = 4; }
            if ( EvtInfo.McbprimeMode[1] == 0 ) { continue; }

            if ( dauId1 == 6 ) {
               if ( abs( dau1->daughter( 0 )->pdgId() ) == 5 ) { MCDaughters[1] = dau1->daughter( 0 ); }
               if ( abs( dau1->daughter( 1 )->pdgId() ) == 5 ) { MCDaughters[1] = dau1->daughter( 1 ); }
            } else { MCDaughters[1] = dau1; }
         } else if ( pdgId == +8 ) { // find t'
            EvtInfo.MctprimeMass[0] = it_gen->p4().mag();

            // t' decay mode            - 0: others, 1: bW, 2: tZ, 3: tH, 4: tgamma
            if ( dauId1 == 5 && dauId2 == 24 ) { EvtInfo.MctprimeMode[0] = 1; }
            if ( dauId1 == 6 && dauId2 == 23 ) { EvtInfo.MctprimeMode[0] = 2; }
            if ( dauId1 == 6 && dauId2 == 25 ) { EvtInfo.MctprimeMode[0] = 3; }
            if ( dauId1 == 6 && dauId2 == 22 ) { EvtInfo.MctprimeMode[0] = 4; }
            if ( EvtInfo.MctprimeMode[0] == 0 ) { continue; }

            if ( dauId1 == 6 ) {
               if ( abs( dau1->daughter( 0 )->pdgId() ) == 5 ) { MCDaughters[0] = dau1->daughter( 0 ); }
               if ( abs( dau1->daughter( 1 )->pdgId() ) == 5 ) { MCDaughters[0] = dau1->daughter( 1 ); }
            } else { MCDaughters[0] = dau1; }

         } else if ( pdgId == -8 ) { // find t'bar
            EvtInfo.MctprimeMass[1] = it_gen->p4().mag();

            // t' decay mode            - 0: others, 1: bW, 2: tZ, 3: tH, 4: tgamma
            if ( dauId1 == 5 && dauId2 == 24 ) { EvtInfo.MctprimeMode[1] = 1; }
            if ( dauId1 == 6 && dauId2 == 23 ) { EvtInfo.MctprimeMode[1] = 2; }
            if ( dauId1 == 6 && dauId2 == 25 ) { EvtInfo.MctprimeMode[1] = 3; }
            if ( dauId1 == 6 && dauId2 == 22 ) { EvtInfo.MctprimeMode[1] = 4; }
            if ( EvtInfo.MctprimeMode[1] == 0 ) { continue; }

            if ( dauId1 == 6 ) {
               if ( abs( dau1->daughter( 0 )->pdgId() ) == 5 ) { MCDaughters[1] = dau1->daughter( 0 ); }
               if ( abs( dau1->daughter( 1 )->pdgId() ) == 5 ) { MCDaughters[1] = dau1->daughter( 1 ); }
            } else { MCDaughters[1] = dau1; }
         } else if ( pdgId == +6 ) { // find top
            EvtInfo.MctopMass[0] = it_gen->p4().mag();
         } else if ( pdgId == -6 ) { // find tbar
            EvtInfo.MctopMass[1] = it_gen->p4().mag();
         } else if ( pdgId == -24 && ( monId == +7 || monId == -8 ) ) { // W- from b' or t' bar
            EvtInfo.McWMass[0] = it_gen->p4().mag();

            // W decay mode       - 0: others, 1: enu, 2: munu, 3: taunu, 4: jj
            if ( dauId1 == 11 && dauId2 == 12 ) { EvtInfo.McWMode[0] = 1; }
            if ( dauId1 == 13 && dauId2 == 14 ) { EvtInfo.McWMode[0] = 2; }
            if ( dauId1 == 15 && dauId2 == 16 ) { EvtInfo.McWMode[0] = 3; }
            if ( dauId1 >= 1 && dauId2 <= 5 ) { EvtInfo.McWMode[0] = 4; }
            if ( EvtInfo.McWMode[0] == 0 ) { continue; }

            MCDaughters[2] = dau1;
            MCDaughters[3] = dau2;
         } else if ( pdgId == +24 && monId == +6 ) { // W+ from t
            EvtInfo.McWMass[1] = it_gen->p4().mag();

            // W decay mode       - 0: others, 1: enu, 2: munu, 3: taunu, 4: jj
            if ( dauId1 == 11 && dauId2 == 12 ) { EvtInfo.McWMode[1] = 1; }
            if ( dauId1 == 13 && dauId2 == 14 ) { EvtInfo.McWMode[1] = 2; }
            if ( dauId1 == 15 && dauId2 == 16 ) { EvtInfo.McWMode[1] = 3; }
            if ( dauId1 >= 1 && dauId2 <= 5 ) { EvtInfo.McWMode[1] = 4; }
            if ( EvtInfo.McWMode[1] == 0 ) { continue; }

            MCDaughters[4] = dau1;
            MCDaughters[5] = dau2;
         } else if ( pdgId == +24 && ( monId == -7 || monId == +8 ) ) { // W+ from b'bar or t'
            EvtInfo.McWMass[2] = it_gen->p4().mag();

            // W decay mode       - 0: others, 1: enu, 2: munu, 3: taunu, 4: jj
            if ( dauId1 == 11 && dauId2 == 12 ) { EvtInfo.McWMode[2] = 1; }
            if ( dauId1 == 13 && dauId2 == 14 ) { EvtInfo.McWMode[2] = 2; }
            if ( dauId1 == 15 && dauId2 == 16 ) { EvtInfo.McWMode[2] = 3; }
            if ( dauId1 >= 1 && dauId2 <= 5 ) { EvtInfo.McWMode[2] = 4; }
            if ( EvtInfo.McWMode[2] == 0 ) { continue; }

            MCDaughters[6] = dau1;
            MCDaughters[7] = dau2;
         } else if ( pdgId == -24 && monId == -6 ) { // W- from tbar
            EvtInfo.McWMass[3] = it_gen->p4().mag();

            // W decay mode       - 0: others, 1: enu, 2: munu, 3: taunu, 4: jj
            if ( dauId1 == 11 && dauId2 == 12 ) { EvtInfo.McWMode[3] = 1; }
            if ( dauId1 == 13 && dauId2 == 14 ) { EvtInfo.McWMode[3] = 2; }
            if ( dauId1 == 15 && dauId2 == 16 ) { EvtInfo.McWMode[3] = 3; }
            if ( dauId1 >= 1 && dauId2 <= 5 ) { EvtInfo.McWMode[3] = 4; }
            if ( EvtInfo.McWMode[3] == 0 ) { continue; }

            MCDaughters[8] = dau1;
            MCDaughters[9] = dau2;
         } else if ( pdgId == 23 && ( monId == +7 || monId == +8 ) ) { // Z from b' or t'
            EvtInfo.McZMass[0] = it_gen->p4().mag();

            // Z decay mode       - 0: others, 1: ee, 2: mumu, 3: tautau, 4: nunu, 5: bb, 6: jj
            if ( dauId1 == 11 && dauId2 == 11 )   { EvtInfo.McZMode[0] = 1; }
            if ( dauId1 == 13 && dauId2 == 13 )   { EvtInfo.McZMode[0] = 2; }
            if ( dauId1 == 15 && dauId2 == 15 )   { EvtInfo.McZMode[0] = 3; }

            if ( ( dauId1 == 12 && dauId2 == 12 ) ||
                 ( dauId1 == 14 && dauId2 == 14 ) ||
                 ( dauId1 == 16 && dauId2 == 16 ) ) { EvtInfo.McZMode[0] = 4; }

            if ( dauId1 == 5 && dauId2 == 5 )   { EvtInfo.McZMode[0] = 5; }
            if ( dauId1 >= 1 && dauId2 <= 4 )   { EvtInfo.McZMode[0] = 6; }
            if ( EvtInfo.McZMode[0] == 0 ) { continue; }

            MCDaughters[10] = dau1;
            MCDaughters[11] = dau2;
         } else if ( pdgId == 23 && ( monId == -7 || monId == -8 ) ) { // Z from b'bar or t'bar
            EvtInfo.McZMass[1] = it_gen->p4().mag();

            // Z decay mode       - 0: others, 1: ee, 2: mumu, 3: tautau, 4: nunu, 5: bb, 6: jj
            if ( dauId1 == 11 && dauId2 == 11 )  { EvtInfo.McZMode[1] = 1; }
            if ( dauId1 == 13 && dauId2 == 13 )  { EvtInfo.McZMode[1] = 2; }
            if ( dauId1 == 15 && dauId2 == 15 )  { EvtInfo.McZMode[1] = 3; }

            if ( ( dauId1 == 12 && dauId2 == 12 ) ||
                 ( dauId1 == 14 && dauId2 == 14 ) ||
                 ( dauId1 == 16 && dauId2 == 16 ) ) { EvtInfo.McZMode[1] = 4; }

            if ( dauId1 == 5 && dauId2 == 5 )   { EvtInfo.McZMode[1] = 5; }
            if ( dauId1 >= 1 && dauId2 <= 4 )   { EvtInfo.McZMode[1] = 6; }
            if ( EvtInfo.McZMode[0] == 0 ) { continue; }

            MCDaughters[12] = dau1;
            MCDaughters[13] = dau2;
         }
      }
   }//mc info

   int mclep_count[2] = {0, 0};
   if ( EvtInfo.McWMode[0] == 1 || EvtInfo.McWMode[0] == 2 ) { mclep_count[1]++; }
   if ( EvtInfo.McWMode[1] == 1 || EvtInfo.McWMode[1] == 2 ) { mclep_count[0]++; }
   if ( EvtInfo.McWMode[2] == 1 || EvtInfo.McWMode[2] == 2 ) { mclep_count[0]++; }
   if ( EvtInfo.McWMode[3] == 1 || EvtInfo.McWMode[3] == 2 ) { mclep_count[1]++; }
   if ( EvtInfo.McZMode[0] == 1 || EvtInfo.McZMode[0] == 2 ) {
      mclep_count[0]++;
      mclep_count[1]++;
   }
   if ( EvtInfo.McZMode[1] == 1 || EvtInfo.McZMode[1] == 2 ) {
      mclep_count[0]++;
      mclep_count[1]++;
   }
   // McSigTag - 0: others, 1: dilepton (opposite-sign), 2: dilepton (same-sign), 3: trilepton, 4: 4-leptons
   if ( mclep_count[0] == 1 && mclep_count[1] == 1 ) { EvtInfo.McSigTag = 1; }
   if ( mclep_count[0] == 2 && mclep_count[1] == 0 ) { EvtInfo.McSigTag = 2; }
   if ( mclep_count[0] == 0 && mclep_count[1] == 2 ) { EvtInfo.McSigTag = 2; }
   if ( mclep_count[0] == 2 && mclep_count[1] == 1 ) { EvtInfo.McSigTag = 3; }
   if ( mclep_count[0] == 1 && mclep_count[1] == 2 ) { EvtInfo.McSigTag = 3; }
   if ( mclep_count[0] == 2 && mclep_count[1] == 2 ) { EvtInfo.McSigTag = 4; }

   // Fill MC generator daughters
   // MC daughters: 0-1: hard jet from b'bar/t'bar, 2-9: W daughters, 10-13: Z daughters
   for( int i = 0; i < 14; i++ ) {
      if ( MCDaughters[i] == NULL ) { continue; }
      EvtInfo.McDauPt[i]    = MCDaughters[i]->pt();
      EvtInfo.McDauEta[i]   = MCDaughters[i]->eta();
      EvtInfo.McDauPhi[i]   = MCDaughters[i]->phi();
      EvtInfo.McDauPdgID[i] = MCDaughters[i]->pdgId();
   }

   //   if(METHandle.isValid()) {
   //     if(debug_>15) cout << "Get MET info\n";
   //     for( std::vector<pat::MET>::const_iterator it_met = METHandle->begin();
   //    it_met != METHandle->end(); it_met++ ) {

   //       EvtInfo.MET        = it_met->pt();
   //       EvtInfo.METPhi        = it_met->phi();
   //       EvtInfo.RawMET        = it_met->uncorrectedPt();
   //       EvtInfo.RawMETPhi     = it_met->uncorrectedPhi();

   //       EvtInfo.SumEt         = it_met->sumEt();
   //       EvtInfo.METSig        = it_met->mEtSig();
   //       EvtInfo.eLong         = it_met->e_longitudinal();
   //       EvtInfo.MaxHadTower   = it_met->maxEtInHadTowers();
   //       EvtInfo.MaxEmTower    = it_met->maxEtInEmTowers();
   //       EvtInfo.FracHad       = it_met->etFractionHadronic();
   //       EvtInfo.FracEm        = it_met->emEtFraction();

   //       const reco::GenMET * genmet = it_met->genMET();
   //       if (genmet!=NULL) {
   //         EvtInfo.GenMET        = genmet->pt();
   //         EvtInfo.GenMETPhi     = genmet->phi();
   //       }
   //     }
   //   }

   if( pfMETHandle.isValid() ) {
      if( debug_ > 15 ) { cout << "Get pfMET info\n"; }
      for( std::vector<pat::MET>::const_iterator it_pfmet = pfMETHandle->begin();
           it_pfmet != pfMETHandle->end(); it_pfmet++ ) {

         EvtInfo.PFMET           = it_pfmet->pt();
         EvtInfo.PFMETPhi        = it_pfmet->phi();
         EvtInfo.PFRawMET        = it_pfmet->uncorrectedPt();
         EvtInfo.PFRawMETPhi     = it_pfmet->uncorrectedPhi();

         EvtInfo.PFMETx          = it_pfmet->px(); //Uly 2011-04-04
         EvtInfo.PFMETy          = it_pfmet->py(); //Uly 2011-04-04

         EvtInfo.PFSumEt         = it_pfmet->sumEt();
         EvtInfo.PFMETSig        = it_pfmet->mEtSig();  //MET Significance = MET / std::sqrt(SumET)
         EvtInfo.PFMETRealSig    = it_pfmet->significance();  //real MET significance
         EvtInfo.PFMETlongitudinal    = it_pfmet->e_longitudinal();  //longitudinal component of the vector sum of energy over all object

         const reco::GenMET* genmet = it_pfmet->genMET();
         if ( !skipGenInfo_ && genmet != NULL ) {
            EvtInfo.PFGenMET        = genmet->pt();
            EvtInfo.PFGenMETPhi     = genmet->phi();
         }
      }
   }

   edm::Handle<std::vector<pat::MET> >          pfMETHandle_TempPlus;
   iEvent.getByLabel( "patType1CorrectedPFMetUnclusteredEnUp",  pfMETHandle_TempPlus );
   if( pfMETHandle_TempPlus.isValid() )
      for( std::vector<pat::MET>::const_iterator it_pfmet = pfMETHandle_TempPlus->begin();
           it_pfmet != pfMETHandle_TempPlus->end(); it_pfmet++ ) {
         EvtInfo.PFMETType1CorrectedPFMetUnclusteredEnUp           = it_pfmet->pt();
      }
   edm::Handle<std::vector<pat::MET> >          pfMETHandle_TempDown;
   iEvent.getByLabel( "patType1CorrectedPFMetUnclusteredEnDown",  pfMETHandle_TempDown );
   if( pfMETHandle_TempDown.isValid() )
      for( std::vector<pat::MET>::const_iterator it_pfmet = pfMETHandle_TempDown->begin();
           it_pfmet != pfMETHandle_TempDown->end(); it_pfmet++ ) {
         EvtInfo.PFMETType1CorrectedPFMetUnclusteredEnDown           = it_pfmet->pt();
      }




   edm::Handle<GenEventInfoProduct> GenEventInfoHandle;
   bool with_GenEventInfo = ( genevtlabel_.size() > 0 ) ? iEvent.getByLabel( genevtlabel_[0], GenEventInfoHandle ) : false;

   if ( with_GenEventInfo && GenEventInfoHandle->hasPDF() ) {
      EvtInfo.PDFid1   = GenEventInfoHandle->pdf()->id.first;
      EvtInfo.PDFid2   = GenEventInfoHandle->pdf()->id.second;
      EvtInfo.PDFx1    = GenEventInfoHandle->pdf()->x.first;
      EvtInfo.PDFx2    = GenEventInfoHandle->pdf()->x.second;
      EvtInfo.PDFscale = GenEventInfoHandle->pdf()->scalePDF;
      EvtInfo.PDFv1    = GenEventInfoHandle->pdf()->xPDF.first;
      EvtInfo.PDFv2    = GenEventInfoHandle->pdf()->xPDF.second;
   }

   // HLT: Booking trigger bits
   edm::Handle<TriggerResults> TrgResultsHandle;
   bool with_TriggerResults = ( hltlabel_.size() > 0 ) ? iEvent.getByLabel( hltlabel_[0], TrgResultsHandle ) : false;
   if ( with_TriggerResults ) {

      const edm::TriggerNames& TrgNames = iEvent.triggerNames( *TrgResultsHandle );
      EvtInfo.TrgCount = 0;
      for( int i = 0; i < N_TRIGGER_BOOKINGS; i++ ) {

         unsigned int TrgIndex = TrgNames.triggerIndex( TriggerBooking[i] );

         if ( TrgIndex == TrgNames.size() ) {
            EvtInfo.TrgBook[i] = -4; // The trigger path is not known in this event.
         } else if ( !TrgResultsHandle->wasrun( TrgIndex ) ) {
            EvtInfo.TrgBook[i] = -3; // The trigger path was not included in this event.
         } else if ( !TrgResultsHandle->accept( TrgIndex ) ) {
            EvtInfo.TrgBook[i] = -2; // The trigger path was not accepted in this event.
         } else if (  TrgResultsHandle->error ( TrgIndex ) ) {
            EvtInfo.TrgBook[i] = -1; // The trigger path has an error in this event.
         } else {
            EvtInfo.TrgBook[i] = +1; // It's triggered.
            EvtInfo.TrgCount++;
         }
      }
      EvtInfo.nHLT = TrgNames.size();
      std::pair<int, int>  psValueCombo;
      for( unsigned int i = 0; i < TrgNames.size(); i++ ) {
         EvtInfo.HLTbits[i] = ( TrgResultsHandle->accept( i ) == true ) ? 1 : 0;
         const std::string triggerName_ = TrgNames.triggerName( i );
         psValueCombo = hltConfig_.prescaleValues( iEvent, iSetup, triggerName_ );
         // 02/13/2015 : new function for prescale of L1/HLT (http://cmslxr.fnal.gov/source/HLTrigger/HLTcore/plugins/HLTEventAnalyzerAOD.cc)
         EvtInfo.HLTPrescaleFactor[i] = ( int )psValueCombo.second;

         //std::cout << "TriggerPath= " << TrgNames.triggerName(i) <<" Prescale= "<< psValueCombo.first*psValueCombo.second
         //    <<" "<< (int)psValueCombo.first<<" "<<(int)psValueCombo.second
         //    << std::endl;


         HLTmaplist_pr = HLTmaplist.find( TrgNames.triggerName( i ) );
         if( HLTmaplist_pr != HLTmaplist.end() ) {
            EvtInfo.HLTName2enum[i] = HLTmaplist_pr->second ;
         } else {
            EvtInfo.HLTName2enum[i] = -1;
         }
         // Print out Trigger table
         //std::cout << "trigger path= " << TrgNames.triggerName(i) << std::endl;
      }

   }
   ///**********************************************************
   //   L1 trigger and techincal trigger bits
   //***********************************************************
   edm::Handle< L1GlobalTriggerReadoutRecord > gtRecord;
   if( gtdigilabel_.size() > 0 ) { iEvent.getByLabel( gtdigilabel_[0], gtRecord ); }

   if( gtRecord.isValid() ) {
      DecisionWord dWord = gtRecord->decisionWord();
      if ( ! dWord.empty() ) { // if board not there this is zero
         // loop over dec. bit to get total rate (no overlap)
         for ( int i = 0; i < 128; ++i ) {
            //  if(dWord[i]!=0 && debug)cout << i << " " << dWord[i] << ": ";
            EvtInfo.L1[i] = dWord[i];
         }
      }
      TechnicalTriggerWord tw = gtRecord->technicalTriggerWord();
      if ( ! tw.empty() ) {
         // loop over dec. bit to get total rate (no overlap)
         for ( int i = 0; i < 64; ++i ) {
            //  if(tw[i]!=0 && debug) cout << i << "  " << tw[i] << ": ";
            EvtInfo.TT[i] = tw[i];
         }
      }
   }

   //by Dmitry Hits for filtering the real data
   //*******************************************************************
   //Add the number of tracks and fraction of high purity tracks in the track collection to the EvtInfo
   //*******************************************************************
   //
   //The desciption of tracks quality selection is here:
   //https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideFinalTrackSelectors
   //
   //Cuts which determine the high purity depend on the CMSSW release:
   //http://cmslxr.fnal.gov/lxr/source/RecoTracker/FinalTrackSelectors/python/selectHighPurity_cfi.py
   //
   //For details see CMS IN-2008/017
   //Currently, if the number of tracks > 10,
   //then 25% of HighPurity tracks have to be present in the event for it to be called good event

   int numhighpurity = 0;
   if( !TurnOffInCMSSW73x )
      if( TrackHandle.isValid() && !TrackHandle.failedToGet() && TrackHandle->size() > 0 ) {

         reco::TrackBase::TrackQuality _trackQuality = reco::TrackBase::qualityByName( "highPurity" );

         edm::View<reco::Track>::const_iterator itk = tracks.begin();
         edm::View<reco::Track>::const_iterator itk_e = tracks.end();
         for( ; itk != itk_e; ++itk ) {
            // std::cout << "HighPurity?  " << itk->quality(_trackQuality) << std::endl;
            if( itk->quality( _trackQuality ) ) { numhighpurity++; }
         }
         EvtInfo.NofTracks = TrackHandle->size();
         EvtInfo.HighPurityFraction = ( float )numhighpurity / ( float )TrackHandle->size();
      }

   if( debug_ > 11 ) {
      for( unsigned i = 0; i < lepcollections_.size(); i++ ) {
         cout << "Lepton Collection " << i << "(" << lepcollections_[i] << "): size " << LepInfo[i].Size << endl;
         for( int j = 0; j < LepInfo[i].Size; j++ )
         { cout << "  Lep " << j << " type,pt,eta,phi " << LepInfo[i].LeptonType[j] << "," << LepInfo[i].Pt[j] << "," << LepInfo[i].Eta[j] << "," << LepInfo[i].Phi[j] << endl; }
      }
   }

   if( debug_ > 5 ) { cout << "Fill tree with all information\n"; }
   root->Fill();
   if( debug_ > 10 ) { cout << "Filled event information: Run " << EvtInfo.RunNo << " Event " << EvtInfo.EvtNo << endl; }

   if( debug_ > 11 ) {
      root->Show( -1, 999 );
      for( unsigned i = 0; i < lepcollections_.size(); i++ ) {
         cout << "After fill, Lepton Collection " << i << "(" << lepcollections_[i] << "): size " << LepInfo[i].Size << endl;
         for( int j = 0; j < LepInfo[i].Size; j++ )
         { cout << "  Lep " << j << " type,pt,eta,phi " << LepInfo[i].LeptonType[j] << "," << LepInfo[i].Pt[j] << "," << LepInfo[i].Eta[j] << "," << LepInfo[i].Phi[j] << endl; }
      }
   }

}


//define this as a plug-in
DEFINE_FWK_MODULE( bprimeKit );

