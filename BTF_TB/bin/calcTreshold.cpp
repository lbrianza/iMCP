/*******************************************************************************                                                                                  
THIS PROGRAM COMPUTES THE TRESHOLDS FOR EACH MCP USING A FIXED S/B VALUE (IN INPUT)
                                                                                                    
    compile with --> c++ -o calcTreshold bin/calcTreshold.cpp `root-config --cflags --glibs`                                                                
    run with --> ./calcTreshold ntuples/reco_prova.root 1000 9 2000  
                          where the arguments are: -inputTree -S/B treshold -number of channels -HV treshold (analyze only MCPs with HV>treshold)
                                                                                                                                                                  
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
#include "TCanvas.h"

//#include "../include/analysis_tools.h"
#include "../include/init_Reco_Tree.h"
#include "../include/histo_func.h"
#include "../include/MCPMap.h"

using namespace std;

//*******MAIN*******************************************************************                                                                                  
int main (int argc, char** argv)
{
  std::cout<<"-----------COMPUTING CHARGE TRESHOLDS FOR EACH CHANNEL------------"<<std::endl;
  std::cout<<"NB: MCP is analyzed only if HV>treshold and PC is ON---------"<<std::endl;

  int SBTreshold=1000;
  int nChannels=9;
  int HVtresh=2000;
  int doPlot=0;

  std::string inputFileName = argv[1];  
  std::cout<<"\nReading file: "<<inputFileName<<std::endl;

  if (argc > 2) {
    SBTreshold = atoi(argv[2]);  //desired treshold (e.g. S/B=1000)
    nChannels = atoi(argv[3]);  //nChannels in the reco tree
    HVtresh = atoi(argv[4]);   //HV treshold
    doPlot = atoi(argv[5]);   //do plots?
  }

  TFile *inputFile = new TFile ((inputFileName).c_str()); 
  TTree *inputTree = (TTree*)inputFile->Get("reco_tree");
  InitRecoTree(inputTree);  

  Fill_inverted_MCPList();

  std::string outputFileName = "cfg/treshold.txt";
  std::ofstream outputFile ((outputFileName).c_str(), std::ofstream::out);

  //-----get the run list and the list of channel to analyze------
  int prevRun=0;
  std::vector<int> goodCh;
  goodCh.clear();

  std::cout<<"Channels to analyze: ";
  inputTree->GetEntry(1);
    {
      for (int iCh=0; iCh<nChannels; iCh++)  //scan all the channels -> save only the channels with PC on and HV>treshold
        {
          if (isPCOn[iCh]!=0 && HV[iCh]>=HVtresh) {
	    goodCh.push_back(iCh);
	    std::cout<<iCh<<" ";
          }
        }
    }
  

  //------analyze the good channels and compute the charge tresholds--------
  std::cout<<"\n--------------------------------\n-->OK, now computing tresholds:"<<std::endl;
  for (int iCh=0; iCh<nChannels; iCh++)
	{
	  //---is it a good channel? check: if not, go to next channel----
	  std::vector<int>::iterator it;
	  it = find (goodCh.begin(), goodCh.end(), iCh);
	  if (it == goodCh.end())    continue;

	  char hSName[100], hBName[100];
	  sprintf(hSName, "hS_%d", iCh);
	  sprintf(hBName, "hB_%d", iCh);

	  TH1F *hS = new TH1F(hSName,hSName, 1000, 0, 10000);
	  TH1F *hB = new TH1F(hBName,hBName, 1000, 0, 10000);
	      
	  char hSDraw[100], hBDraw[100];
	  sprintf(hSDraw, "charge[%d]>>%s", iCh, hSName);
	  sprintf(hBDraw, "baseline[%d]>>%s", iCh, hBName);

	  char cut[300];
	  sprintf(cut, "sci_front_adc>500 && sci_front_adc<1500 && isPCOn[0]!=0 && HV>=%d", HVtresh); 

	  inputTree->Draw(hSDraw,cut,"goff");
	  inputTree->Draw(hBDraw,cut,"goff");

	  for (int iBin=1; iBin<10000; iBin++)
	    {
	       double S = hS->Integral(iBin, 10000);
	       double B = hB->Integral(iBin, 10000);
	       if (S/B > SBTreshold) 
	         {
		   std::cout<<" Channel: "<<iCh<<" ("<<inverted_MCPList.at(iCh)<<")   Treshold Value: "<<iBin*10<<std::endl;
	           outputFile<<iCh<<" "<<iBin*10<<std::endl;
	           break;
	         }
       
	    }
	  if (doPlot==1) {
	    char canvasName[100];
	    sprintf(canvasName, "plots/tresholdScan/charge_channel_%d.pdf", iCh);
	    TCanvas *c1 = new TCanvas ();
	    c1->cd();
	    hS->SetLineColor(2);
	    hB->SetLineColor(4);
	    hS->Draw();
	    hB->Draw("same");
	    c1->Print(canvasName,"pdf");
	  }

	}

  std::cout<<"\nResults printed in "<<outputFileName<<std::endl;
  return 0;
}
