#if !defined(__CINT__) || defined(__CLING__)
#include "AliAODInputHandler.h"
#include "AliAnalysisAlien.h"
#include "AliPhysicsSelectionTask.h"
#include "AliMultSelectionTask.h"
#include "AliAnalysisManager.h"
#include "AliMCEventHandler.h"
#include "AliAnalysisTaskNucleiYieldtemp.h"
#include "AliESDInputHandler.h"
#endif

void run_KIAF(const char* dataset = "test1.list",
              const char* option = "AOD_Dev") {
  gSystem->Load("libTree.so");
  gSystem->Load("libGeom.so");
  gSystem->Load("libVMC.so");
  gSystem->Load("libPhysics.so");
  gSystem->Load("libSTEERBase. so");
  gSystem->Load("libESD.so");
  gSystem->Load("libANALYSIS.so");
  gSystem->Load("libOADB.so");
  gSystem->Load("libANALYSISalice.so");
  gSystem->Load("libpythia6_4_21.so");

  gSystem->Setenv("alien_CLOSE_SE", "working_disk_SE");
  // ROOT 6 MODE
  // add aliroot indlude path
  gInterpreter->ProcessLine(
      Form(".include %s/include", gSystem->ExpandPathName("$ROOTSYS")));
  gInterpreter->ProcessLine(
      Form(".include %s/include", gSystem->ExpandPathName("$ALICE_ROOT")));
  gInterpreter->ProcessLine(
      Form(".include %s/include", gSystem->ExpandPathName("$ALICE_PHYSICS")));

  bool isaa = kFALSE;
  bool ismc = kFALSE;
  bool isDev = kFALSE;
  bool setmixing = kFALSE;
  bool isaod = kFALSE;
  int nmix = 10;
  bool highmult = kFALSE;
  TString foption = option;
  const char* taskname = "Sigma1385";
  const char* suffix = "MB_";
  if (foption.Contains("MC"))
    ismc = kTRUE;
  if (foption.Contains("Dev"))
    isDev = kTRUE;
  if (foption.Contains("AOD"))
    isaod = true;

  // analysis manager
  AliAnalysisManager* mgr =
      new AliAnalysisManager(Form("%s%s", taskname, option));
  AliInputEventHandler* handler;
  if (isaod)
    handler = new AliAODInputHandler();
  else
    handler = new AliESDInputHandler();

  // handler->SetNeedField(1);
  mgr->SetInputEventHandler(handler);

  if (ismc) {
    AliMCEventHandler* mcHandler = new AliMCEventHandler();
    mgr->SetMCtruthEventHandler(mcHandler);
  }
  //
  // Physics Selection
  AliPhysicsSelectionTask* physSelTask =
      reinterpret_cast<AliPhysicsSelectionTask*>(gInterpreter->ExecuteMacro(
          Form("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C(%d)",
               ismc)));
  // Multiplicity selection
  AliMultSelectionTask* MultSlection =
      reinterpret_cast<AliMultSelectionTask*>(gInterpreter->ExecuteMacro(
          "$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/"
          "AddTaskMultSelection.C"));
  MultSlection->SetAddInfo(kTRUE);
  MultSlection->SetSelectedTriggerClass(AliVEvent::kAny);
  // MultSlection->SetAlternateOADBforEstimators("LHC16k"); //if needed
  // PID response
  AliAnalysisTask* fPIDResponse =
      reinterpret_cast<AliAnalysisTask*>(gInterpreter->ExecuteMacro(
          Form("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C(%d)", ismc)));
  if (isDev) {
    gInterpreter->LoadMacro("AliAnalysisTaskNucleiYieldtemp.cxx+g");
    // AliAnalysisTaskXi1530temp *myTask =
    // reinterpret_cast<AliAnalysisTaskXi1530temp*>(gInterpreter->ExecuteMacro(Form("AddTaskXi1530.c(\"%s\",\"%s\",%i,%d,%d,%d,%d)",taskname,option,nmix,highmult,isaa,ismc,setmixing)));

    AliAnalysisTaskNucleiYieldtemp* myTask =
        reinterpret_cast<AliAnalysisTaskNucleiYieldtemp*>(
            gInterpreter->ExecuteMacro("AddTaskNucleiYieldtemp_LHC15o.C(kFALSE,AliPID::kHe3,1000020030,\"he3\")"));
    double p[4] = {1.,1.,74.,63.};
    myTask->fTOFfunctionPars.Set(4,p);
    //float pt[17] = {
    //1.1,1.5,1.95, 2.45, 2.95, 3.45, 3.95, 4.45, 4.95, 5.45,
    //5.95, 6.45, 6.95,8.0,9.0,10.0
    //};
    float pt[17] = {0.95, 1.45, 1.95, 2.45, 2.95, 3.45, 3.95, 4.45, 4.95, 5.45, 5.95, 6.45 ,6.95,7.45, 7.95, 8.95, 9.95};
    float corr[3] = {-1.67762e-03f,2.76265e-01,-1.72861e-01};
    myTask->fPtCorrectionM.Set(3,corr);
    myTask->fPtCorrectionA.Set(3,corr);
    myTask->SetEnablePtCorrection(false);
    myTask->SetPtBins(16,pt);
    myTask->SetRequireTPCpidSigmas(4.5);
    float resolution = 0.05871;
    float bethe[5]={-166.11733,-0.11020473,0.10851357,2.7018593,-0.087597824};
    myTask->SetCustomTPCpid(bethe,resolution);
    myTask->fEventCut.SetManualMode();
    myTask->fEventCut.SetupPbPb2018();
    myTask->fINT7intervals = {10.,30.,50.,90.};

    // For LHC18 analysis
    myTask->SetRequireDeadZoneWidth(3.0);
    myTask->SetRequireCutGeoNcrNclLength(130);
    myTask->SetRequireCutGeoNcrNclGeom1Pt(1.5);
    myTask->SetRequireCutGeoNcrNclFractionNcr(0.85);
    myTask->SetRequireCutGeoNcrNclFractionNcl(0.7);

    // Additional TOF Clean Up
    myTask->fOptionalTOFcleanup = 5.0;

  } else {
    AliAnalysisTaskNucleiYield* myTask =
      reinterpret_cast<AliAnalysisTaskNucleiYield*>(
          gInterpreter->ExecuteMacro("$ALICE_PHYSICS/PWGLF/NUCLEX/Nuclei/NucleiPbPb/AddTaskNucleiYield_LHC15o.C(kFALSE,AliPID::kHe3,1000020030,\"he3\")"));
    double p[4] = {1.,1.,74.,63.};
    myTask->fTOFfunctionPars.Set(4,p);
    //float pt[17] = {
    //1.1,1.5,1.95, 2.45, 2.95, 3.45, 3.95, 4.45, 4.95, 5.45,
    //5.95, 6.45, 6.95,8.0,9.0,10.0
    //};
    float pt[17] = {0.95, 1.45, 1.95, 2.45, 2.95, 3.45, 3.95, 4.45, 4.95, 5.45, 5.95, 6.45 ,6.95,7.45, 7.95, 8.95, 9.95};
    float corr[3] = {-1.67762e-03f,2.76265e-01,-1.72861e-01};
    myTask->fPtCorrectionM.Set(3,corr);
    myTask->fPtCorrectionA.Set(3,corr);
    myTask->SetEnablePtCorrection(false);
    myTask->SetPtBins(16,pt);
    myTask->SetRequireTPCpidSigmas(4.5);
    float resolution = 0.05871;
    float bethe[5]={-166.11733,-0.11020473,0.10851357,2.7018593,-0.087597824};
    myTask->SetCustomTPCpid(bethe,resolution);
    myTask->fEventCut.SetManualMode();
    myTask->fEventCut.SetupPbPb2018();
    myTask->fINT7intervals = {10.,30.,50.,90.};

    // For LHC18 analysis
    myTask->SetRequireDeadZoneWidth(3.0);
    myTask->SetRequireCutGeoNcrNclLength(130);
    myTask->SetRequireCutGeoNcrNclGeom1Pt(1.5);
    myTask->SetRequireCutGeoNcrNclFractionNcr(0.85);
    myTask->SetRequireCutGeoNcrNclFractionNcl(0.7);

    // Additional TOF Clean Up
    // myTask->fOptionalTOFcleanup = 5.0;
  }
  if (!mgr->InitAnalysis())
    return;
  mgr->PrintStatus();

  // start analysis
  Printf("Starting Analysis....");

  // TChain* chain = CreateESDChain(dataset,-1);
  TChain* chain = new TChain("ESDTree");
  std::stringstream esdChain;
  if (isaod) {
    esdChain << ".x " << gSystem->Getenv("ALICE_PHYSICS")
             << "/PWG/EMCAL/macros/CreateAODChain.C(";
    esdChain << "\"" << dataset << "\", -1);";
  } else {
    esdChain << ".x " << gSystem->Getenv("ALICE_PHYSICS")
             << "/PWG/EMCAL/macros/CreateESDChain.C(";
    esdChain << "\"" << dataset << "\", -1);";
  }
  chain = reinterpret_cast<TChain*>(gROOT->ProcessLine(esdChain.str().c_str()));
  mgr->StartAnalysis("local", chain);
}
