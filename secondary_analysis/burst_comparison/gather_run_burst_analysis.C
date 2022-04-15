#include<TROOT.h>
#include<iostream>
#include<iomanip>
#include<TString.h>
#include<TSQLServer.h>
#include<TString.h>
#include<TSQLResult.h>
#include<TSQLRow.h>
#include<TGraph.h>
#include<TGraphErrors.h>
#include<TMultiGraph.h>
#include<TLegend.h>
#include<TStyle.h>
#include<vector>

/*
CREATE TABLE pcrex_run_burst_analysis (
  id_runburst INT NOT NULL,
  runburst_0_mean     
  runburst_1_mean     
  runburst_1_error    
  rubburst_2_mean     
  rubburst_2_error    
  runburst_5_mean     
  runburst_5_error    
  runburst_10_mean    
  runburst_10_error   
  runburst_25_mean    
  runburst_25_error   
  runburst_50_mean    
  runburst_50_error   
  runburst_100_mean   
  runburst_100_error  
  runburst_250_mean   
  runburst_250_error  
  PRIMARY KEY (id_runburst),
  FOREIGN KEY (id_runburst) REFERENCES moller_run(id_run)
);
*/

Int_t gather_run_burst_analysis(){

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

  TString select_query = "SELECT pcrex_run_burst_analysis.* , rundet_pcrex_group ";
  TString from_query   = "FROM pcrex_run_burst_analysis, moller_run_details ";
  TString where_query  = "WHERE id_runburst = id_rundet AND rundet_type LIKE 'beam_pol%' ";
  TString order_query  = "ORDER BY id_rundet ASC;";

  TString total_query  = select_query + from_query + where_query + order_query;

  cout << "query: " << endl << "     " << total_query << endl;

  TSQLResult * result = server->Query(total_query);
  TSQLRow    * row;

  if(result == NULL){
    delete row;
    delete result;
    server->Close();
    delete server;
    exit(-5);
  }

  const Int_t numcols = result->GetFieldCount();
  const Int_t numrows = result->GetRowCount();

  cout << "Cols: " << numcols << "\nRows: " << numrows << endl;

  row = result->Next();
  if(row == NULL){
    delete row;
    delete result;
    server->Close();
    delete server;
    exit (-10);
  }

  std::vector< Double_t > runnumbersprex;
  std::vector< Double_t > asympctchgprex;
  std::vector< Double_t > asympctdifprex;

  std::vector< Double_t > runnumberscrex;
  std::vector< Double_t > asympctchgcrex;
  std::vector< Double_t > asympctdifcrex;

  for(Int_t i = 0; i < numrows; i++){
    cout << "Starting row #" << i << endl;
    Int_t    id_runburst;
    Int_t    id_group;
    Double_t runburst_0_mean[1];   //0 
    Double_t runburst_1_mean[1];   //1  
    Double_t runburst_1_error[1];    
    Double_t runburst_2_mean[1];   //2  
    Double_t runburst_2_error[1];    
    Double_t runburst_5_mean[1];   //3  
    Double_t runburst_5_error[1];    
    Double_t runburst_10_mean[1];  //4  
    Double_t runburst_10_error[1];   
    Double_t runburst_25_mean[1];  //5  
    Double_t runburst_25_error[1];   
    Double_t runburst_50_mean[1];  //6  
    Double_t runburst_50_error[1];   
    Double_t runburst_100_mean[1]; //7  
    Double_t runburst_100_error[1];  
    Double_t runburst_250_mean[1]; //8  
    Double_t runburst_250_error[1];
    Double_t all_asymmetry_mean[9];
    Double_t all_asymmetry_error[9];
    Double_t all_asymmetry_pos[9] = {0.,1.,2.,3.,4.,5.,6.,7.,8.,};
    Double_t runburst_0_pos[1]   = {0.};
    Double_t runburst_1_pos[1]   = {1.};
    Double_t runburst_2_pos[1]   = {2.};
    Double_t runburst_5_pos[1]   = {3.};
    Double_t runburst_10_pos[1]  = {4.};
    Double_t runburst_25_pos[1]  = {5.};
    Double_t runburst_50_pos[1]  = {6.};
    Double_t runburst_100_pos[1] = {7.};
    Double_t runburst_250_pos[1] = {8.};

    for(Int_t j = 0; j < numcols; j++){
      TString val    = row->GetField(j);
      TString field  = result->GetFieldName(j);
      Double_t value = val.Atof();
      if( field=="rundet_pcrex_group"){ id_group = value; }
      if( field=="id_runburst"      ){ id_runburst = value;cout << "Found new run... " << id_runburst << endl;}
      if( field=="runburst_0_mean"  ){ runburst_0_mean[0]=value; all_asymmetry_mean[0]=value; all_asymmetry_error[0]=0.0;}

      if( field=="runburst_1_mean"  ){ runburst_1_mean[0]=value;   all_asymmetry_mean[1]=value; }
      if( field=="runburst_2_mean"  ){ runburst_2_mean[0]=value;   all_asymmetry_mean[2]=value; }
      if( field=="runburst_5_mean"  ){ runburst_5_mean[0]=value;   all_asymmetry_mean[3]=value; }
      if( field=="runburst_10_mean" ){ runburst_10_mean[0]=value;  all_asymmetry_mean[4]=value; }
      if( field=="runburst_25_mean" ){ runburst_25_mean[0]=value;  all_asymmetry_mean[5]=value; }
      if( field=="runburst_50_mean" ){ runburst_50_mean[0]=value;  all_asymmetry_mean[6]=value; }
      if( field=="runburst_100_mean"){ runburst_100_mean[0]=value; all_asymmetry_mean[7]=value; }
      if( field=="runburst_250_mean"){ runburst_250_mean[0]=value; all_asymmetry_mean[8]=value; }

      if( field=="runburst_1_error"  ){ runburst_1_error[0]=value;   all_asymmetry_error[1]=value; }
      if( field=="runburst_2_error"  ){ runburst_2_error[0]=value;   all_asymmetry_error[2]=value; }
      if( field=="runburst_5_error"  ){ runburst_5_error[0]=value;   all_asymmetry_error[3]=value; }
      if( field=="runburst_10_error" ){ runburst_10_error[0]=value;  all_asymmetry_error[4]=value; }
      if( field=="runburst_25_error" ){ runburst_25_error[0]=value;  all_asymmetry_error[5]=value; }
      if( field=="runburst_50_error" ){ runburst_50_error[0]=value;  all_asymmetry_error[6]=value; }
      if( field=="runburst_100_error"){ runburst_100_error[0]=value; all_asymmetry_error[7]=value; }
      if( field=="runburst_250_error"){ runburst_250_error[0]=value; all_asymmetry_error[8]=value; }

    }

    if( id_runburst >= 17093 && id_runburst <= 18423 ) runnumbersprex.push_back( id_runburst );
    if( id_runburst >= 17093 && id_runburst <= 18423 ) asympctchgprex.push_back( 100 * (runburst_10_mean[0] - runburst_1_mean[0])/runburst_1_mean[0] );
    if( id_runburst >= 17093 && id_runburst <= 18423 ) asympctdifprex.push_back( 100 * fabs(runburst_10_mean[0] - runburst_1_mean[0])/(0.5 * (runburst_10_mean[0] + runburst_1_mean[0] ) ) );

    if( id_runburst >= 18865 && id_runburst <= 19396 ) runnumberscrex.push_back( id_runburst );
    if( id_runburst >= 18865 && id_runburst <= 19396 ) asympctchgcrex.push_back( 100 * (runburst_10_mean[0] - runburst_1_mean[0])/runburst_1_mean[0] );
    if( id_runburst >= 18865 && id_runburst <= 19396 ) asympctdifcrex.push_back( 100 * fabs(runburst_10_mean[0] - runburst_1_mean[0])/(0.5 * (runburst_10_mean[0] + runburst_1_mean[0] ) ) );

    //DO THE GRAPH STUFF HERE AND SAVE WILL DELETE ON LOOPING
    TMultiGraph * mgraph = new TMultiGraph();
    mgraph->SetTitle( Form("Asymmetry v. Burst Size :: Run #%i :: Group %i",id_runburst,id_group) );

    auto leg = new TLegend(0.88,0.12,0.99,0.88);

    TGraph *gr0 = new TGraph( 1,runburst_0_pos,runburst_0_mean );
    gr0->SetTitle("Mean Over All Patterns");
    gr0->SetMarkerStyle(29);
    gr0->SetMarkerSize(3);
    gr0->SetMarkerColor(kOrange);
    mgraph->Add(gr0);
    leg->AddEntry(gr0);

    TGraphErrors *gr1 = new TGraphErrors( 1,runburst_1_pos,runburst_1_mean ,0, runburst_1_error );
    gr1->SetTitle("By Single Pattern");
    gr1->SetMarkerStyle(20);
    gr1->SetMarkerSize(3);
    gr1->SetMarkerColor(kRed);
    mgraph->Add(gr1);
    leg->AddEntry(gr1);

    TGraphErrors *gr2 = new TGraphErrors( 1,runburst_2_pos,runburst_2_mean ,0, runburst_2_error );
    gr2->SetTitle("2 Pattern Burst");
    gr2->SetMarkerStyle(21);
    gr2->SetMarkerSize(3);
    gr2->SetMarkerColor(kBlue+2);
    mgraph->Add(gr2);
    leg->AddEntry(gr2);

    TGraphErrors *gr3 = new TGraphErrors( 1,runburst_5_pos,runburst_5_mean ,0, runburst_5_error );
    gr3->SetTitle("5 Pattern Burst");
    gr3->SetMarkerStyle(22);
    gr3->SetMarkerSize(3);
    gr3->SetMarkerColor(kCyan+1);
    mgraph->Add(gr3);
    leg->AddEntry(gr3);

    TGraphErrors *gr4 = new TGraphErrors( 1,runburst_10_pos,runburst_10_mean ,0, runburst_10_error );
    gr4->SetTitle("10 Pattern Burst");
    gr4->SetMarkerStyle(23);
    gr4->SetMarkerSize(3);
    gr4->SetMarkerColor(kGreen);
    mgraph->Add(gr4);
    leg->AddEntry(gr4);

    TGraphErrors *gr5 = new TGraphErrors( 1,runburst_25_pos,runburst_25_mean ,0, runburst_25_error );
    gr5->SetTitle("25 Pattern Burst");
    gr5->SetMarkerStyle(33);
    gr5->SetMarkerSize(3);
    gr5->SetMarkerColor(kMagenta);
    mgraph->Add(gr5);
    leg->AddEntry(gr5);

    TGraphErrors *gr6 = new TGraphErrors( 1,runburst_50_pos,runburst_50_mean ,0, runburst_50_error );
    gr6->SetTitle("50 Pattern Burst");
    gr6->SetMarkerStyle(34);
    gr6->SetMarkerSize(3);
    gr6->SetMarkerColor(kTeal);
    mgraph->Add(gr6);
    leg->AddEntry(gr6);

    TGraphErrors *gr7 = new TGraphErrors( 1,runburst_100_pos,runburst_100_mean,0,runburst_100_error );
    gr7->SetTitle("100 Pattern Burst");
    gr7->SetMarkerStyle(47);
    gr7->SetMarkerSize(3);
    gr7->SetMarkerColor(kSpring+9);
    mgraph->Add(gr7);
    leg->AddEntry(gr7);

    TGraphErrors *gr8 = new TGraphErrors( 1,runburst_250_pos,runburst_250_mean,0,runburst_250_error );
    gr8->SetTitle("250 Pattern Burst");
    gr8->SetMarkerStyle(39);
    gr8->SetMarkerSize(3);
    gr8->SetMarkerColor(kViolet+1);
    mgraph->Add(gr8);
    leg->AddEntry(gr8);

    TCanvas * C = new TCanvas("C","C",1500,500);
    mgraph->Draw("AP");
    leg->Draw();

    C->SaveAs( Form("Burst_Comparison_Run_%i__Group_%i.png",id_runburst,id_group) );

    row = result->Next();
  }

  TH1F * hburstpctchgprex = new TH1F("hburstpctchgprex","PREX Pattern to Burst Asym Shift | Percent Change Patt->Burst;Percent [%]",201,-1,1);
  TH1F * hburstpctdifprex = new TH1F("hburstpctdifprex","PREX Pattern to Burst Asym Shift | Percent Diff  fabs(diff())/average()",201,-1,1);
  TH1F * hburstpctchgcrex = new TH1F("hburstpctchgcrex","CREX Pattern to Burst Asym Shift | Percent Change Patt->Burst;Percent [%]",201,-1,1);
  TH1F * hburstpctdifcrex = new TH1F("hburstpctdifcrex","CREX Pattern to Burst Asym Shift | Percent Change Patt->;Percent [%]",201,-1,1);

  TGraph * grBurstPctChangePrex = new TGraph(runnumbersprex.size(),&runnumbersprex[0],&asympctchgprex[0]); 
  grBurstPctChangePrex->SetMarkerStyle(7);
  grBurstPctChangePrex->SetMarkerColor(kRed);
  grBurstPctChangePrex->SetTitle("PREX Percent Change");

  TGraph * grBurstPctChangeCrex = new TGraph(runnumbersprex.size(),&runnumbersprex[0],&asympctdifprex[0]);  
  grBurstPctChangeCrex->SetMarkerStyle(7);
  grBurstPctChangeCrex->SetMarkerColor(kMagenta);
  grBurstPctChangeCrex->SetTitle("PREX Percent Diff");

  TGraph * grBurstPctDifferencePrex = new TGraph(runnumberscrex.size(),&runnumberscrex[0],&asympctchgcrex[0]);  
  grBurstPctDifferencePrex->SetMarkerStyle(7);
  grBurstPctDifferencePrex->SetMarkerColor(kBlue);
  grBurstPctDifferencePrex->SetTitle("CREX Percent Change");

  TGraph * grBurstPctDifferenceCrex = new TGraph(runnumberscrex.size(),&runnumberscrex[0],&asympctdifcrex[0]);  
  grBurstPctDifferenceCrex->SetMarkerStyle(7);
  grBurstPctDifferenceCrex->SetMarkerColor(kCyan);
  grBurstPctDifferenceCrex->SetTitle("PREX Percent Diff");

  TMultiGraph * mgraph2 = new TMultiGraph();
  mgraph2->SetTitle("Asymmetry Pattern #rightarrow Burst | Percent Change and Percent Difference;Run Number;Percent [%]");

  auto leg2 = new TLegend(0.88,0.12,0.99,0.88);
  mgraph2->Add(grBurstPctChangePrex);
  mgraph2->Add(grBurstPctChangeCrex);
  mgraph2->Add(grBurstPctDifferencePrex);
  mgraph2->Add(grBurstPctDifferenceCrex);
  leg2->AddEntry(grBurstPctChangePrex);
  leg2->AddEntry(grBurstPctChangeCrex);
  leg2->AddEntry(grBurstPctDifferencePrex);
  leg2->AddEntry(grBurstPctDifferenceCrex);
   
  TCanvas * CC1 = new TCanvas("CC1","CC1",800,800);
  hburstpctchgprex->Draw("HIST");
  CC1->SaveAs("hburstpctchgprex.pdf");
   
  TCanvas * CC2 = new TCanvas("CC2","CC2",800,800);
  hburstpctdifprex->Draw("HIST");
  CC2->SaveAs("hburstpctdifprex.pdf");
   
  TCanvas * CC3 = new TCanvas("CC3","CC3",800,800);
  hburstpctchgcrex->Draw("HIST");
  CC3->SaveAs("hburstpctchgcrex.pdf");
   
  TCanvas * CC4 = new TCanvas("CC4","CC4",800,800);
  hburstpctdifcrex->Draw("HIST");
  CC4->SaveAs("hburstpctdifcrex.pdf");

  TCanvas * CC5 = new TCanvas("CC5","CC5",1600,800);
  mgraph2->Draw("AP");
  CC5->SaveAs("hburstpattdifferencebyrungraph.pdf");
}
