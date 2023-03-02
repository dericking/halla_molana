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

#define PTARG 0.08005

void  doTGraphGRP( std::vector<Double_t>& V, std::vector<vector<Double_t>>& v ){
  for(Int_t i = 0; i < v.size(); i++){ V.push_back( v[i][0] ); }
}
//NOTE Items 3/4 of arrays are sign factors for HWP and Wein settings
void  doTGraphPOL( std::vector<Double_t>& V, std::vector<vector<Double_t>>& v ){
  for(Int_t i = 0; i < v.size(); i++){ V.push_back( 100 * v[i][5] / v[i][2] / PTARG * v[i][3] * v[i][4] ); }
}
void  doTGraphERR( std::vector<Double_t>& V, std::vector<vector<Double_t>>& v ){
  for(Int_t i = 0; i < v.size(); i++){ V.push_back( 100 * v[i][6] / v[i][2] / PTARG ); }
}
//Will take the polarization vector and error vectors to correct 4um runs
void  do4umPolCorr( std::vector<Double_t>& Vg , std::vector<Double_t>& Vp , std::vector<Double_t>& Ve, Double_t CF , Double_t CFe){
  //cout << "4um Conversion Factor: " << CF << " +/- " << CFe << endl;
  std::vector< Int_t > i4umgroups = { 1023 , 1024 , 1027 , 1030 , 1031 , 1038 , 1039 , 1042 , 1043 , 1044 , 1045 , 1046 };
  for(Int_t i = 0; i < Vg.size(); i++){
    if( std::find( i4umgroups.begin(), i4umgroups.end(), Vg[i] ) != i4umgroups.end() ) {
      cout << "Adjusting " << (Int_t)Vg[i] << endl;
      cout << "  [Before] Pol: " << Vp[i] << " +/- " << Ve[i] << endl;     
      Vp[i] *= CF;
      Ve[i] *= CF;
      cout << "   [After] Pol: " << Vp[i] << " +/- " << Ve[i] << endl;
    }
  }
}

Int_t gatherGroupData(){

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
  TString where_query  = "WHERE group_type='beam_pol' AND id_group >= 1020 ";
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
  //PREX STANDARD
  std::vector< vector < Double_t > > pli;
  std::vector< vector < Double_t > > pri;
  std::vector< vector < Double_t > > plo;
  std::vector< vector < Double_t > > pro;
  //PREX 4um(3) & 10um(2) IN AND OUT AFTER G1035
  std::vector< vector < Double_t > > p3li;
  std::vector< vector < Double_t > > p3lo;
  std::vector< vector < Double_t > > p3ri;
  std::vector< vector < Double_t > > p3ro;
  std::vector< vector < Double_t > > p2li;
  std::vector< vector < Double_t > > p2lo;
  std::vector< vector < Double_t > > p2ri;
  std::vector< vector < Double_t > > p2ro;
  //PREX 4um(3) & 10um(2) Graphs for Fits
  std::vector< vector < Double_t > > p3i;
  std::vector< vector < Double_t > > p3o;
  std::vector< vector < Double_t > > p2i;
  std::vector< vector < Double_t > > p2o;
  //PREX 4um IN/OUT and 10um IN/OUT means
  std::vector< vector < Double_t > > p2m;
  std::vector< vector < Double_t > > p3m;
  //PREX After 8/18 Prelim Polarizations 4um to be adjusted
  std::vector< vector < Double_t > > ppi1;
  std::vector< vector < Double_t > > ppo1;

  //CREX STANDARD
  std::vector< vector < Double_t > > cli;
  std::vector< vector < Double_t > > cri;
  std::vector< vector < Double_t > > clo;
  std::vector< vector < Double_t > > cro;

  //Pointers to be used in sorting
  std::vector< vector < Double_t > > *towhom1 = nullptr;
  std::vector< vector < Double_t > > *towhom2 = nullptr;
  std::vector< vector < Double_t > > *towhom3 = nullptr;
  std::vector< vector < Double_t > > *towhom4 = nullptr;
  std::vector< vector < Double_t > > *towhom5 = nullptr;
  std::vector< vector < Double_t > > *towhom0 = nullptr;


  //ADDITIONAL COORECTION FACTOR BASED ON DAILY PAIR RATIO - COLLECT ASYMMETRIES AND ERRORS
  Double_t a1050(0.),a1051(0.),a1053(0.),a1054(0.),a1062(0.),a1063(0.),a1065(0.),a1066(0.),a1070(0.),a1073(0.),a1074(0.),
           a1098(0.),a1071(0.),a1080(0.),a1077(0.),a1079(0.),a1078(0.),a1093(0.),a1097(0.);
  Double_t e1050(0.),e1051(0.),e1053(0.),e1054(0.),e1062(0.),e1063(0.),e1065(0.),e1066(0.),e1070(0.),e1073(0.),e1074(0.),
           e1098(0.),e1071(0.),e1080(0.),e1077(0.),e1079(0.),e1078(0.),e1093(0.),e1097(0.);

  //CURERNTLY: group, date, anpow, hwp, wein, asym, asym_err, systematic, target
  for(Int_t i = 0; i < numrows; i++){
    //Save to vector
    std::vector< Double_t > temp;
    Int_t ihwp = 0;
    Int_t wein = 0;
    Int_t idgr = 0;
    Int_t targ = 0;
    for(Int_t j = 0; j < numcols; j++){
      TString value = row->GetField(j);
      TString field = result->GetFieldName(j);
      if( field=="group_hwp"){ ihwp = value.Atoi(); }
      if( field=="group_wein"){ wein = value.Atoi(); }
      if( field=="id_group"){ idgr = value.Atoi(); }
      if( field=="group_date"){ value = value.ReplaceAll("-",""); }
      if( field=="group_systematic" && value == "" ){ value = "0.0"; }
      if( field=="group_target"){  targ = value.Atoi(); }
      temp.push_back( value.Atof() );
    }


    if(      ihwp == -1 && wein == -1 && idgr <  3000 ) {towhom1 = &plo;}
    else if( ihwp == -1 && wein ==  1 && idgr <  3000 ) {towhom1 = &pro;}
    else if( ihwp ==  1 && wein == -1 && idgr <  3000 ) {towhom1 = &pli;}
    else if( ihwp ==  1 && wein ==  1 && idgr <  3000 ) {towhom1 = &pri;}
    else if( ihwp == -1 && wein == -1 && idgr >= 3000 ) {towhom1 = &clo;}
    else if( ihwp == -1 && wein ==  1 && idgr >= 3000 ) {towhom1 = &cro;}
    else if( ihwp ==  1 && wein == -1 && idgr >= 3000 ) {towhom1 = &cli;}
    else if( ihwp ==  1 && wein ==  1 && idgr >= 3000 ) {towhom1 = &cri;}
    else {cout << "exiting code 2, idgr: " << idgr << endl;exit(2);}
    if(towhom1) towhom1->push_back(temp);

    if( idgr > 1022 && idgr < 2000){
        if(      ihwp == -1 && wein == -1 && targ == 3 ) {towhom2 = &p3lo;}
        else if( ihwp == -1 && wein ==  1 && targ == 3 ) {towhom2 = &p3ro;}
        else if( ihwp ==  1 && wein == -1 && targ == 3 ) {towhom2 = &p3li;}
        else if( ihwp ==  1 && wein ==  1 && targ == 3 ) {towhom2 = &p3ri;}
        else if( ihwp == -1 && wein == -1 && targ == 2 ) {towhom2 = &p2lo;}
        else if( ihwp == -1 && wein ==  1 && targ == 2 ) {towhom2 = &p2ro;}
        else if( ihwp ==  1 && wein == -1 && targ == 2 ) {towhom2 = &p2li;}
        else if( ihwp ==  1 && wein ==  1 && targ == 2 ) {towhom2 = &p2ri;}
        else {cout << "exiting code 3, idgr: " << idgr << ", ihwp: " << ihwp << ", wein: " << wein << ", targ: " << targ << endl;exit(3);}
    }
    if(towhom2) towhom2->push_back(temp);

    if( idgr > 1022 && idgr < 2000){//SEEME: 81-90 systematics; Marked properly in DB now... leave as is though.
      if(idgr < 1081 || idgr > 1090){
        if(      ihwp == -1 && targ == 3 ) {towhom3 = &p3o; }
        else if( ihwp ==  1 && targ == 3 ) {towhom3 = &p3i; }
        else if( ihwp == -1 && targ == 2 ) {towhom3 = &p2o; }
        else if( ihwp ==  1 && targ == 2 ) {towhom3 = &p2i; }
        else {cout << "exiting code 4, idgr: " << idgr << endl;exit(4);}
      }
    }
    if(towhom3) towhom3->push_back(temp);


    if( idgr > 1048 && idgr < 2000 ){//SEEME: 81-90 systematics; Marked properly in DB now... leave as is though.
      if(idgr < 1081 || idgr > 1090){
        if(      ihwp == -1 && targ == 3 ) {towhom4 = &p2m;}
        else if( ihwp ==  1 && targ == 3 ) {towhom4 = &p2m;}
        else if( ihwp == -1 && targ == 2 ) {towhom4 = &p3m;}
        else if( ihwp ==  1 && targ == 2 ) {towhom4 = &p3m;}
        else {cout << "exiting code 5, idgr: " << idgr << endl;exit(5);}
      }
    }
    if(towhom4) towhom4->push_back(temp);


    //Initialize PREX Pol Measurements Vector
    std::vector< Int_t > polgroups = { 1023 , 1024 , 1027 , 1030 , 1031 , 
                                       1038 , 1039 , 1042 , 1043 , 1044 , 
                                       1045 , 1046 , 1053 , 1054 , 1062 , 
                                       1063 , 1073 , 1074 , 1079 , 1080 , 
                                       1093 , 1094 };
    if( std::find( polgroups.begin(), polgroups.end(), idgr ) != polgroups.end() ) {
      cout << "Found PREX Pol Group " << idgr << endl;
      if(      ihwp == -1) {towhom5 = &ppo1;}
      else if( ihwp ==  1) {towhom5 = &ppi1;}
      else {cout << "exiting code 6, idgr: " << idgr << " ihwp state not matched???" << endl;exit(6);} 
    }
    if(towhom5) towhom5->push_back(temp);


    //SINCE ALL OF THE OTHER AZZ/POL PLOTS ARE FINE WHEN ADJUSTED BY HWP STATE SIGN AND WEIN SIGN I'LL JUST
    //TAKE THE ABSOLUTE VALUES OF THE ASYMS AND NOT WORRY ABOUT SIGN ADJUSTMENT.
    if     (idgr==1050){ a1050=fabs(temp[5]) ; e1050=fabs(temp[6]) ;}    
    else if(idgr==1051){ a1051=fabs(temp[5]) ; e1051=fabs(temp[6]) ;}  
    else if(idgr==1053){ a1053=fabs(temp[5]) ; e1053=fabs(temp[6]) ;}  
    else if(idgr==1054){ a1054=fabs(temp[5]) ; e1054=fabs(temp[6]) ;}  
    else if(idgr==1062){ a1062=fabs(temp[5]) ; e1062=fabs(temp[6]) ;}  
    else if(idgr==1063){ a1063=fabs(temp[5]) ; e1063=fabs(temp[6]) ;}  
    else if(idgr==1065){ a1065=fabs(temp[5]) ; e1065=fabs(temp[6]) ;}  
    else if(idgr==1066){ a1066=fabs(temp[5]) ; e1066=fabs(temp[6]) ;}  
    else if(idgr==1070){ a1070=fabs(temp[5]) ; e1070=fabs(temp[6]) ;}  
    else if(idgr==1073){ a1073=fabs(temp[5]) ; e1073=fabs(temp[6]) ;}  
    else if(idgr==1074){ a1074=fabs(temp[5]) ; e1074=fabs(temp[6]) ;}  
    else if(idgr==1098){ a1098=fabs(temp[5]) ; e1098=fabs(temp[6]) ;}  
    else if(idgr==1071){ a1071=fabs(temp[5]) ; e1071=fabs(temp[6]) ;}  
    else if(idgr==1080){ a1080=fabs(temp[5]) ; e1080=fabs(temp[6]) ;}  
    else if(idgr==1077){ a1077=fabs(temp[5]) ; e1077=fabs(temp[6]) ;}  
    else if(idgr==1079){ a1079=fabs(temp[5]) ; e1079=fabs(temp[6]) ;}  
    else if(idgr==1078){ a1078=fabs(temp[5]) ; e1078=fabs(temp[6]) ;}  
    else if(idgr==1093){ a1093=fabs(temp[5]) ; e1093=fabs(temp[6]) ;}  
    else if(idgr==1097){ a1097=fabs(temp[5]) ; e1097=fabs(temp[6]) ;}
   

    row = result->Next();
    towhom1 = NULL;
    towhom2 = NULL;
    towhom3 = NULL;
    towhom4 = NULL;
    towhom5 = NULL;
  }

  // CALCULATE RATIOS FOR PAIRS, ERROR WEIGHT FOR SUMS, USE FRACTIONAL ERRORS TO PROPAGATE AND CONVERT BACK TO RELATIVE
  // ERROR-WEIGHTED SUM --> MEAN = SUM( mean_i / error_i**2 ) / SUM( 1 / error_i**2 )
  // ERROR OF WEIGHTED MEAN --> ERR = SQRT( 1 / SUM( 1 / error_i**2 )

  // 8/26 pairs
  Double_t r1i  = a1053 / a1051;
  Double_t e1i  = sqrt( pow(e1053/a1053,2) + pow(e1051/a1051,2) ) * r1i; //Add fractional in quadrature convert back to absolute
  Double_t r1o  = a1054 / a1050;
  Double_t e1o  = sqrt( pow(e1054/a1054,2) + pow(e1050/a1050,2) ) * r1o; //Add fractional in quadrature convert back to absolute
  /// 8/31 pairs
  Double_t r2id = ( a1065 / pow(e1065,2) + a1070 / pow(e1070,2) ) / ( pow(e1065,-2) + pow(e1070,-2) );
  Double_t r2ide= sqrt( 1 / ( pow(e1065,-2) + pow(e1070,-2) ) ); //standard error of the weighted mean
  Double_t r2i  = a1063 / r2id;
  Double_t e2i  = sqrt( pow(e1063/a1063,2) + pow(r2ide/r2id,2) ) * r2i; //Add fractional in quadrature convert back to absolute
  //
  Double_t r2o  = a1062 / a1066;
  Double_t e2o  = sqrt( pow(e1062/a1062,2) + pow(e1066/a1066,2) ) * r2o; //Add fractional in quadrature convert back to absolute
  //
  Double_t r2on = ( a1073 / pow(e1073,2) + a1074 / pow(e1074,2) ) / ( pow(e1073,-2) + pow(e1074,-2) );
  Double_t r2one= sqrt( 1 / ( pow(e1073,-2) + pow(e1074,-2) ) ); //standard error of the weighted mean
  Double_t r2od = ( a1071 / pow(e1071,2) + a1098 / pow(e1098,2) ) / ( pow(e1071,-2) + pow(e1098,-2) );
  Double_t r2ode= sqrt( 1 / ( pow(e1071,-2) + pow(e1098,-2) ) ); //standard error of the weighted mean
  Double_t r2o2 = r2on / r2od;
  Double_t e2o2 = sqrt( pow(r2one/r2on,2) + pow(r2ode/r2od,2) ) * r2o2; //Add fractional in quadrature convert back to absolute
  // 9/4 pairs
  Double_t r3i  = a1079 / a1078;
  Double_t e3i  = sqrt( pow(e1079/a1079,2) + pow(e1078/a1078,2) )*r3i; //Add fractional in quadrature convert back to absolute
  Double_t r3o  = a1080 / a1077;
  Double_t e3o  = sqrt( pow(e1080/a1080,2) + pow(e1077/a1077,2) )*r3o; //Add fractional in quadrature convert back to absolute
  // 9/8 pairs
  Double_t r4o  = a1093 / a1097;
  Double_t e4o  = sqrt( pow(e1093/a1093,2) + pow(e1097/a1097,2) )*r4o; //Add fractional in quadrature convert back to absolute

  std::vector< Double_t > pair_numb = {2,3,6,7,8,11,12,15};
  std::vector< Double_t > pairratio = {r1o,r1i,r2o,r2o2,r2i,r3o,r3i,r4o};
  std::vector< Double_t > pairerror = {e1o,e1i,e2o,e2o2,e2i,e3o,e3i,e4o};

  for(Int_t i = 0; i < pair_numb.size(); i++){
    cout << std::setprecision(9) << "[" << pair_numb[i]
         << "]: " << pairratio[i] << " +/- " << pairerror[i] << endl;
  }

  //PREX WHOLE
  std::vector< Double_t > plogrp; doTGraphGRP(plogrp,plo);
  std::vector< Double_t > plopol; doTGraphPOL(plopol,plo);
  std::vector< Double_t > ploerr; doTGraphERR(ploerr,plo);
  std::vector< Double_t > pligrp; doTGraphGRP(pligrp,pli);
  std::vector< Double_t > plipol; doTGraphPOL(plipol,pli);
  std::vector< Double_t > plierr; doTGraphERR(plierr,pli);
  std::vector< Double_t > progrp; doTGraphGRP(progrp,pro);
  std::vector< Double_t > propol; doTGraphPOL(propol,pro);
  std::vector< Double_t > proerr; doTGraphERR(proerr,pro);
  std::vector< Double_t > prigrp; doTGraphGRP(prigrp,pri);
  std::vector< Double_t > pripol; doTGraphPOL(pripol,pri);
  std::vector< Double_t > prierr; doTGraphERR(prierr,pri);

  //PREXII SEPARATED BY WIEN AND IHWP STATES
  std::vector< Double_t > p2logrp; doTGraphGRP(p2logrp,p2lo);
  std::vector< Double_t > p2lopol; doTGraphPOL(p2lopol,p2lo);
  std::vector< Double_t > p2loerr; doTGraphERR(p2loerr,p2lo);
  std::vector< Double_t > p2ligrp; doTGraphGRP(p2ligrp,p2li);
  std::vector< Double_t > p2lipol; doTGraphPOL(p2lipol,p2li);
  std::vector< Double_t > p2lierr; doTGraphERR(p2lierr,p2li);
  std::vector< Double_t > p2rogrp; doTGraphGRP(p2rogrp,p2ro);
  std::vector< Double_t > p2ropol; doTGraphPOL(p2ropol,p2ro);
  std::vector< Double_t > p2roerr; doTGraphERR(p2roerr,p2ro);
  std::vector< Double_t > p2rigrp; doTGraphGRP(p2rigrp,p2ri);
  std::vector< Double_t > p2ripol; doTGraphPOL(p2ripol,p2ri);
  std::vector< Double_t > p2rierr; doTGraphERR(p2rierr,p2ri);

  std::vector< Double_t > p3logrp; doTGraphGRP(p3logrp,p3lo);
  std::vector< Double_t > p3lopol; doTGraphPOL(p3lopol,p3lo);
  std::vector< Double_t > p3loerr; doTGraphERR(p3loerr,p3lo);
  std::vector< Double_t > p3ligrp; doTGraphGRP(p3ligrp,p3li);
  std::vector< Double_t > p3lipol; doTGraphPOL(p3lipol,p3li);
  std::vector< Double_t > p3lierr; doTGraphERR(p3lierr,p3li);
  std::vector< Double_t > p3rogrp; doTGraphGRP(p3rogrp,p3ro);
  std::vector< Double_t > p3ropol; doTGraphPOL(p3ropol,p3ro);
  std::vector< Double_t > p3roerr; doTGraphERR(p3roerr,p3ro);
  std::vector< Double_t > p3rigrp; doTGraphGRP(p3rigrp,p3ri);
  std::vector< Double_t > p3ripol; doTGraphPOL(p3ripol,p3ri);
  std::vector< Double_t > p3rierr; doTGraphERR(p3rierr,p3ri);

  //COMBINED WIEN with 4um and 10um SEPARATE IHWP STATES
  std::vector< Double_t > p2ogrp; doTGraphGRP(p2ogrp,p2o);
  std::vector< Double_t > p2opol; doTGraphPOL(p2opol,p2o);
  std::vector< Double_t > p2oerr; doTGraphERR(p2oerr,p2o);
  std::vector< Double_t > p2igrp; doTGraphGRP(p2igrp,p2i);
  std::vector< Double_t > p2ipol; doTGraphPOL(p2ipol,p2i);
  std::vector< Double_t > p2ierr; doTGraphERR(p2ierr,p2i);
  std::vector< Double_t > p3ogrp; doTGraphGRP(p3ogrp,p3o);
  std::vector< Double_t > p3opol; doTGraphPOL(p3opol,p3o);
  std::vector< Double_t > p3oerr; doTGraphERR(p3oerr,p3o);
  std::vector< Double_t > p3igrp; doTGraphGRP(p3igrp,p3i);
  std::vector< Double_t > p3ipol; doTGraphPOL(p3ipol,p3i);
  std::vector< Double_t > p3ierr; doTGraphERR(p3ierr,p3i);

  //MEAN 4um HWP IN+OUT and 10um IN+OUT to determine scaling constant/factor.
  std::vector< Double_t > p2mgrp; doTGraphGRP(p2mgrp,p2m);
  std::vector< Double_t > p2mpol; doTGraphPOL(p2mpol,p2m);
  std::vector< Double_t > p2merr; doTGraphERR(p2merr,p2m);
  std::vector< Double_t > p3mgrp; doTGraphGRP(p3mgrp,p3m);
  std::vector< Double_t > p3mpol; doTGraphPOL(p3mpol,p3m);
  std::vector< Double_t > p3merr; doTGraphERR(p3merr,p3m);

  //PREX Semi-final Adjusted Polarizations
  std::vector< Double_t > ppi1grp; doTGraphGRP(ppi1grp,ppi1);
  std::vector< Double_t > ppi1pol; doTGraphPOL(ppi1pol,ppi1);
  std::vector< Double_t > ppi1err; doTGraphERR(ppi1err,ppi1);
  std::vector< Double_t > ppo1grp; doTGraphGRP(ppo1grp,ppo1);
  std::vector< Double_t > ppo1pol; doTGraphPOL(ppo1pol,ppo1);
  std::vector< Double_t > ppo1err; doTGraphERR(ppo1err,ppo1);

  //CREX 
  std::vector< Double_t > clogrp; doTGraphGRP(clogrp,clo);
  std::vector< Double_t > clopol; doTGraphPOL(clopol,clo);
  std::vector< Double_t > cloerr; doTGraphERR(cloerr,clo);
  std::vector< Double_t > cligrp; doTGraphGRP(cligrp,cli);
  std::vector< Double_t > clipol; doTGraphPOL(clipol,cli);
  std::vector< Double_t > clierr; doTGraphERR(clierr,cli);
  std::vector< Double_t > crogrp; doTGraphGRP(crogrp,cro);
  std::vector< Double_t > cropol; doTGraphPOL(cropol,cro);
  std::vector< Double_t > croerr; doTGraphERR(croerr,cro);
  std::vector< Double_t > crigrp; doTGraphGRP(crigrp,cri);
  std::vector< Double_t > cripol; doTGraphPOL(cripol,cri);
  std::vector< Double_t > crierr; doTGraphERR(crierr,cri);


  //MUST BE CALLED BEFORE GRAPH BULLSHIT
  gStyle->SetOptFit(1111);

  TMultiGraph * pmgr   = new TMultiGraph();
  TMultiGraph * cmgr   = new TMultiGraph();
  TMultiGraph * p2mgr  = new TMultiGraph();//prex targ #2
  TMultiGraph * p3mgr  = new TMultiGraph();//prex targ #3
  TMultiGraph * pmgrin = new TMultiGraph();//prex targ 2/3 in
  TMultiGraph * pmgrout= new TMultiGraph();//prex targ 2/3 out
  TMultiGraph * p23iomg= new TMultiGraph();
  TMultiGraph * ppiomgr= new TMultiGraph();

  //113/23: down-triangle ==> HWP-OUT; 109/22: up-triangle ==> HWP-in
  //RED: Wein-Left; Blue: Wein-Right
  Float_t pmarkersize = 2;
  Float_t cmarkersize = 2;
  Int_t downtriangle = 23;
  Int_t uptriangle   = 22;
  TGraphErrors *grplo = new TGraphErrors( (Int_t)(plogrp.size()) , &(plogrp[0]) , &(plopol[0]) , 0 , &(ploerr[0]) );
  grplo->SetTitle("ihwp-out,wien-left");
  grplo->SetMarkerStyle(downtriangle);
  grplo->SetMarkerSize(pmarkersize);
  grplo->SetMarkerColor(kRed);
  pmgr->Add(grplo);
  TGraphErrors *grpli = new TGraphErrors( (Int_t)(pligrp.size()) , &(pligrp[0]) , &(plipol[0]) , 0 , &(ploerr[0]) );
  grpli->SetTitle("ihwp-in, wein-left");
  grpli->SetMarkerStyle(uptriangle);
  grpli->SetMarkerSize(pmarkersize);
  grpli->SetMarkerColor(kRed);
  pmgr->Add(grpli);
  TGraphErrors *grpro = new TGraphErrors( (Int_t)(progrp.size()) , &(progrp[0]) , &(propol[0]) , 0 , &(ploerr[0]) );
  grpro->SetTitle("ihwp-out,wien-right");
  grpro->SetMarkerStyle(downtriangle);
  grpro->SetMarkerSize(pmarkersize);
  grpro->SetMarkerColor(kBlue);
  pmgr->Add(grpro);
  TGraphErrors *grpri = new TGraphErrors( (Int_t)(prigrp.size()) , &(prigrp[0]) , &(pripol[0]) , 0 , &(ploerr[0]) );
  grpri->SetTitle("ihwp-in,wien-right");
  grpri->SetMarkerStyle(uptriangle);
  grpri->SetMarkerSize(pmarkersize);
  grpri->SetMarkerColor(kBlue);
  pmgr->Add(grpri);
  
  TGraphErrors *grclo = new TGraphErrors( (Int_t)(clogrp.size()) , &(clogrp[0]) , &(clopol[0]) , 0 , &(cloerr[0]) );
  grclo->SetTitle("ihwp-out,wien-left");
  grclo->SetMarkerStyle(downtriangle);
  grclo->SetMarkerSize(cmarkersize);
  grclo->SetMarkerColor(kRed);
  cmgr->Add(grclo);
  TGraphErrors *grcli = new TGraphErrors( (Int_t)(cligrp.size()) , &(cligrp[0]) , &(clipol[0]) , 0 , &(clierr[0]) );
  grcli->SetTitle("ihwp-in, wein-left");
  grcli->SetMarkerStyle(uptriangle);
  grcli->SetMarkerSize(cmarkersize);
  grcli->SetMarkerColor(kRed);
  cmgr->Add(grcli);
  TGraphErrors *grcro = new TGraphErrors( (Int_t)(crogrp.size()) , &(crogrp[0]) , &(cropol[0]) , 0 , &(croerr[0]) );
  grcro->SetTitle("ihwp-out,wien-right");
  grcro->SetMarkerStyle(downtriangle);
  grcro->SetMarkerSize(cmarkersize);
  grcro->SetMarkerColor(kBlue);
  cmgr->Add(grcro);
  TGraphErrors *grcri = new TGraphErrors( (Int_t)(crigrp.size()) , &(crigrp[0]) , &(cripol[0]) , 0 , &(crierr[0]) );
  grcri->SetTitle("ihwp-in,wien-right");
  grcri->SetMarkerStyle(uptriangle);
  grcri->SetMarkerSize(cmarkersize);
  grcri->SetMarkerColor(kBlue);
  cmgr->Add(grcri);

  //113/23: down-triangle ==> HWP-OUT; 109/22: up-triangle ==> HWP-in
  //RED: Wein-Left; Blue: Wein-Right
  Float_t p2markersize  = 2;
  Float_t p3markersize  = 2;
  Int_t   wienleftmarker= 20;
  Int_t   wienrghtmarker= 21;
  Int_t   p2ihwpincol   = kAzure+7;
  Int_t   p2ihwpoutcol  = kCyan+2;
  Int_t   p3ihwpincol   = kOrange+7;
  Int_t   p3ihwpoutcol  = kMagenta+2;
  TGraphErrors *grp2lo = new TGraphErrors( (Int_t)(p2logrp.size()) , &(p2logrp[0]) , &(p2lopol[0]) , 0 , &(p2loerr[0]) );
  grp2lo->SetTitle("10um,ihwp-out,wien-left");
  grp2lo->SetMarkerStyle(wienleftmarker);
  grp2lo->SetMarkerSize(p2markersize);
  grp2lo->SetMarkerColor(p2ihwpoutcol);
  p2mgr->Add(grp2lo);
  pmgrout->Add(grp2lo);
  //TGraphErrors *grp2li = new TGraphErrors( (Int_t)(p2ligrp.size()) , &(p2ligrp[0]) , &(p2lipol[0]) , 0 , &(p2loerr[0]) );
  TGraphErrors *grp2li = new TGraphErrors( (Int_t)(p2ligrp.size()) , &(p2ligrp[0]) , &(p2lipol[0]) , 0 , &(p2lierr[0]) );
  grp2li->SetTitle("10um,ihwp-in, wein-left");
  grp2li->SetMarkerStyle(wienleftmarker);
  grp2li->SetMarkerSize(p2markersize);
  grp2li->SetMarkerColor(p2ihwpincol);
  p2mgr->Add(grp2li);
  pmgrin->Add(grp2li);
  TGraphErrors *grp2ro = new TGraphErrors( (Int_t)(p2rogrp.size()) , &(p2rogrp[0]) , &(p2ropol[0]) , 0 , &(p2loerr[0]) );
  grp2ro->SetTitle("10um,ihwp-out,wien-right");
  grp2ro->SetMarkerStyle(wienrghtmarker);
  grp2ro->SetMarkerSize(p2markersize);
  grp2ro->SetMarkerColor(p2ihwpoutcol);
  p2mgr->Add(grp2ro);
  pmgrout->Add(grp2ro);
  TGraphErrors *grp2ri = new TGraphErrors( (Int_t)(p2rigrp.size()) , &(p2rigrp[0]) , &(p2ripol[0]) , 0 , &(p2loerr[0]) );
  grp2ri->SetTitle("10um,ihwp-in,wien-right");
  grp2ri->SetMarkerStyle(wienrghtmarker);
  grp2ri->SetMarkerSize(p2markersize);
  grp2ri->SetMarkerColor(p2ihwpincol);
  p2mgr->Add(grp2ri);
  pmgrin->Add(grp2ri);
  
  TGraphErrors *grp3lo = new TGraphErrors( (Int_t)(p3logrp.size()) , &(p3logrp[0]) , &(p3lopol[0]) , 0 , &(p3loerr[0]) );
  grp3lo->SetTitle("4um,ihwp-out,wien-left");
  grp3lo->SetMarkerStyle(wienleftmarker);
  grp3lo->SetMarkerSize(p3markersize);
  grp3lo->SetMarkerColor(p3ihwpoutcol);
  p3mgr->Add(grp3lo);
  pmgrout->Add(grp3lo);
  TGraphErrors *grp3li = new TGraphErrors( (Int_t)(p3ligrp.size()) , &(p3ligrp[0]) , &(p3lipol[0]) , 0 , &(p3lierr[0]) );
  grp3li->SetTitle("4um,ihwp-in, wein-left");
  grp3li->SetMarkerStyle(wienleftmarker);
  grp3li->SetMarkerSize(p3markersize);
  grp3li->SetMarkerColor(p3ihwpincol);
  p3mgr->Add(grp3li);
  pmgrin->Add(grp3li);
  TGraphErrors *grp3ro = new TGraphErrors( (Int_t)(p3rogrp.size()) , &(p3rogrp[0]) , &(p3ropol[0]) , 0 , &(p3roerr[0]) );
  grp3ro->SetTitle("4um,ihwp-out,wien-right");
  grp3ro->SetMarkerStyle(wienrghtmarker);
  grp3ro->SetMarkerSize(p3markersize);
  grp3ro->SetMarkerColor(p3ihwpoutcol);
  p3mgr->Add(grp3ro);
  pmgrout->Add(grp3ro);
  TGraphErrors *grp3ri = new TGraphErrors( (Int_t)(p3rigrp.size()) , &(p3rigrp[0]) , &(p3ripol[0]) , 0 , &(p3rierr[0]) );
  grp3ri->SetTitle("4um,ihwp-in,wien-right");
  grp3ri->SetMarkerStyle(wienrghtmarker);
  grp3ri->SetMarkerSize(p3markersize);
  grp3ri->SetMarkerColor(p3ihwpincol);
  p3mgr->Add(grp3ri);
  pmgrin->Add(grp3ri);

  //TGRAPHS FOR FITS
  TGraphErrors *grp3o = new TGraphErrors( (Int_t)(p3ogrp.size()) , &(p3ogrp[0]) , &(p3opol[0]) , 0 , &(p3oerr[0]) );
  grp3o->SetTitle("4um,ihwp-out,wien-left");
  grp3o->SetMarkerStyle(wienleftmarker);
  grp3o->SetMarkerSize(0);
  grp3o->SetMarkerColor(p3ihwpoutcol);
  grp3o->Fit("pol0");
  TF1 * fitgrp3o = grp3o->GetFunction("pol0");
  fitgrp3o->SetLineColor(p3ihwpoutcol);
  fitgrp3o->SetLineStyle(7);
  p3mgr->Add(grp3o);
  pmgrout->Add(grp3o);

  TGraphErrors *grp3i = new TGraphErrors( (Int_t)(p3igrp.size()) , &(p3igrp[0]) , &(p3ipol[0]) , 0 , &(p3ierr[0]) );
  grp3i->SetTitle("4um,ihwp-in, wein-left");
  grp3i->SetMarkerStyle(wienleftmarker);
  grp3i->SetMarkerSize(0);
  grp3i->SetMarkerColor(p3ihwpincol);
  grp3i->Fit("pol0");
  TF1 * fitgrp3i = grp3i->GetFunction("pol0");
  fitgrp3i->SetLineColor(p3ihwpincol);
  fitgrp3i->SetLineStyle(7);
  p3mgr->Add(grp3i);
  pmgrin->Add(grp3i);

  TGraphErrors *grp2o = new TGraphErrors( (Int_t)(p2ogrp.size()) , &(p2ogrp[0]) , &(p2opol[0]) , 0 , &(p2oerr[0]) );
  grp2o->SetTitle("4um,ihwp-out,wien-right");
  grp2o->SetMarkerStyle(wienrghtmarker);
  grp2o->SetMarkerSize(0);
  grp2o->SetMarkerColor(p2ihwpoutcol);
  grp2o->Fit("pol0");
  TF1 * fitgrp2o = grp2o->GetFunction("pol0");
  fitgrp2o->SetLineColor(p2ihwpoutcol);
  fitgrp2o->SetLineStyle(7);
  p2mgr->Add(grp2o);
  pmgrout->Add(grp2o);

  TGraphErrors *grp2i = new TGraphErrors( (Int_t)(p2igrp.size()) , &(p2igrp[0]) , &(p2ipol[0]) , 0 , &(p2ierr[0]) );
  grp2i->SetTitle("4um,ihwp-in,wien-right");
  grp2i->SetMarkerStyle(wienrghtmarker);
  grp2i->SetMarkerSize(0);
  grp2i->SetMarkerColor(p2ihwpincol);
  grp2i->Fit("pol0");
  TF1 * fitgrp2i = grp2i->GetFunction("pol0");
  fitgrp2i->SetLineColor(p2ihwpincol);
  fitgrp2i->SetLineStyle(7);
  p2mgr->Add(grp2i);
  pmgrin->Add(grp2i);

  //Graphs for 10um and 4um IN+OUT Means to determine constant for adjustment
  TGraphErrors *grp2m = new TGraphErrors( (Int_t)(p2mgrp.size()) , &(p2mgrp[0]) , &(p2mpol[0]) , 0 , &(p2merr[0]) );
    grp2m->SetTitle("4um IN+OUT");
    grp2m->SetMarkerStyle(20);
    grp2m->SetMarkerSize(2);
    grp2m->SetMarkerColor(kTeal+4);//10 kPink-3
    grp2m->Fit("pol0");
    TF1 * fitgrp2m = grp2m->GetFunction("pol0");
      fitgrp2m->SetLineColor(kTeal+4);
      fitgrp2m->SetLineStyle(7);
    p23iomg->Add(grp2m);
  TGraphErrors *grp3m = new TGraphErrors( (Int_t)(p3mgrp.size()) , &(p3mgrp[0]) , &(p3mpol[0]) , 0 , &(p3merr[0]) );
    grp3m->SetTitle("10um IN+OUT");
    grp3m->SetMarkerStyle(20);
    grp3m->SetMarkerSize(2);
    grp3m->SetMarkerColor(kViolet+4);//10 kPink-3
    grp3m->Fit("pol0");
    TF1 * fitgrp3m = grp3m->GetFunction("pol0");
      fitgrp3m->SetLineColor(kViolet+4);
      fitgrp3m->SetLineStyle(7);
    p23iomg->Add(grp3m);

  //Calculate correction to 4um Pol from above fits.
  Double_t d4um_pol_mean = fitgrp3m->GetParameter(0);
  Double_t d4um_pol_err  = fitgrp3m->GetParError(0);
  Double_t d10um_pol_mean= fitgrp2m->GetParameter(0);
  Double_t d10um_pol_err = fitgrp2m->GetParError(0);

  //4-to-10 Converstion Ratio
  TGraphErrors *grratios = new TGraphErrors( (Int_t)(pair_numb.size()) , &(pair_numb[0]) , &(pairratio[0]) , 0 , &(pairerror[0]) );
  grratios->SetTitle("Aggregated Results ::  PREX-II Moller Polarimetry 10#mum/4#mum Ratios Used to Compute 4um #rightarrow 10um Correction;Ratio of 10um:4um Measurement;Measurement Pair ID");
  grratios->SetMarkerStyle(20);
  grratios->SetMarkerSize(2);
  grratios->SetMarkerColor(kBlack);
  grratios->Fit("pol0");
  TF1 * fitgrratios = grratios->GetFunction("pol0");
      fitgrratios->SetLineColor(kBlack);
      fitgrratios->SetLineStyle(7);
  
  Double_t d4to10_corr_fact     = fitgrratios->GetParameter(0);
  Double_t d4to10_corr_fact_err = fitgrratios->GetParError(0);

  //CORRECTION FOR 4um FOILS TAKES PLACE HERE IN FUNCTION ABOVE MAIN() DONE ON IN AND OUT
  //PREX Semi-final Adjusted Polarizations
  do4umPolCorr( ppi1grp , ppi1pol , ppi1err , d4to10_corr_fact , d4to10_corr_fact_err);
  do4umPolCorr( ppo1grp , ppo1pol , ppo1err , d4to10_corr_fact , d4to10_corr_fact_err);

  //Final PREX Plot
  Int_t colFinalIn = kAzure-3;
  Int_t colFinalOut= kPink-3;
  TGraphErrors *grppo1 = new TGraphErrors( (Int_t)(ppo1grp.size()) , &(ppo1grp[0]) , &(ppo1pol[0]) , 0 , &(ppo1err[0]) );
    grppo1->SetTitle("HWP-OUT");
    grppo1->SetMarkerStyle(20);
    grppo1->SetMarkerSize(2);
    grppo1->SetMarkerColor(kPink-3);
    grppo1->Fit("pol0");
    TF1 * fitgrppo1 = grppo1->GetFunction("pol0");
      fitgrppo1->SetLineColor(kPink-3);
      fitgrppo1->SetLineStyle(7);
    ppiomgr->Add(grppo1);
  TGraphErrors *grppi1 = new TGraphErrors( (Int_t)(ppi1grp.size()) , &(ppi1grp[0]) , &(ppi1pol[0]) , 0 , &(ppi1err[0]) );
    grppi1->SetTitle("HWP-IN");
    grppi1->SetMarkerStyle(20);
    grppi1->SetMarkerSize(2);
    grppi1->SetMarkerColor(kAzure-3);
    grppi1->Fit("pol0");
    TF1 * fitgrppi1 = grppi1->GetFunction("pol0");
      fitgrppi1->SetLineColor(kAzure-3);
      fitgrppi1->SetLineStyle(7);
    ppiomgr->Add(grppi1);

  //Legends
  auto ppiolegend = new TLegend(0.83,0.12,0.99,0.36);
  ppiolegend->AddEntry(grppo1);
  ppiolegend->AddEntry(grppi1);

  auto plegend = new TLegend(0.83,0.12,0.99,0.36);
  plegend->AddEntry(grplo);
  plegend->AddEntry(grpli);
  plegend->AddEntry(grpro);
  plegend->AddEntry(grpri);

  auto p2legend = new TLegend(0.83,0.12,0.99,0.36);
  p2legend->AddEntry(grp2lo);
  p2legend->AddEntry(grp2li);

  auto p3legend = new TLegend(0.83,0.12,0.99,0.36);
  p3legend->AddEntry(grp3lo);
  p3legend->AddEntry(grp3ro);
  p3legend->AddEntry(grp3li);
  p3legend->AddEntry(grp3ri);

  auto pinlegend = new TLegend(0.83,0.12,0.99,0.36);
  pinlegend->AddEntry(grp2li);
  pinlegend->AddEntry(grp3ri);
  pinlegend->AddEntry(grp3li);

  auto poutlegend = new TLegend(0.83,0.12,0.99,0.36);
  poutlegend->AddEntry(grp2ro);
  poutlegend->AddEntry(grp2lo);
  poutlegend->AddEntry(grp3ro);
  poutlegend->AddEntry(grp3lo);

  auto clegend = new TLegend(0.83,0.12,0.99,0.36);
  clegend->AddEntry(grclo);
  clegend->AddEntry(grcli);
  clegend->AddEntry(grcro);
  clegend->AddEntry(grcri);

  auto p23mlegend = new TLegend(0.83,0.12,0.99,0.36);
  p23mlegend->AddEntry(grp3m);
  p23mlegend->AddEntry(grp2m);

  Int_t  mgrlimitp1 = 1110;
  Int_t  mgrlimitp2 = 1105;
  Int_t  mgrlimitp3 = 3048;

  Int_t  canvas_width = 1500;
  Int_t  canvas_height = 621;//927:Golden;621:Silver;455:Bronze 


  TCanvas * canprex = new TCanvas("canprex","canprex",canvas_width,canvas_height);
  pmgr->SetTitle("(Aggregated Results v0) PREX-II Moller Polarimetry;Moller Polarimetry Group Number;Polarization [\%]");
  gPad->SetGrid(2,2);
  pmgr->GetXaxis()->SetTitleSize(0.04);
  pmgr->GetYaxis()->SetTitleSize(0.04);
  pmgr->GetYaxis()->SetTitleOffset(0.85);
  pmgr->GetXaxis()->SetLimits( pmgr->GetXaxis()->GetXmin()-3. , pmgr->GetXaxis()->GetXmax()+10 );
  pmgr->Draw("AP");  
  pmgr->GetYaxis()->SetRangeUser( 87.5 , 92 );
  TPaveText *ptpp1_1a = new TPaveText(1035.25,91.8,1040.75,92.05);
  ptpp1_1a->AddText("8/18/2019");
  ptpp1_1a->Draw("");
  TPaveText *ptpp1_2a = new TPaveText(1041,91.8,1046.5,92.05);
  ptpp1_2a->AddText("8/21/2019");
  ptpp1_2a->Draw("");
  TPaveText *ptpp1_3a = new TPaveText(1050.75,91.8,1056.25,92.05);
  ptpp1_3a->AddText("08/26/2019");
  ptpp1_3a->Draw("");
  TPaveText *ptpp1_4a = new TPaveText(1060.75,91.8,1075.25,92.05);
  ptpp1_4a->AddText("08/31/2019");
  ptpp1_4a->Draw("");
  TPaveText *ptpp1_5a = new TPaveText(1076.75,91.8,1082.25,92.05);
  ptpp1_5a->AddText("09/04/2019");
  ptpp1_5a->Draw("");
  TPaveText *ptpp1_6a = new TPaveText(1090.75,91.8,1096.25,92.05);
  ptpp1_6a->AddText("09/08/2019");
  ptpp1_6a->Draw("");
  TPaveText *ptpp1_7a = new TPaveText(1022.,91.8,1027.5,92.05);
  ptpp1_7a->AddText("08/04/2019");
  ptpp1_7a->Draw("");
  TPaveText *ptpp1_8a = new TPaveText(1027.75,91.8,1033.25,92.05);
  ptpp1_8a->AddText("08/10/2019");
  ptpp1_8a->Draw("");
  gPad->Update();
  canprex->Modified();
  plegend->Draw("SAME");
  canprex->SaveAs("aggregated_prex_results_v0.png");
  canprex->Print("THESIS_PLOTS/PREX_reference_all.pdf");
 

  TCanvas * canp2rex = new TCanvas("canp2rex","canp2rex",canvas_width,canvas_height);
  p2mgr->SetTitle("Aggregated Results :: PREX-II Moller Polarimetry :: 10um Target Comparison (For Reference Only);Moller Polarimetry Group Number;Polarization [\%]");
  gPad->SetGrid(2,2);
  p2mgr->GetXaxis()->SetTitleSize(0.04);
  p2mgr->GetYaxis()->SetTitleSize(0.04);
  p2mgr->GetYaxis()->SetTitleOffset(0.85);
  p2mgr->GetXaxis()->SetLimits( p2mgr->GetXaxis()->GetXmin()-2. , mgrlimitp2 );
  p2mgr->GetYaxis()->SetRangeUser( 86.5 , 91.5 );
  p2mgr->Draw("AP");
  fitgrp2o->Draw("SAME");
  gPad->Update();
  TPaveStats* statp2o = (TPaveStats*)(grp2o->FindObject("stats"));
  statp2o->SetX1NDC(0.83);
  statp2o->SetX2NDC(0.99);
  statp2o->SetY1NDC(0.64);
  statp2o->SetY2NDC(0.88);
  statp2o->SetTextColor(p2ihwpoutcol);
  statp2o->Draw("SAMES");
  fitgrp2i->Draw("SAME");
  TPaveStats* statp2i = (TPaveStats*)(grp2i->FindObject("stats"));
  statp2i->SetX1NDC(0.83);
  statp2i->SetX2NDC(0.99);
  statp2i->SetY1NDC(0.38);
  statp2i->SetY2NDC(0.62);
  statp2i->SetTextColor(p2ihwpincol);
  statp2i->Draw("SAMES");
  gPad->Update();
  p2legend->Draw("SAME");
  canp2rex->SaveAs("aggregated_prex_results_v1_1.png");
  canp2rex->Print("THESIS_PLOTS/PREX_reference_10_in_out_left.pdf");


  TCanvas * canp3rex = new TCanvas("canp3rex","canp3rex",canvas_width,canvas_height);
  p3mgr->SetTitle("Aggregated Results :: PREX-II Moller Polarimetry 4um Target Comparison (For Reference Only);Moller Polarimetry Group Number;Polarization [\%]");
  gPad->SetGrid(2,2);
  p3mgr->GetXaxis()->SetLimits( p3mgr->GetXaxis()->GetXmin()-2. , mgrlimitp2 + 3 );
  p3mgr->GetYaxis()->SetRangeUser( 86.5 , 91.5 );
  p3mgr->Draw("AP");
  p3mgr->GetXaxis()->SetTitleSize(0.04);
  p3mgr->GetYaxis()->SetTitleSize(0.04);
  p3mgr->GetYaxis()->SetTitleOffset(0.85);
  fitgrp3o->Draw("SAME");
  gPad->Update();
  TPaveStats* statp3o = (TPaveStats*)(grp3o->FindObject("stats"));
  statp3o->SetX1NDC(0.83);
  statp3o->SetX2NDC(0.99);
  statp3o->SetY1NDC(0.64);
  statp3o->SetY2NDC(0.88);
  statp3o->SetTextColor(p3ihwpoutcol);
  statp3o->Draw("SAMES");
  fitgrp3i->Draw("SAME");
  TPaveStats* statp3i = (TPaveStats*)(grp3i->FindObject("stats"));
  statp3i->SetX1NDC(0.83);
  statp3i->SetX2NDC(0.99);
  statp3i->SetY1NDC(0.38);
  statp3i->SetY2NDC(0.62);
  statp3i->SetTextColor(p3ihwpincol);
  statp3i->Draw("SAMES");
  gPad->Update();
  p3legend->Draw("SAME");
  canp3rex->SaveAs("aggregated_prex_results_v1_2.png");
  canp3rex->Print("THESIS_PLOTS/PREX_reference_4_in_out_left_right.pdf");


  TCanvas * canprex23in = new TCanvas("canprex23in","canprex23in",canvas_width,canvas_height);
  pmgrin->SetTitle("Aggregated Results :: PREX-II Moller Polarimetry 4um & 10um HPW-IN Comparison (For Reference Only);Moller Polarimetry Group Number;Polarization [\%]");
  gPad->SetGrid(2,2);
  pmgrin->GetXaxis()->SetTitleSize(0.04);
  pmgrin->GetYaxis()->SetTitleSize(0.04);
  pmgrin->GetYaxis()->SetTitleOffset(0.85);
  pmgrin->GetXaxis()->SetLimits( p3mgr->GetXaxis()->GetXmin()-2. , mgrlimitp2 );
  pmgrin->GetYaxis()->SetRangeUser( 86.5 , 91.5 );
  pmgrin->Draw("AP");
  fitgrp2i->Draw("SAME");
  gPad->Update();
  TPaveStats* statp2iA = (TPaveStats*)(grp2i->FindObject("stats"));
  statp2iA->SetX1NDC(0.83);
  statp2iA->SetX2NDC(0.99);
  statp2iA->SetY1NDC(0.64);
  statp2iA->SetY2NDC(0.88);
  statp2iA->SetTextColor(kAzure+7);
  statp2iA->Draw("SAMES");
  fitgrp3i->Draw("SAME");
  TPaveStats* statp3iA = (TPaveStats*)(grp3i->FindObject("stats"));
  statp3iA->SetX1NDC(0.83);
  statp3iA->SetX2NDC(0.99);
  statp3iA->SetY1NDC(0.38);
  statp3iA->SetY2NDC(0.62);
  statp3iA->SetTextColor(kOrange+7);
  statp3iA->Draw("SAMES");
  gPad->Update();
  pinlegend->Draw("SAME");
  canprex23in->SaveAs("aggregated_prex_results_v1_3.png");
  canprex23in->Print("THESIS_PLOTS/PREX_reference_4_10_in_left_right.pdf");


  TCanvas * canprex23out = new TCanvas("canprex23out","canprex23out",canvas_width,canvas_height);
  pmgrout->SetTitle("Aggregated Results :: PREX-II Moller Polarimetry 4um & 10um HPW-OUT Comparison (For Reference Only);Moller Polarimetry Group Number;Polarization [\%]");
  gPad->SetGrid(2,2);
  pmgrout->GetXaxis()->SetTitleSize(0.04);
  pmgrout->GetYaxis()->SetTitleSize(0.04);
  pmgrout->GetYaxis()->SetTitleOffset(0.85);
  pmgrout->GetXaxis()->SetLimits( p3mgr->GetXaxis()->GetXmin()-2. , mgrlimitp2 + 2 );
  pmgrout->GetYaxis()->SetRangeUser( 86.5 , 91.5 );
  pmgrout->Draw("AP");
  fitgrp2o->Draw("SAME");
  gPad->Update();
  TPaveStats* statp2oA = (TPaveStats*)(grp2o->FindObject("stats"));
  statp2oA->SetX1NDC(0.83);
  statp2oA->SetX2NDC(0.99);
  statp2oA->SetY1NDC(0.64);
  statp2oA->SetY2NDC(0.88);
  statp2oA->SetTextColor(p2ihwpoutcol);
  statp2oA->Draw("SAMES");
  fitgrp3o->Draw("SAME");
  TPaveStats* statp3oA = (TPaveStats*)(grp3o->FindObject("stats"));
  statp3oA->SetX1NDC(0.83);
  statp3oA->SetX2NDC(0.99);
  statp3oA->SetY1NDC(0.38);
  statp3oA->SetY2NDC(0.62);
  statp3oA->SetTextColor(p3ihwpoutcol);
  statp3oA->Draw("SAMES");
  gPad->Update();
  poutlegend->Draw("SAME");
  canprex23out->SaveAs("aggregated_prex_results_v1_4.png");
  canprex23out->Print("THESIS_PLOTS/PREX_reference_4_10_out_left_right.pdf");


  TCanvas * canprex23mean = new TCanvas("canprex23mean","canprex23mean",canvas_width,canvas_height);
  p23iomg->SetTitle("Aggregated Results :: PREX-II Moller Polarimetry 4um and 10um HWP-IN/OUT Means Comparison (For Reference Only);Moller Polarimetry Group Number;Polarization [\%]");
  gPad->SetGrid(2,2);
  p23iomg->GetXaxis()->SetTitleSize(0.04);
  p23iomg->GetYaxis()->SetTitleSize(0.04);
  p23iomg->GetYaxis()->SetTitleOffset(0.85);
  p23iomg->GetXaxis()->SetLimits( p23iomg->GetXaxis()->GetXmin()-3 , p23iomg->GetXaxis()->GetXmax()+10 );
  p23iomg->GetYaxis()->SetRangeUser( 86.5 , 91.5 );
  p23iomg->Draw("AP");
  fitgrp2m->Draw("SAME");
  gPad->Update();
  TPaveStats* statp2m = (TPaveStats*)(grp2m->FindObject("stats"));
  statp2m->SetX1NDC(0.83);
  statp2m->SetX2NDC(0.99);
  statp2m->SetY1NDC(0.38);
  statp2m->SetY2NDC(0.62);
  statp2m->SetTextColor(kTeal+4);
  statp2m->Draw("SAMES");
  fitgrp3m->Draw("SAME");
  TPaveStats* statp3m = (TPaveStats*)(grp3m->FindObject("stats"));
  statp3m->SetX1NDC(0.83);
  statp3m->SetX2NDC(0.99);
  statp3m->SetY1NDC(0.64);
  statp3m->SetY2NDC(0.88);
  statp3m->SetTextColor(kViolet+4);
  statp3m->Draw("SAMES");
  gPad->Update();
  p23mlegend->Draw("SAME");
  canprex23mean->SaveAs("aggregated_prex_results_v2_0.png");
  canprex23mean->Print("THESIS_PLOTS/PREX_results_4_10_inout.pdf");


  TCanvas * canprexprelimpol = new TCanvas("canprexprelimpol","canprexprelimpol",canvas_width,canvas_height);
  gPad->SetGrid(2,2);
  ppiomgr->SetTitle( Form("PREX-II Polarizations :: 4um Scaling Factor %1.4f #pm %1.4f;Moller Polarimetry Group Number;Polarization [\%]",d4to10_corr_fact,d4to10_corr_fact_err) );
  ppiomgr->GetXaxis()->SetTitleSize(0.04);
  ppiomgr->GetYaxis()->SetTitleSize(0.04);
  ppiomgr->GetYaxis()->SetTitleOffset(0.85);
  ppiomgr->Draw("AP");
  ppiomgr->GetXaxis()->SetLimits( ppiomgr->GetXaxis()->GetXmin()-3 , ppiomgr->GetXaxis()->GetXmax()+10 );
  ppiomgr->GetYaxis()->SetRangeUser( 87.5 , 92 );
  gPad->Update();
  canprexprelimpol->Modified();
  ppiomgr->Draw("AP");
  fitgrppi1->Draw("SAME");
  gPad->Update();
  TPaveStats* statppo1 = (TPaveStats*)(grppo1->FindObject("stats"));
  statppo1->SetX1NDC(0.83);
  statppo1->SetX2NDC(0.99);
  statppo1->SetY1NDC(0.64);
  statppo1->SetY2NDC(0.88);
  statppo1->SetTextColor(colFinalOut);
  statppo1->Draw("SAMES");
  fitgrppo1->Draw("SAME");
  TPaveStats* statppi1 = (TPaveStats*)(grppi1->FindObject("stats"));
  statppi1->SetX1NDC(0.83);
  statppi1->SetX2NDC(0.99);
  statppi1->SetY1NDC(0.38);
  statppi1->SetY2NDC(0.62);
  statppi1->SetTextColor(colFinalIn);
  statppi1->Draw("SAMES");
  gPad->Update();
  ppiolegend->Draw("SAME");
  TPaveText *ptpp1_1 = new TPaveText(1035.25,91.8,1040.75,92.05);
  ptpp1_1->AddText("8/18/2019");
  ptpp1_1->Draw("");
  TPaveText *ptpp1_2 = new TPaveText(1041,91.8,1046.5,92.05);
  ptpp1_2->AddText("8/21/2019");
  ptpp1_2->Draw("");
  TPaveText *ptpp1_3 = new TPaveText(1050.75,91.8,1056.25,92.05);
  ptpp1_3->AddText("08/26/2019");
  ptpp1_3->Draw("");
  TPaveText *ptpp1_4 = new TPaveText(1060.75,91.8,1075.25,92.05);
  ptpp1_4->AddText("08/31/2019");
  ptpp1_4->Draw("");
  TPaveText *ptpp1_5 = new TPaveText(1076.75,91.8,1082.25,92.05);
  ptpp1_5->AddText("09/04/2019");
  ptpp1_5->Draw("");
  TPaveText *ptpp1_6 = new TPaveText(1090.75,91.8,1096.25,92.05);
  ptpp1_6->AddText("09/08/2019");
  ptpp1_6->Draw("");
  TPaveText *ptpp1_7 = new TPaveText(1022.,91.8,1027.5,92.05);
  ptpp1_7->AddText("08/04/2019");
  ptpp1_7->Draw("");
  TPaveText *ptpp1_8 = new TPaveText(1027.75,91.8,1033.25,92.05);
  ptpp1_8->AddText("08/10/2019");
  ptpp1_8->Draw("");
  gPad->Update();
  canprexprelimpol->Modified();
  canprexprelimpol->SaveAs("aggregated_prex_results_v2_2.png");
  canprexprelimpol->Print("THESIS_PLOTS/PREX_Final_Results.pdf");
 

  TCanvas * can_ratios = new TCanvas("can_ratios","can_ratios",canvas_width,canvas_height);
  //gPad->SetGrid(2,2);
  //TPaveStats* statratio = (TPaveStats*)(grratios->FindObject("stats"));
  //statratio->Draw("SAMES");
  fitgrratios->Draw("SAME");
  grratios->GetXaxis()->SetTitleSize(0.04);
  grratios->GetYaxis()->SetTitleSize(0.04);
  grratios->GetYaxis()->SetTitleOffset(0.85);
  grratios->Draw("AP");
  gPad->Update();
  can_ratios->SaveAs( "aggregated_prex_results_v2_1.png" );
  can_ratios->Print( "THESIS_PLOTS/PREX_10_4_Ratios.pdf" );


  //CREX -- We'll probably want to plot this with current data from Compton.
  TCanvas * cancrex = new TCanvas("cancrex","cancrex",canvas_width,canvas_height);
  gPad->SetGrid(2,2);

  cmgr->GetXaxis()->SetTitleSize(0.04);
  cmgr->GetYaxis()->SetTitleSize(0.04);
  cmgr->GetYaxis()->SetTitleOffset(0.85);

  //////////////////////////////////
  //TODO: Blinding the axis for CREX results from Compton , remove next three lines to return axis.
  cmgr->SetTitle("(Blinded) Aggregated Results CREX Moller Polarimetry With Dates; Group Number;Polarization \%");
  cmgr->GetHistogram()->GetYaxis()->SetLabelOffset(999.);
  cmgr->GetHistogram()->GetYaxis()->SetLabelSize(0.01);
  ////////////////////////////

  cmgr->SetTitle("Aggregated Results CREX Moller Polarimetry With Dates; Group Number;Polarization \%");
  cmgr->GetXaxis()->SetLimits( cmgr->GetXaxis()->GetXmin()-2. , cmgr->GetXaxis()->GetXmax()+4 );
  cmgr->GetYaxis()->SetRangeUser( 84.5 , 89.5 );
  cmgr->Draw("AP");
  clegend->Draw("SAME");
  TPaveText *ptcp1_1 = new TPaveText(3001.75,89.3,3004.25,89.55);
  ptcp1_1->AddText("12/01/2019");
  ptcp1_1->Draw("");
  TPaveText *ptcp1_2 = new TPaveText(3010.75,89.3,3014.25,89.55);
  ptcp1_2->AddText("01/07/2020");
  ptcp1_2->Draw("");
  TPaveText *ptcp1_3 = new TPaveText(3015.75,89.3,3020.25,89.55);
  ptcp1_3->AddText("01/27/2020");
  ptcp1_3->Draw("");
  TPaveText *ptcp1_4 = new TPaveText(3020.75,89.3,3024.25,89.55);
  ptcp1_4->AddText("02/08/2020");
  ptcp1_4->Draw("");
  TPaveText *ptcp1_5 = new TPaveText(3036.75,89.3,3040.25,89.55);
  ptcp1_5->AddText("02/24/2020");
  ptcp1_5->Draw("");
  TPaveText *ptcp1_6 = new TPaveText(3040.625,89.3,3043.375,89.55);
  ptcp1_6->AddText("03/18/2020");
  ptcp1_6->Draw("");
  TPaveText *ptcp1_7 = new TPaveText(3043.75,89.3,3046.75,89.55);
  ptcp1_7->AddText("08/19/2020");
  ptcp1_7->Draw("");
  TPaveText *ptcp1_8 = new TPaveText(3047.25,89.3,3050.25,89.55);
  ptcp1_8->AddText("09/04/2020");
  ptcp1_8->Draw("");
  TPaveText *ptcp1_9 = new TPaveText(3050.75,89.3,3054.25,89.55);
  ptcp1_9->AddText("09/16/2020");
  ptcp1_9->Draw("");
  gPad->Update();
  cancrex->Modified();
  cancrex->SaveAs("THESIS_PLOTS/aggregated_crex_results_v0.pdf");
  cancrex->SaveAs("aggregated_crex_results_v0.png");

  delete row;
  server->Close();
  delete result;
  delete server;

  return 100;
}
