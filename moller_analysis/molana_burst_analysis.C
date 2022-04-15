#include <TH2.h>
#include <TSQLServer.h>
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
#include <TMultiGraph.h>
#include <TSQLResult.h>
#include <TSQLRow.h>

/*
CREATE TABLE pcrex_run_asym_burst (
  id_asym                    INT NOT NULL  COMMENT 'run number',
  asym_raw                   FLOAT  COMMENT 'raw asymmetry',
  asym_raw_err               FLOAT  COMMENT 'raw asymmetry error',
  asym_acc                   FLOAT  COMMENT 'accid corrected asym',
  asym_acc_err               FLOAT  COMMENT 'accid corrected asym error',
  asym_bcm                   FLOAT  COMMENT 'raw bcm normalized',
  asym_bcm_err               FLOAT  COMMENT 'raw bcm norm error',
  asym_accbcm                FLOAT  COMMENT 'accid corrected bcm normalized asym',
  asym_accbcm_err            FLOAT  COMMENT 'accid corrected bcm normalized asym error',
  asym_dt                    FLOAT  COMMENT 'raw asym deadtime corrected',
  asym_dt_err                FLOAT  COMMENT 'raw asym deadtime corr error',
  asym_full                  FLOAT  COMMENT 'fully corrected asymmetry',
  asym_full_err              FLOAT  COMMENT 'fully corrected asymmetry error',
  asym_Aa                    FLOAT  COMMENT 'accidental asymmetry over run (accidental corr)',
  asym_Aa2                   FLOAT  COMMENT 'accidental asymmetry over whole run',
  asym_Al                    FLOAT  COMMENT 'left asym over run (deadtime corr)',
  asym_Aq                    FLOAT  COMMENT 'bcm asymmetry over run',
  asym_Ra                    FLOAT  COMMENT 'average accidental rate',
  asym_Rl                    FLOAT  COMMENT 'average left rate',
  asym_Rq                    FLOAT  COMMENT 'average bcm rate',
  asym_pattern               FLOAT  COMMENT '4:quartet; 8:octet',
  asym_freq                  FLOAT  COMMENT 'helicity frequency',
  PRIMARY KEY (id_asym),
  FOREIGN KEY (id_asym) REFERENCES moller_run(id_run)
);
*/

Bool_t isnonnumber(char c){
  return !(c >= '0' && c <= '9');
}

//WE'VE SETTLED ON BURST SIZE OF 10; WE CAN SET THAT AS DEFAULT AND THEREFORE ONLY A STRING WILL NEED TO BE PASSED.
//THIS WILL OUTPUT ASYMMETRIES SO THERE'S NO NEED FOR PTARG OR ANPOW.
Int_t molana_burst_analysis(string FNAME, Int_t BURST = 10){

  const Double_t euler       = 2.71828182845904;//POOR JACOB BERNOULLI
  const Double_t deadtimetau = 0.00001572;
  Bool_t writetosql = true;

  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  //STRIP RUN NUMBER FROM FILE NAME
  std::string fnametrun = FNAME.substr( FNAME.find_last_of("\\/")+1 );
  std::string fnamecopy = fnametrun;
  fnamecopy.erase(std::remove_if(fnamecopy.begin(),fnamecopy.end(),isnonnumber),fnamecopy.end());
  stringstream ss;
  ss << fnamecopy;
  Int_t RUNN;
  ss >> RUNN;

  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  //CHECK FOR THE EXISTENCE OF ENVIRONMENTAL VARIABLES - FROM DON
  if(!gSystem->Getenv("MOLLER_ROOTFILE_DIR")){
    
    cout <<"Set environment variable MOLLER_ROOTFILE_DIR\n"
         <<"to point at the directory where the .root files\n"
         <<"should be stored then rerun."<<endl;
    exit(0);
  }
  if(!gSystem->Getenv("MOLANA_DB_HOST")){
    cout << "Set environment variable MOLANA_DB_HOST" << endl
         << "to point at the directory where the .root files" << endl
         << "should be stored then rerun." << endl << endl
         << "Turning off write to SQL database functionality." << endl;
    writetosql = false;
  }
  if(!gSystem->Getenv("MOLANA_DB_USER")){
    cout << "Set environment variable MOLANA_DB_USER" << endl
         << "to point at the directory where the .root files" << endl
         << "should be stored then rerun." << endl << endl
         << "Turning off write to SQL database functionality." << endl;
    writetosql = false;
  }
  if(!gSystem->Getenv("MOLANA_DB_PASS")){
    cout << "Set environment variable MOLANA_DB_PASS" << endl
         << "to point at the directory where the .root files" << endl
         << "should be stored then rerun." << endl << endl
         << "Turning off write to SQL database functionality." << endl;
    writetosql = false;
  }
  if(!gSystem->Getenv("MOLANA_DB_NAME")){
    cout << "Set environment variable MOLANA_DB_NAME" << endl
         << "to point at the directory where the .root files" << endl
         << "should be stored then rerun." << endl << endl
         << "Turning off write to SQL database functionality." << endl;
    writetosql = false;
  }

  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  //LOAD ROOT FILE
  TString filename("");
  filename.Form("%s/%s",gSystem->Getenv("MOLLER_ROOTFILE_DIR"),FNAME.c_str());
  cout << "filename: " << filename << endl;
  TChain trPatt("trPatt");
  trPatt.Add( filename );

  TString db_host;
  TString db_user;
  TString db_pass;
  if(writetosql){
    db_host = Form("mysql://%s/%s",gSystem->Getenv("MOLANA_DB_HOST"),gSystem->Getenv("MOLANA_DB_NAME"));
    db_user = Form("%s",gSystem->Getenv("MOLANA_DB_USER"));
    db_pass = Form("%s",gSystem->Getenv("MOLANA_DB_PASS"));
  }


  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // QUERY DB FOR FREQ AND PATTERN
  TSQLServer* server = TSQLServer::Connect(db_host,db_user,db_pass);

  TString select_query = "SELECT set_hel_freq, set_hel_pattern, set_t_stable, set_t_settle ";
  TString from_query   = "FROM moller_settings ";
  TString where_query  = Form("WHERE id_set_run = %i ;", RUNN);
  TString total_query  = select_query + from_query + where_query;
  cout << "query: " << total_query << endl;
  TSQLResult * result = server->Query(total_query);
  TSQLRow    * row;

  const Int_t numcols = result->GetFieldCount();
  const Int_t numrows = result->GetRowCount();

  row = result->Next();
  if(row == NULL){
    delete row;
    delete result;
    server->Close();
    delete server;
    exit (-10);
  }
  TString FREQ("");
  TString PATT("");
  TString TSTB("");
  TString TSTL("");
  for(Int_t i = 0; i < numrows; i++){
    for(Int_t j = 0; j < numcols; j++){
      TString value = row->GetField(j);
      TString field = result->GetFieldName(j);
      if( field=="set_hel_freq"){ FREQ = value; }
      if( field=="set_hel_pattern"){ PATT = value; }
      if( field=="set_t_stable"){ TSTB = value; }
      if( field=="set_t_settle"){ TSTL = value; }
    }
  }
  Int_t   freq    = FREQ.Atoi();
  Float_t tstable = TSTB.Atof();
  Float_t tsettle = TSTL.Atof();

  Float_t freqeff = 1000000. * 1 / tstable;

  cout << "molana_burst_analysis(): Base frequency is " << freq << endl;

  //Account for the settling time of the laser
  //TODO: FIXME: This needs to be more comprehensive to avoid potential problems if 
  //for some reason TStable isn't in DB or reports incorrectly.  TSettle can be 
  //used to calculate this value. And, if all else fails, use the frequency for
  //the rate calculations. For 120hz this is about 1.3%, for 240hz it's about 2.4%
  //this limit may need to be adjusted

  if( (freqeff / freq) < 1.03 ) freq = freqeff; 

  cout << "molana_burst_analysis(): Effective frequecy for rate calculations is " << freq << endl;

  Int_t patt(0);
  if(PATT.Contains("Quartet")) patt = 4;
  if(PATT.Contains("Octet"  )) patt = 8;

  cout << "Settings for run " << RUNN << ": freq(" << freq << "), patt(" << patt << ")" << endl;


  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // WRITE DATA STATS TO SQL DATABASE ON START
  if(writetosql){
    TSQLServer* ServerStart = TSQLServer::Connect(db_host,db_user,db_pass);
    TString queryStart("");
    //TString queryStart(Form("replace into moller_run (id_run) values (%d)",RUNN));
    TSQLResult* resultStart = ServerStart->Query(queryStart.Data());
    queryStart.Form("insert ignore into pcrex_run_asym_burst (id_asym,asym_pattern,asym_freq) values (%i,%d,%d);",RUNN,patt,freq);
    cout << "Initial populate: " << queryStart << endl;
    resultStart = ServerStart->Query(queryStart.Data());
    ServerStart->Close();
  }


  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // Holders
  Double_t        coin0 = 0.0;
  Double_t        coin1 = 0.0;
  Double_t        coinavg = 0.0;
  Double_t        coinasymuncorr = 0.0;
  Double_t        coinasym = 0.0;
  Double_t        left0 = 0.0;
  Double_t        left1 = 0.0;
  Double_t        leftavg = 0.0;
  Double_t        leftasym = 0.0;
  Double_t        right0 = 0.0;
  Double_t        right1 = 0.0;
  Double_t        rightavg = 0.0;
  Double_t        rightasym = 0.0;
  Double_t        accid0 = 0.0;
  Double_t        accid1 = 0.0;
  Double_t        accidavg = 0.0;
  Double_t        accidasym = 0.0;
  Double_t        bcm0 = 0.0;
  Double_t        bcm1 = 0.0;
  Double_t        bcmavg = 0.0;
  Double_t        bcmasym = 0.0;
  Int_t           time = 0;
  Int_t           pattnumb = 0;
  Int_t           pattsign = 0;
  Int_t           error = 0;
  Int_t           run = 0;
  Int_t           runincrindex = 0;
  TBranch        *b_coin0;
  TBranch        *b_coin1;
  TBranch        *b_coinavg;
  TBranch        *b_coinasymuncorr;
  TBranch        *b_coinasym;
  TBranch        *b_left0;
  TBranch        *b_left1;
  TBranch        *b_leftavg;
  TBranch        *b_leftasym;
  TBranch        *b_right0;
  TBranch        *b_right1;
  TBranch        *b_rightavg;
  TBranch        *b_rightasym;
  TBranch        *b_accid0;
  TBranch        *b_accid1;
  TBranch        *b_accidavg;
  TBranch        *b_accidasym;
  TBranch        *b_bcm0;
  TBranch        *b_bcm1;
  TBranch        *b_bcmavg;
  TBranch        *b_bcmasym;
  TBranch        *b_time;
  TBranch        *b_pattnumb;
  TBranch        *b_pattsign;
  TBranch        *b_error;
  TBranch        *b_run;
  TBranch        *b_runincrindex;

  trPatt.SetBranchAddress("coin0",          &coin0,     &b_coin0);
  trPatt.SetBranchAddress("coin1",          &coin1,     &b_coin1);
  trPatt.SetBranchAddress("coinavg",        &coinavg,   &b_coinavg);
  trPatt.SetBranchAddress("coinasymuncorr", &coinasymuncorr, &b_coinasymuncorr);
  trPatt.SetBranchAddress("coinasym",       &coinasym,  &b_coinasym);
  trPatt.SetBranchAddress("left0",          &left0,     &b_left0);
  trPatt.SetBranchAddress("left1",          &left1,     &b_left1);
  trPatt.SetBranchAddress("leftavg",        &leftavg,   &b_leftavg);
  trPatt.SetBranchAddress("leftasym",       &leftasym,  &b_leftasym);
  trPatt.SetBranchAddress("right0",         &right0,    &b_right0);
  trPatt.SetBranchAddress("right1",         &right1,    &b_right1);
  trPatt.SetBranchAddress("rightavg",       &rightavg,  &b_rightavg);
  trPatt.SetBranchAddress("rightasym",      &rightasym, &b_rightasym);
  trPatt.SetBranchAddress("accid0",         &accid0,    &b_accid0);
  trPatt.SetBranchAddress("accid1",         &accid1,    &b_accid1);
  trPatt.SetBranchAddress("accidavg",       &accidavg,  &b_accidavg);
  trPatt.SetBranchAddress("accidasym",      &accidasym, &b_accidasym);
  trPatt.SetBranchAddress("bcm0",           &bcm0,      &b_bcm0);
  trPatt.SetBranchAddress("bcm1",           &bcm1,      &b_bcm1);
  trPatt.SetBranchAddress("bcmavg",         &bcmavg,    &b_bcmavg);
  trPatt.SetBranchAddress("bcmasym",        &bcmasym,   &b_bcmasym);
  trPatt.SetBranchAddress("time", &time, &b_time);
  trPatt.SetBranchAddress("pattnumb", &pattnumb, &b_pattnumb);
  trPatt.SetBranchAddress("pattsign", &pattsign, &b_pattsign);
  trPatt.SetBranchAddress("error", &error, &b_error);
  trPatt.SetBranchAddress("run", &run, &b_run);
  trPatt.SetBranchAddress("runincrindex", &runincrindex, &b_runincrindex);

  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // LEAVE THIS AS A CHECK NOW. FINISH UP CORRECTING ERROR RECORDING BUG IN PATTERN FILE JUST SET INT AS ( ERROR_CODE += 2**(ERROR_TYPE) )
  // COMPUTE FINAL CUTS
  Int_t   bcmbin = 5000;
  Float_t bcmmin = 0;
  Float_t bcmmax = 5000;
  TH1F * hbcm1 = new TH1F("hbcm1",Form("Pattern BCM distribution for Prompted Errorless Events : RUN %d",RUNN),bcmbin,bcmmin,bcmmax);
  TH1F * hbcm2 = new TH1F("hbcm2",Form("Pattern BCM Distribution -- Cut for A_{q} Outliers : RUN %d",RUNN),bcmbin,bcmmin,bcmmax);
  Int_t    asymbin = 601;
  Double_t asymmin = -1.0;
  Double_t asymmax =  1.0;
  TH1F * haq1  = new TH1F("haq1",Form("Pattern Aq Distribution for Prompted Errorless Events : RUN %d",RUNN),asymbin,asymmin,asymmax);
  TH1F * haq2  = new TH1F("haq2",Form("Pattern Aq Distribution -- Cut for A_{q} Outliers : RUN %d",RUNN),asymbin,asymmin,asymmax);
  Float_t bcmstdev = 0.;
  Float_t bcm_mean = 0.;
  Int_t  goodpatts = 0.;

  Int_t unixtimemin(2147483647); // START AT OPPOSITES
  Int_t unixtimemax(0);
  Long64_t nentries = trPatt.GetEntries();
  cout << "Total entries: " << nentries << endl;
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    nb = trPatt.GetEntry(jentry);   nbytes += nb;

    if(time > unixtimemax) unixtimemax = time;
    if(time < unixtimemin) unixtimemin = time;
     
    if(error == 0) hbcm1->Fill(bcmavg);
    if(error == 0) haq1->Fill(bcmasym);

   }

   //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
   // COMPUTE CUTS RANGE HERE
   bcmstdev = hbcm1->GetStdDev();
   bcm_mean = hbcm1->GetMean();
   Float_t aqstdev = haq1->GetStdDev();
   Float_t aq_mean = haq1->GetMean();

   cout << "StartUnix: " << unixtimemin << ", and EndUnix: " << unixtimemax << endl;

   cout << "BCM Mean: " << bcm_mean << ", BCM StdDev: " << bcmstdev << endl;
   cout << "BCM Cuts [" << bcm_mean - 4*bcmstdev << "," << bcm_mean + 4*bcmstdev << "]" << endl;

   Float_t bcmcutmin = bcm_mean - 4*bcmstdev;
   Float_t bcmcutmax = bcm_mean + 4*bcmstdev;


   //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
   //BURST ANALYSIS STUFF HERE
   TGraph * gr_asym_uncorr = new TGraph();
     gr_asym_uncorr->SetTitle( Form("BURST-LEVEL :: RAW ASYMMETRY : RUN %i WITH BURST SIZE %i;Local JLab Time; Uncorrected Asymmetry",RUNN,BURST));
     gr_asym_uncorr->SetMarkerStyle(6);
   TGraph * gr_asym_acccor = new TGraph();
     gr_asym_acccor->SetTitle( Form("BURST-LEVEL :: ACCIDENTAL-CORRECTED ASYMMETRY : RUN %i with BURST SIZE %i;Local JLab Time; Accidental Correted Asymmetry",RUNN,BURST));
     gr_asym_acccor->SetMarkerStyle(6);
   TGraph * gr_asym_bcmnrm = new TGraph();
     gr_asym_bcmnrm->SetTitle( Form("BURST-LEVEL :: RAW ASYMMETRY BCM NORMALIZED : RUN %i WITH BURST SIZE %i;Local JLab Time; BCM Corrected Asymmetry",RUNN,BURST));
     gr_asym_bcmnrm->SetMarkerStyle(6);
   TGraph * gr_asym_cornor = new TGraph();
     gr_asym_cornor->SetTitle( Form("BURST-LEVEL :: ACCIDENTAL-CORRECTED ASYMMETRY BCM NORMALIZED : RUN %i with BURST SIZE %i;Local JLab Time; Accidental & BCM Corrected Asymmetry",RUNN,BURST));
     gr_asym_cornor->SetMarkerStyle(6);
   TGraph * gr_asym_dtcorr = new TGraph();
     gr_asym_dtcorr->SetTitle( Form("BURST-LEVEL :: DEADTIME-ONLY CORRECTED ASYMMETRY : RUN %i with BURST SIZE %i;Local JLab Time; Dead Time Corrected Asymmetry",RUNN,BURST));
     gr_asym_dtcorr->SetMarkerStyle(6);
   TGraph * gr_asym_final = new TGraph();
     gr_asym_final->SetTitle( Form("BURST-LEVEL :: FULLY CORRECTED ASYMMETRY : RUN %i WITH BURST SIZE %i;Local JLab Time; Fully Corrected Asymmetry",RUNN,BURST));
     gr_asym_final->SetMarkerStyle(6);

   TGraph * gr_asym_BCM = new TGraph();
     gr_asym_BCM->SetTitle( Form("BURST-LEVEL :: BCM ASMMETRY : RUN %i WITH BURST SIZE %i;Local JLab Time; BCM Asymmetry",RUNN,BURST));
     gr_asym_BCM->SetMarkerStyle(6);
   TGraph * gr_asym_LEFT = new TGraph();
     gr_asym_LEFT->SetTitle( Form("BURST-LEVEL :: LEFT ASYMMETRY : RUN %i WITH BURST SIZE %i;Local JLab Time; Left Singles Asymmetry",RUNN,BURST));
     gr_asym_LEFT->SetMarkerStyle(6);
   TGraph * gr_asym_ACCID = new TGraph();
     gr_asym_ACCID->SetTitle( Form("BURST-LEVEL :: ACCIDENTAL ASYMMETRY : RUN %i WITH BURST SIZE %i;Local JLab Time; Accidentals Asymmetry",RUNN,BURST));
     gr_asym_ACCID->SetMarkerStyle(6);

   TGraph * gr_rate_LEFT = new TGraph();
     gr_rate_LEFT->SetTitle( Form("BURST-LEVEL :: LEFT RATE : RUN %i WITH BURST SIZE %i;Local JLab Time; Left Rate [Hz]",RUNN,BURST));
     gr_rate_LEFT->SetMarkerStyle(6);
   TGraph * gr_rate_ACCID = new TGraph();
     gr_rate_ACCID->SetTitle( Form("BURST-LEVEL :: ACCID RATE : RUN %i WITH BURST SIZE %i;Local JLab Time; Accidentals Rate [Hz]",RUNN,BURST));
     gr_rate_ACCID->SetMarkerStyle(6);
   TGraph * gr_rate_BCM = new TGraph();
     gr_rate_BCM->SetTitle( Form("BURST-LEVEL :: BCM RATE : RUN %i WITH BURST SIZE %i;Local JLab Time; BCM Rate [Hz]",RUNN,BURST));
     gr_rate_BCM->SetMarkerStyle(6);
   TGraph * gr_rate_COIN = new TGraph();
     gr_rate_COIN->SetTitle( Form("BURST-LEVEL :: COINCIDENCE RATE : RUN %i WITH BURST SIZE %i;Local JLab Time; Coincidence Rate [Hz]",RUNN,BURST));
     gr_rate_COIN->SetMarkerStyle(6);

   Int_t   asymbin2 =  240;
   Float_t asymmin2 = -0.4;
   Float_t asymmax2 =  0.4;

   TH1F * h_asym = new TH1F("h_asym",Form("Asymmetry Distribution : RUNN %i with BLOCK SIZE %i;Asymmetry;Count [N]",RUNN,BURST),asymbin2,asymmin2,asymmax2);

   Double_t coin0sum  = 0.;
   Double_t coin1sum  = 0.;
   Double_t accid0sum = 0.;
   Double_t accid1sum = 0.;
   Double_t bcm0sum   = 0.;
   Double_t bcm1sum   = 0.;
   Double_t left0sum  = 0.;
   Double_t left1sum  = 0.;
   Double_t right0sum = 0.;
   Double_t right1sum = 0.;
  
   Int_t    addtlcuts = 0;
   Int_t    errorcuts = 0;

   Int_t    acc_skips = 0;

   Double_t accid0sum2= 0.;
   Double_t accid1sum2= 0.;

   nentries = trPatt.GetEntries();
   cout << "Total entries: " << nentries << endl;
   nbytes = 0;
   nb = 0;
   Int_t entry1 = 0;
   Int_t entry2 = 0;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      nb = trPatt.GetEntry(jentry);   nbytes += nb;

      if( error == 0  && bcmavg > bcmcutmin && bcmavg < bcmcutmax){

        //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
        //Sum holders
        coin0sum  += coin0;
        coin1sum  += coin1;
        accid0sum += accid0;
        accid1sum += accid1;
        accid0sum2+= accid0;
        accid1sum2+= accid1;
        bcm0sum   += bcm0;
        bcm1sum   += bcm1;
        left0sum  += left0;
        left1sum  += left1;

        entry1++;

        //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
        //Record the burst
        if(BURST > 0 && entry1%BURST == 0 ){
          //cout << "[" << (Int_t)(entry1/BURST)-1 << "]" << "Calculating the asymmetries..." << endl;

          Double_t RAW             = (coin1sum-coin0sum)/(coin1sum+coin0sum);
          Double_t RAW_BCM_NORM    = (coin1sum/bcm1sum-coin0sum/bcm0sum)/(coin1sum/bcm1sum+coin0sum/bcm0sum);
          Double_t CORR            = ( (coin1sum-accid1sum) - (coin0sum-accid0sum) ) / ( (coin1sum-accid1sum) + (coin0sum-accid0sum) );
          Double_t CORR_BCM_NORM   = ( (coin1sum-accid1sum)/bcm1sum - (coin0sum-accid0sum)/bcm0sum ) / ( (coin1sum-accid1sum)/bcm1sum + (coin0sum-accid0sum)/bcm0sum );

          //TODO: SHOULD WE BE NORMALIZING THE LEFT AND RIGHT RATES BY BCM? 
          Double_t DEADTIME_RATE_0 = left0sum * (freq / (patt / 2) ) / BURST;  //rate in hertz
          Double_t DEADTIME_RATE_1 = left1sum * (freq / (patt / 2) ) / BURST;  //rate in hertz

          Double_t DT_CORR_FAC_0   = pow(euler,-1*DEADTIME_RATE_0/1000*deadtimetau);   //needs kHz rate
          Double_t DT_CORR_FAC_1   = pow(euler,-1*DEADTIME_RATE_1/1000*deadtimetau);   //needs kHz rate

          Double_t DEADTIME_CORR   = ( (coin1sum)/DT_CORR_FAC_1 - (coin0sum)/DT_CORR_FAC_0 ) / ( (coin1sum)/DT_CORR_FAC_1 + (coin0sum)/DT_CORR_FAC_0 );
          Double_t FULLY_CORRECTED = ( (coin1sum-accid1sum)/DT_CORR_FAC_1/bcm1sum - (coin0sum-accid0sum)/DT_CORR_FAC_0/bcm0sum ) / ( (coin1sum-accid1sum)/DT_CORR_FAC_1/bcm1sum + (coin0sum-accid0sum)/DT_CORR_FAC_0/bcm0sum );

          Double_t LEFT_ASYM       = ( left1sum / bcm1sum - left0sum / bcm0sum ) / ( left1sum / bcm1sum + left0sum / bcm0sum );

          //Dealing with div/0 issue... Taking asym over whole run so this doens't matter anymore.
          Double_t ACCID_ASYM(0.);
          if( (accid1 + accid0) > 0 ) ACCID_ASYM = ( accid1sum / bcm1sum - accid0sum / bcm0sum ) / ( accid1sum / bcm1sum + accid0sum / bcm0sum );
          if( (accid1 + accid0) == 0) acc_skips++; //Need to adjust TGraph Entries.

          Double_t BCM_ASYM        = ( bcm1sum - bcm0sum ) / ( bcm1sum + bcm0sum );
   
          //FIXME: TRY NORMALIZE LEFT AND ACCIDENTAL BY THE BCM. DOES THIS CHANGE RESULTS? ==> Acc/Coin Rate would be unchanged anyway.
          Double_t LEFT_RATE       = ( ( DEADTIME_RATE_0 + DEADTIME_RATE_1 ) / 2. ) / ( bcm1sum + bcm0sum ) * ( bcm_mean*BURST ) ;
          Double_t ACCID_RATE      = ( ( accid1sum + accid0sum ) / patt * freq / ( bcm1sum + bcm0sum ) * ( bcm_mean*BURST ) ) / BURST;
          Double_t BCM_RATE        = ( ( bcm1sum + bcm0sum ) / patt * freq ) / BURST;
          Double_t COIN_RATE       = ( ( ( coin1sum + coin0sum ) / patt * freq / ( bcm1sum + bcm0sum ) * ( bcm_mean*BURST ) ) ) / BURST;

          gr_asym_uncorr->SetPoint( (Int_t)(entry1/BURST)-1 , time , RAW             );
          gr_asym_bcmnrm->SetPoint( (Int_t)(entry1/BURST)-1 , time , RAW_BCM_NORM    );
          gr_asym_acccor->SetPoint( (Int_t)(entry1/BURST)-1 , time , CORR            );
          gr_asym_cornor->SetPoint( (Int_t)(entry1/BURST)-1 , time , CORR_BCM_NORM   );
          gr_asym_dtcorr->SetPoint( (Int_t)(entry1/BURST)-1 , time , DEADTIME_CORR   );
          gr_asym_final->SetPoint(  (Int_t)(entry1/BURST)-1 , time , FULLY_CORRECTED );

          gr_asym_LEFT->SetPoint(   (Int_t)(entry1/BURST)-1 , time , LEFT_ASYM  );

          if( (accid1 + accid0) > 0 ) gr_asym_ACCID->SetPoint(  (Int_t)(entry1/BURST)-1-acc_skips , time , ACCID_ASYM );

          gr_asym_BCM->SetPoint(    (Int_t)(entry1/BURST)-1 , time , BCM_ASYM   );

          gr_rate_LEFT->SetPoint(   (Int_t)(entry1/BURST)-1 , time , LEFT_RATE  );
          gr_rate_ACCID->SetPoint(  (Int_t)(entry1/BURST)-1 , time , ACCID_RATE );
          gr_rate_BCM->SetPoint(    (Int_t)(entry1/BURST)-1 , time , BCM_RATE   );
          gr_rate_COIN->SetPoint(   (Int_t)(entry1/BURST)-1 , time , COIN_RATE  );

          coin0sum  = 0.;
          coin1sum  = 0.;
          accid0sum = 0.;
          accid1sum = 0.;
          bcm0sum   = 0.;
          bcm1sum   = 0.;
          left0sum  = 0.;
          left1sum  = 0.;

        }

     } else {

       if(error!=0) errorcuts++;
       if(error==0) addtlcuts++;
     }

   }

  cout << "Total error cuts: " << errorcuts << endl;
  cout << "Total additional cuts made: " << addtlcuts << endl;


  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  //FITTING AND PLOTTING
  Int_t width_allplots = 1500;
  Int_t height_ratio_g = 927;
  Int_t height_ratio_s = 621;
  Int_t height_ratio_b = 454;

  gStyle->SetOptFit(1111);

  TCanvas * Cgr_asym_uncorr = new TCanvas("Uncorr_Asym","Uncorr_Asym",width_allplots,height_ratio_b);
  gr_asym_uncorr->Draw("AP");
  gr_asym_uncorr->Fit("pol0");
  TF1 * fit_gr_asym_uncorr = gr_asym_uncorr->GetFunction("pol0");
  gr_asym_uncorr->GetXaxis()->SetNdivisions(505);
  gr_asym_uncorr->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_asym_uncorr->GetXaxis()->SetTimeDisplay(1);
  gr_asym_uncorr->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_asym_uncorr->GetXaxis()->SetLabelOffset(0.02);
  gr_asym_uncorr->Draw("AP");

  TCanvas * Cgr_asym_acccor = new TCanvas("Accid-Corr_Asym","Accid-Corr_Asym",width_allplots,height_ratio_b);
  gr_asym_acccor->Draw("AP");
  gr_asym_acccor->Fit("pol0");
  TF1 * fit_gr_asym_acccor = gr_asym_acccor->GetFunction("pol0");
  gr_asym_acccor->GetXaxis()->SetNdivisions(505);
  gr_asym_acccor->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_asym_acccor->GetXaxis()->SetTimeDisplay(1);
  gr_asym_acccor->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_asym_acccor->GetXaxis()->SetLabelOffset(0.02);
  gr_asym_acccor->Draw("AP");

  TCanvas * Cgr_asym_bcmnrm = new TCanvas("BCM-Norm_Asym","BCM-Norm_Asym",width_allplots,height_ratio_b);
  gr_asym_bcmnrm->Draw("AP");
  gr_asym_bcmnrm->Fit("pol0");
  TF1 * fit_gr_asym_bcmnrm = gr_asym_bcmnrm->GetFunction("pol0");
  gr_asym_bcmnrm->GetXaxis()->SetNdivisions(505);
  gr_asym_bcmnrm->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_asym_bcmnrm->GetXaxis()->SetTimeDisplay(1);
  gr_asym_bcmnrm->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_asym_bcmnrm->GetXaxis()->SetLabelOffset(0.02);
  gr_asym_bcmnrm->Draw("AP");

  TCanvas * Cgr_asym_cornor = new TCanvas("Accid-Corr_BCM-Norm_Asym","Accid-Corr_BCM-Norm_Asym",width_allplots,height_ratio_b);
  gr_asym_cornor->Draw("AP");
  gr_asym_cornor->Fit("pol0");
  TF1 * fit_gr_asym_cornor = gr_asym_cornor->GetFunction("pol0");
  gr_asym_cornor->GetXaxis()->SetNdivisions(505);
  gr_asym_cornor->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_asym_cornor->GetXaxis()->SetTimeDisplay(1);
  gr_asym_cornor->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_asym_cornor->GetXaxis()->SetLabelOffset(0.02);
  gr_asym_cornor->Draw("AP");

  TCanvas * Cgr_asym_dtcorr = new TCanvas("DT-Corr_Asym","DT-Corr_Asym",width_allplots,height_ratio_b);
  gr_asym_dtcorr->Draw("AP");
  gr_asym_dtcorr->Fit("pol0");
  TF1 * fit_gr_asym_dtcorr = gr_asym_dtcorr->GetFunction("pol0");
  gr_asym_dtcorr->GetXaxis()->SetNdivisions(505);
  gr_asym_dtcorr->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_asym_dtcorr->GetXaxis()->SetTimeDisplay(1);
  gr_asym_dtcorr->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_asym_dtcorr->GetXaxis()->SetLabelOffset(0.02);
  gr_asym_dtcorr->Draw("AP");

  TCanvas * Cgr_asym_final = new TCanvas("Final-Corrected-Asymmetry","Final-Corrected-Asymmetry",width_allplots,height_ratio_b);
  gr_asym_final->Draw("AP");
  gr_asym_final->Fit("pol0");
  TF1 * fit_gr_asym_final = gr_asym_final->GetFunction("pol0");
  gr_asym_final->GetXaxis()->SetNdivisions(505);
  gr_asym_final->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_asym_final->GetXaxis()->SetTimeDisplay(1);
  gr_asym_final->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_asym_final->GetXaxis()->SetLabelOffset(0.02);
  gr_asym_final->Draw("AP");

  TCanvas * Cgr_asym_ACCID = new TCanvas("Accidental-Asymmetry","Accidental-Asymmetry",width_allplots,height_ratio_b);
  gr_asym_ACCID->Draw("AP");
  gr_asym_ACCID->Fit("pol0");
  TF1 * fit_gr_asym_ACCID = gr_asym_ACCID->GetFunction("pol0");
  gr_asym_ACCID->GetXaxis()->SetNdivisions(505);
  gr_asym_ACCID->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_asym_ACCID->GetXaxis()->SetTimeDisplay(1);
  gr_asym_ACCID->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_asym_ACCID->GetXaxis()->SetLabelOffset(0.02);
  gr_asym_ACCID->Draw("AP");

  TCanvas * Cgr_asym_LEFT  = new TCanvas("Left-Asymmetry","Left-Asymmetry",width_allplots,height_ratio_b);
  gr_asym_LEFT->Draw("AP");
  gr_asym_LEFT->Fit("pol0");
  TF1 * fit_gr_asym_LEFT = gr_asym_LEFT->GetFunction("pol0");
  gr_asym_LEFT->GetXaxis()->SetNdivisions(505);
  gr_asym_LEFT->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_asym_LEFT->GetXaxis()->SetTimeDisplay(1);
  gr_asym_LEFT->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_asym_LEFT->GetXaxis()->SetLabelOffset(0.02);
  gr_asym_LEFT->Draw("AP");

  TCanvas * Cgr_asym_BCM   = new TCanvas("BCM-Asymmetry","BCM-Asymmetry",width_allplots,height_ratio_b);
  gr_asym_BCM->Draw("AP");
  gr_asym_BCM->Fit("pol0");
  TF1 * fit_gr_asym_BCM = gr_asym_BCM->GetFunction("pol0");
  gr_asym_BCM->GetXaxis()->SetNdivisions(505);
  gr_asym_BCM->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_asym_BCM->GetXaxis()->SetTimeDisplay(1);
  gr_asym_BCM->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_asym_BCM->GetXaxis()->SetLabelOffset(0.02);
  gr_asym_BCM->Draw("AP");

  TCanvas * Cgr_rate_left  = new TCanvas("Left-Rate","Left-Rate",width_allplots,height_ratio_b);
  gr_rate_LEFT->Draw("AP");
  gr_rate_LEFT->Fit("pol0");
  TF1 * fit_gr_rate_LEFT = gr_rate_LEFT->GetFunction("pol0");
  gr_rate_LEFT->GetXaxis()->SetNdivisions(505);
  gr_rate_LEFT->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_rate_LEFT->GetXaxis()->SetTimeDisplay(1);
  gr_rate_LEFT->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_rate_LEFT->GetXaxis()->SetLabelOffset(0.02);
  gr_rate_LEFT->Draw("AP");

  TCanvas * Cgr_rate_accid  = new TCanvas("Accid-Rate","Accid-Rate",width_allplots,height_ratio_b);
  gr_rate_ACCID->Draw("AP");
  gr_rate_ACCID->Fit("pol0");
  TF1 * fit_gr_rate_ACCID = gr_rate_ACCID->GetFunction("pol0");
  gr_rate_ACCID->GetXaxis()->SetNdivisions(505);
  gr_rate_ACCID->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_rate_ACCID->GetXaxis()->SetTimeDisplay(1);
  gr_rate_ACCID->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_rate_ACCID->GetXaxis()->SetLabelOffset(0.02);
  gr_rate_ACCID->Draw("AP");

  TCanvas * Cgr_rate_bcm  = new TCanvas("BCM-Rate","BCM-Rate",width_allplots,height_ratio_b);
  gr_rate_BCM->Draw("AP");
  gr_rate_BCM->Fit("pol0");
  TF1 * fit_gr_rate_BCM = gr_rate_BCM->GetFunction("pol0");
  gr_rate_BCM->GetXaxis()->SetNdivisions(505);
  gr_rate_BCM->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_rate_BCM->GetXaxis()->SetTimeDisplay(1);
  gr_rate_BCM->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_rate_BCM->GetXaxis()->SetLabelOffset(0.02);
  gr_rate_BCM->Draw("AP");

  TCanvas * Cgr_rate_coin  = new TCanvas("COIN-Rate","COIN-Rate",width_allplots,height_ratio_b);
  gr_rate_COIN->Draw("AP");
  gr_rate_COIN->Fit("pol0");
  TF1 * fit_gr_rate_COIN = gr_rate_COIN->GetFunction("pol0");
  gr_rate_COIN->GetXaxis()->SetNdivisions(505);
  gr_rate_COIN->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_rate_COIN->GetXaxis()->SetTimeDisplay(1);
  gr_rate_COIN->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_rate_COIN->GetXaxis()->SetLabelOffset(0.02);
  gr_rate_COIN->Draw("AP");




  Cgr_rate_left->SaveAs(   Form( "41__burst_analysis_%i_left_rate.png", RUNN) );

  Cgr_rate_accid->SaveAs(  Form( "43__burst_analysis_%i_accd_rate.png", RUNN) );
  Cgr_rate_coin->SaveAs(   Form( "44__burst_analysis_%i_coin_rate.png", RUNN) );
  Cgr_rate_bcm->SaveAs(    Form( "45__burst_analysis_%i_qbcm_rate.png", RUNN) );

  Cgr_asym_LEFT->SaveAs(   Form( "51__burst_analysis_%i_left_sing_asym.png", RUNN) );

  Cgr_asym_ACCID->SaveAs(  Form( "53__burst_analysis_%i_accidentl_asym.png", RUNN) );
  Cgr_asym_BCM->SaveAs(    Form( "54__burst_analysis_%i_charge_asymmet.png", RUNN) );

  Cgr_asym_uncorr->SaveAs( Form( "61__burst_analysis_%i_not_corrected_asym.png", RUNN) );
  Cgr_asym_acccor->SaveAs( Form( "62__burst_analysis_%i_acc_corrected_asym.png", RUNN) );
  Cgr_asym_bcmnrm->SaveAs( Form( "63__burst_analysis_%i_bcm_corrected_asym.png", RUNN) );
  Cgr_asym_cornor->SaveAs( Form( "64__burst_analysis_%i_acc_and_qcorr_asym.png", RUNN) );
  Cgr_asym_dtcorr->SaveAs( Form( "65__burst_analysis_%i_deadtime_corr_asym.png", RUNN) );
  Cgr_asym_final->SaveAs(  Form( "66__burst_analysis_%i_fullcorrected_asym.png", RUNN) );




  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // WRITE DATA STATS TO SQL DATABASE ON START ... THIS WAY RUN DATA IS INSERTED EVEN IF DATA IS BAD
  TString insert_query("");
  TString values_query("");
  TString clause_query("");

  Float_t unc      = fit_gr_asym_uncorr->GetParameter(0);
  Float_t unce     = fit_gr_asym_uncorr->GetParError(0);
  Float_t bcmn     = fit_gr_asym_bcmnrm->GetParameter(0);
  Float_t bcmne    = fit_gr_asym_bcmnrm->GetParError(0);
  Float_t acorr    = fit_gr_asym_acccor->GetParameter(0);
  Float_t acorre   = fit_gr_asym_acccor->GetParError(0);
  Float_t acornor  = fit_gr_asym_cornor->GetParameter(0);
  Float_t acornore = fit_gr_asym_cornor->GetParError(0);
  Float_t dtcor    = fit_gr_asym_dtcorr->GetParameter(0);
  Float_t dtcore   = fit_gr_asym_dtcorr->GetParError(0);
  Float_t full     = fit_gr_asym_final->GetParameter(0);
  Float_t fulle    = fit_gr_asym_final->GetParError(0);

  Float_t A_l = fit_gr_asym_LEFT->GetParameter(0);
  Float_t A_q = fit_gr_asym_BCM->GetParameter(0);
  Float_t A_a = fit_gr_asym_ACCID->GetParameter(0);
  Float_t R_l = fit_gr_rate_LEFT->GetParameter(0);
  Float_t R_a = fit_gr_rate_ACCID->GetParameter(0);
  Float_t R_q = fit_gr_rate_BCM->GetParameter(0);
  Float_t R_c = fit_gr_rate_COIN->GetParameter(0);

  Float_t A_a2 = (accid1sum2 - accid0sum2) / (accid1sum2 + accid0sum2);


  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  //SEND IT TO THE DATABASE TABLE
  if(writetosql){
    TString update("");
    TString set1("");
    TString set2("");
    TString set3("");
    TString set4("");
    TString set5("");
    TString set6("");
    TString where("");

    TSQLServer * ServerEnd = TSQLServer::Connect(db_host,db_user,db_pass);

    update = "UPDATE pcrex_run_asym_burst SET ";
    set1.Form("asym_raw=%0.8f,asym_raw_err=%0.8f,asym_acc=%0.8f,asym_acc_err=%0.8f,asym_bcm=%0.8f,asym_bcm_err=%0.8f,",unc,unce,acorr,acorre,bcmn,bcmne);
    set2.Form("asym_accbcm=%0.8f,asym_accbcm_err=%0.8f,asym_dt=%0.8f,asym_dt_err=%0.8f,asym_full=%0.8f,asym_full_err=%0.8f,",acornor,acornore,dtcor,dtcore,full,fulle);
    set3.Form("asym_Aa=%0.8f,asym_Aa2=%0.8f,asym_Al=%0.8f,asym_Aq=%0.8f,asym_Ra=%0.8f,asym_Rl=%0.8f,asym_Rq=%0.8f,asym_Rc=%0.8f ",A_a,A_a2,A_l,A_q,R_a,R_l,R_q,R_c);
    where.Form("WHERE id_asym=%d;",RUNN);

    TString finalquery = update + set1 + set2 + set3 + where;

    cout << "Database push: " << endl << "     " << finalquery << endl;
    TSQLResult * resultEnd = ServerEnd->Query(finalquery.Data());
    ServerEnd->Close();
  }

  return(100);
}
