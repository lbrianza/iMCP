/*******************************************************************************                                                                                  
THIS CODE COMPUTES THE TRESHOLD FOR EACH MCP USING A FIXED S/B VALUE (IN INPUT)
                                                                                                    
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


//*******MAIN*******************************************************************                                                                                  
int main (int argc, char** argv)
{

  std::string inputFile = argv[1];
  int SBTreshold = atoi(argv[2]);
  int nChannels = atoi(argv[3]);

  TFile *inputFile = new TFile (("ntuples/reco_"+inputFile+".root").c_str()); 
  TTree *inputTree = (TTree*)inputFile->Get("");



  int colorsExt[9] = {kRed+1, kBlue, kGreen+2, kAzure+7, kOrange+7, kCyan, kBlack};
  std::vector<int> colors;
  for(int posVec = 0; posVec<6; ++posVec){
    colors.push_back(colorsExt[posVec]);
  }

  std::vector<std::string> nameMCP;
  nameMCP.push_back("MiB1");
  nameMCP.push_back("MiB2");
  nameMCP.push_back("Planacon");
  nameMCP.push_back("MiB3");
  nameMCP.push_back("Roma_ampli");
  nameMCP.push_back("Roma_Noampli");

  TFile** inF = new TFile[4];

  TH1F* chHistoBase_All_PCon[6];
  TH1F* chHistoBase_All_PCoff[6];
  TH1F* chHistoSignal_All_PCon[6];
  TH1F* chHistoSignal_All_PCoff[6];


  for(int iS=0; iS<4; ++iS){
    inF[iS] = TFile::Open(Form("Scan%d_outHistos.root",iS+1), "read");
  }
  chHistoBase_All_PCon[0] = (TH1F*)inF[0]->Get(Form("histoBase_All_Ch%d",0));
  chHistoBase_All_PCon[0]->Add((TH1F*)inF[1]->Get(Form("histoBase_All_Ch%d",0)), 1);
  chHistoBase_All_PCon[0]->Add((TH1F*)inF[2]->Get(Form("histoBase_All_Ch%d",0)), 1);
  chHistoBase_All_PCon[0]->Add((TH1F*)inF[3]->Get(Form("histoBase_All_Ch%d",0)), 1);
  chHistoBase_All_PCon[0]->SetName("MiB 1 PC on B");
  chHistoBase_All_PCon[0]->SetLineWidth(2);
  chHistoBase_All_PCon[0]->SetLineColor(colors.at(0));
  std::cout << "chHistoBase_All_PCon[0]->GetEntries() = " << chHistoBase_All_PCon[0]->GetEntries() << std::endl;
  //
  chHistoBase_All_PCon[1] = (TH1F*)inF[0]->Get(Form("histoBase_All_Ch%d",1));
  chHistoBase_All_PCon[1]->SetName("Mib 2 PC on B");
  //   chHistoBase_All_PCon[1]->Add((TH1F*)inF[1]->Get(Form("histoBase_All_Ch%d",4)), 1);
  //   chHistoBase_All_PCon[1]->Add((TH1F*)inF[2]->Get(Form("histoBase_All_Ch%d",4)), 1);
  //  chHistoBase_All_PCon[1]->SetName("Mib 2+3 PC on B");
  chHistoBase_All_PCon[1]->SetLineWidth(2);
  chHistoBase_All_PCon[1]->SetLineColor(colors.at(1));
  std::cout << "chHistoBase_All_PCon[1]->GetEntries() = " << chHistoBase_All_PCon[1]->GetEntries() << std::endl;
  //
  chHistoBase_All_PCoff[1] = (TH1F*)inF[1]->Get(Form("histoBase_All_Ch%d",1));
  chHistoBase_All_PCoff[1]->Add((TH1F*)inF[3]->Get(Form("histoBase_All_Ch%d",1)), 1);
  chHistoBase_All_PCoff[1]->SetName("Mib 2 PC off B");
  //   chHistoBase_All_PCoff[1]->Add((TH1F*)inF[0]->Get(Form("histoBase_All_Ch%d",4)), 1);
  //   chHistoBase_All_PCoff[1]->Add((TH1F*)inF[3]->Get(Form("histoBase_All_Ch%d",4)), 1);
  //   chHistoBase_All_PCoff[1]->SetName("Mib 2+3 PC off B");
  chHistoBase_All_PCoff[1]->SetLineWidth(2);
  chHistoBase_All_PCoff[1]->SetLineColor(colors.at(1));
  std::cout << "chHistoBase_All_PCoff[1]->GetEntries() = " << chHistoBase_All_PCoff[1]->GetEntries() << std::endl;
  //
  //  chHistoBase_All_PCoff[2] = (TH1F*)inF[0]->Get(Form("histoBase_All_Ch%d",3));
  //  chHistoBase_All_PCoff[2]->Add((TH1F*)inF[2]->Get(Form("histoBase_All_Ch%d",3)), 1);
  chHistoBase_All_PCoff[2] = (TH1F*)inF[0]->Get(Form("histoBase_All_Ch%d",3));
  chHistoBase_All_PCoff[2]->Add((TH1F*)inF[2]->Get(Form("histoBase_All_Ch%d",3)), 1);
  chHistoBase_All_PCoff[2]->Add((TH1F*)inF[3]->Get(Form("histoBase_All_Ch%d",3)), 1);
  chHistoBase_All_PCoff[2]->SetName("Plana PC off B");
  chHistoBase_All_PCoff[2]->SetLineWidth(2);
  chHistoBase_All_PCoff[2]->SetLineColor(colors.at(2));
  std::cout << "chHistoBase_All_PCoff[2]->GetEntries() = " << chHistoBase_All_PCoff[2]->GetEntries() << std::endl;
  //
  chHistoBase_All_PCon[2] = (TH1F*)inF[1]->Get(Form("histoBase_All_Ch%d",3));
  chHistoBase_All_PCon[2]->SetName("Plana PC on B");
  chHistoBase_All_PCon[2]->SetLineWidth(2);
  chHistoBase_All_PCon[2]->SetLineColor(colors.at(2));
  std::cout << "chHistoBase_All_PCon[2]->GetEntries() = " << chHistoBase_All_PCon[2]->GetEntries() << std::endl;
  //
  chHistoBase_All_PCoff[3] = (TH1F*)inF[0]->Get(Form("histoBase_All_Ch%d",4));
  chHistoBase_All_PCoff[3]->Add((TH1F*)inF[3]->Get(Form("histoBase_All_Ch%d",4)), 1);
  chHistoBase_All_PCoff[3]->SetName("Mib 3 PC off B");
  chHistoBase_All_PCoff[3]->SetLineWidth(2);
  chHistoBase_All_PCoff[3]->SetLineColor(colors.at(3));
  std::cout << "chHistoBase_All_PCoff[3]->GetEntries() = " << chHistoBase_All_PCoff[3]->GetEntries() << std::endl;
  //
  chHistoBase_All_PCon[3] = (TH1F*)inF[1]->Get(Form("histoBase_All_Ch%d",4));
  chHistoBase_All_PCon[3]->Add((TH1F*)inF[2]->Get(Form("histoBase_All_Ch%d",4)), 1);
  chHistoBase_All_PCon[3]->SetName("MiB 3 PC on B");
  chHistoBase_All_PCon[3]->SetLineWidth(2);
  chHistoBase_All_PCon[3]->SetLineColor(colors.at(3));
  std::cout << "chHistoBase_All_PCon[3]->GetEntries() = " << chHistoBase_All_PCon[3]->GetEntries() << std::endl;
  //
  chHistoBase_All_PCon[4] = (TH1F*)inF[0]->Get(Form("histoBase_All_Ch%d",5));
  chHistoBase_All_PCon[4]->Add((TH1F*)inF[1]->Get(Form("histoBase_All_Ch%d",5)), 1);
  chHistoBase_All_PCon[4]->Add((TH1F*)inF[2]->Get(Form("histoBase_All_Ch%d",5)), 1);
  chHistoBase_All_PCon[4]->SetName("Roma PC on B");
  chHistoBase_All_PCon[4]->SetLineWidth(2);
  chHistoBase_All_PCon[4]->SetLineColor(colors.at(4));
  std::cout << "chHistoBase_All_PCon[4]->GetEntries() = " << chHistoBase_All_PCon[4]->GetEntries() << std::endl;
  //
  chHistoBase_All_PCoff[4] = (TH1F*)inF[3]->Get(Form("histoBase_All_Ch%d",1));
  chHistoBase_All_PCoff[4]->SetName("Roma PC off B");
  chHistoBase_All_PCoff[4]->SetLineWidth(2);
  chHistoBase_All_PCoff[4]->SetLineColor(colors.at(4));
  std::cout << "chHistoBase_All_PCoff[4]->GetEntries() = " << chHistoBase_All_PCoff[4]->GetEntries() << std::endl;
  //
  chHistoBase_All_PCon[5] = (TH1F*)inF[3]->Get(Form("histoBase_All_Ch%d",5));
  chHistoBase_All_PCon[5]->SetName("Roma PC on noAmpli B");
  chHistoBase_All_PCon[5]->SetLineWidth(2);
  chHistoBase_All_PCon[5]->SetLineColor(colors.at(5));
  std::cout << "chHistoBase_All_PCon[5]->GetEntries() = " << chHistoBase_All_PCon[5]->GetEntries() << std::endl;
  ////////////////
  chHistoSignal_All_PCon[0] = (TH1F*)inF[0]->Get(Form("histoSignal_All_Ch%d",0));
  chHistoSignal_All_PCon[0]->Add((TH1F*)inF[1]->Get(Form("histoSignal_All_Ch%d",0)), 1);
  chHistoSignal_All_PCon[0]->Add((TH1F*)inF[2]->Get(Form("histoSignal_All_Ch%d",0)), 1);
  chHistoSignal_All_PCon[0]->Add((TH1F*)inF[3]->Get(Form("histoSignal_All_Ch%d",0)), 1);
  chHistoSignal_All_PCon[0]->SetName("MiB 1 PC on S ");
  chHistoSignal_All_PCon[0]->SetLineWidth(2);
  chHistoSignal_All_PCon[0]->SetLineColor(colors.at(0));
  std::cout << "chHistoSignal_All_PCon[0]->GetEntries() = " << chHistoSignal_All_PCon[0]->GetEntries() << std::endl;
  //  //  chHistoSignal_All_PCon[1] = (TH1F*)inF[0]->Get(Form("histoSignal_All_Ch%d",1));
  chHistoSignal_All_PCon[1] = (TH1F*)inF[0]->Get(Form("histoSignal_Ch%d_Scan_%d",1,9));   // Max 1 Min 9
  chHistoSignal_All_PCon[1]->SetName("Mib 2 PC on S");
  //   chHistoSignal_All_PCon[1]->Add((TH1F*)inF[1]->Get(Form("histoSignal_Ch%d_Scan_%d",4,10)),1);
  //   chHistoSignal_All_PCon[1]->Add((TH1F*)inF[2]->Get(Form("histoSignal_All_Ch%d",4)), 1);
  //   chHistoSignal_All_PCon[1]->SetName("Mib 2+3 PC on S");
  chHistoSignal_All_PCon[1]->SetLineWidth(2);
  chHistoSignal_All_PCon[1]->SetLineColor(colors.at(1));
  std::cout << "chHistoSignal_All_PCon[1]->GetEntries() = " << chHistoSignal_All_PCon[1]->GetEntries() << std::endl;
  //
  //  chHistoSignal_All_PCoff[1] = (TH1F*)inF[1]->Get(Form("histoSignal_All_Ch%d",1));
  //  chHistoSignal_All_PCoff[1]->Add((TH1F*)inF[3]->Get(Form("histoSignal_All_Ch%d",1)), 1);
  chHistoSignal_All_PCoff[1] = (TH1F*)inF[1]->Get(Form("histoSignal_Ch%d_Scan_%d",1,10));   // Max 1 Min 10
  chHistoSignal_All_PCoff[1]->Add((TH1F*)inF[3]->Get(Form("histoSignal_Ch%d_Scan_%d",1,1)), 1);   // Max 11 Min 1
  chHistoSignal_All_PCoff[1]->SetName("Mib 2 PC off S");
  //   chHistoSignal_All_PCoff[1]->Add((TH1F*)inF[0]->Get(Form("histoSignal_Ch%d_Scan_%d",4,8)), 1);
  //   chHistoSignal_All_PCoff[1]->Add((TH1F*)inF[3]->Get(Form("histoSignal_Ch%d_Scan_%d",4,1)), 1);
  //   chHistoSignal_All_PCoff[1]->SetName("Mib 2+3 PC off S");
  chHistoSignal_All_PCoff[1]->SetLineWidth(2);
  chHistoSignal_All_PCoff[1]->SetLineColor(colors.at(1));
  std::cout << "chHistoSignal_All_PCoff[1]->GetEntries() = " << chHistoSignal_All_PCoff[1]->GetEntries() << std::endl;
  //
  //   chHistoSignal_All_PCoff[2] = (TH1F*)inF[0]->Get(Form("histoSignal_All_Ch%d",3));
  //   chHistoSignal_All_PCoff[2]->Add((TH1F*)inF[2]->Get(Form("histoSignal_All_Ch%d",3)), 1);
  //   chHistoSignal_All_PCoff[2]->Add((TH1F*)inF[3]->Get(Form("histoSignal_All_Ch%d",3)), 1);
  chHistoSignal_All_PCoff[2] = (TH1F*)inF[0]->Get(Form("histoSignal_Ch%d_Scan_%d",3,16));  // Max 1 Min 16
  chHistoSignal_All_PCoff[2]->Add((TH1F*)inF[2]->Get(Form("histoSignal_Ch%d_Scan_%d",3,6)), 1);  // Max 10 Min 6
  chHistoSignal_All_PCoff[2]->Add((TH1F*)inF[3]->Get(Form("histoSignal_Ch%d_Scan_%d",3,1)), 1);  // Max 11 Min 1
  chHistoSignal_All_PCoff[2]->SetName("Plana PC off S");
  std::cout << "chHistoSignal_All_PCoff[2]->GetEntries() = " << chHistoSignal_All_PCoff[2]->GetEntries() << std::endl;
  //
  //  chHistoSignal_All_PCon[2] = (TH1F*)inF[1]->Get(Form("histoSignal_All_Ch%d",3));
  chHistoSignal_All_PCon[2] = (TH1F*)inF[1]->Get(Form("histoSignal_Ch%d_Scan_%d",3,10));  // Max 1 Min 10
  chHistoSignal_All_PCon[2]->SetName("Plana PC on S");
  chHistoSignal_All_PCon[2]->SetLineWidth(2);
  chHistoSignal_All_PCon[2]->SetLineColor(colors.at(2));
  std::cout << "chHistoSignal_All_PCon[2]->GetEntries() = " << chHistoSignal_All_PCon[2]->GetEntries() << std::endl;
  //
  //   chHistoSignal_All_PCoff[3] = (TH1F*)inF[0]->Get(Form("histoSignal_All_Ch%d",4));
  //   chHistoSignal_All_PCoff[3]->Add((TH1F*)inF[3]->Get(Form("histoSignal_All_Ch%d",4)), 1);
  chHistoSignal_All_PCoff[3] = (TH1F*)inF[0]->Get(Form("histoSignal_Ch%d_Scan_%d",4,8));   // Max 16 Min 8
  chHistoSignal_All_PCoff[3]->Add((TH1F*)inF[3]->Get(Form("histoSignal_Ch%d_Scan_%d",4,1)), 1);  // Max 11 Min 1
  chHistoSignal_All_PCoff[3]->SetName("Mib 3 PC off S");
  chHistoSignal_All_PCoff[3]->SetLineWidth(2);
  chHistoSignal_All_PCoff[3]->SetLineColor(colors.at(3));
  std::cout << "chHistoSignal_All_PCoff[3]->GetEntries() = " << chHistoSignal_All_PCoff[3]->GetEntries() << std::endl;
  //
  //   chHistoSignal_All_PCon[3] = (TH1F*)inF[1]->Get(Form("histoSignal_All_Ch%d",4));
  //   chHistoSignal_All_PCon[3]->Add((TH1F*)inF[2]->Get(Form("histoSignal_All_Ch%d",4)), 1);
  chHistoSignal_All_PCon[3] = (TH1F*)inF[1]->Get(Form("histoSignal_Ch%d_Scan_%d",4,10)); // Max 1 Min 10
  chHistoSignal_All_PCon[3]->Add((TH1F*)inF[2]->Get(Form("histoSignal_All_Ch%d",4)), 1);  // tensione fissa
  chHistoSignal_All_PCon[3]->SetName("MiB 3 PC on S");
  chHistoSignal_All_PCon[3]->SetLineWidth(2);
  chHistoSignal_All_PCon[3]->SetLineColor(colors.at(3));
  std::cout << "chHistoSignal_All_PCon[3]->GetEntries() = " << chHistoSignal_All_PCon[3]->GetEntries() << std::endl;
  //
  //   chHistoSignal_All_PCon[4] = (TH1F*)inF[0]->Get(Form("histoSignal_All_Ch%d",5));
  //   chHistoSignal_All_PCon[4]->Add((TH1F*)inF[1]->Get(Form("histoSignal_All_Ch%d",5)), 1);
  //   chHistoSignal_All_PCon[4]->Add((TH1F*)inF[2]->Get(Form("histoSignal_All_Ch%d",5)), 1);
  chHistoSignal_All_PCon[4] = (TH1F*)inF[0]->Get(Form("histoSignal_All_Ch%d",5));
  chHistoSignal_All_PCon[4]->Add((TH1F*)inF[1]->Get(Form("histoSignal_All_Ch%d",5)), 1);
  chHistoSignal_All_PCon[4]->Add((TH1F*)inF[2]->Get(Form("histoSignal_Ch%d_Scan_%d",5,9)), 1); // Max 1 Min 9
  chHistoSignal_All_PCon[4]->SetName("Roma PC on ");
  chHistoSignal_All_PCon[4]->SetLineWidth(2);
  chHistoSignal_All_PCon[4]->SetLineColor(colors.at(4));
  std::cout << "chHistoSignal_All_PCon[4]->GetEntries() = " << chHistoSignal_All_PCon[4]->GetEntries() << std::endl;
  //
  //  chHistoSignal_All_PCoff[4] = (TH1F*)inF[2]->Get(Form("histoSignal_All_Ch%d",1));
  chHistoSignal_All_PCoff[4] = (TH1F*)inF[2]->Get(Form("histoSignal_Ch%d_Scan_%d",1,9)); // Max 1 Min 9
  chHistoSignal_All_PCoff[4]->SetName("Roma PC off S");
  chHistoSignal_All_PCoff[4]->SetLineWidth(2);
  chHistoSignal_All_PCoff[4]->SetLineColor(colors.at(4));
  std::cout << "chHistoSignal_All_PCoff[4]->GetEntries() = " << chHistoSignal_All_PCoff[4]->GetEntries() << std::endl;
  //
  chHistoSignal_All_PCon[5] = (TH1F*)inF[3]->Get(Form("histoSignal_All_Ch%d",5));
  chHistoSignal_All_PCon[5]->SetName("Roma PC on noAmpli S");
  chHistoSignal_All_PCon[5]->SetLineWidth(2);
  chHistoSignal_All_PCon[5]->SetLineColor(colors.at(5));
  std::cout << "chHistoSignal_All_PCon[5]->GetEntries() = " << chHistoSignal_All_PCon[5]->GetEntries() << std::endl;

  TGraph* gCont_SoverB_on[6];
  TGraph* gCont_SoverB_off[6];

  for(int iw=0; iw<6; ++iw){
      gCont_SoverB_on[iw] = new TGraph();
      gCont_SoverB_on[iw]->SetMarkerColor(colors.at(iw)); 
      gCont_SoverB_on[iw]->SetLineColor(colors.at(iw)); 
      gCont_SoverB_on[iw]->SetMarkerStyle(21); 
      gCont_SoverB_on[iw]->SetMarkerSize(0.6); 

      gCont_SoverB_off[iw] = new TGraph();
      gCont_SoverB_off[iw]->SetMarkerColor(colors.at(iw)); 
      gCont_SoverB_off[iw]->SetLineColor(colors.at(iw)); 
      gCont_SoverB_off[iw]->SetMarkerStyle(21); 
      gCont_SoverB_off[iw]->SetMarkerSize(0.6); 
  }

  TLegend* leg = new TLegend(0.88,0.65,0.98,0.85);
  leg->SetFillColor(0);
  leg->SetTextFont(41);
  for (int iw=0;iw<6;iw++) {
    leg->AddEntry(chHistoBase_All_PCon[iw],nameMCP.at(iw).c_str(),"l");
  }

  int binMax = 5000;
  int binSub = 4500;
  for(int iBin=0; iBin<binMax; ++iBin){
    for (int iw=0;iw<6;iw++){

      double ratioB = -1;
      double numB = chHistoBase_All_PCon[iw]->Integral(1, chHistoBase_All_PCon[iw]->FindBin(iBin - binSub));
      double denB = chHistoBase_All_PCon[iw]->Integral(1, chHistoBase_All_PCon[iw]->GetNbinsX());
      if(denB != 0) ratioB = numB/denB;

      double ratioS = -1;
      double numS = chHistoSignal_All_PCon[iw]->Integral(1, chHistoSignal_All_PCon[iw]->FindBin(iBin - binSub));
      double denS = chHistoSignal_All_PCon[iw]->Integral(1, chHistoSignal_All_PCon[iw]->GetNbinsX());
      if(denS != 0) ratioS = numS/denS;

      if(ratioB != 0 && denB != -1) {
	gCont_SoverB_on[iw]->SetPoint(iBin, iBin-binSub, ratioS/ratioB);
      }
      else gCont_SoverB_on[iw]->SetPoint(iBin, iBin-binSub, -1);

      //      std::cout << " ON ch " << nameMCP.at(iw) << " ratioB = " << ratioB << " ratioS = " << ratioS << std::endl;
      ////// OFF
	ratioB = -1;
	ratioS = -1;
      if(iw != 0 && iw != 5){
	numB = chHistoBase_All_PCoff[iw]->Integral(1, chHistoBase_All_PCoff[iw]->FindBin(iBin - binSub));
	denB = chHistoBase_All_PCoff[iw]->Integral(1, chHistoBase_All_PCoff[iw]->GetNbinsX());
	if(denB != 0) ratioB = numB/denB;
	
	numS = chHistoSignal_All_PCoff[iw]->Integral(1, chHistoSignal_All_PCoff[iw]->FindBin(iBin - binSub));
	denS = chHistoSignal_All_PCoff[iw]->Integral(1, chHistoSignal_All_PCoff[iw]->GetNbinsX());
	if(denS != 0) ratioS = numS/denS;
      }
      if(ratioB != 0 && denB != -1)  gCont_SoverB_off[iw]->SetPoint(iBin, iBin-binSub, ratioS/ratioB);
      else gCont_SoverB_off[iw]->SetPoint(iBin, iBin-binSub, -1);

      //      std::cout << " OFF ch " << nameMCP.at(iw) << " ratioB = " << ratioB << " ratioS = " << ratioS << std::endl;
    }
  }


  TCanvas* cON = new TCanvas();
  chHistoBase_All_PCon[0]->Draw();
  chHistoSignal_All_PCon[0]->Draw("same");
  for (int iw=1;iw<6;iw++){
    chHistoBase_All_PCon[iw]->Draw("same");
    chHistoSignal_All_PCon[iw]->Draw("same");
  }
  //  chHistoBase_All_PCon[5]->Draw();
  //  chHistoSignal_All_PCon[5]->Draw("same");
  leg->Draw("same");


  TCanvas* cOF = new TCanvas();
  chHistoBase_All_PCoff[1]->Draw();
  chHistoSignal_All_PCoff[1]->Draw("same");
  for (int iw=2;iw<5;iw++){
    chHistoBase_All_PCoff[iw]->Draw("same");
    chHistoSignal_All_PCoff[iw]->Draw("same");
  }
  leg->Draw("same");



  TCanvas* cRatio_ON = new TCanvas();
  gPad->SetGrid();
  gCont_SoverB_on[0]->GetXaxis()->SetTitle("Integral (ADC) PC on");
  gCont_SoverB_on[0]->GetYaxis()->SetTitle("S/B");
  gCont_SoverB_on[0]->GetXaxis()->SetRangeUser(-300,0.);
  gCont_SoverB_on[0]->GetYaxis()->SetRangeUser(0.,500.);
  gCont_SoverB_on[0]->Draw("apl");
  for (int iw=1;iw<6;iw++){
    gCont_SoverB_on[iw]->Draw("pl,same");
  }
  leg->Draw("same");

  TCanvas* cRatio_OFF = new TCanvas();
  gPad->SetGrid();
  gCont_SoverB_off[1]->GetXaxis()->SetTitle("Integral (ADC) PC off");
  gCont_SoverB_off[1]->GetYaxis()->SetTitle("S/B");
  gCont_SoverB_off[1]->GetXaxis()->SetRangeUser(-400,0.);
  gCont_SoverB_off[1]->GetYaxis()->SetRangeUser(0.,500.);
  gCont_SoverB_off[1]->Draw("apl");
  for (int iw=2;iw<5;iw++){
    gCont_SoverB_off[iw]->Draw("pl,same");
  }
  leg->Draw("same");

}


void doAltro(){



  int nScan = 16;

  int goodCh[3] = {1, 3, 4}; 

  std::vector<std::string> nameMCP;
  nameMCP.push_back("MiB1");
  nameMCP.push_back("MiB2");
  nameMCP.push_back("ScB");
  nameMCP.push_back("Planacon");
  nameMCP.push_back("MiB3");
  nameMCP.push_back("RomaX");


  TFile* inF = TFile::Open("outHistos.root");
  TGraph* hRatio[9];

  int colorsExt[9] = {kRed+1, kBlue, kPink, kGreen+1, kCyan+2, kOrange+7, kAzure+7, kWhite, kBlack};
  std::vector<int> colors;
  for(int posVec = 0; posVec<6; ++posVec){
    colors.push_back(colorsExt[posVec]);
  }

  TLegend* leg = new TLegend(0.88,0.65,0.98,0.85);
  leg->SetFillColor(0);
  leg->SetTextFont(41);
  for (int iw=0;iw<6;iw++) {
    leg->AddEntry(hRatio[iw],nameMCP.at(iw).c_str(),"l");
  }

  TCanvas* c = new TCanvas();

  TH1F* hAmpMax[9];
  TH1F* hBaseLine[9];

  float thres = 20;

  for(int iSC=1; iSC<=nScan+1; ++iSC){
  //  for(int iSC=1; iSC<=2; ++iSC){
    for(int iCh=0; iCh<6; ++iCh){
      if(iCh == 2) continue;

      char h1[10];
      sprintf (h1,"AM_Ch%d_Scan%d",iCh, iSC);
      hAmpMax[iCh] = (TH1F*)inF->Get("h1");


      char h2[10];
      sprintf (h2,"Bs_Ch%d_Scan%d",iCh, iSC);
      hBaseLine[iCh] = (TH1F*)inF->Get("h2");
    
      std::cout << "preso " << hAmpMax[iCh]->FindBin(thres) << std::endl;
    float ratio = (hAmpMax[iCh]->Integral( hAmpMax[iCh]->FindBin(thres), hAmpMax[iCh]->MaximumBin()) /
		   hBaseLine[iCh]->Integral( hBaseLine[iCh]->FindBin(thres), hBaseLine[iCh]->MaximumBin()) );
    hRatio->SetPoint(iSc, iSc, ratio);
    hRatio[iw]->SetMarkerColor(colors.at(iw));
    
    c->cd();
    if(iScan == 1 && iCh == 0)    hRatio[iw]->Draw("apl");
    else hRatio[iw]->Draw("p,same");
    }
  }
 
  c->cd();
  leg->Draw("same");
  
  
  return 0;
}
