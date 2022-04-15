#include<TROOT.h>
#include<TRandom.h>
#include<TSystem.h>
#include<TFile.h>
#include<TTree.h>
#include<TH1.h>
#include<TH2.h>
#include<TGraphErrors.h>
#include<TMultiGraph.h>
#include<TString.h>
#include<TCanvas.h>
#include<iostream>

Int_t genasymplots(Int_t GROUP, Float_t PATTPOL0VAL, Float_t PATTPOL0ERR, Float_t PATTGAUSVAL, Float_t PATTGAUSERR, Float_t PBLKPOL0VAL, Float_t PBLKPOL0ERR, Float_t PBLKGAUSVAL, Float_t PBLKGAUSERR){

    Float_t Ay[1]  = {PATTPOL0VAL};
    Float_t Ax[1]  = {1.0};
    Float_t Aye[1] = {PATTPOL0ERR};
    Float_t Bx[1]  = {2.0};
    Float_t By[1]  = {PATTGAUSVAL};
    Float_t Bye[1] = {PATTGAUSERR};
    Float_t Cx[1]  = {3.0};
    Float_t Cy[1]  = {PBLKPOL0VAL};
    Float_t Cye[1] = {PBLKPOL0ERR};
    Float_t Dx[1]  = {4.0};
    Float_t Dy[1]  = {PBLKGAUSVAL};
    Float_t Dye[1] = {PBLKGAUSERR};
    

    TGraphErrors * grA = new TGraphErrors(1,Ax,Ay,0,Aye);
    grA->SetMarkerColor(kRed);
    grA->SetMarkerStyle(20);
    //grA->SetMarkerStyle(107);
    grA->SetLineColor(kBlack);
    grA->SetMarkerSize(2);
    
    TGraphErrors * grB = new TGraphErrors(1,Bx,By,0,Bye);
    grB->SetMarkerColor(kOrange);
    grB->SetMarkerStyle(21);
    //grB->SetMarkerStyle(108);
    grB->SetLineColor(kBlack);
    grB->SetMarkerSize(2);
    
    TGraphErrors * grC = new TGraphErrors(1,Cx,Cy,0,Cye);
    grC->SetMarkerColor(kGreen);
    grC->SetMarkerStyle(22);
    //grC->SetMarkerStyle(109);
    grC->SetLineColor(kBlack);
    grC->SetMarkerSize(2);
    
    TGraphErrors * grD = new TGraphErrors(1,Dx,Dy,0,Dye);
    grD->SetMarkerColor(kBlue);
    grD->SetMarkerStyle(23);    
    //grD->SetMarkerStyle(110);    
    grD->SetLineColor(kBlack);
    grD->SetMarkerSize(2);

    TMultiGraph * mgr  = new TMultiGraph();
    mgr->SetTitle( Form("Plot of Fits : Group %d;Fit;Asymmetry",GROUP) );
    mgr->Add(grA);
    mgr->Add(grB);
    mgr->Add(grC);
    mgr->Add(grD);

    TCanvas * C = new TCanvas("C","C",750,375);
    mgr->Draw("AP");

    C->SaveAs( Form("Group-%d_Asymmetry-Fits.png",GROUP) );

    return(100);
}


