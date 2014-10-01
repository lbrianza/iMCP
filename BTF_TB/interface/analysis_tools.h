/*************************************************************

    tools for the BTF iMCP data.
    1-Charge treshold for every MCP
    2-Waveform manipulation function

*************************************************************/

#ifndef __analysis_tools__
#define __analysis_tools__

#include <cstdio>
#include <cstdlib>
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

#include "TTree.h"
#include "TF1.h"

#include "histo_func.h"

using namespace std;

#define DIGITIZER_SAMPLING_UNIT 0.2 //digitizer samples width (ns)
#define MS_SAMPLING_UNIT 0.01 //mean signal samples width (ns)
#define MS_LOW_TIME -15
#define MS_HIGH_TIME 30

#define HODOX_ADC_START_CHANNEL 16
#define HODOX_ADC_MID_CHANNEL 19
#define HODOX_ADC_END_CHANNEL 23
#define HODOY_ADC_START_CHANNEL 24
#define HODOY_ADC_MID_CHANNEL 27
#define HODOY_ADC_END_CHANNEL 31

//------------------------------------------------------------------------------
//---initialize all the pointer addressed by the outTree branches
float time_CF[10]={};
float amp_max[10]={};
float charge[10]={};
float charge_CF[10]={};
float baseline[10]={};
//---global branches
int sci_front_adc=0, run_id=0;
//---hodoscope branches
int fibreX[8]={};
int fibreY[8]={}; 

int isPCOn[10]={};
int HV[10]={};
int isTriggerOn[10]={};

void init();

void SetOutTree(TTree* outTree);

//----------------------------------------------------------------------------------------
void DFT_lowCut(vector<float>* samples, float f_cut);

//---------------------------------------------------------------------------------------
//---estimate the baseline in a given range and then subtract it from the signal 
float SubtractBaseline(int tb1, int tb2, vector<float>* samples);

//---------------------------------------------------------------------------------------
//---estimate time (ns) with CFD, samples must be a negative signal and baseline subtracted
float TimeConstFrac(int t1, int t2, const vector<float>* samples, float AmpFraction, 
                    float step=DIGITIZER_SAMPLING_UNIT, int Nsamples = 5);

//---------------------------------------------------------------------------------------
//---compute the maximum amplitude for negative signals (range selectable)

float AmpMax(int t1, int t2, const vector<float>* samples);

//---------------------------------------------------------------------------------------
//---compute charge for a given signal in a given samples interval 
float ComputeIntegral(int t1, int t2, const vector<float>* samples);

//---------------------------------------------------------------------------------------
//---build signal fitfunc
TF1* GetFitFunc(TString Ch_n, histoFunc* wave, float t1, float t2); 

#endif
