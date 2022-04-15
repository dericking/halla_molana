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
#include <fstream>

/*
CREATE TABLE pcrex_run_burst_analysis (
  id_runburst INT NOT NULL,
  runburst_0_mean     FLOAT,
  runburst_1_mean     FLOAT,
  runburst_1_error    FLOAT,
  rubburst_2_mean     FLOAT,
  rubburst_2_error    FLOAT,
  runburst_5_mean     FLOAT,
  runburst_5_error    FLOAT,
  runburst_10_mean    FLOAT,
  runburst_10_error   FLOAT,
  runburst_25_mean    FLOAT,
  runburst_25_error   FLOAT,
  runburst_50_mean    FLOAT,
  runburst_50_error   FLOAT,
  runburst_100_mean   FLOAT,
  runburst_100_error  FLOAT,
  runburst_250_mean   FLOAT,
  runburst_250_error  FLOAT,
  PRIMARY KEY (id_runburst),
  FOREIGN KEY (id_runburst) REFERENCES moller_run(id_run)
);
*/

Bool_t isnonnumber(char c){
  return !(c >= '0' && c <= '9');
}

Int_t molana_run_burst_analysis(string FNAME, Double_t PTARG, Double_t ANPOW, Int_t BURST)
{
  if(BURST != 0 || BURST != 1 || BURST != 2 || BURST != 5 || BURST != 10 || BURST != 25 || BURST != 50 || BURST != 100 || BURST != 250){cout<<"BURST SIZED FINE..."<<endl;}else{cout << "BURST SIZE NOT ALLOWED";exit(0);}

  Bool_t writetosql = true;

  ///////////////////////////////////////////////////////////////  (╯°□°）╯︵ ┻━┻
  //STRIP RUN NUMBER FROM FILE NAME
  std::string fnametrun = FNAME.substr( FNAME.find_last_of("\\/")+1 );
  std::string fnamecopy = fnametrun;
  fnamecopy.erase(std::remove_if(fnamecopy.begin(),fnamecopy.end(),isnonnumber),fnamecopy.end());
  stringstream ss;
  ss << fnamecopy;
  Int_t RUNN;
  ss >> RUNN;

  ///////////////////////////////////////////////////////////////  (╯°□°）╯︵ ┻━┻
  //LOAD ROOT FILE
  TChain trPatt("trPatt");
  trPatt.Add( FNAME.c_str() );

  ///////////////////////////////////////////////////////////////  (╯°□°）╯︵ ┻━┻
  //CHECK FOR THE EXISTENCE OF ENVIRONMENTAL VARIABLES
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

   Int_t   bcmbin = 2000;
   Float_t bcmmin = 0;
   Float_t bcmmax = 2000;

   TH1F * hbcm1 = new TH1F("hbcm1",Form("Pattern BCM distribution for Prompted Errorless Events : RUN %d",RUNN),bcmbin,bcmmin,bcmmax);
   TH1F * hbcm2 = new TH1F("hbcm2",Form("Pattern BCM Distribution -- Cut for A_{q} Outliers : RUN %d",RUNN),bcmbin,bcmmin,bcmmax);

   Int_t    asymbin = 601;
   Double_t asymmin = -1.0;
   Double_t asymmax =  1.0;
   TH1F * haq1  = new TH1F("haq1",Form("Pattern Aq Distribution for Prompted Errorless Events : RUN %d",RUNN),asymbin,asymmin,asymmax);
   TH1F * haq2  = new TH1F("haq2",Form("Pattern Aq Distribution -- Cut for A_{q} Outliers : RUN %d",RUNN),asymbin,asymmin,asymmax);

   Float_t bcmstdev = 0;
   Float_t bcm_mean = 0;
   
   //HOW TO SIZE THESE... POINTS AREN'T EXACT... TIMES CHANGE... COULD BE GAPS...  Hmmm...

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

      //if(error != 0) cout << error << endl;

   }

   ///////////////////////////////////////////////
   //Compute Cuts
   bcmstdev = hbcm1->GetStdDev();
   bcm_mean = hbcm1->GetMean();
   Float_t aqstdev = haq1->GetStdDev();
   Float_t aq_mean = haq1->GetMean();

   cout << "StartUnix: " << unixtimemin << ", and EndUnix: " << unixtimemax << endl;

   cout << "Aq Mean: " << aq_mean << ", Aq StdDev: " << aqstdev << endl;
   cout << "Aq Cuts [" << aq_mean - 4*aqstdev << "," << aq_mean + 4*aqstdev << ")" << endl;

   cout << "Aq Mean: " << aq_mean << ", Aq StdDev: " << aqstdev << endl;
   cout << "Aq Cuts [" << aq_mean - 4*aqstdev << "," << aq_mean + 4*aqstdev << ")" << endl;

   Float_t bcmcutmin = bcm_mean - 4*bcmstdev;
   Float_t bcmcutmax = bcm_mean + 4*bcmstdev;

   Float_t aqcutmin = aq_mean - 5*aqstdev;
   Float_t aqcutmax = aq_mean + 5*aqstdev;


   //BURST LOOP HERE

   ///////////////////////////////////////////////
   //HISTOGRAMS AND GRAPHS ... LEAVE POLARIZATION FOR NOW EVEN THOUGH ONLY DOING ASYM
   //TGraph * gr_polzn_good = new TGraph();
   //  gr_polzn_good->SetTitle( Form("Polarization [Azz: %f, Ptarg: %f ] Over Time : GROUP %i;Local JLab Time;Polzn",ANPOW,PTARG,GROUP));
   //  gr_polzn_good->SetMarkerStyle(6);
   //TGraph * gr_polzn_badd = new TGraph();
   //  gr_polzn_badd->SetTitle( Form("Polarization [Azz: %f, Ptarg: %f ] : GROUP %i;Local JLab Time;Polzn",ANPOW,PTARG,GROUP));
   //  gr_polzn_badd->SetMarkerStyle(6);
   //
   //Double_t polmin = -8.;
   //Double_t polmax =  8.;
   //Int_t    polbin = 601;
   //
   //TH1F * h_polgaus = new TH1F("h_polgaus",Form("Polarization [Azz: %f, Ptarg: %f ] Distribution : GROUP %i;Polarization",ANPOW,PTARG,GROUP),polbin,polmin,polmax)
   ///
   TGraph * gr_asym_good = new TGraph();
     gr_asym_good->SetTitle( Form("Asymmetry Over Time : RUN %i with BURST SIZE %i;Local JLab Time; Asymmetry; Count",RUNN,BURST));
     gr_asym_good->SetMarkerStyle(6);
   TGraph * gr_asym_badd = new TGraph();
     gr_asym_badd->SetTitle( Form("Asymmetry Over Time : RUN %i with BURST SIZE %i;Local JLab Time; Asymmetry; Count",RUNN,BURST));
     gr_asym_badd->SetMarkerStyle(6);

   Int_t   asymbin2 =  240;
   Float_t asymmin2 = -0.4;
   Float_t asymmax2 =  0.4;

   TH1F * h_asym = new TH1F("h_asym",Form("Asymmetry Distribution : RUNN %i with BLOCK SIZE %i;Asymmetry;Count [N]",RUNN,BURST),asymbin2,asymmin2,asymmax2);
   ///

   Double_t coin0sum  = 0.;
   Double_t coin1sum  = 0.;
   Double_t accid0sum = 0.;
   Double_t accid1sum = 0.;
   Double_t bcm0sum   = 0.;
   Double_t bcm1sum   = 0.;

   Double_t alleventsasym = 0.;

   nentries = trPatt.GetEntries();
   cout << "Total entries: " << nentries << endl;
   nbytes = 0;
   nb = 0;
   Int_t entry1 = 0;
   Int_t entry2 = 0;

   ofstream output;
   output.open("output.csv");
   Bool_t printoutput = true;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      nb = trPatt.GetEntry(jentry);   nbytes += nb;

      //debugging -- can remove after error bug fixed.... error codes stop after 256 or eighth bit manipulation... ????
      if( error == 0 && (std::isnan(coinasym) || std::isinf(coinasym) ) ){
        cout << "!!! PROBLEM @ " << jentry << " for run " << run << " at index " << pattnumb << "| isnan: " << std::isnan(coinasym) << ", isinf: " << std::isinf(coinasym) << endl;
        error = -1;
        cout << "!!! Flagged as error for now. Error code set to " << error << endl;
      }

      if(error == 0 && bcmasym > aqcutmin && bcmasym < aqcutmax && bcmavg > bcmcutmin && bcmavg < bcmcutmax){

        if(printoutput) output << std::setw(12) << jentry << std::setw(12) << error << std::setw(12) << coin1 << std::setw(12) << coin0;

        Double_t molasym  = -999.;
        Double_t chgasym  = -999.;

        /////////////////////////////////////////////////////
        //Sum
        coin0sum  += coin0;
        coin1sum  += coin1;
        accid0sum += accid0;
        accid1sum += accid1;
        bcm0sum   += bcm0;
        bcm1sum   += bcm1;

        /////////////////////////////////////////////////////
        //Calculate block asymmetries... only using asym
        if(BURST > 0 && entry1%BURST == 0 && entry1 != 0){
          //cout << "[" << (Int_t)(entry1/BURST)-1 << "]" << "Calculating the asymmetries..." << endl;

          if(printoutput) output << std::setw(12) << coin1sum << std::setw(12) << coin0sum;

          coin0sum += -1*accid0sum;
          coin1sum += -1*accid1sum;
          coin0sum = coin0sum / bcm0sum;
          coin1sum = coin1sum / bcm1sum;

          molasym = (coin1sum - coin0sum) / (coin1sum + coin0sum);
          chgasym = (bcm1sum - bcm0sum) / (bcm0sum + bcm1sum);

          if(time > unixtimemax) cout << "ALERT! TIME PROBLEM!!!" << endl; //why do plots sometimes not scale to right dates

          gr_asym_good->SetPoint( (Int_t)(entry1/BURST)-1 , time , molasym );
         
          coin0sum = 0;
          coin1sum = 0;
          accid0sum = 0;
          accid1sum = 0;
          bcm0sum = 0;
          bcm1sum = 0;


        }

        if(printoutput) output << endl;
        entry1++;


      } else {
        entry2++;
      }

   }


        if(BURST == 0){
          coin0sum += -1*accid0sum;
          coin1sum += -1*accid1sum;
          coin0sum = coin0sum / bcm0sum;
          coin1sum = coin1sum / bcm1sum;

          alleventsasym = (coin1sum - coin0sum) / (coin1sum + coin0sum);

        }


  //////////////////////////////////////////////////////////  (╯°□°）╯︵ ┻━┻
  //FITTING AND PLOTTING
  Int_t cw = 1500;
  Int_t ch = 500;
  Float_t run_burst_asym;
  Float_t run_burst_error;
  Float_t run_burst_chi2ndf;

  if(BURST > 0){
    gStyle->SetOptFit(111);
    TCanvas * Cgr_asym_good = new TCanvas("Cgr_asym_good","Cgr_asym_good",cw,ch);
    gr_asym_good->Draw("AP");
    gr_asym_good->Fit("pol0");
    TF1 * fitgrasym = gr_asym_good->GetFunction("pol0");
    gr_asym_good->GetXaxis()->SetNdivisions(505);
    gr_asym_good->GetXaxis()->SetRangeUser(unixtimemin,unixtimemax);
    gr_asym_good->GetXaxis()->SetTimeDisplay(1);
    gr_asym_good->GetXaxis()->SetTimeFormat("#splitline{%y/%m/%d}{ %H:%M} %F1970-01-01 00:00:00");
    gr_asym_good->GetXaxis()->SetLabelOffset(0.02);
    gr_asym_good->Draw("AP");
    run_burst_asym     = fitgrasym->GetParameter(0);
    run_burst_error = fitgrasym->GetParError(0);
    run_burst_chi2ndf  = fitgrasym->GetChisquare() / (Float_t)fitgrasym->GetNDF();
  }
  if(BURST == 0){
    //AVERAGE ASYMMETRY USING ALL CYCLES
    run_burst_asym     = alleventsasym;
    run_burst_error    = 0.;
    run_burst_chi2ndf  = 0.;
  }

  cout << " BLOCK: " << BURST << endl
       << "  ASYM: " << run_burst_asym << endl
       << " ERROR: " << run_burst_error << endl
       << "X2/NDF: " << run_burst_chi2ndf << endl << endl;

  //////////////////////////////////////////////////////////  (╯°□°）╯︵ ┻━┻
  // WRITE DATA STATS TO SQL DATABASE ON START ... THIS WAY RUN DATA IS INSERTED EVEN IF DATA IS BAD
  TString insert_query("");
  TString values_query("");
  TString clause_query("");
  if(writetosql){
    cout << "molana_anaysis.C() ==> Writing prompt stats to " << db_host << "." << endl;
    TSQLServer * ServerEnd = TSQLServer::Connect(db_host,db_user,db_pass);

    insert_query = "INSERT INTO pcrex_run_burst_analysis ";
    if(BURST > 0) values_query.Form("(id_runburst,runburst_%i_mean,runburst_%i_error) VALUES (%i,%.8f, %.8f) ",BURST,BURST,RUNN,run_burst_asym,run_burst_error);
    if(BURST > 0)clause_query.Form("ON DUPLICATE KEY UPDATE runburst_%i_mean = %.8f, runburst_%i_error = %.8f;",BURST,run_burst_asym,BURST,run_burst_error);

    if(BURST == 0) values_query.Form("(id_runburst,runburst_%i_mean) VALUES (%i, %.8f) ",BURST,RUNN,run_burst_asym);
    if(BURST == 0)clause_query.Form("ON DUPLICATE KEY UPDATE runburst_%i_mean = %.8f; ",BURST,run_burst_asym);

    TString queryEnd     = insert_query + values_query + clause_query;

    cout << "Database query: " << endl << "     " << queryEnd << endl;
    TSQLResult * resultEnd = ServerEnd->Query(queryEnd.Data());
    ServerEnd->Close();
  }

  return(100);
}
