#include "TChain.h"
#include "TH1D.h"
#include "stdio.h"
#include <iostream>
#include "TCanvas.h"

int getRateAndBCMincrements(int run){
  double singl1, singr1, coinc, bcm, accid;
  TChain *ch = new TChain("trInc");
  ch->Add(Form("/work/halla/moller/disk1/a-molana/moller_data/molana_increments_%i.root",run));
  TCanvas *c = new TCanvas("c","c",0,0,900,900);
  c->Divide(2,3);
  c->cd(1);
  ch->Draw("singl1/0.03375>>hsingl1");
  TH1D *hsingl1 = (TH1D*)gDirectory->Get("hsingl1");
  singl1=hsingl1->GetMean();
  c->cd(2);
  ch->Draw("singr1/0.03375>>hsingr1");
  TH1D *hsingr1 = (TH1D*)gDirectory->Get("hsingr1");
  singr1=hsingr1->GetMean();
  c->cd(3);
  ch->Draw("coinc1/0.03375>>hcoinc");
  TH1D *hcoinc = (TH1D*)gDirectory->Get("hcoinc");
  coinc=hcoinc->GetMean();
  c->cd(4);
  ch->Draw("accid1/0.03375>>haccid");
  TH1D *haccid = (TH1D*)gDirectory->Get("haccid");
  accid=haccid->GetMean();
  c->cd(5);
  ch->Draw("bcm/0.03375>>hbcm");
  TH1D *hbcm = (TH1D*)gDirectory->Get("hbcm");
  bcm=hbcm->GetMean();

  cout<<singl1<<"  "<<singr1<<"  "<<coinc<<"  "<<accid<<"  "<<bcm<<endl;
  return 0;
}
