#include<TROOT.h>
#include<TRandom.h>
#include<TSystem.h>
#include<TFile.h>
#include<TTree.h>
#include<TH1.h>
#include<TH2.h>
#include<iostream>

Int_t genasym(){
  Double_t rate = 151.1;
  Double_t width = 4.3;
  Double_t asym = 0.0122;

  TH1F * h1 = new TH1F("asym1","asym",177,-0.2,0.2); 
  TH1F * h2 = new TH1F("asym2","asym",201,-0.2,0.2); 
  TH1F * h3 = new TH1F("asym3","asym",201,-0.2,0.2); 
  TH1F * h4 = new TH1F("asym4","asym",201,-0.2,0.2); 
  TH1F * h5 = new TH1F("asym5","asym",201,-0.2,0.2); 
  TH1F * h6 = new TH1F("asym6","asym",201,-0.2,0.2); 
  TH1F * h7 = new TH1F("asym7","asym",201,-0.2,0.2); 
  TH1F * h8 = new TH1F("asym8","asym",201,-0.2,0.2); 
  TH1F * h9 = new TH1F("asym9","asym",201,-0.2,0.2); 
  TH1F * r = new TH1F("rate","rate",201,0,201);

  for(Int_t i=0; i < 50000 ; ++i){
    Double_t ASYM = gRandom->Poisson(asym);
    Double_t np = gRandom->Gaus( rate*(1+asym) , width*(1+asym) ); //ARE THESE CORRECT? Second argument????
    Double_t nn = gRandom->Gaus( rate*(1-asym) , width*(1-asym) ); //ARE THESE CORRECT? Second argument????
    r->Fill(nn);
    r->Fill(np);
    np = (np - Int_t(np) > 0.5 ? (Double_t)((Int_t)np)+1 : (Double_t)((Int_t)np)); 
    nn = (nn - Int_t(nn) > 0.5 ? (Double_t)((Int_t)nn)+1 : (Double_t)((Int_t)nn));
    cout << "[" << std::setw(8) << i << "] n+: " << std::fixed << np << " , n-: " << std::fixed << nn << endl;
    Double_t asym = 1./(Double_t)(np-nn)/(Double_t)(np+nn);
    h1->Fill(asym);
  } 

  TCanvas * C = new TCanvas("C","C",1600,800);
  C->Divide(2,1);
  C->cd(1);
  h1->Draw();
  C->cd(2);
  r->Draw();

  return 100;

}


