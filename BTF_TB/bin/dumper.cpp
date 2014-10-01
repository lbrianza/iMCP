/*******************************************************************************
-
    compile with --> c++ -o dumper dumper.cpp `root-config --cflags --glibs`
    run with --> ./dumper cfg_file inputFolder nChannels outputName

*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream> 
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <iomanip>
#include <ctime>
#include <map>
#include <math.h>

#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TChain.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TMath.h"
#include "TString.h"
#include "TClass.h"
#include "TApplication.h"

#include "../include/analysis_tools.h"
#include "../include/init_tree_BTF.h"
#include "../include/histo_func.h"

//*******MAIN*******************************************************************
int main (int argc, char** argv)
{  

    //--------Read Options--------------------------------
    ifstream inputCfg (argv[1], ios::in);
    std::string inputFolder = argv[2];
    int nChannels = atoi (argv[3]);
    std::string outputFile = argv[4];

    //---------output tree----------------
    TFile* outROOT = TFile::Open(("ntuples/reco_"+outputFile+".root").c_str(),"recreate");  
    outROOT->cd();
    TTree* outTree = new TTree("reco_tree", "reco_tree");
    outTree->SetDirectory(0);
    SetOutTree(outTree);

    int runN=0, chNumber=0, HVtemp=0;
    int PC=0, trigger=0;
    std::string name;

    std::map<int, int> PCOn;
    std::map<int, int> HVVal; 
    std::map<int, int> triggerOn; 
    std::map<int, std::string> MCPName; 
    std::vector<int> runNumber;
    runNumber.clear();

    //-------start to read the cfg file--------
    while(!inputCfg.eof())  
    {
      PCOn.clear();
      HVVal.clear();
      triggerOn.clear();
      MCPName.clear();

      //-----fill maps--------
      for (int count=0; count<nChannels; count++)   //read exactly nChannels lines of the cfg file -> be careful to give the right number in input!!!!
	{
	  inputCfg >> runN >> chNumber >> HVtemp >> PC >> trigger >> name;

	  PCOn.insert(std::make_pair(chNumber,PC)); 
	  HVVal.insert(std::make_pair(chNumber,HVtemp)); 
	  triggerOn.insert(std::make_pair(chNumber,trigger)); 
	  MCPName.insert(std::make_pair(chNumber,name)); 
	}
      runNumber.push_back(runN);
      
      //-----Definitions
      vector<float> digiCh[9];
      float timeCF[9], timeAM[9];
      float baseline[9];
      float intBase[9], intSignal[9], intSignalCF[9], ampMax[9];
      ///int fibreX[8], hodoYchannels[8];
      //---Chain
      TChain* chain = new TChain("eventRawData");
      InitTree(chain);
      //-----Read raw data tree-----------------------------------------------
      char iRun_str[40];
      sprintf(iRun_str, "%s/run_IMCP_%d_*.root", (inputFolder).c_str(), runNumber.back());
      chain->Add(iRun_str);
      cout << "Reading:  "<<iRun_str << endl;
      //-----Data loop--------------------------------------------------------
      for(int iEntry=0; iEntry<chain->GetEntries(); iEntry++)
        {
	    if(iEntry % 1000 == 0)
		cout << "read entry: " << iEntry << endl;
            //-----Unpack data--------------------------------------------------
            //---always clear the std::vector !!!

            for(int iCh=0; iCh<nChannels; iCh++)
            {
                digiCh[iCh].clear();
            }
            //---Read the entry
            chain->GetEntry(iEntry);
            //---Read SciFront ADC value and set the e- multiplicity 

	    for(int iCh=0; iCh<nAdcChannels; iCh++)
		{
                    if(adcBoard[iCh] == 1 && adcChannel[iCh] == 0) 
                        sci_front_adc = adcData[iCh];
		    if(adcBoard[iCh] == 1 && adcChannel[iCh] >= HODOX_ADC_START_CHANNEL &&
		       adcChannel[iCh] <= HODOX_ADC_END_CHANNEL)
			fibreX[(adcChannel[iCh]-HODOX_ADC_START_CHANNEL)] = adcData[iCh];
		    if(adcBoard[iCh] == 1 && adcChannel[iCh] >= HODOY_ADC_START_CHANNEL &&
		       adcChannel[iCh] <= HODOY_ADC_END_CHANNEL)
			fibreY[(adcChannel[iCh]-HODOY_ADC_START_CHANNEL)] = adcData[iCh];
		}
	    
		//---Read digitizer samples
             for(int iSample=0; iSample<nDigiSamples; iSample++)
	       {
		 if(MCPName.at(digiChannel[iSample]) == "Planacon")
			digiCh[digiChannel[iSample]].push_back(-digiSampleValue[iSample]);
		 else
                        digiCh[digiChannel[iSample]].push_back(digiSampleValue[iSample]);
	       }
                //---loop over MPC's channels
             for(int iCh=0; iCh<nChannels; iCh++)
                {
                    baseline[iCh]=SubtractBaseline(5, 25, &digiCh[iCh]);
                    timeCF[iCh]=TimeConstFrac(47, 500, &digiCh[iCh], 0.5);
                    timeAM[iCh]=TimeConstFrac(47, 500, &digiCh[iCh], 1);
                    int t1 = (int)(timeCF[iCh]/0.2) - 3;
                    int t2 = (int)(timeCF[iCh]/0.2) + 17;
                    intBase[iCh] = ComputeIntegral(26, 46, &digiCh[iCh]);
                    if(t1 > 50 && t1 < 1024 && t2 > 50 && t2 < 1024)
                    {
                        ampMax[iCh] = AmpMax(t1, t2, &digiCh[iCh]);
                        intSignal[iCh] = ComputeIntegral(t1, t2, &digiCh[iCh]);
			intSignalCF[iCh] = ComputeIntegral((int)(timeAM[iCh]/0.2)-5, (int)(timeAM[iCh]/0.2), &digiCh[iCh]);
                    }
                    else
                    {
                        ampMax[iCh] = AmpMax(0, 1024, &digiCh[iCh]);
                        intSignal[iCh] = ComputeIntegral(50, 70, &digiCh[iCh]);
			intSignalCF[iCh] = 2000;
                    }
                }
             //-----Dump WaveForm Infos------------------------------------------------

	     //dump ntuple
	     for (int iCh=0; iCh<nChannels; iCh++)
		  {
		    time_CF[iCh]   = timeCF[iCh];
		    amp_max[iCh]   = -ampMax[iCh];
		    charge[iCh]    = -intSignal[iCh];
		    charge_CF[iCh] = -intSignalCF[iCh];
		    baseline[iCh]  = -intBase[iCh];
	    
		    isPCOn[iCh]      = PCOn.at(iCh);
		    HV[iCh]          = HVVal.at(iCh);
		    isTriggerOn[iCh] = triggerOn.at(iCh);
		  }

      	     run_id = runNumber.back();
	     outTree->Fill();    
	}     
        //---Get ready for next run
        chain->Delete();
    }

    //-----close everything-----------------------------------------------------
    outTree->Write();
    outROOT->Close();
    
//---------Done-----------------------------------------------------------------
}

        
