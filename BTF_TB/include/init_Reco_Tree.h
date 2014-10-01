#ifndef __init_Reco_Tree__
#define __init_Reco_Tree__

#include "TTree.h"
#include "TChain.h"

    // Declaration of leaf types
    float                 time_CF[10];   
    float                 amp_max[10];   
    float                 charge[10];   
    float                 baseline[10];   
    int                   fibreX[8];   
    int                   fibreY[8];   
    int                   sci_front_adc;   
    int                   run_id;   
    int                   isPCOn[10];   
    int                   HV[10];   
    int                   isTriggerOn[10];   

    // List of branches
    TBranch        *b_time_CF;   //!
    TBranch        *b_amp_max;   //!
    TBranch        *b_baseline;   //!
    TBranch        *b_charge;   //!
    TBranch        *b_fibreX;   //!
    TBranch        *b_fibreY;   //!
    TBranch        *b_sci_front_adc;   //!
    TBranch        *b_run_id;   //!
    TBranch        *b_isPCOn;   //!
    TBranch        *b_HV;   //!
    TBranch        *b_isTriggerOn;   //!
   
    void InitRecoTree(TTree* nt)
    {
      nt->SetBranchAddress("time_CF", &time_CF, &b_time_CF);
        nt->SetBranchAddress("amp_max", &amp_max, &b_amp_max);
        nt->SetBranchAddress("charge", &charge, &b_charge);
        nt->SetBranchAddress("baseline", &baseline, &b_baseline);
        nt->SetBranchAddress("fibreX", &fibreX, &b_fibreX);
        nt->SetBranchAddress("fibreY", &fibreY, &b_fibreY);
        nt->SetBranchAddress("sci_front_adc", &sci_front_adc, &b_sci_front_adc);
        nt->SetBranchAddress("run_id", &run_id, &b_run_id);
        nt->SetBranchAddress("isPCOn", &isPCOn, &b_isPCOn);
        nt->SetBranchAddress("HV", &HV, &b_HV);
        nt->SetBranchAddress("isTriggerOn", &isTriggerOn, &b_isTriggerOn);
    }

#endif
