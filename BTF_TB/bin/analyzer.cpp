/*****************************************************************************************
    This program compute charge or efficiency for all the run in a selected Scan for
    selected MCP under test. Also the hodoscope cut are selectable.

    compile with ---> c++ -o analyzer bin/analyzer.cpp `root-config --cflags --glibs --libs`
    run with:      ./analyzer cfg/treshold.txt MiB3 all eff HVScan Scan2
          where the arguments are: 1) cfg file with the tresholds 2) MCP to analyze 3) type of hodoscope cut (all/tight) 4) analysis type (eff,time) 5) Scan type (HV or X0) 6) suffix of reco file in input

 ****************************************************************************************/
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

#include "TROOT.h"
#include "TSystem.h"
#include "TKey.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TString.h"
#include "TCut.h"
#include "TMath.h"
#include "TApplication.h"
#include "TError.h"
#include "TCanvas.h"

#include "../include/init_Reco_Tree.h"
#include "../include/MCPMap.h"

#define RES_TRIG 58.93
#define RES_TRIG_ERR 0.373

using namespace std;

int main(int argc, char** argv)
{
  gSystem->Load("libTree");
  char *label, *hodo_cut, *doWhat, *scanType;
  Fill_MCPList();
  Fill_inverted_MCPList();

  std::ifstream inputCfg (argv[1],ios::in);
  std::string MCP = argv[2];
  hodo_cut = argv[3];
  doWhat = argv[4];
  scanType = argv[5];
  label = argv[6];

  int MCPNumber = MCPList.at(MCP);
  std::map <int,int> treshold;
  int run, ch, tresh;

  //---open cfg file and fill map with treshold for each channel----
  while(!inputCfg.eof())
      {
	inputCfg >> ch >> tresh;
	treshold.insert(std::make_pair(ch,tresh));
      }


  //-------ROOT Object definitions------------------------------------------------------
  TCanvas* c1 = new TCanvas();
  c1->cd();

    //------Build TCut and draw variables--------------------------------------------------
  char str_cut_sig[200]="";
  char str_cut_sig_2D[200]="";
  char str_cut_trig0[200]="";
  char str_cut_trig1[200]="";
  char str_cut_hodoX[250]="";
  char str_cut_hodoY[250]="";
  char var_sig[100]="";
  char var_base[100]="";
  char var_time[100]="";
  char var_trig0[100]="";
  char var_trig1[100]="";
    //-----Draw cut-----
  //---open tree and get: run list and corresponding HV-----
  TFile* inFile = TFile::Open("ntuples/reco_"+TString(label)+".root", "r");
  TTree* nt = (TTree*)inFile->Get("reco_tree");

  InitRecoTree(nt);
  std::vector<int> runList, HVVal;
  runList.clear();  HVVal.clear();
  std::vector<float> X0Step;
  X0Step.clear();
  int prevRun=0;

  for (int iEntry=0; iEntry<nt->GetEntries(); iEntry++)
    {
      nt->GetEntry(iEntry);
      if (run_id!=prevRun) {
	runList.push_back(run_id);
	HVVal.push_back(HV[MCPNumber]);
	X0Step.push_back(X0);
	prevRun=run_id;
      }
    }


  nt->GetEntry(1);
    //---HV Scan
  //  if(TString(scanType).Contains("Scan") == 1)
  //   {  

      sprintf(str_cut_sig, "charge[%d] > %d", MCPNumber, treshold.at(MCPNumber));
	//(strcmp(MCP, "MiB2") == 0)
	//sprintf(str_cut_sig_2D, "-charge_%s > -13.28*amp_max_%s - 350", MCP, MCP);
      sprintf(str_cut_trig0, "charge[%d] < %d  && charge[%d] < %d && sci_front_adc < 500",MCPList.at("Trig1"), treshold.at(MCPList.at("Trig1")), MCPList.at("Trig2"), treshold.at(MCPList.at("Trig2")));
      sprintf(str_cut_trig1, "charge[%d] > %d  && charge[%d] > %d && sci_front_adc > 500 && sci_front_adc < 1500",MCPList.at("Trig1"), treshold.at(MCPList.at("Trig1")), MCPList.at("Trig2"), treshold.at(MCPList.at("Trig2")));
      // }

    //---Hodoscope cut
  float thX[8]={113,95,127,118,94,134,133,160};
  float thY[8]={110,101,119,136,94,98,87,97};
  if(strcmp(hodo_cut,"all") == 0)
    {
	sprintf(str_cut_hodoX, "(fibreX[0] > %f || fibreX[1] > %f || fibreX[2] > %f || fibreX[3] > %f || fibreX[4] > %f || "
		"fibreX[5] > %f || fibreX[6] > %f || fibreX[7] > %f)", thX[0], thX[1], thX[2], thX[3], thX[4], thX[5], thX[6], thX[7]);
	sprintf(str_cut_hodoY, "(fibreY[0] > %f || fibreY[1] > %f || fibreY[2] > %f || fibreY[3] > %f || fibreY[4] > %f || "
		"fibreY[5] > %f || fibreY[6] > %f || fibreY[7] > %f)", thY[0], thY[1], thY[2], thY[3], thY[4], thY[5], thY[6], thY[7]);
    }
  else if(strcmp(hodo_cut,"tight") == 0)
    {
	sprintf(str_cut_hodoX, "(fibreX[3] > %f || fibreX[4] > %f)", thX[3], thX[4]);
	sprintf(str_cut_hodoY, "(fibreY[3] > %f || fibreY[4] > %f)", thY[3], thY[4]);
    }
    //-----construct TCut-----
  TCut cut_sig = str_cut_sig;
  TCut cut_sig_2D = str_cut_sig_2D;
  TCut cut_trig0 = str_cut_trig0;
  TCut cut_trig1 = str_cut_trig1;
  TCut cut_hodoX = str_cut_hodoX;
  TCut cut_hodoY = str_cut_hodoY;

  //-------Runs loop---------------------------------------------------------------------
  for(int i=0; i<runList.size(); i++)
    {

      char h_sig_name[20], h_base_name[20], h_trig1_name[20], h_trig0_name[20], res_func_name[20], h_time_name[20];
	sprintf(h_sig_name, "h_sig_%d", runList.at(i));
	sprintf(h_base_name, "h_base_%d", runList.at(i));
	sprintf(h_trig1_name, "h_trig1_%d", runList.at(i));
	sprintf(h_trig0_name, "h_trig0_%d", runList.at(i));
	sprintf(h_time_name, "h_time_%d", runList.at(i));
	sprintf(res_func_name, "res_func_%d", runList.at(i));


      TH1F* h_sig= new TH1F(h_sig_name, h_sig_name, 500, -5000, 25000);
      TH1F* h_base = new TH1F(h_base_name, h_base_name, 500, 5000, 25000);
      TH1F* h_trig1 = new TH1F(h_trig1_name, h_trig1_name, 500, -5000, 25000);
      TH1F* h_trig0 = new TH1F(h_trig0_name, h_trig0_name, 500, -5000, 25000);
      TH1F* h_time = new TH1F(h_time_name, h_time_name, 500, -5, 5);
      TF1* res_func = new TF1("res_func", "gausn", -10, 10);

    //-----Draw variables-----
      sprintf(var_sig, "charge[%d]>>%s", MCPNumber, h_sig_name);
      sprintf(var_base, "baseline[%d]>>%s", MCPNumber, h_base_name);
      sprintf(var_time, "(time_CF[%d]-time_CF[%d])>>%s", MCPNumber, MCPList.at("Trig1"), h_time_name);
      sprintf(var_trig0, "charge[%d]>>%s", MCPList.at("Trig1"), h_trig0_name);
      sprintf(var_trig1, "charge[%d]>>%s", MCPList.at("Trig1"), h_trig1_name);


	//---Run cut
	char cut_run[20];
	sprintf(cut_run, "run_id == %d", runList.at(i));
     	//-----Draw and print infos-----
	nt->Draw(var_sig, cut_trig1 && cut_sig && cut_sig_2D && cut_hodoX && cut_hodoY && cut_run);
	nt->Draw(var_base, cut_trig0 && cut_sig && cut_sig_2D && cut_hodoX && cut_hodoY && cut_run);
	nt->Draw(var_trig0,cut_trig0 && cut_hodoX && cut_hodoY && cut_run);
	nt->Draw(var_trig1,cut_trig1 && cut_hodoX && cut_hodoY && cut_run);
	float eff = (h_sig->GetEntries()-h_base->GetEntries()*h_trig1->GetEntries()/h_trig0->GetEntries())/h_trig1->GetEntries();
	float e_eff = TMath::Sqrt((TMath::Abs(eff*(1-eff)))/h_trig1->GetEntries());
	if(eff < 0)
	    eff = 0;
	char var_name[3] = "X0";
	if(TString(scanType).Contains("HV") == 1)
	    sprintf(var_name, "HV");

	//	std::cout<<HVVal.at(0)<<" "<<eff<<std::endl;
	//---Eff study
	if(strcmp(doWhat,"eff") == 0)
	{
	    if(i == 0)
	    {
		printf("---------Efficiency----------\n");
		printf(" %s\teff\te_%s\te_eff\n", var_name, var_name);
		printf("-----------------------------\n");
	    }
	    if(TString(scanType).Contains("HV") == 1)
	      //   std::cout<<HVVal.at(i)<<"\t"<<eff<<"\t"<<0.<<" "<<e_eff<<std::endl;
	      printf("%d\t%.3f\t%.3f\t%.3f\n", HVVal.at(i), eff, 0., e_eff);
	    else
	      printf("%.3f\t%.3f\t%.3f\t%.3f\n", X0Step.at(i), eff, 0., e_eff);
	    if(i == (runList.size()-1))    
		printf("-----------------------------\n");
	}
	//---Charge study
	else if(strcmp(doWhat,"Q") == 0)
	{
	    if(i == 0)
	    {
		printf("---------Efficiency----------\n");
		printf(" %s\tQ\te_%s\te_Q\n", var_name, var_name);
		printf("-----------------------------\n");
	    }
	    nt->Draw(var_sig, cut_trig1 && cut_sig_2D && cut_hodoX && cut_hodoY && cut_run);
	    if(TString(scanType).Contains("HV") == 1)
	      printf("%d\t%.0f\t%.0f\t%.0f\n", HVVal.at(i), -h_sig->GetMean(), 0., h_sig->GetMeanError());
	    else
	      printf("%.3f\t%.0f\t%.0f\t%.0f\n", X0Step.at(i), -h_sig->GetMean(), 0., h_sig->GetMeanError());
	    if(i == (runList.size()-1))    
		printf("-----------------------------\n");
	}
	//---Time study 
	else if(strcmp(doWhat,"time") == 0)
	{
	    nt->Draw(var_time, cut_trig1 && cut_sig && cut_sig_2D && cut_hodoX && cut_hodoY && cut_run);
	    res_func->SetParameters(h_time->GetEntries()/2, h_time->GetMean(), h_time->GetRMS()/2);
	    res_func->SetParLimits(0, 0, h_time->GetEntries()*2);
	    res_func->SetParLimits(2, 0, h_time->GetRMS());
	    res_func->SetRange(h_time->GetMean()-2*h_time->GetRMS(), h_time->GetMean()+2*h_time->GetRMS());
	    h_time->Fit(res_func, "QR");
	    float err_time = res_func->GetParError(2)*1000;
	    float t_res = sqrt(pow(res_func->GetParameter(2)*1000, 2) - pow(float(RES_TRIG), 2));
	    float e_t_res = sqrt(pow(err_time*res_func->GetParameter(2)*1000, 2) + pow(float(RES_TRIG_ERR)*RES_TRIG, 2))/
		t_res;
	    float prob = res_func->GetProb();
	    if(i == 0)
	    {
		printf("----------Time Resolution(ps)----------\n");
		printf(" %s\tt_res\te_%s\te_t_res\tX²_prob\n", var_name, var_name);
		printf("---------------------------------------\n");
	    }
	    if(TString(scanType).Contains("HV") == 1)
	      printf("%d\t%.1f\t%.0f\t%.1f\t%.3f\n", HVVal.at(i), t_res, 0., e_t_res, prob);
	    else
	      printf("%.3f\t%.0f\t%.3f\t%.0f\t%.3f\n", X0Step.at(i), t_res, 0., e_t_res, prob);
	    if(i == (runList.size()-1))    
		printf("---------------------------------------\n");
	    TCanvas* c = new TCanvas();
	    char plot_name[100];

	    std::string command = "if [ ! -e plots/time_resolutions/"+string(label)+" ] ; then mkdir plots/time_resolutions/"+label+" ; fi";
	    system(command.c_str());

	    sprintf(plot_name, "plots/time_resolutions/%s/%s_%d.pdf", label, MCP.c_str(), i);
	    h_time->Draw();
	    c->Print(plot_name, "pdf");
	}
    }    
  return 0;     
}

