/*************************************************************

    tools for the BTF iMCP data.
    1-Charge treshold for every MCP
    2-Waveform manipulation function

*************************************************************/

#include "..interface/analysis_tools.h"

using namespace std;

void init()
{
}

void SetOutTree(TTree* outTree)
{

    //---standard analysis branches
    outTree->Branch("time_CF",&time_CF,"time_CF[10]/F");
    outTree->Branch("amp_max",&amp_max,"amp_max[10]/F");
    outTree->Branch("charge",&charge,"charge[10]/F");
    outTree->Branch("charge_CF",&charge_CF,"charge_CF[10]/F");
    outTree->Branch("baseline",&baseline,"baseline[10]/F");
    //---hodoscope branches
    outTree->Branch("fibreX",&fibreX,"fibreX[8]/I");
    outTree->Branch("fibreY",&fibreY,"fibreY[8]/I");
    //---global branches
    outTree->Branch("sci_front_adc",&sci_front_adc,"sci_front_adc/I");
    outTree->Branch("run_id",&run_id,"run_id/I");
    //---additional branches
    outTree->Branch("isPCOn",&isPCOn,"isPCOn[10]/I");
    outTree->Branch("HV",&HV,"HV[10]/I");
    outTree->Branch("isTriggerOn",&isTriggerOn,"isTriggerOn[10]/I");
    //    outTree->Branch("MCPName"+nameMCP->at(Ch_1),&baseline_Ch1,"baseline_"+nameMCP->at(Ch_1)+"/F");
}

//----------------------------------------------------------------------------------------
void DFT_lowCut(vector<float>* samples, float f_cut)
{
    float* a = (float*)malloc(sizeof(float)*samples->size());
    float* b = (float*)malloc(sizeof(float)*samples->size());
    for (int k=0; k<samples->size(); k++) 
    { 
        a[k] = 0;
        b[k] = 0; 
        for (int t=0; t<samples->size(); t++) 
        { 
            float angle = 2 * M_PI * t * k / samples->size(); 
            a[k] += samples->at(t) * cos(angle); 
            b[k] += samples->at(t) * sin(angle); 
        } 
    }
}

//---------------------------------------------------------------------------------------
//---estimate the baseline in a given range and then subtract it from the signal 
float SubtractBaseline(int tb1, int tb2, vector<float>* samples)
{
    float baseline=0;
    //---compute baseline
    for(int iSample=tb1; iSample<tb2; iSample++)
    {
        baseline += samples->at(iSample);
    }
    baseline = baseline/(float)(tb2-tb1);
    //---subtract baseline
    for(int iSample=0; iSample<samples->size(); iSample++)
    {
        samples->at(iSample) = samples->at(iSample) - baseline;
    }
    return baseline;
}

//---------------------------------------------------------------------------------------
//---estimate time (ns) with CFD, samples must be a negative signal and baseline subtracted
float TimeConstFrac(int t1, int t2, const vector<float>* samples, float AmpFraction, 
                    float step=DIGITIZER_SAMPLING_UNIT, int Nsamples = 5)
{
    float xx= 0.;
    float xy= 0.;
    float Sx = 0.;
    float Sy = 0.;
    float Sxx = 0.;
    float Sxy = 0.;
    float Chi2 = 0.;
    int minSample=t1;
    int cfSample=t1; // first sample over AmpMax*CF 
    float minValue=0;

    for(int iSample=t1; iSample<t2; iSample++)
    {
        if(samples->at(iSample) < samples->at(minSample)) minSample = iSample;
    }
    minValue = samples->at(minSample);
    if(AmpFraction == 1) 
        return minSample*step;
    for(int iSample=minSample; iSample>t1; iSample--)
    {
        if(samples->at(iSample) > minValue*AmpFraction) 
        {
            cfSample = iSample;
            break;
        }
    }
    for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
    {
        if(cfSample+n<0) continue;
        xx = (cfSample+n)*(cfSample+n)*step*step;
        xy = (cfSample+n)*step*(samples->at(cfSample+n));
        Sx = Sx + (cfSample+n)*step;
        Sy = Sy + samples->at(cfSample+n);
        Sxx = Sxx + xx;
        Sxy = Sxy + xy;
    }

    float Delta = Nsamples*Sxx - Sx*Sx;
    float A = (Sxx*Sy - Sx*Sxy) / Delta;
    float B = (Nsamples*Sxy - Sx*Sy) / Delta;

    float sigma2 = pow(step/sqrt(12)*B,2);
 
    for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
    {
        if(cfSample+n<0) continue;
        Chi2 = Chi2 + pow(samples->at(cfSample+n) - A - B*((cfSample+n)*step),2)/sigma2;
    } 
    // A+Bx = AmpFraction * amp
    float interpolation = (samples->at(minSample) * AmpFraction - A) / B;
    return interpolation;
}

//---------------------------------------------------------------------------------------
//---compute the maximum amplitude for negative signals (range selectable)

float AmpMax(int t1, int t2, const vector<float>* samples)
{
    int minSample=t1;
    for(int iSample=t1; iSample<t2; iSample++)
    {
        if(samples->at(iSample) < samples->at(minSample)) minSample = iSample;
    }
    return samples->at(minSample);
}

//---------------------------------------------------------------------------------------
//---compute charge for a given signal in a given samples interval 
float ComputeIntegral(int t1, int t2, const vector<float>* samples)
{
    float integral=0;
    for(int bin=t1; bin<t2; bin++)
        integral += samples->at(bin);

    return integral;
}

//---------------------------------------------------------------------------------------
//---build signal fitfunc
TF1* GetFitFunc(TString Ch_n, histoFunc* wave, float t1, float t2) 
{
    Ch_n = "fitFunc_"+Ch_n;
    TF1* fitFunc = new TF1(Ch_n, wave, t1, t2, 4, "histoFunc");
    fitFunc->SetParLimits(1, 0.5, 1.5);
    fitFunc->FixParameter(3, 0.);
    fitFunc->SetNpx(100000);
    
    return fitFunc;
}
