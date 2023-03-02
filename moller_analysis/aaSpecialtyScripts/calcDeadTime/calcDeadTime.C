#include "TH1D.h"
#include "TF1.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TSystem.h"
#include "TStyle.h"
#include <iostream>
#include <stdio.h>
using namespace std;

TChain* calcDeadTime(int run_start, int run_end = 0){
  gStyle->SetOptFit(1111);
  TChain *ch = new TChain("trInc");
  int nruns = run_end==0 ? 1 : run_start - run_end;
  int nfound = 0;
  for(int i=run_start;i<=run_end;++i){
    nfound += ch->Add(Form("%s/molana_increments_%i*root", gSystem->Getenv("MOLLER_ROOTFILE_DIR"),i));
  }
  cout<<nfound<<" files found and added to chain."<<endl;
  //return ch;
  TCanvas *c = new TCanvas("c","c",0,0,1200,1000);
  c->Divide(2,2);
  c->cd(1);
  ch->Draw("(ledflash-(coinc1-accid1))/ledflash:singl1/1000.>>hl(300,,300)");
  gPad->Update();
  TH2D *hl = (TH2D*)gDirectory->Get("hl");
  hl->SetTitle("Missing LED Signal Fraction vs Left Singles Rate");
  hl->GetXaxis()->SetTitle("Left Detector Singles Rate (kHz)");
  hl->GetYaxis()->SetTitle("Missing LED Signal Fraction");
  gPad->Update();
  c->cd(2);
  ch->Draw("(ledflash-(coinc1-accid1))/ledflash:singr1/1000.>>hr(300,,300)");
  gPad->Update();
  TH2D *hr = (TH2D*)gDirectory->Get("hr");
  hr->SetTitle("Missing LED Signal Fraction vs Right Singles Rate");
  hr->GetXaxis()->SetTitle("Right Detector Singles Rate (kHz)");
  hr->GetYaxis()->SetTitle("Missing LED Signal Fraction");
  gPad->Update();
  c->cd(3);
  ch->Draw("(ledflash-(coinc1-accid1))/ledflash:singl1/1000.>>hlpr","","prof");
  gPad->Update();
  TProfile *hlpr = (TProfile*)gDirectory->Get("hlpr");
  hlpr->SetTitle("Missing LED Signal Fraction vs Left Singles Rate");
  hlpr->GetXaxis()->SetTitle("Left Detector Singles Rate (kHz)");
  hlpr->GetYaxis()->SetTitle("Missing LED Signal Fraction");
  TF1 *fl = new TF1("fl","pol0",0,1);
  cout<<"Dead time correction for left singles rate: "<<fl->GetParameter(1)*100<<" (%/kHz)"<<endl;
  hlpr->Fit(fl);
  gPad->Update();
  c->cd(4);
  ch->Draw("(ledflash-(coinc1-accid1))/ledflash:singr1/1000.>>hrpr","","prof");
  gPad->Update();
  TProfile *hrpr = (TProfile*)gDirectory->Get("hrpr");
  hrpr->SetTitle("Missing LED Signal Fraction vs Right Singles Rate");
  hrpr->GetXaxis()->SetTitle("Right Detector Singles Rate (kHz)");
  hrpr->GetYaxis()->SetTitle("Missing LED Signal Fraction");
  TF1 *fr = new TF1("fr","pol0",0,1);
  hrpr->Fit(fr);
  gPad->Update();
  cout<<"Dead time correction for right singles rate: "<<fr->GetParameter(1)*100<<" (%/kHz)"<<endl;
  return ch;
}
