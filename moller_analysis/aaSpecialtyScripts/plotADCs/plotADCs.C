#include "TCanvas.h"
#include "TChain.h"
#include "TH1D.h"
#include "TString.h"
#include "TLine.h"
#include "TDirectory.h"
#include "TSystem.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TChain.h"
#include <iostream>
using namespace std;

TChain* plotADCs(int run, bool pedestal_subtract = 1){
  //gStyle->SetPadRightMargin(0.05);
  //pedestals for 8 PMTs, Sum Left, Sum Right, Sum All
  const int N_CH=11;
  double PMT_ped[N_CH] = {38.9,58.7,40.0,70.1,57.6,57.4,30.1,24.0,72.1,57.6,70.3};

  Double_t ADCchannels = 1100;
  Double_t ADCchPerBin = 5;
  Double_t ADCbinTotal = 1100./5.; //This should ideally result in an integer

  if(!pedestal_subtract) for(int i=0;i<N_CH;++i)PMT_ped[i]=0;
  gStyle->SetOptStat("rem");
  gStyle->SetStatH(0.25);
  gROOT->ForceStyle();
  TChain *ch = new TChain("trADC");
  ch->Add(Form("%s/moller*%i.root",gSystem->Getenv("MOLLER_ROOTFILE_DIR"),run));
  TCanvas *c = new TCanvas("MollerADCs","MollerADCs",0,0,950,1100);
  c->Divide(2,4);
  TCanvas *c2 = new TCanvas("MollerADCsums","MollerADCsums",950,0,950,800);
  c2->Divide(2,2);
  TH1D *h[10];
  int order[8] = {1,3,5,7,2,4,6,8};
  for(int i=0;i<8;++i){
    c->cd(order[i])->SetLogy();
    gPad->SetGrid();
    //ch->Draw(Form("iadc[%i]-%f>>h[%i]",i,PMT_ped[i],i),"","LF2");
    ch->Draw(Form("iadc[%i]-%f>>h[%i](%f,0,%f)",i,PMT_ped[i],i,ADCbinTotal,ADCchannels),"","LF2");
    h[i] = (TH1D*)gDirectory->Get(Form("h[%i]",i));
  }
  Float_t binMin(ADCchannels);
  Float_t binMax(1.);
  for(int i=0; i<8; ++i){
    if( h[i]->FindFirstBinAbove(0.,1) < binMin ) binMin = h[i]->FindFirstBinAbove(0.,1);
    if( h[i]->FindLastBinAbove(0.,1)  > binMax ) binMax = h[i]->FindLastBinAbove(0.,1);
  }
  binMin = floor(binMin / 50.)*50.; 
  binMax = ceil(binMax / 50.)*50.; 
  cout << "binMin: " << binMin << " and binMax: " << binMax << endl;
  for(Int_t i=0;i<8;++i){
    h[i]->SetTitle(Form("ADC Spectrum for PMT %i  |  Run %i",i+1,run));
    h[i]->UseCurrentStyle();
    h[i]->SetLineColor(kGreen+2);
    h[i]->SetLineWidth(2);
    h[i]->GetXaxis()->SetLabelSize(0.06);
    h[i]->GetYaxis()->SetLabelSize(0.05);
    h[i]->GetXaxis()->SetRangeUser( binMin*ADCchannels/ADCbinTotal,binMax*ADCchannels/ADCbinTotal );
    c->cd(order[i])->SetLogy();
    h[i]->Draw();
    gPad->Update();gPad->Modified();
  }
  c->SaveAs( Form("100_adc_individual_plots_%i.png",run) );


  TString sum[3] = {"Left","Right","All"};
  for(int i=8;i<11;++i){
    c2->cd(i-7)->SetGrid();
    //ch->Draw(Form("iadc[%i]-%f>>h[%i]",i,PMT_ped[i],i));
    ch->Draw(Form("iadc[%i]-%f>>h[%i](%f,0,%f)",i,PMT_ped[i],i,ADCbinTotal,ADCchannels));
    h[i] = (TH1D*)gDirectory->Get(Form("h[%i]",i));
    h[i]->SetTitle(Form("ADC Spectrum for %s PMT Sum",sum[i-8].Data()));
    h[i]->UseCurrentStyle();
    h[i]->SetLineColor(kBlue);
    h[i]->SetLineWidth(2);
    h[i]->GetXaxis()->SetRangeUser( h[i]->FindFirstBinAbove(0.,1)*ADCchannels/ADCbinTotal , h[i]->FindLastBinAbove(0.,1)*ADCchannels/ADCbinTotal ); 
    h[i]->Draw();
    gPad->Update();
  }
  c->SaveAs( Form("101_adc_individual_plots_%i.png",run) );

  return ch;
}
