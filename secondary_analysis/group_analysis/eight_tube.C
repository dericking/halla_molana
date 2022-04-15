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
#include<algorithm>

Int_t eight_tube(){

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

  TString sel1_query   = "SELECT id_group, group_date, group_anpow, ";
  TString sel2_query   = "CASE WHEN group_hwp = 'IN' THEN 1 WHEN group_hwp = 'OUT' THEN -1 END AS group_hwp, ";
  //TString sel3_query   = "group_wein, group_asym, group_asym_err, group_systematic, group_target ";
  TString sel3_query   = "group_wein, group_block_asym AS group_asym, group_block_asym_err, group_systematic, group_target ";
  TString from_query   = "FROM pcrex_groups ";
  TString where_query  = "WHERE group_type LIKE 'beam_pol%' AND (id_group = 1089 || id_group = 1090 || id_group = 1010 OR id_group = 1019 OR id_group = 1077 OR id_group = 1078 ) ";
  TString order_query  = "ORDER BY id_group ASC;";

  TString total_query  = sel1_query + sel2_query + sel3_query + from_query + where_query + order_query;

  cout << "query: " << total_query << endl;

  TSQLResult * result = server->Query(total_query);
  TSQLRow    * row;

  if(result == NULL){
    delete row;
    delete result;
    server->Close();
    delete server;
    exit(-5);
  }

  Double_t four2tenfactor = 1.01008;

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

  Double_t a1010(0.), a1019(0.), a1089(0.), a1090(0.), e1010(0.), e1019(0.), e1089(0.), e1090(0.), a1077(0.), e1077(0.), a1078(0.), e1078(0.); 

  Double_t anpow = 0.77103;
  Double_t ptarg = 0.08005;

  for(Int_t i = 0; i < numrows; i++){
    //Save to vector
    std::vector< Double_t > temp;
    Int_t ihwp = 0;
    Int_t wein = 0;
    Int_t idgr = 0;
    Int_t targ = 0;
    for(Int_t j = 0; j < numcols; j++){
      //cout << "col#" << j << endl;
      TString value = row->GetField(j);
      TString field = result->GetFieldName(j);
      if( field=="group_hwp"){ ihwp = value.Atoi(); }
      if( field=="group_wein"){ wein = value.Atoi(); }
      if( field=="id_group"){ idgr = value.Atoi(); }
      if( field=="group_date"){ value = value.ReplaceAll("-",""); }
      if( field=="group_systematic" && value == "" ){ value = "0.0"; }
      if( field=="group_target"){  targ = value.Atoi(); }
      //cout << value << endl;
      temp.push_back( value.Atof() );
    }

  
    if(idgr == 1010){ a1010 = fabs(temp[5]) / anpow / ptarg * 100; e1010 = temp[6] / anpow / ptarg * 100; }
    if(idgr == 1019){ a1019 = fabs(temp[5]) / anpow / ptarg * 100; e1019 = temp[6] / anpow / ptarg * 100; }
    if(idgr == 1089){ a1089 = fabs(temp[5]) / anpow / ptarg * 100; e1089 = temp[6] / anpow / ptarg * 100; }
    if(idgr == 1090){ a1090 = fabs(temp[5]) / anpow / ptarg * 100; e1090 = temp[6] / anpow / ptarg * 100; }
    if(idgr == 1077){ a1077 = fabs(temp[5]) / anpow / ptarg * 100; e1077 = temp[6] / anpow / ptarg * 100; }
    if(idgr == 1078){ a1078 = fabs(temp[5]) / anpow / ptarg * 100; e1078 = temp[6] / anpow / ptarg * 100; }

    row = result->Next();
  }

  std::vector <Double_t> xjuly = {1.,2.};
  std::vector <Double_t> xsept = {11.,12.};
  std::vector <Double_t> pjuly = {a1010,a1019};
  std::vector <Double_t> ejuly = {e1010,e1019};
  std::vector <Double_t> psept = {a1089,a1090};
  std::vector <Double_t> esept = {e1089,e1090};



  cout << std::setprecision(8) << endl;
  for(Int_t i = 0; i < 2; i++) cout << "  pjuly[" << i << "]: " << pjuly[i] << " +/- " << ejuly[i] << endl;
  for(Int_t i = 0; i < 2; i++) cout << "  psept[" << i << "]: " << psept[i] << " +/- " << esept[i] << endl;

  std::vector <Double_t> pjuly10 = {a1010*four2tenfactor,a1019*four2tenfactor};
  std::vector <Double_t> ejuly10 = {sqrt(e1010*e1010 + (.0015*.0015)),sqrt(e1019*e1019 + (.0015*.0015))};
  std::vector <Double_t> psept10 = {a1089*four2tenfactor,a1090*four2tenfactor};
  std::vector <Double_t> esept10 = {sqrt(e1089*e1089 + (.0015*.0015)),sqrt(e1090*e1090 + (.0015*.0015))};


  
  cout << std::setprecision(8) << endl;
  for(Int_t i = 0; i < 2; i++) cout << "  pjuly10[" << i << "]: " << pjuly10[i] << " +/- " << ejuly10[i] << endl;
  for(Int_t i = 0; i < 2; i++) cout << "  psept10[" << i << "]: " << psept10[i] << " +/- " << esept10[i] << endl;

  //ROUGH NUMBER NOT PROPERLY WEIGHTED
  Double_t eight2tworatio = (a1077+a1078) / (a1089+a1090);
  Double_t eight2twoerror = sqrt( e1077*e1077/(a1077*a1077) + e1089*e1089/(a1089*a1089) + e1090*e1090/(a1090*a1090) + e1078*e1078/(a1078*a1078) );

  cout << "8:2 Diff = " << eight2tworatio << "  +/-  " << eight2twoerror << endl;


  std::vector <Double_t> pjuly10u = {a1010*four2tenfactor*eight2tworatio,a1019*four2tenfactor*eight2tworatio};
  std::vector <Double_t> ejuly10u = {sqrt(e1010*e1010*(1 + 2*(1.0015*1.0015))),sqrt(e1019*e1019 + 2*(.0015*.0015))};
  std::vector <Double_t> psept10u = {a1089*four2tenfactor*eight2tworatio,a1090*four2tenfactor*eight2tworatio};
  std::vector <Double_t> esept10u = {sqrt(e1089*e1089 + 2*(.0015*.0015)),sqrt(e1090*e1090 + 2*(.0015*.0015))};

  cout << std::setprecision(8) << endl;
  for(Int_t i = 0; i < 2; i++) cout << "  pjuly10u[" << i << "]: " << pjuly10u[i] << " +/- " << ejuly10u[i] << endl;
  for(Int_t i = 0; i < 2; i++) cout << "  psept10u[" << i << "]: " << psept10u[i] << " +/- " << esept10u[i] << endl;

  TMultiGraph * mgr1 = new TMultiGraph();
  TGraphErrors *firsta = new TGraphErrors( (Int_t)(pjuly.size()) , &(xjuly[0]) , &(pjuly[0]) , 0 , &(ejuly[0]) );
    firsta->SetTitle("july");
    firsta->SetMarkerStyle(20);
    firsta->SetMarkerSize(2);
    firsta->SetMarkerColor(kRed);
  mgr1->Add(firsta);
  TGraphErrors *firstb = new TGraphErrors( (Int_t)(xsept.size()) , &(xsept[0]) , &(psept[0]) , 0 , &(esept[0]) );
    firstb->SetTitle("sept");
    firstb->SetMarkerStyle(20);
    firstb->SetMarkerSize(2);
    firstb->SetMarkerColor(kBlue);
  mgr1->Add(firstb);


  std::vector <Double_t> xjuly10 = {3.,4.};
  std::vector <Double_t> xsept10 = {13.,14.};
  TGraphErrors *seconda = new TGraphErrors( (Int_t)(pjuly10.size()) , &(xjuly10[0]) , &(pjuly10[0]) , 0 , &(ejuly10[0]) );
    seconda->SetTitle("10um adjusted");
    seconda->SetMarkerStyle(21);
    seconda->SetMarkerSize(2);
    seconda->SetMarkerColor(kRed+2);
  mgr1->Add(seconda);
  TGraphErrors *secondb = new TGraphErrors( (Int_t)(xsept10.size()) , &(xsept10[0]) , &(psept10[0]) , 0 , &(esept10[0]) );
    secondb->SetTitle("10um adjusted");
    secondb->SetMarkerStyle(21);
    secondb->SetMarkerSize(2);
    secondb->SetMarkerColor(kBlue+2);
  mgr1->Add(secondb);

  std::vector <Double_t> xjuly10u = {5.,6.};
  std::vector <Double_t> xsept10u = {15.,16.};
  TGraphErrors *thirda = new TGraphErrors( (Int_t)(pjuly10u.size()) , &(xjuly10u[0]) , &(pjuly10u[0]) , 0 , &(ejuly10u[0]) );
    thirda->SetTitle("10um adjusted");
    thirda->SetMarkerStyle(21);
    thirda->SetMarkerSize(2);
    thirda->SetMarkerColor(kRed+2);
  mgr1->Add(thirda);
  TGraphErrors *thirdb = new TGraphErrors( (Int_t)(xsept10u.size()) , &(xsept10u[0]) , &(psept10u[0]) , 0 , &(esept10u[0]) );
    thirdb->SetTitle("10um adjusted");
    thirdb->SetMarkerStyle(21);
    thirdb->SetMarkerSize(2);
    thirdb->SetMarkerColor(kBlue+2);
  mgr1->Add(thirdb);



  TCanvas * c1 = new TCanvas("c1","c1",1500,700);
  mgr1->Draw("AP");

  return 100;

}
