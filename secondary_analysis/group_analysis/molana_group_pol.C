#include <TH2.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TObjString.h>
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
#include <TLegend.h>

Int_t molana_group_pol(TString runnumbers, Int_t GROUP, Double_t PTARG, Double_t ANPOW)
{
  Bool_t writetosql = true;

  const Int_t    BURST = 10;
  const Double_t EULER = 2.71828182845904;//RIP JACOB BERNOULLI
  const Double_t DTTAU = 0.00001572;


  ///////////////////////////////////////////////////////////////  (╯°□°）╯︵ ┻━┻
  //CHECK FOR THE EXISTENCE OF ENVIRONMENTAL VARIABLES - FROM DON'S CODE
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

  TString db_host;
  TString db_user;
  TString db_pass;

  if(writetosql){
    db_host = Form("mysql://%s/%s",gSystem->Getenv("MOLANA_DB_HOST"),gSystem->Getenv("MOLANA_DB_NAME"));
    db_user = Form("%s",gSystem->Getenv("MOLANA_DB_USER"));
    db_pass = Form("%s",gSystem->Getenv("MOLANA_DB_PASS"));
  }


  ///////////////////////////////////////////////////////////////  (╯°□°）╯︵ ┻━┻
  //LOAD ROOT FILE
  TChain trPatt("trPatt");

  TObjArray * tx = runnumbers.Tokenize(",");
  cout << "Runs passed: " << tx->GetEntries() << endl;
  Int_t last_run(0);
  for (Int_t i = 0; i < tx->GetEntries(); i++){
    cout << "i: " << i << endl;
    if(i == tx->GetEntries()) break;
    std::stringstream ss;
    ss << ((TObjString *)(tx->At(i)))->String();
    Int_t run = atoi( ss.str().c_str() );
    last_run = run;
    cout << "Look for: " << Form("%s/molana_patterns_%i.root",gSystem->Getenv("MOLLER_ROOTFILE_DIR"),run) << endl;
    trPatt.Add( Form("%s/molana_patterns_%i.root",gSystem->Getenv("MOLLER_ROOTFILE_DIR"),run)  );
  }

  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // QUERY DB FOR FREQ AND PATTERN BASED ON LAST RUN
  TSQLServer* server = TSQLServer::Connect(db_host,db_user,db_pass);

  TString select_query = "SELECT set_hel_freq, set_hel_pattern ";
  TString from_query   = "FROM moller_settings ";
  TString where_query  = Form("WHERE id_set_run = %i ;", last_run);
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
  for(Int_t i = 0; i < numrows; i++){
    for(Int_t j = 0; j < numcols; j++){
      TString value = row->GetField(j);
      TString field = result->GetFieldName(j);
      if( field=="set_hel_freq"){ FREQ = value; }
      if( field=="set_hel_pattern"){ PATT = value; }
    }
  }
  Int_t freq = FREQ.Atoi();

  Int_t patt(0);
  if(PATT.Contains("Quartet")) patt = 4;
  if(PATT.Contains("Octet"  )) patt = 8;

  cout << "Settings pulled for run #" << last_run << ": freq(" << freq << "), patt(" << patt << ")" << endl;


  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // ROOTFILE CRAP
  Double_t        coin0(0.0);
  Double_t        coin1(0.0);
  Double_t        coinavg(0.0);
  Double_t        coinasymuncorr(0.0);
  Double_t        coinasym(0.0);
  Double_t        left0(0.0);
  Double_t        left1(0.0);
  Double_t        leftavg(0.0);
  Double_t        leftasym(0.0);
  Double_t        right0(0.0);
  Double_t        right1(0.0);
  Double_t        rightavg(0.0);
  Double_t        rightasym(0.0);
  Double_t        accid0(0.0);
  Double_t        accid1(0.0);
  Double_t        accidavg(0.0);
  Double_t        accidasym(0.0);
  Double_t        bcm0(0.0);
  Double_t        bcm1(0.0);
  Double_t        bcmavg(0.0);
  Double_t        bcmasym(0.0);
  Int_t           time(0);
  Int_t           pattnumb(0);
  Int_t           pattsign(0);
  Int_t           error(0);
  Int_t           run(0);
  Int_t           runincrindex(0);

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
 
  trPatt.SetBranchAddress("time",           &time,      &b_time);
  trPatt.SetBranchAddress("pattnumb",       &pattnumb,  &b_pattnumb);
  trPatt.SetBranchAddress("pattsign",       &pattsign,  &b_pattsign);
  trPatt.SetBranchAddress("error",          &error,     &b_error);
  trPatt.SetBranchAddress("run",            &run,       &b_run);
  trPatt.SetBranchAddress("runincrindex",   &runincrindex, &b_runincrindex);

  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // FIRST PASS OVER -- FIND START & END ; COMPUTE FINAL CUTS RANGE
  Int_t   bcmbin = 2500;
  Float_t bcmmin = 0;
  Float_t bcmmax = 2500;

  TH1F * hbcm1 = new TH1F("hbcm1",Form("Pattern-Level BCM Sum Distribution for Prompted Errorless Events : Group %d",GROUP),bcmbin,bcmmin,bcmmax);
  TH1F * hbcm2 = new TH1F("hbcm2",Form("Pattern-Level BCM Sum Distribution -- Cut for BCM > 4#sigma Outliers : Group %d",GROUP),bcmbin,bcmmin,bcmmax);

  Int_t    asymbin = 601;
  Double_t asymmin = -0.8;
  Double_t asymmax =  0.8;
  TH1F * haq1  = new TH1F("haq1",Form("Pattern-Level Aq Distribution for Prompted Errorless Events : Group %d",GROUP),asymbin,asymmin,asymmax);
  TH1F * haq2  = new TH1F("haq2",Form("Pattern-Level Aq Distribution -- Cut for BCM > 4#sigma Outliers : Group %d",GROUP),asymbin,asymmin,asymmax);

  Float_t bcmstdev = 0;
  Float_t bcm_mean = 0;
   
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
  //DEFINE HISTOGRAMS AND TGRAPHS TO BE OUTPUT WITH GROUP ANALYSIS
  TH2F * h_polrzn = new TH2F("h_polrzn",Form("Pattern-Level Polarization Candle Plot : GROUP %i;Local JLab Time; Polarization",GROUP),200,unixtimemin,unixtimemax,801,-8.,8.);
   
  TGraph * gr_polzn_good = new TGraph();
    gr_polzn_good->SetTitle( Form("Pattern-Level Polarization [Azz: %f, Ptarg: %f ] Over Time : GROUP %i;Local JLab Time;Polzn",ANPOW,PTARG,GROUP));
    gr_polzn_good->SetMarkerStyle(6);

  TGraph * gr_polzn_badd = new TGraph();
    gr_polzn_badd->SetTitle( Form("Pattern-Level Polarization [Azz: %f, Ptarg: %f ] : GROUP %i;Local JLab Time;Polzn",ANPOW,PTARG,GROUP));
    gr_polzn_badd->SetMarkerStyle(6);

  Double_t polmin = -8.;
  Double_t polmax =  8.;
  Int_t    polbin = 601;
  TH1F * h_polgaus = new TH1F("h_polgaus",Form("Pattern-Level Polarization [Azz: %f, Ptarg: %f ] Distribution : GROUP %i;Polarization",ANPOW,PTARG,GROUP),601,-8.,8.);

  TGraph * gr_qasym = new TGraph();
    gr_qasym->SetTitle( Form("Pattern-Level Charge Asymmetry Over Time : GROUP %i;Local JLab Time;Charge Asymmetry",GROUP));
    gr_qasym->SetMarkerStyle(6);

  TH1F * h_qasym = new TH1F("h_qasym",Form("Pattern-Level Charge Asymmetry Distribution : GROUP %i;Charge Asymmetry",GROUP),asymbin,asymmin,asymmax);
  TH2F * h_qasym2d = new TH2F("h_qasym2d",Form("Pattern-Level Charge Asymmetry Candle Plot : GROUP %i;Local JLab Time; Charge Asymmetry",GROUP),200,unixtimemin,unixtimemax,901,-1.,1.);

  TGraph * gr_asym = new TGraph();
    gr_asym->SetTitle( Form("Pattern-Level Asymmetry Over Time : GROUP %i;Local JLab Time; Asymmetry; Count",GROUP));
    gr_asym->SetMarkerStyle(6);

  Int_t   asymbin2 = 360;
  Float_t asymmin2 = -0.4;
  Float_t asymmax2 = 0.4;

  TH1F * h_asym = new TH1F("h_asym",Form("Pattern-Level Acc/DT/Q Corrected Moller Asymmetry Distribution : GROUP %i;Asymmetry;Count [N]",GROUP),asymbin2,asymmin2,asymmax2);

  TH2F * h_asym2d = new TH2F("h_asym2d",Form("Pattern-Level Asymmetry Candle Plot: GROUP %i;Local JLab Time; Asymmetry",GROUP),200,unixtimemin,unixtimemax,901,-1.,1.);

  TMultiGraph * mgraph = new TMultiGraph;
  mgraph->SetTitle(Form("BCM Over Time for Group %d [Good and Cut Events] -- TEST FOR DON -- Reduce to 5 cats (Adj'cy, Hel, Beam, Scaler, Clock); Local JLab Time; BCM Rate",GROUP));
  TGraph * gr_bcm_good = new TGraph();
    gr_bcm_good->SetTitle( Form("Pattern-Level BCM (Good Events) : GROUP %i;Local JLab Time;Polzn",GROUP));
    gr_bcm_good->SetMarkerStyle(7);
    gr_bcm_good->SetMarkerStyle(kBlack);
  TGraph * gr_bcm_badd_adj = new TGraph();
    gr_bcm_badd_adj->SetTitle( "Adjacency Cut" );
    gr_bcm_badd_adj->SetMarkerStyle(7);
    gr_bcm_badd_adj->SetMarkerColor(kOrange+1);
    gr_bcm_badd_adj->SetFillColor(kOrange+1);
  TGraph * gr_bcm_badd_hel = new TGraph();
    gr_bcm_badd_hel->SetTitle( "Helicity (EC 1-3)" );
    gr_bcm_badd_hel->SetMarkerStyle(7);
    gr_bcm_badd_hel->SetMarkerColor(kGreen+2);
    gr_bcm_badd_hel->SetFillColor(kGreen+2);
  TGraph * gr_bcm_badd_bcm = new TGraph();
    gr_bcm_badd_bcm->SetTitle( "Beam (EC 4)" );
    gr_bcm_badd_bcm->SetMarkerStyle(7);
    gr_bcm_badd_bcm->SetMarkerColor(kRed);
    gr_bcm_badd_bcm->SetFillColor(kRed);
  TGraph * gr_bcm_badd_sc1 = new TGraph();
    gr_bcm_badd_sc1->SetTitle( "PLU1 Scaler (EC 5-9)" );
    gr_bcm_badd_sc1->SetMarkerStyle(7);
    gr_bcm_badd_sc1->SetMarkerColor(kBlue);
    gr_bcm_badd_sc1->SetFillColor(kBlue);
  TGraph * gr_bcm_badd_sc2 = new TGraph();
    gr_bcm_badd_sc2->SetTitle( "PLU2 !~ PLU1 (EC 10-11)" );
    gr_bcm_badd_sc2->SetMarkerStyle(7);
    gr_bcm_badd_sc2->SetMarkerColor(kCyan);
    gr_bcm_badd_sc2->SetFillColor(kCyan);
  TGraph * gr_bcm_badd_clk = new TGraph();
    gr_bcm_badd_clk->SetTitle( "CLOCK EC(12)" );
    gr_bcm_badd_clk->SetMarkerStyle(5);
    gr_bcm_badd_clk->SetMarkerColor(kViolet+9);
    gr_bcm_badd_clk->SetFillColor(kViolet+9);


  TH1D * h_asym_burst = new TH1D("h_asym_burst",Form("Burst-Level Acc/DT/Q Corrected Moller Asymmetry Distribution : GROUP %i",GROUP),(Int_t)(asymbin2),asymmin2,asymmax2 );
  TH1D * h_qasym_block = new TH1D("h_qasym_block",Form("Burst-Level Charge Asymmetry Distribution : GROUP %i",GROUP),(Int_t)(asymbin2),asymmin,asymmax );

  TGraph * gr_asym_block = new TGraph();
  gr_asym_block->SetTitle( Form("Burst-Level Fully-Corrected Acc/Q/DT Asymmetry Over Time : GROUP %i;Local JLab Time; Asymmetry; Count",GROUP));
  gr_asym_block->SetMarkerStyle(6);

  TGraph * gr_acc_asym_block = new TGraph();
  gr_acc_asym_block->SetTitle( Form("Burst-Level Accidental Asymmetry Over Time : GROUP %i;Local JLab Time; Asymmetry; Count",GROUP));
  gr_acc_asym_block->SetMarkerStyle(6);


  Double_t coin0sum(0.);
  Double_t coin1sum(0.);
  Double_t accid0sum(0.);
  Double_t accid1sum(0.);
  Double_t bcm0sum(0.);
  Double_t bcm1sum(0.);
  Double_t left0sum(0.);
  Double_t left1sum(0.);
  Double_t right0sum(0.);
  Double_t right1sum(0.);

  Int_t accidskips(0);
  Int_t entry1(0);
  Int_t entry2_1(0);
  Int_t entry2_2(0);
  Int_t entry2_3(0);
  Int_t entry2_4(0);
  Int_t entry2_5(0);
  Int_t entry2_6(0);

  nentries = trPatt.GetEntries();
  cout << "Total entries: " << nentries << endl;
  nbytes = 0;
  nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
      nb = trPatt.GetEntry(jentry);   nbytes += nb;

      //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
      // THIS IS NOW OBSOLETE ... WATCH TO MAKE SURE NO MORE PROLEMS ARE FLAGGED
      if( error == 0 && (std::isnan(coinasym) || std::isinf(coinasym) ) ){
        cout << "!!! PROBLEM @ " << jentry << " for run " << run << " at index " << pattnumb << "| isnan: " << std::isnan(coinasym) << ", isinf: " << std::isinf(coinasym) << endl;
        error = -1;
        cout << "!!! Flagged as error for now. Error code set to " << error << endl;
        exit(-137);
      }

      if(error == 0 && bcmavg > bcmcutmin && bcmavg < bcmcutmax){

        //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
        // FILL OUT THE BY-PATTERN GROUP PLOTS FIRST
        Double_t DEADTIME_RATE_0 = left0 * (freq / (patt / 2) );  //rate in hertz
        Double_t DEADTIME_RATE_1 = left1 * (freq / (patt / 2) );  //rate in hertz

        Double_t DT_CORR_FAC_0   = pow(EULER,-1*DEADTIME_RATE_0/1000*DTTAU);   //needs kHz rate
        Double_t DT_CORR_FAC_1   = pow(EULER,-1*DEADTIME_RATE_1/1000*DTTAU);   //needs kHz rate

        Double_t chgasym = (bcm1 - bcm0) / (bcm0 + bcm1);

        Double_t fully_corrected_coin_1 = (coin1-accid1)/DT_CORR_FAC_1/bcm1;
        Double_t fully_corrected_coin_0 = (coin0-accid0)/DT_CORR_FAC_0/bcm0;

        Double_t fully_corrected_asym   = (fully_corrected_coin_1 - fully_corrected_coin_0) / (fully_corrected_coin_1 + fully_corrected_coin_0);

        h_polrzn->Fill(time,fully_corrected_asym / PTARG / ANPOW);
        gr_polzn_good->SetPoint(entry1,time,fully_corrected_asym / PTARG / ANPOW);
        gr_bcm_good->SetPoint(entry1,time,bcmavg);
        h_polgaus->Fill(fully_corrected_asym / PTARG / ANPOW);
        gr_qasym->SetPoint(entry1,time,chgasym);
        h_qasym->Fill(chgasym);
        h_qasym2d->Fill(time,chgasym);
        gr_asym->SetPoint(entry1,time,fully_corrected_asym);
        h_asym->Fill(fully_corrected_asym);
        h_asym2d->Fill(time,fully_corrected_asym);
        hbcm2->Fill(bcmavg);
        haq2->Fill(chgasym);


        //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
        // START OUT BURST SUMMATIONS -- THESE WILL RESET TO ZERO WHEN BURST SIZE ACHIEVED
        coin0sum  += coin0;
        coin1sum  += coin1;
        accid0sum += accid0;
        accid1sum += accid1;
        bcm0sum   += bcm0;
        bcm1sum   += bcm1;
        left0sum  += left0;
        left1sum  += left1;

        entry1++;

        //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
        // ONCE WE HIT PROPER NUMBER OF ENTRIES FOR BLOCK SIZE
        if(entry1%BURST == 0 && entry1 != 1){
      
          Double_t DEADTIME_RATE_BURST_0 = left0sum * (freq / (patt / 2) ) / BURST;  //rate in hertz
          Double_t DEADTIME_RATE_BURST_1 = left1sum * (freq / (patt / 2) ) / BURST;  //rate in hertz

          Double_t DT_CORR_FAC_BURST_0   = pow(EULER,-1*DEADTIME_RATE_BURST_0/1000*DTTAU);   //needs kHz rate
          Double_t DT_CORR_FAC_BURST_1   = pow(EULER,-1*DEADTIME_RATE_BURST_1/1000*DTTAU);   //needs kHz rate

          Double_t fully_corrected_coinsum_1 = (coin1sum-accid1sum)/DT_CORR_FAC_BURST_1/bcm1sum;
          Double_t fully_corrected_coinsum_0 = (coin0sum-accid0sum)/DT_CORR_FAC_BURST_0/bcm0sum;

          Double_t fully_corrected_burst_asym = (fully_corrected_coinsum_1 - fully_corrected_coinsum_0) / (fully_corrected_coinsum_1 + fully_corrected_coinsum_0);

          Double_t chgasym_burst = (bcm1sum - bcm0sum) / (bcm0sum + bcm1sum);

          Double_t accasym = 0;
          if( (accid1sum + accid0sum) != 0 ){ 
            accasym = (accid1sum - accid0sum) / (accid1sum + accid0sum);
          } else {
            accidskips++;
          }

          h_asym_burst->Fill( fully_corrected_burst_asym );
          h_qasym_block->Fill( chgasym_burst );
          gr_asym_block->SetPoint( (Int_t)(entry1/BURST)-1 , time , fully_corrected_burst_asym );

          if( (accid1sum + accid0sum) != 0 ) gr_acc_asym_block->SetPoint( (Int_t)(entry1/BURST)-1-accidskips , time , accasym );
         
          
          //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
          // FLUSH THE SUMS
          coin0sum  = 0;
          coin1sum  = 0;
          accid0sum = 0;
          accid1sum = 0;
          bcm0sum   = 0;
          bcm1sum   = 0;
          left0sum  = 0;
          left1sum  = 0;
        }

      } else {
        //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
        // FILL ERROR EVENTS HERE - LET'S COLOR THIS UP FOR DR. JONES
        //gr_polzn_badd->SetPoint(entry2,time,coinasym / PTARG / ANPOW); 
	if( error == -1 )
          { gr_bcm_badd_adj->SetPoint(entry2_1,time,bcmavg); entry2_1++; }
	if( floor(log2(error)) == 0 || floor(log2(error)) == 1 || floor(log2(error)) == 2 )
          { gr_bcm_badd_hel->SetPoint(entry2_2,time,bcmavg); entry2_2++; }
	if( floor(log2(error)) == 3)
          { gr_bcm_badd_bcm->SetPoint(entry2_3,time,bcmavg); entry2_3++; }
	if( floor(log2(error)) >= 4 && floor(log2(error)) <= 8 )
          { gr_bcm_badd_sc1->SetPoint(entry2_4,time,bcmavg); entry2_4++; }
	if( floor(log2(error)) == 9 || floor(log2(error)) == 10 )
          { gr_bcm_badd_sc2->SetPoint(entry2_5,time,bcmavg); entry2_5++; }
	if( floor(log2(error)) == 11)
          { gr_bcm_badd_clk->SetPoint(entry2_6,time,bcmavg); entry2_6++; }

      }

   }

  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // START THE PLOTTING HERE
  gStyle->SetOptFit(111);

  Int_t cw = 1500;
  Int_t ch = 500;

  Int_t bufferzone = 15;

  TCanvas * Ch_polzn = new TCanvas("Ch_polzn","Ch_polzn",cw,ch);
  h_polrzn->Draw("candle1");
  h_polrzn->GetXaxis()->SetNdivisions(505);
  h_polrzn->GetXaxis()->SetTimeDisplay(1);
  h_polrzn->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M%p} %F1970-01-01 00:00:00");
  h_polrzn->GetXaxis()->SetLabelOffset(0.02);

  TCanvas * Cgr_polzn_good = new TCanvas("Cgr_polzn_good","Cgr_polzn_good",cw,ch);
  gr_polzn_good->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_polzn_good->Draw("AP");
  gr_polzn_good->Fit("pol0");
  TF1 * fitgrpolr = gr_polzn_good->GetFunction("pol0");
  gr_polzn_good->GetXaxis()->SetNdivisions(505);
  gr_polzn_good->GetXaxis()->SetTimeDisplay(1);
  gr_polzn_good->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_polzn_good->GetXaxis()->SetLabelOffset(0.02);
  gr_polzn_good->Draw("AP");

  TCanvas * Ch_polgaus = new TCanvas("Ch_polgaus","Ch_polgaus",cw,ch);
  //h_polgaus->Draw();
  h_polgaus->Fit("gaus");
  TF1 * fitpolgaus = h_polgaus->GetFunction("gaus");
  h_polgaus->GetXaxis()->SetRangeUser( (((Double_t)polmax-(Double_t)polmin)/(Double_t)polbin*(Double_t)h_polgaus->FindFirstBinAbove(0,1)*(1-bufferzone)+(Double_t)polmin) ,
                                     (  ((Double_t)polmax-(Double_t)polmin)/(Double_t)polbin*(Double_t)h_polgaus->FindLastBinAbove (0,1)*(1+bufferzone)+(Double_t)polmin) );
  h_polgaus->Draw();

  TCanvas * Cgr_qasym = new TCanvas("Cgr_qasym","Cgr_qasym",cw,ch);
  gr_qasym->Draw("AP");
  gr_qasym->Fit("pol0");
  TF1 * fitgrqasym = gr_qasym->GetFunction("pol0");
  gr_qasym->GetXaxis()->SetNdivisions(505);
  gr_qasym->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_qasym->GetXaxis()->SetTimeDisplay(1);
  gr_qasym->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_qasym->GetXaxis()->SetLabelOffset(0.02);
  gr_qasym->Draw("AP");

  TCanvas * Ch_qasym2d = new TCanvas("Ch_qasym2d","Ch_qasym2d",cw,ch);
  h_qasym2d->GetYaxis()->SetRangeUser(-0.05,0.05);
  h_qasym2d->GetXaxis()->SetNdivisions(505);
  h_qasym2d->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  h_qasym2d->GetXaxis()->SetTimeDisplay(1);
  h_qasym2d->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  h_qasym2d->GetXaxis()->SetLabelOffset(0.02);
  h_qasym2d->Draw("candle");

  TCanvas * Cgr_asym = new TCanvas("Cgr_asym","Cgr_asym",cw,ch);
  gr_asym->Draw("AP");
  gr_asym->Fit("pol0");
  TF1 * fitgrasym = gr_asym->GetFunction("pol0");
  gr_asym->GetXaxis()->SetNdivisions(505);
  gr_asym->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_asym->GetXaxis()->SetTimeDisplay(1);
  gr_asym->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_asym->GetXaxis()->SetLabelOffset(0.02);
  gr_asym->Draw("AP");


  TCanvas * Ch_asyms = new TCanvas("Ch_asyms","Ch_asyms",cw,ch);
  Ch_asyms->Divide(2,1);
  Ch_asyms->cd(1);
  h_asym->Draw();
  h_asym->Fit("gaus");
  TF1 * fitasymgaus = h_asym->GetFunction("gaus");
  h_asym->Draw();
  Ch_asyms->cd(2);
  h_qasym->Draw();
  h_qasym->GetXaxis()->SetRangeUser( (((Double_t)asymmax-(Double_t)asymmin)/(Double_t)asymbin*(Double_t)h_qasym->FindFirstBinAbove(0,1)*(1-bufferzone)+(Double_t)asymmin) ,
                                   (  ((Double_t)asymmax-(Double_t)asymmin)/(Double_t)asymbin*(Double_t)h_qasym->FindLastBinAbove (0,1)*(1+bufferzone)+(Double_t)asymmin) );
  h_qasym->Draw();


  TCanvas * Ch_asym2d = new TCanvas("Ch_asym2d","Ch_asym2d",cw,ch);
  h_asym2d->GetYaxis()->SetRangeUser(-0.2,0.2);
  h_asym2d->GetXaxis()->SetNdivisions(505);
  h_asym2d->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  h_asym2d->GetXaxis()->SetTimeDisplay(1);
  h_asym2d->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  h_asym2d->GetXaxis()->SetLabelOffset(0.02);
  h_asym2d->Draw("candle");

  TCanvas * Cgr_bcmtime = new TCanvas("Cgr_bcmtime","Cgr_bcmtime",cw,ch);
  TF1 * fitgrbcmgood = gr_bcm_good->GetFunction("pol0");
  TLegend * leg_bcmtime = new TLegend(0.88,0.20,0.99,0.80);
  leg_bcmtime->AddEntry(gr_bcm_badd_adj);
  leg_bcmtime->AddEntry(gr_bcm_badd_hel);
  leg_bcmtime->AddEntry(gr_bcm_badd_bcm);
  leg_bcmtime->AddEntry(gr_bcm_badd_sc1);
  leg_bcmtime->AddEntry(gr_bcm_badd_sc2);
  leg_bcmtime->AddEntry(gr_bcm_badd_clk);
  mgraph->Add(gr_bcm_good);
  mgraph->Add(gr_bcm_badd_adj);
  mgraph->Add(gr_bcm_badd_hel);
  mgraph->Add(gr_bcm_badd_bcm);
  mgraph->Add(gr_bcm_badd_sc1);
  mgraph->Add(gr_bcm_badd_sc2);
  mgraph->Add(gr_bcm_badd_clk);
  mgraph->GetXaxis()->SetNdivisions(505);
  mgraph->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  mgraph->GetXaxis()->SetTimeDisplay(1);
  mgraph->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  mgraph->GetXaxis()->SetLabelOffset(0.02);
  mgraph->Draw("AP");
  leg_bcmtime->Draw("SAME");
  
  TCanvas * Cbcm = new TCanvas("Cbcm","Cbcm",cw,ch);
  Cbcm->Divide(2,1);
  Cbcm->cd(1)->SetLogy(1);
  hbcm1->Draw("HIST");
  hbcm1->GetXaxis()->SetRangeUser( (((Double_t)bcmmax-(Double_t)bcmmin)/(Double_t)bcmbin*(Double_t)hbcm1->FindFirstBinAbove(0,1)*(1-bufferzone)+(Double_t)bcmmin) ,
                                 (  ((Double_t)bcmmax-(Double_t)bcmmin)/(Double_t)bcmbin*(Double_t)hbcm1->FindLastBinAbove (0,1)*(1+bufferzone)+(Double_t)bcmmin) );
  hbcm1->Draw("HIST");
  Cbcm->cd(2)->SetLogy(1);
  hbcm2->Draw("HIST");
  hbcm2->GetXaxis()->SetRangeUser( (((Double_t)bcmmax-(Double_t)bcmmin)/(Double_t)bcmbin*(Double_t)hbcm2->FindFirstBinAbove(0,1)*(1-bufferzone)+(Double_t)bcmmin) ,
                                 (  ((Double_t)bcmmax-(Double_t)bcmmin)/(Double_t)bcmbin*(Double_t)hbcm2->FindLastBinAbove (0,1)*(1+bufferzone)+(Double_t)bcmmin) );
  hbcm2->Draw("HIST");

  TCanvas * Caq = new TCanvas("Caq","Caq",cw,ch);
  Caq->Divide(2,1);
  Caq->cd(1)->SetLogy(1);
  haq1->Draw();
  haq1->GetXaxis()->SetRangeUser( (((Double_t)asymmax-(Double_t)asymmin)/(Double_t)asymbin*(Double_t)haq1->FindFirstBinAbove(0,1)*(1-bufferzone)+(Double_t)asymmin) ,
                                (  ((Double_t)asymmax-(Double_t)asymmin)/(Double_t)asymbin*(Double_t)haq1->FindLastBinAbove (0,1)*(1+bufferzone)+(Double_t)asymmin) );
  haq1->Draw();
  Caq->cd(2)->SetLogy(1);
  haq2->Draw();
  haq2->GetXaxis()->SetRangeUser( (((Double_t)asymmax-(Double_t)asymmin)/(Double_t)asymbin*(Double_t)haq2->FindFirstBinAbove(0,1)*(1-bufferzone)+(Double_t)asymmin) ,
                                (  ((Double_t)asymmax-(Double_t)asymmin)/(Double_t)asymbin*(Double_t)haq2->FindLastBinAbove (0,1)*(1+bufferzone)+(Double_t)asymmin) );
  haq2->Draw();


  TCanvas * C100 = new TCanvas("C100","C100",cw,ch);
  C100->Divide(2,1);
  C100->cd(1);
  h_asym_burst->Draw();
  h_asym_burst->GetXaxis()->SetRangeUser( (((Double_t)asymmax-(Double_t)asymmin)/(Double_t)asymbin*(Double_t)h_asym_burst->FindFirstBinAbove(0,1)*(1-bufferzone)+(Double_t)asymmin) ,
                                (  ((Double_t)asymmax-(Double_t)asymmin)/(Double_t)asymbin*(Double_t)h_asym_burst->FindLastBinAbove (0,1)*(1+bufferzone)+(Double_t)asymmin) );
  h_asym_burst->Fit("gaus");
  TF1 * fitasymblkgaus = h_asym_burst->GetFunction("gaus");
  haq2->GetXaxis()->SetRangeUser( (((Double_t)asymmax-(Double_t)asymmin)/(Double_t)asymbin*(Double_t)haq2->FindFirstBinAbove(0,1)*(1-bufferzone)+(Double_t)asymmin) ,
                                (  ((Double_t)asymmax-(Double_t)asymmin)/(Double_t)asymbin*(Double_t)haq2->FindLastBinAbove (0,1)*(1+bufferzone)+(Double_t)asymmin) );
  h_asym_burst->Draw();
  C100->cd(2);
  h_qasym_block->Draw();

  /*
  ///////////////////////////// THESIS PLOT EXAMPLE
  TCanvas * CThesis = new TCanvas("CThesis","CThesis",cw,ch);
  CThesis->Divide(2,1);
  CThesis->cd(1);
  h_asym->Draw();
  h_asym->Fit("gaus");
  h_asym->GetXaxis()->SetRangeUser( -0.2 , 0.4 );
  TF1 * fitasymgaus = h_asym->GetFunction("gaus");
  h_asym->Draw();
  CThesis->cd(2);
  h_asym_burst->Fit("gaus");
  h_asym_burst->Draw();
  h_asym_burst->GetXaxis()->SetRangeUser( -0.2 , 0.2 );
  h_asym_burst->Draw();

  CThesis->Print( Form("CThesis_Group_%i.pdf",GROUP) );
  */

  TCanvas * Cgr_asym_block = new TCanvas("Cgr_qasym_block","Cgr_qasym_block",cw,ch);
  gr_asym_block->Draw("AP");
  gr_asym_block->Fit("pol0");
  TF1 * fitgrasymblock = gr_asym_block->GetFunction("pol0");
  gr_asym_block->GetXaxis()->SetNdivisions(505);
  gr_asym_block->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_asym_block->GetXaxis()->SetTimeDisplay(1);
  gr_asym_block->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_asym_block->GetXaxis()->SetLabelOffset(0.02);
  gr_asym_block->Draw("AP");

  TCanvas * Cgr_acc_asym_blk = new TCanvas("Cgr_acc_asym_blk","Cgr_acc_asym_blk",cw,ch);
  gr_acc_asym_block->Draw("AP");
  gr_acc_asym_block->Fit("pol0");
  TF1 * fitgraccasymblock = gr_acc_asym_block->GetFunction("pol0");
  gr_acc_asym_block->GetXaxis()->SetNdivisions(505);
  gr_acc_asym_block->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
  gr_acc_asym_block->GetXaxis()->SetTimeDisplay(1);
  gr_acc_asym_block->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
  gr_acc_asym_block->GetXaxis()->SetLabelOffset(0.02);
  gr_acc_asym_block->Draw("AP");



  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // START SAVING IMAGES HERE ... RESAVE HISTS AS LOGY() AS WELL... 
  Cbcm->SaveAs(            Form("00_Group-%d__pattren-BC-distribution-pre-and-post-cut.png",GROUP) );
  Caq->SaveAs(             Form("01_Group-%d__pattern-AQ-distribution-pre-and-post-cut.png",GROUP) );
  Cgr_bcmtime->SaveAs(     Form("02_Group-%d__pattern-BCM-over-time-both-good-and-bads.png",GROUP) );
  Cgr_asym->SaveAs(        Form("03_Group-%d__pattern-graph-of-corrasym-across-allruns.png",GROUP) );
  Ch_asyms->SaveAs(        Form("04_Group-%d__pattern-corr_moller_asym_and_charge_asym.png",GROUP) );
  Ch_asym2d->SaveAs(       Form("05_Group-%d__pattern-candle-plot-corrasym-all-grp-run.png",GROUP) );
  Cgr_qasym->SaveAs(       Form("06_Group-%d__pattern-graph-of-charge-asymm-allgrpruns.png",GROUP) );
  Ch_qasym2d->SaveAs(      Form("07_Group-%d__pattern-candle-plot-of-charge-asymmetery.png",GROUP) );
  C100->SaveAs(            Form("10_Group-%d__burst-moller-corr-asymm-and-charge-asymm.png",GROUP) );
  
  C100->cd(1)->SetLogy();
  C100->cd(2)->SetLogy();
  C100->SaveAs(            Form("11_Group-%d__burst-moller-corr-asymm-and-q-asymm-logy.png",GROUP) );

  Cgr_asym_block->SaveAs(  Form("12_Group-%d__burst-moller-corrected-asymm-over-allrun.png",GROUP) );
  Cgr_acc_asym_blk->SaveAs(Form("13_Group-%d__burst-accidental-asymmetry-over-all-runs.png",GROUP) );



  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // PICK OUT FIT DATA AND PASS TO DATABASE
  Float_t group_pol      = fitgrpolr->GetParameter(0);
  Float_t group_pol_err  = fitgrpolr->GetParError(0);

  Float_t group_asym     = fitgrasym->GetParameter(0);
  Float_t group_asym_err = fitgrasym->GetParError(0);

  cout << "Pattern-level asymm: " << group_asym << " +/- " << group_asym_err << endl;
  
  Float_t group_asymgaus          = fitasymgaus->GetParameter(1);
  Float_t group_asymgaus_err      = fitasymgaus->GetParError(1);
  Float_t group_asymgaus_chi2ndf  = fitasymgaus->GetChisquare() / (Float_t)fitasymgaus->GetNDF();

  Float_t group_polgaus           = fitpolgaus->GetParameter(1);
  Float_t group_polgaus_err       = fitpolgaus->GetParError(1);
  Float_t group_polgaus_chi2ndf   = fitpolgaus->GetChisquare() / (Float_t)fitasymgaus->GetNDF();
  
  Float_t group_block_asym        = fitgrasymblock->GetParameter(0);
  Float_t group_block_asym_err    = fitgrasymblock->GetParError(0);

  cout << "Burst-level asymm: " << group_block_asym << " +/- " << group_block_asym_err << endl << endl;

  Float_t group_block_asymgaus          = fitasymblkgaus->GetParameter(1);
  Float_t group_block_asymgaus_err      = fitasymblkgaus->GetParError(1);
  Float_t group_block_asymgaus_chi2ndf  = fitasymblkgaus->GetChisquare() / (Float_t)fitasymblkgaus->GetNDF();

  //////////////////////////////////////////////////////////  ⠂⠂⠂⠂⠂◘⠨⠨⠨⠨⠨⠨⠨⠨
  // WRITE DATA STATS TO SQL DATABASE ON START ... THIS WAY RUN DATA IS INSERTED EVEN IF DATA IS BAD
  if(writetosql){
    cout << "molana_anaysis.C() ==> Writing prompt stats to " << db_host << "." << endl;
    TSQLServer * ServerEnd = TSQLServer::Connect(db_host,db_user,db_pass);
    TString sqlEndUpdate1 = Form("UPDATE pcrex_groups SET group_asym = %.8f, group_asym_err = %.8f, ",group_asym,group_asym_err);
    TString sqlEndUpdate2 = Form("group_asymgaus = %.8f, group_asymgaus_err = %.8f, group_asymgaus_chi2ndf = %.8f, ",group_asymgaus,group_asymgaus_err,group_asymgaus_chi2ndf);
    TString sqlEndUpdate4 = Form("group_block_asymgauss = %.8f, group_block_asymgauss_err = %.8f, group_block_asymgauss_chi2ndf = %.8f,",group_block_asymgaus,group_block_asymgaus_err,group_block_asymgaus_chi2ndf);
    TString sqlEndUpdate5 = Form("group_block_asym = %.8f, group_block_asym_err = %.8f, ",group_block_asym,group_block_asym_err);
    TString sqlEndUpdate6 = Form("group_anpow = %.8f, group_targpol = %.8f, group_dttau = %.8f WHERE id_group = %d;",ANPOW,PTARG,DTTAU,GROUP);
    
    TString sqlEndUpdate = sqlEndUpdate1 + sqlEndUpdate2 + sqlEndUpdate4 + sqlEndUpdate5 + sqlEndUpdate6;
    TSQLResult * resultEnd = ServerEnd->Query(sqlEndUpdate.Data());
    ServerEnd->Close();
  }

  return(100);
}
