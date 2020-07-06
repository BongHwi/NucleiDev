#if !defined(__CINT__) || defined(__CLING__)
#include "AliAODInputHandler.h"
#include "AliESDInputHandler.h"
#include "AliAnalysisAlien.h"
#include "AliPhysicsSelectionTask.h"
#include "AliMultSelectionTask.h"
#include "AliAnalysisManager.h"
#include "AliMCEventHandler.h"
#include "AliAnalysisTaskNucleiYield.h"
#include "AliAnalysisTaskNucleiYieldtemp.h"
#endif
vector<Int_t> LHC18q = {
    295585, 295586, 295588, 295589, 295610, 295611, 295612, 295615, 295666, 295667, 295668, 295673, 295675, 295676, 295712, 295714, 295717, 295718, 295719, 295721, 295723, 295725, 295754, 295755, 295758, 295759, 295762, 295763, 295786, 295788, 295791, 295816, 295818, 295819, 295822, 295825, 295826, 295829, 295831, 295853, 295854, 295855, 295856, 295859, 295860, 295861, 295909, 295910, 295913, 295936, 295937, 295941, 295942, 296016, 296060, 296062, 296063, 296065, 296066, 296074, 296123, 296132, 296133, 296134, 296135, 296142, 296143, 296191, 296192, 296194, 296195, 296196, 296197, 296198, 296240, 296241, 296242, 296243, 296244, 296246, 296247, 296269, 296270, 296273, 296279, 296280, 296303, 296304, 296309, 296312, 296375, 296376, 296377, 296378, 296379, 296380, 296381, 296383, 296414, 296415, 296419, 296420, 296423, 296424, 296433, 296472, 296509, 296510, 296511, 296512, 296516, 296547, 296548, 296549, 296550, 296551, 296552, 296553, 296594, 296615, 296616, 296618, 296619, 296621, 296622, 296623};  // for run
// vector<Int_t> LHC18q = {295585};  // for run


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

  // gSystem->Setenv("alien_CLOSE_SE", "working_disk_SE");
  // ROOT 6 MODE
  // add aliroot indlude path
  gInterpreter->ProcessLine(
      Form(".include %s/include", gSystem->ExpandPathName("$ROOTSYS")));
  gInterpreter->ProcessLine(
      Form(".include %s/include", gSystem->ExpandPathName("$ALICE_ROOT")));
  gInterpreter->ProcessLine(
      Form(".include %s/include", gSystem->ExpandPathName("$ALICE_PHYSICS")));
  bool isGrid = true;
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
    gInterpreter->LoadMacro("AliAnalysisTaskNucleiYieldtemp.cxx++g");
    // AliAnalysisTaskXi1530temp *myTask =
    // reinterpret_cast<AliAnalysisTaskXi1530temp*>(gInterpreter->ExecuteMacro(Form("AddTaskXi1530.c(\"%s\",\"%s\",%i,%d,%d,%d,%d)",taskname,option,nmix,highmult,isaa,ismc,setmixing)));

    AliAnalysisTaskNucleiYieldtemp* myTask =
        reinterpret_cast<AliAnalysisTaskNucleiYieldtemp*>(
            gInterpreter->ExecuteMacro("AddTaskNucleiYieldtemp_LHC15o.C(kFALSE,AliPID::kHe3,1000020030,\"he3\")"));

  } else {
    AliAnalysisTaskNucleiYield* myTask =
      reinterpret_cast<AliAnalysisTaskNucleiYield*>(
          gInterpreter->ExecuteMacro("$ALICE_PHYSICS/PWGLF/NUCLEX/Nuclei/NucleiPbPb/AddTaskNucleiYield_LHC15o.C(kFALSE,AliPID::kHe3,1000020030,\"he3\")"));
    double p[4] = {1., 1., 74., 63.};
  myTask->fTOFfunctionPars.Set(4, p);
  // float pt[17] = {
  // 1.1,1.5,1.95, 2.45, 2.95, 3.45, 3.95, 4.45, 4.95, 5.45,
  // 5.95, 6.45, 6.95,8.0,9.0,10.0
  //};
  float pt[17] = {0.95, 1.45, 1.95, 2.45, 2.95, 3.45, 3.95, 4.45, 4.95,
                  5.45, 5.95, 6.45, 6.95, 7.45, 7.95, 8.95, 9.95};
  float corr[3] = {-1.67762e-03f, 2.76265e-01, -1.72861e-01};
  myTask->fPtCorrectionM.Set(3, corr);
  myTask->fPtCorrectionA.Set(3, corr);
  myTask->SetEnablePtCorrection(false);
  myTask->SetPtBins(16, pt);
  myTask->SetRequireTPCpidSigmas(4.5);
  float resolution = 0.05871;
  float bethe[5] = {-166.11733, -0.11020473, 0.10851357, 2.7018593,
                    -0.087597824};
  myTask->SetCustomTPCpid(bethe, resolution);

  // myTask->SetRequireTPCrecPoints(100);
  myTask->SetRequireMaxDCAz(0.1);

  myTask->SetTPCActiveLengthCut(false);  // This will add TPCActiveTrackCut but
                                       // not applying in the AcceptTrack()
  myTask->SaveTrees();
  }
  if (!mgr->InitAnalysis())
    return;
  mgr->PrintStatus();

  // start analysis
  Printf("Starting Analysis....");
  if(!isGrid){
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
  else{
    const char* taskname = "NucleiTest";
    const char* option = "LHC18q_filterred";
    const char* gridmode = "full";
    // create the alien handler and attach it to the manager
        AliAnalysisAlien* plugin = new AliAnalysisAlien();
        plugin->AddIncludePath(
            "-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include "
            "-I$ALICE_PHYSICS/include");
        // plugin->Load("libpythia6_4_21.so");
        plugin->SetAnalysisSource("AliAnalysisTaskNucleiYieldtemp.cxx");
        plugin->SetAdditionalLibs(
            "AliAnalysisTaskNucleiYieldtemp.cxx AliAnalysisTaskNucleiYieldtemp.h");
        plugin->SetAliPhysicsVersion("vAN-20200701_ROOT6-1");
        plugin->SetAPIVersion("V1.1x");
        if (!ismc)
            plugin->SetRunPrefix("000");
        plugin->SetGridDataDir("/alice/data/2018/LHC18q");
        plugin->SetDataPattern("filter_pass3/AOD/*/AliAOD.root");
        for (auto run : LHC18q)
            plugin->AddRunNumber(run);
      plugin->SetSplitMaxInputFileNumber(100);
        plugin->SetExecutable(Form("%s%s.sh", taskname, option));
        plugin->SetTTL(40000);
        plugin->SetJDLName(Form("%s%s.jdl", taskname, option));
        plugin->SetKeepLogs(kTRUE);
        // plugin->SetMaxMergeStages(3);
        plugin->SetMaxMergeFiles(100);
        plugin->SetMergeViaJDL(kTRUE);
        plugin->SetOneStageMerging(kFALSE);
        plugin->SetCheckCopy(kFALSE);
        // plugin->SetNrunsPerMaster(kFALSE);
        plugin->SetUseSubmitPolicy(kTRUE);

        plugin->SetGridWorkingDir(Form("%s%s", taskname, option));
        plugin->SetGridOutputDir("out");

        // plugin->SetOutputToRunNo(kTRUE);
        plugin->SetOverwriteMode(kTRUE);
        plugin->SetUser("blim");
        mgr->SetGridHandler(plugin);

        plugin->SetRunMode(gridmode);
        plugin->SetNtestFiles(1);

        mgr->StartAnalysis("grid");
  }
}
