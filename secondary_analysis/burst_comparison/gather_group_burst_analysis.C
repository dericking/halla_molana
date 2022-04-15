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
CREATE TABLE pcrex_group_burst_analysis (
  id_groupburst INT NOT NULL,
  groupburst_0_mean     
  groupburst_1_mean     
  groupburst_1_error    
  groupburst_2_mean     
  groupburst_2_error    
  groupburst_5_mean     
  groupburst_5_error    
  groupburst_10_mean    
  groupburst_10_error   
  groupburst_25_mean    
  groupburst_25_error   
  groupburst_50_mean    
  groupburst_50_error   
  groupburst_100_mean   
  groupburst_100_error  
  groupburst_250_mean   
  groupburst_250_error  
  PRIMARY KEY (id_groupburst),
  FOREIGN KEY (id_groupburst) REFERENCES moller_run(id_run)
);
*/

Int_t gather_group_burst_analysis(){

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

  TString select_query = "SELECT DISTINCT pcrex_group_burst_analysis.* ";
  TString from_query   = "FROM pcrex_group_burst_analysis, pcrex_groups ";
  TString where_query  = "WHERE id_groupburst = id_group AND group_type LIKE 'beam_pol%' OR group_type = 'spin_dance' OR group_type = 'false_asym' ";
  TString order_query  = "ORDER BY id_groupburst ASC;";

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

  for(Int_t i = 0; i < numrows; i++){
    cout << "Starting row #" << i << endl;
    Int_t    id_groupburst;
    Int_t    id_group;
    Double_t groupburst_0_mean[1];   //0 
    Double_t groupburst_1_mean[1];   //1  
    Double_t groupburst_1_error[1];    
    Double_t groupburst_2_mean[1];   //2  
    Double_t groupburst_2_error[1];    
    Double_t groupburst_5_mean[1];   //3  
    Double_t groupburst_5_error[1];    
    Double_t groupburst_10_mean[1];  //4  
    Double_t groupburst_10_error[1];   
    Double_t groupburst_25_mean[1];  //5  
    Double_t groupburst_25_error[1];   
    Double_t groupburst_50_mean[1];  //6  
    Double_t groupburst_50_error[1];   
    Double_t groupburst_100_mean[1]; //7  
    Double_t groupburst_100_error[1];  
    Double_t groupburst_250_mean[1]; //8  
    Double_t groupburst_250_error[1];
    Double_t all_asymmetry_mean[9];
    Double_t all_asymmetry_error[9];
    Double_t all_asymmetry_pos[9] = {0.,1.,2.,3.,4.,5.,6.,7.,8.,};
    Double_t groupburst_0_pos[1]   = {0.};
    Double_t groupburst_1_pos[1]   = {1.};
    Double_t groupburst_2_pos[1]   = {2.};
    Double_t groupburst_5_pos[1]   = {3.};
    Double_t groupburst_10_pos[1]  = {4.};
    Double_t groupburst_25_pos[1]  = {5.};
    Double_t groupburst_50_pos[1]  = {6.};
    Double_t groupburst_100_pos[1] = {7.};
    Double_t groupburst_250_pos[1] = {8.};

    for(Int_t j = 0; j < numcols; j++){
      TString val    = row->GetField(j);
      TString field  = result->GetFieldName(j);
      Double_t value = val.Atof();
      if( field=="id_groupburst"      ){ id_groupburst = value;cout << "Found new run... " << id_groupburst << endl;}
      if( field=="groupburst_0_mean"  ){ groupburst_0_mean[0]=value; all_asymmetry_mean[0]=value; all_asymmetry_error[0]=0.0;}

      if( field=="groupburst_1_mean"  ){ groupburst_1_mean[0]=value;   all_asymmetry_mean[1]=value; }
      if( field=="groupburst_2_mean"  ){ groupburst_2_mean[0]=value;   all_asymmetry_mean[2]=value; }
      if( field=="groupburst_5_mean"  ){ groupburst_5_mean[0]=value;   all_asymmetry_mean[3]=value; }
      if( field=="groupburst_10_mean" ){ groupburst_10_mean[0]=value;  all_asymmetry_mean[4]=value; }
      if( field=="groupburst_25_mean" ){ groupburst_25_mean[0]=value;  all_asymmetry_mean[5]=value; }
      if( field=="groupburst_50_mean" ){ groupburst_50_mean[0]=value;  all_asymmetry_mean[6]=value; }
      if( field=="groupburst_100_mean"){ groupburst_100_mean[0]=value; all_asymmetry_mean[7]=value; }
      if( field=="groupburst_250_mean"){ groupburst_250_mean[0]=value; all_asymmetry_mean[8]=value; }

      if( field=="groupburst_1_error"  ){ groupburst_1_error[0]=value;   all_asymmetry_error[1]=value; }
      if( field=="groupburst_2_error"  ){ groupburst_2_error[0]=value;   all_asymmetry_error[2]=value; }
      if( field=="groupburst_5_error"  ){ groupburst_5_error[0]=value;   all_asymmetry_error[3]=value; }
      if( field=="groupburst_10_error" ){ groupburst_10_error[0]=value;  all_asymmetry_error[4]=value; }
      if( field=="groupburst_25_error" ){ groupburst_25_error[0]=value;  all_asymmetry_error[5]=value; }
      if( field=="groupburst_50_error" ){ groupburst_50_error[0]=value;  all_asymmetry_error[6]=value; }
      if( field=="groupburst_100_error"){ groupburst_100_error[0]=value; all_asymmetry_error[7]=value; }
      if( field=="groupburst_250_error"){ groupburst_250_error[0]=value; all_asymmetry_error[8]=value; }
    }

    //DO THE GRAPH STUFF HERE AND SAVE WILL DELETE ON LOOPING
    TMultiGraph * mgraph = new TMultiGraph();
    mgraph->SetTitle( Form("Asymmetry v. Burst Size :: GROUP #%i",id_groupburst) );

    auto leg = new TLegend(0.88,0.12,0.99,0.88);

    TGraph *gr0 = new TGraph( 1,groupburst_0_pos,groupburst_0_mean );
    gr0->SetTitle("Mean Over All Patterns");
    gr0->SetMarkerStyle(29);
    gr0->SetMarkerSize(3);
    gr0->SetMarkerColor(kOrange);
    mgraph->Add(gr0);
    leg->AddEntry(gr0);

    TGraphErrors *gr1 = new TGraphErrors( 1,groupburst_1_pos,groupburst_1_mean ,0, groupburst_1_error );
    gr1->SetTitle("By Single Pattern");
    gr1->SetMarkerStyle(20);
    gr1->SetMarkerSize(3);
    gr1->SetMarkerColor(kRed);
    mgraph->Add(gr1);
    leg->AddEntry(gr1);

    TGraphErrors *gr2 = new TGraphErrors( 1,groupburst_2_pos,groupburst_2_mean ,0, groupburst_2_error );
    gr2->SetTitle("2 Pattern Burst");
    gr2->SetMarkerStyle(21);
    gr2->SetMarkerSize(3);
    gr2->SetMarkerColor(kBlue+2);
    mgraph->Add(gr2);
    leg->AddEntry(gr2);

    TGraphErrors *gr3 = new TGraphErrors( 1,groupburst_5_pos,groupburst_5_mean ,0, groupburst_5_error );
    gr3->SetTitle("5 Pattern Burst");
    gr3->SetMarkerStyle(22);
    gr3->SetMarkerSize(3);
    gr3->SetMarkerColor(kCyan+1);
    mgraph->Add(gr3);
    leg->AddEntry(gr3);

    TGraphErrors *gr4 = new TGraphErrors( 1,groupburst_10_pos,groupburst_10_mean ,0, groupburst_10_error );
    gr4->SetTitle("10 Pattern Burst");
    gr4->SetMarkerStyle(23);
    gr4->SetMarkerSize(3);
    gr4->SetMarkerColor(kGreen);
    mgraph->Add(gr4);
    leg->AddEntry(gr4);

    TGraphErrors *gr5 = new TGraphErrors( 1,groupburst_25_pos,groupburst_25_mean ,0, groupburst_25_error );
    gr5->SetTitle("25 Pattern Burst");
    gr5->SetMarkerStyle(33);
    gr5->SetMarkerSize(3);
    gr5->SetMarkerColor(kMagenta);
    mgraph->Add(gr5);
    leg->AddEntry(gr5);

    TGraphErrors *gr6 = new TGraphErrors( 1,groupburst_50_pos,groupburst_50_mean ,0, groupburst_50_error );
    gr6->SetTitle("50 Pattern Burst");
    gr6->SetMarkerStyle(34);
    gr6->SetMarkerSize(3);
    gr6->SetMarkerColor(kTeal);
    mgraph->Add(gr6);
    leg->AddEntry(gr6);

    TGraphErrors *gr7 = new TGraphErrors( 1,groupburst_100_pos,groupburst_100_mean,0,groupburst_100_error );
    gr7->SetTitle("100 Pattern Burst");
    gr7->SetMarkerStyle(47);
    gr7->SetMarkerSize(3);
    gr7->SetMarkerColor(kSpring+9);
    mgraph->Add(gr7);
    leg->AddEntry(gr7);

    TGraphErrors *gr8 = new TGraphErrors( 1,groupburst_250_pos,groupburst_250_mean,0,groupburst_250_error );
    gr8->SetTitle("250 Pattern Burst");
    gr8->SetMarkerStyle(39);
    gr8->SetMarkerSize(3);
    gr8->SetMarkerColor(kViolet+1);
    mgraph->Add(gr8);
    leg->AddEntry(gr8);

    TCanvas * C = new TCanvas("C","C",1500,500);
    mgraph->Draw("AP");
    leg->Draw();

    C->SaveAs( Form("Burst_Comparison_Group_%i.png",id_groupburst) );

    row = result->Next();
  }

}
