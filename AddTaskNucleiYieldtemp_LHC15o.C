/// \file AddTaskNucleiYield.C
/// \brief Simple macro to add the task to a grid job
///
/// The task is here added several times to analyse different particle species and to investigate
/// different set of cuts in only one run.
///
/// \author Maximiliano Puccio <maximiliano.puccio@cern.ch>, University and INFN Torino
/// \date Feb 19th, 2015

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Rtypes.h>
#include <TString.h>
#include "AliAnalysisTaskNucleiYieldtemp.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisDataContainer.h"
#include "AliPID.h"
#endif

AliAnalysisTaskNucleiYieldtemp* AddTaskNucleiYieldtemp_LHC15o(Bool_t isMC = kFALSE,
    AliPID::EParticleType part = AliPID::kDeuteron,
    Int_t pdgCode = 1000010020,
    TString tskname = "deuteron",
    TString suffix = "") {

  // Get the current analysis manager
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Error("AddTaskNucleiYield", "No analysis manager found.");
    return 0x0;
  }

  // Check the analysis type using the event handlers connected to the analysis manager.
  if (!mgr->GetInputEventHandler()) {
    ::Error("AddTaskNucleiYield", "This task requires an input event handler");
    return 0x0;
  }

  tskname.Append(Form("%s",suffix.Data()));

  AliAnalysisTaskNucleiYieldtemp *deu = new AliAnalysisTaskNucleiYieldtemp(tskname);

  deu->SetParticleType(part);
  deu->SetPDG(pdgCode);
  deu->SetIsMC(isMC);
  deu->SetDCABins(80,-0.5,0.5);

  //float p[5] = {7.25105e-01,4.99820e+01,2.35714e-10,2.49196e+00,1.41570e+01};
  float cent[11] = {0.f,5.f,10.f,20.f,30.f,40.f,50.f,60.f,70.f,80.f,90.f};
  deu->SetCentBins(10, cent);
  deu->SetUseFlattening(false);

  float dcabins[53] = {
    -1.30,-1.20,-1.10,-1.00,-0.90,-0.80,-0.70,-0.60,-0.50,-0.40,
    -0.35,-0.30,-0.25,-0.20,-0.15,-0.12,-0.10,-0.09,-0.08,-0.07,
    -0.06,-0.05,-0.04,-0.03,-0.02,-0.01, 0.00, 0.01, 0.02, 0.03,
     0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10, 0.12, 0.15, 0.20,
     0.25, 0.30, 0.35, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 1.00,
     1.10, 1.20, 1.30
  };
  deu->SetDCABins(52,dcabins);
  double p[4] = {1., 1., 74., 63.};
  deu->fTOFfunctionPars.Set(4, p);
  // float pt[17] = {
  // 1.1,1.5,1.95, 2.45, 2.95, 3.45, 3.95, 4.45, 4.95, 5.45,
  // 5.95, 6.45, 6.95,8.0,9.0,10.0
  //};
  float pt[17] = {0.95, 1.45, 1.95, 2.45, 2.95, 3.45, 3.95, 4.45, 4.95,
                  5.45, 5.95, 6.45, 6.95, 7.45, 7.95, 8.95, 9.95};
  float corr[3] = {-1.67762e-03f, 2.76265e-01, -1.72861e-01};
  deu->fPtCorrectionM.Set(3, corr);
  deu->fPtCorrectionA.Set(3, corr);
  deu->SetEnablePtCorrection(false);
  deu->SetPtBins(16, pt);
  deu->SetRequireTPCpidSigmas(4.5);
  float resolution = 0.05871;
  float bethe[5] = {-166.11733, -0.11020473, 0.10851357, 2.7018593,
                    -0.087597824};
  deu->SetCustomTPCpid(bethe, resolution);

  // deu->SetRequireTPCrecPoints(100);
  deu->SetRequireMaxDCAz(0.1);

  deu->SetTPCActiveLengthCut(false);  // This will add TPCActiveTrackCut but
                                       // not applying in the AcceptTrack()
  deu->SaveTrees();

  mgr->AddTask(deu);

  TString output = "AnalysisResults.root";
  AliAnalysisDataContainer *deuCont = mgr->CreateContainer(Form("mpuccio_%s",tskname.Data()),
      TList::Class(),
      AliAnalysisManager::kOutputContainer,
      output.Data());
  AliAnalysisDataContainer *deuCont1 = mgr->CreateContainer(Form("RTree%s",tskname.Data()),
      TTree::Class(),
      AliAnalysisManager::kOutputContainer,
      output.Data());
  AliAnalysisDataContainer *deuCont2 = mgr->CreateContainer(Form("STree%s",tskname.Data()),
      TTree::Class(),
      AliAnalysisManager::kOutputContainer,
      output.Data());
  mgr->ConnectInput  (deu,  0, mgr->GetCommonInputContainer());
  mgr->ConnectOutput (deu,  1, deuCont);
  mgr->ConnectOutput (deu,  2, deuCont1);
  mgr->ConnectOutput (deu,  3, deuCont2);
  return deu;
}

