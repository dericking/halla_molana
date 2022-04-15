#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <sstream>
#include <TString.h>
#include <iostream>
#include <TSystem.h>
#include <TBranch.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TF1.h>
#include <TChain.h>

Int_t molana_pcrex_frontpage(){
   
  if(!gSystem->Getenv("MOLANA_DB_HOST")){
    cout << "Set environment variable MOLANA_DB_HOST not found." << endl;
    exit(0);
  }
  if(!gSystem->Getenv("MOLANA_DB_USER")){
    cout << "Set environment variable MOLANA_DB_USER not found." << endl;
    exit(1);
  }
  if(!gSystem->Getenv("MOLANA_DB_PASS")){
    cout << "Set environment variable MOLANA_DB_PASS not found." << endl;
    exit(2);
  }
  if(!gSystem->Getenv("MOLANA_DB_NAME")){
    cout << "Set environment variable MOLANA_DB_NAME not found." << endl;
    exit(3);
  }

  TString db_host;
  TString db_user;
  TString db_pass;

  db_host = Form("mysql://%s/%s",gSystem->Getenv("MOLANA_DB_HOST"),gSystem->Getenv("MOLANA_DB_NAME"));
  db_user = Form("%s",gSystem->Getenv("MOLANA_DB_USER"));
  db_pass = Form("%s",gSystem->Getenv("MOLANA_DB_PASS"));




  return(100);
}
