#include "TCanvas.h"
#include "TChain.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"
#include "TLine.h"
#include "TDirectory.h"
#include "TSystem.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TPaveText.h"
#include "TPaveStats.h"
#include <iostream>
using namespace std;

int plotBCM(int run, TString filename=""){
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetOptStat("rem");
  gStyle->SetStatW(0.3);
  gStyle->SetStatH(0.23);
  gROOT->ForceStyle();
  TString fname = filename.Length()==0 ? Form("%s/molana_increments_%i.root",gSystem->Getenv("MOLLER_ROOTFILE_DIR"),run) : filename;
  TChain *ch = new TChain("trInc");
  ch->Add(fname.Data());
  TCanvas *c2 = new TCanvas("MollerBCM","MollerBCM",800,0,700,900);
  c2->Divide(1,2);
  c2->cd(1);
  double max = (int)(1.6*ch->GetMaximum("bcm"));
  if(max>2000) max=2000.;
  ch->Draw(Form("bcm>>h1(%i,0,%f)",int(max),max),"","h1st");
  TH1D *h1 = (TH1D*)gDirectory->Get(Form("h1"));
  h1->SetTitle(Form("BCM Counts per Helicity Window"));
  c2-> Modified(); c2->Update();
  TPaveStats *ps = (TPaveStats*)h1->GetListOfFunctions()->FindObject("stats");
  ps->SetY1NDC(0.5);
  h1->SetLineColor(kBlue);
  h1->SetLineWidth(2);
  h1->Draw();
  gPad->Update();
  h1->GetXaxis()->SetTitle("BCM Counts per Helicity Window");
  gPad->Update();
  int binmode = (int)max*0.01;
  double mode =  h1->GetBinCenter(binmode);
  double max_on = h1->GetBinContent(binmode);
  for(int i=binmode+1;i<h1->GetNbinsX();++i)
    if(h1->GetBinContent(i)>max_on){
      max_on = h1->GetBinContent(i);
      mode = h1->GetBinCenter(i);
    }
  TPaveText *pt = new TPaveText(0.7,0.15,0.94,0.3,"ndc");
  pt->SetFillColor(0);
  pt->SetBorderSize(0);
  pt->SetShadowColor(0);
  pt->AddText(Form("Peak BCM: %i",(int)mode));
  pt->SetTextColor(kBlue);
  pt->Draw();
  gPad->Update();
  gStyle->SetOptFit(1111);
  TF1 *f = new TF1("f","gaus",mode-10,mode+10);
  h1->Fit(f,"r");
  cout<<"The BCM counts per helicity window is approximately "<<mode<<endl;
  c2->cd(2);
  double n = 1.1*(double)ch->GetEntries();
  ch->Draw(Form("bcm:Entry$>>h2(100,0,%f,300,0,%f)",n,max));
  TH2D *h2 = (TH2D*)gDirectory->Get(Form("h2"));
  gPad->Update();
  h2->SetTitle(Form("BCM Counts vs Entry"));
  // h2->UseCurrentStyle();
  h2->SetLineColor(kBlack);
  h2->SetLineWidth(2);
  h2->Draw();
  gPad->Update();
  h2->GetXaxis()->SetTitle("Entry");
  h2->GetYaxis()->SetTitle("BCM Counts per Helicity Window");
  gPad->Update();
  TPaveStats *ps2 = (TPaveStats*)h2->FindObject("stats");
  ps2->SetY1NDC(0.7);
  return mode;
}
