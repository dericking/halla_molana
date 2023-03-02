/*************************************************************************************
* Script to plot rate scans -- Eric Jan 2023
* Command line usage: root -l plotRateScans+'(mag,start,end,group,bcmperua,exclude)'
* Mag: (int) Magnet being scanned
* Start: (int) Starting run number
* End: (int) Ending run number
* Group: (int) [optional] default 0; group number -- if assigned will print and copy 
* BcmPerMuAmp: (float) [optinal] default 1; bcm per microamp to scale to rates  
* Exclude: (char *) [optional] default ""; list of runs to exclude "1234,1235,1236"
**************************************************************************************/

#include<TROOT.h>
#include<TSystem.h>
#include<iostream>
#include<iomanip>
#include<TString.h>
#include<TSQLServer.h>
#include<TString.h>
#include<TSQLResult.h>
#include<TSQLRow.h>
#include<TGraph.h>
#include<TGraphErrors.h>
#include<TCanvas.h>
#include<TMultiGraph.h>
#include<TLegend.h>
#include<TStyle.h>
#include<TPad.h>
#include<vector>
#include<algorithm>

#include<string>
#include<sstream>

Int_t plotRateScans(Int_t magnet, Int_t startRun, Int_t endRun, Int_t group=0, Float_t bcmperua=1., TString excludeRunList=""){

  Int_t setwvalue = 12;         
  Int_t setprecvalue = 0;
  if( bcmperua==1. ) setprecvalue = 2;
  const Float_t bcmPerMicroamp = bcmperua;
  
  //We will want to calculate exactly what runs will be included... although
  //I think we can query all of the runs in the specified range
  vector<Int_t> useTheseRuns;  //Final list of runs to be plotted
  vector<Int_t> cutTheseRuns;  //List of runs to cut from specified range

  //////////////////////////////////////////////////////////////
  //// Runs to be looked at 
  //// TODO: Let's start off with just the list of included runs.
  vector<Int_t> vExcludeRuns;
  std::stringstream ss;
  ss << excludeRunList;
  while(ss.good()){
    string ss_parse;
    getline(ss,ss_parse,',');
    vExcludeRuns.push_back( std::atoi(ss_parse.c_str()) );
  }

  ///////////////////////////////////////////////////////////////  (╯°□°）╯︵ ┻━┻
  //CHECK FOR THE EXISTENCE OF ENVIRONMENTAL VARIABLES
  if(!gSystem->Getenv("MOLLER_ROOTFILE_DIR")){
    cout <<"Set environment variable MOLLER_ROOTFILE_DIR\n"
         <<"to point at the directory where the .root files\n"
         <<"should be stored then rerun."<<endl;
    exit(1);
  }
  if(!gSystem->Getenv("MOLANA_DB_HOST")){
    cout << "Set environment variable MOLANA_DB_HOST" << endl
         << "to point at the directory where the .root files" << endl
         << "should be stored then rerun." << endl << endl
         << "Turning off write to SQL database functionality." << endl;
  }
  if(!gSystem->Getenv("MOLANA_DB_USER")){
    cout << "Set environment variable MOLANA_DB_USER" << endl
         << "to point at the directory where the .root files" << endl
         << "should be stored then rerun." << endl << endl
         << "Turning off write to SQL database functionality." << endl;
  }
  if(!gSystem->Getenv("MOLANA_DB_PASS")){
    cout << "Set environment variable MOLANA_DB_PASS" << endl
         << "to point at the directory where the .root files" << endl
         << "should be stored then rerun." << endl << endl
         << "Turning off write to SQL database functionality." << endl;
  }
  if(!gSystem->Getenv("MOLANA_DB_NAME")){
    cout << "Set environment variable MOLANA_DB_NAME" << endl
         << "to point at the directory where the .root files" << endl
         << "should be stored then rerun." << endl << endl
         << "Turning off write to SQL database functionality." << endl;
  }

  TString db_host;
  TString db_user;
  TString db_pass;

  db_host = Form("mysql://%s/%s",gSystem->Getenv("MOLANA_DB_HOST"),gSystem->Getenv("MOLANA_DB_NAME"));
  db_user = Form("%s",gSystem->Getenv("MOLANA_DB_USER"));
  db_pass = Form("%s",gSystem->Getenv("MOLANA_DB_PASS"));

  TSQLServer* server = TSQLServer::Connect(db_host,db_user,db_pass);
  
  //TODO: ADD MAGNET SETTINGS TO THIS
  TString select1_query = "SELECT id_run, run_leftrate, run_rightrate, run_coinrate, ";
  TString select2_query = "run_accrate, run_bcm, run_asym, run_asymerr, ";
  TString select3_query = "runmag_q1set, runmag_q2set, runmag_q3set, runmag_q4set, runmag_dpset, runmag_hhset "; 
  TString from_query    = "FROM moller_run, moller_run_magnets ";
  TString where_query; 
    where_query.Form("WHERE id_run >= %i AND id_run <= %i AND id_run = id_runmag ",startRun,endRun);
  TString order_query   = "ORDER BY id_run ASC;";

  TString total_query  = select1_query + select2_query + select3_query + from_query + where_query + order_query;
 
  //For debugging purposes if needed
  cout << "query: " << total_query << endl;

  TSQLResult * result = server->Query(total_query);
  TSQLRow    * row = NULL;

  if(result == NULL){
    delete row;
    delete result;
    server->Close();
    delete server;
    exit(-5);
  }

  const Int_t numcols = result->GetFieldCount();
  const Int_t numrows = result->GetRowCount();

  //Debugging printout if needed.
  //cout << "Cols: " << numcols << "\nRows: " << numrows << endl;

  row = result->Next();
  if(row == NULL){
    delete row;
    delete result;
    server->Close();
    delete server;
    exit (-10);
  }
  
  //EXAMPLE CODE!!!!!!!!
  //CURERNTLY: 
  //  0: run, 
  //  1: left, 
  //  2: right, 
  //  3: coin, 
  //  4: acc, 
  //  5: bcm, 
  //  6: asym, 
  //  7: asymerr, 
  //  8: magq1, 
  //  9: magq2, 
  // 10: magq3, 
  // 11: magq4, 
  // 12: magdp, 
  // 13: maghh
  // Declare vectors that will hold table data outside the for(for()) that cycles through the data.
  vector<Float_t> vRun;
  vector<Float_t> vLeftrate;
  vector<Float_t> vRghtrate;
  vector<Float_t> vAccdrate;
  vector<Float_t> vCoinrate;
  vector<Float_t> vBcm;
  vector<Float_t> vAsymval;
  vector<Float_t> vAsymerr;
  vector<Float_t> vQ1set;
  vector<Float_t> vQ2set;
  vector<Float_t> vQ3set;
  vector<Float_t> vQ4set;
  vector<Float_t> vDpset;
  vector<Float_t> vHhset;
  for(Int_t i = 0; i < numrows; i++){ //Goes over each row of the returned data.
    //cout << " >>> ROW #" << i << " <<< " << endl;
    std::vector< Double_t > temp; //TODO: WHY AM I USING THIS???
    Int_t   run = 0;
    Float_t leftrate = 0.0;
    Float_t rghtrate = 0.0;
    Float_t accdrate = 0.0;
    Float_t coinrate = 0.0;
    Float_t bcm     = 0.0;
    Float_t asymval = 0.0;
    Float_t asymerr = 0.0;
    Float_t q1set = 0.0;
    Float_t q2set = 0.0;
    Float_t q3set = 0.0;
    Float_t q4set = 0.0;
    Float_t dpset = 0.0;
    Float_t hhset = 0.0;
    for(Int_t j = 0; j < numcols; j++){ //Goes over each field in the row data
      //cout << "Column #" << j << endl;
      TString value = row->GetField(j);
      TString field = result->GetFieldName(j);
      //cout << " Field Name: " << field << endl;
      if( field=="id_run"       ){ run      = value.Atoi(); /*cout << " *run: " << run << endl;*/}
      if( field=="run_leftrate" ){ leftrate = value.Atof(); }
      if( field=="run_rightrate"){ rghtrate = value.Atof(); }
      if( field=="run_accrate"  ){ accdrate = value.Atof(); }
      if( field=="run_coinrate" ){ coinrate = value.Atof(); }
      if( field=="run_asym"     ){ asymval  = value.Atof(); }
      if( field=="run_asymerr"  ){ asymerr  = value.Atof(); }
      if( field=="run_bcm"      ){ bcm      = value.Atof(); }
      if( field=="runmag_q1set" ){ q1set    = value.Atof(); /*cout << "  *q1: " << q1set << endl;*/}
      if( field=="runmag_q2set" ){ q2set    = value.Atof(); /*cout << "  *q2: " << q1set << endl;*/}
      if( field=="runmag_q3set" ){ q3set    = value.Atof(); /*cout << "  *q3: " << q1set << endl;*/}
      if( field=="runmag_q4set" ){ q4set    = value.Atof(); /*cout << "  *q4: " << q1set << endl;*/}
      if( field=="runmag_dpset" ){ dpset    = value.Atof(); /*cout << "  *dp: " << q1set << endl;*/}
      if( field=="runmag_hhset" ){ hhset    = value.Atof(); /*cout << "  *hh: " << q1set << endl;*/}
      temp.push_back( value.Atof() ); //TODO: WHY AM I DOING THIS???
      } // End for(each row field)

    //TODO: Verify this works, we want a conditional statement here that ignores runs on the exclude list.
    //Excluded list has been converted to a <vector> named vExcludedRuns
    if(1){
      vRun.push_back(run);
      vLeftrate.push_back(leftrate);
      vRghtrate.push_back(rghtrate);
      vAccdrate.push_back(accdrate);
      vCoinrate.push_back(coinrate);
      vAsymval.push_back(asymval);
      vAsymerr.push_back(asymerr);
      vBcm.push_back(bcm);
      vQ1set.push_back(q1set);
      vQ2set.push_back(q2set);
      vQ3set.push_back(q3set);
      vQ4set.push_back(q4set);
      vDpset.push_back(dpset);
      vHhset.push_back(hhset); 
    } // End if(exclude) conditional

    row = result->Next();
  } // End for(each data row)

  // Pick the proper magnet :)
  vector<Float_t> vMagnet;
  if(magnet==1) vMagnet = vQ1set;
  if(magnet==2) vMagnet = vQ2set;
  if(magnet==3) vMagnet = vQ3set;
  if(magnet==4) vMagnet = vQ4set;
  if(magnet==5) vMagnet = vDpset;
  if(magnet==6) vMagnet = vHhset;
  
  // Normalize the left,right,acc,coin vectors by the BCM vector, scale to max rate, then print
  cout << std::fixed;
  cout << endl << "BCM Normalized Values" << endl;
  if (bcmPerMicroamp>1.) cout << "  Scaled to per uA rates" << endl;
  cout << std::setw(setwvalue) << "Run" 
       << std::setw(setwvalue) << "MagSet" 
       << std::setw(setwvalue) << "Left" 
       << std::setw(setwvalue) << "Right" 
       << std::setw(setwvalue) << "Accid" 
       << std::setw(setwvalue) << "Coin" 
       << endl;
  for(Int_t i = 0; i < (Int_t)vRun.size(); i++){
    vLeftrate[i] *= 1. / vBcm[i] * bcmPerMicroamp;
    vRghtrate[i] *= 1. / vBcm[i] * bcmPerMicroamp;
    vAccdrate[i] *= 1. / vBcm[i] * bcmPerMicroamp;
    vCoinrate[i] *= 1. / vBcm[i] * bcmPerMicroamp;
    cout << std::setprecision(0) 
         << std::setw(setwvalue) << vRun[i] 
         << std::setprecision(2) 
         << std::setw(setwvalue) << vMagnet[i] 
         << std::setw(setwvalue) << vLeftrate[i] 
         << std::setw(setwvalue) << vRghtrate[i] 
         << std::setw(setwvalue) << vAccdrate[i] 
         << std::setw(setwvalue) << vCoinrate[i] 
         << endl;
  }

  TString vertUnit = "";
  if( bcmPerMicroamp > 1.) vertUnit += "[Hz/#mu Amp]";
  if( bcmPerMicroamp ==1.) vertUnit += "[per BCM]"; 

  // Assign data to graphs
  TGraph * grLeftrate = new TGraph( vMagnet.size() , &vMagnet[0] , &vLeftrate[0] );
  grLeftrate->SetTitle( Form("Left Rate; Magnet %i Setpoint [Amps];Rate %s",magnet,vertUnit.Data() ) );
  grLeftrate->SetLineWidth(2);
  grLeftrate->SetLineColor( kRed );
  grLeftrate->SetMarkerColor( kRed );
  grLeftrate->SetMarkerStyle( 21 );
  
  TGraph * grRghtrate = new TGraph( vMagnet.size() , &vMagnet[0] , &vRghtrate[0] );
  grRghtrate->SetTitle( Form("Right Rate;Magnet %i Setting [Amps];Rate %s",magnet,vertUnit.Data() ) );
  grRghtrate->SetLineWidth(2);
  grRghtrate->SetLineColor( kBlue );
  grRghtrate->SetMarkerColor( kBlue );
  grRghtrate->SetMarkerStyle( 21 );
  
  TGraph * grAccdrate = new TGraph( vMagnet.size() , &vMagnet[0] , &vAccdrate[0] );
  grAccdrate->SetTitle( Form("Accidental Rate;Magnet %i Setting [Amps];Rate %s",magnet,vertUnit.Data() ) );
  grAccdrate->SetLineWidth(2);
  grAccdrate->SetLineColor( kGreen );
  grAccdrate->SetMarkerColor( kGreen );
  grAccdrate->SetMarkerStyle( 21 );
  
  TGraph * grCoinrate = new TGraph( vMagnet.size() , &vMagnet[0] , &vCoinrate[0] );
  grCoinrate->SetTitle( Form("Coincidence Rate;Magnet %i Setting [Amps];Rate %s",magnet,vertUnit.Data() ) );
  grCoinrate->SetLineWidth(2);
  grCoinrate->SetLineColor( kBlack );
  grCoinrate->SetMarkerColor( kBlack );
  grCoinrate->SetMarkerStyle( 21 );
  
  // Plot the data
  TCanvas * C = new TCanvas("C","C",1200,800);
  C->Divide(2,2);
  C->cd(1)->SetGrid();
  grLeftrate->Draw("AP");
  C->cd(2)->SetGrid();
  grRghtrate->Draw("AP");
  C->cd(3)->SetGrid();
  grAccdrate->Draw("AP");
  C->cd(4)->SetGrid();
  grCoinrate->Draw("AP");

  if(group!=0) C->Print( Form("rate_scan_group_%i",group) );

  return 0;
  
}

