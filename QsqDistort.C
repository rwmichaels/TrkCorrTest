// QsqDistort.C: Distort Qsq and compare to undistorted
//
// R. Michaels, July 2021
//
// Need to link the root file, e.g.
//   ln -s prexLHRS_1984_-1.root AfileL.root
//
// Do this in ROOT:
// root [0] .L HrsTrkCorr.C+
// root [1] .L QsqDistort.C+
// root [2]  qsq_distort()
//
// Observations
// I tried to extract the X_beam from Y_tgt; to first order
// they are the same.  However, there is a problem:
// Y_tgt is fixed at zero by the matrix elements !
// So, instead I added a random X_beam the size of the raster.
// If the X_beam is rastered symmetrically about zero the average
// Qsq shift is small (< 0.3%).  That's good!  The residuals cancel.
// If I artificially restricted X_beam to 0 to +3mm I get an average
// <tg_ph> shift of about -0.4 mrad and Qsq increases by of order 1%
// The sign of the shift in Qsq depends on which HRS.
// Opposite : X raster 0 to -3mm I get an average <tg_ph> shift ~ +0.4 mrad
// and Qsq decreases by ~ -1 %  So, we are counting on cancellations of 1%.

#include <limits>
#include "HrsTrkCorr.h"
#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TRandom.h"
#include <iostream>

using namespace std;

void qsq_distort() {

   gROOT->Reset();
   gStyle->SetOptStat(1);
   gStyle->SetStatH(0.3);
   gStyle->SetStatW(0.3);
   gStyle->SetTitleH(0.09);
   gStyle->SetTitleW(0.3);
   gStyle->SetLabelSize(0.05,"x");
   gStyle->SetLabelSize(0.05,"y");
   gROOT->ForceStyle();

   char RootFile[80];
   char ThetaVarName[80];
   char PhiVarName[80];
   char QsqVarName[80];
   char MomVarName[80];
   Int_t MAXEVENTS = 600000; // max num events
   Int_t numevts;  // = min of (MAXEVENTS, num entrees in Tree)
   Int_t col,row,idx;
   Double_t dtreeth,dtreeph;
   Double_t dtrqsq,dtrmom;
   Double_t dtgth,dtgph,delth,delph;
   Double_t Ebeam = 0.95;
   Double_t theta0 = 4.74;
   Double_t th0rad,costheta1, costheta2, cos0,sin0;
   Double_t xsign = 1;
   Double_t qsqcalc1,qsqcalc2;
   Long64_t tent=0; 
   Double_t bigdelph,bigdelth;
   bigdelph = 0.0025;
   bigdelth = 0.0035;
   dtgth=0; dtgph=0; dtreeth=0; dtreeph=0;
   // acceptance cuts
   Double_t thcutlo = -0.05;
   Double_t thcuthi =  0.05;
   Double_t phcutlo = -0.025;
   Double_t phcuthi =  0.025;
   // Raster size
   Double_t rastersize = 3.0; // mm.  raster is +/- this.  nominally 3.
   // Raster on (1) or off (0)
   Int_t irast = 1;
   // Raster pattern assumption:  0: normal +/-.  -1 use minus X only, +1 +X.
   Int_t raster_pattern=0;
   Double_t rnd1,rsign;   
   
   string Which_Spectrometer="R";   // change from L to R  (left vs right HRS)
   
   th0rad = theta0 * 3.1415926/180.;
   cos0 = TMath::Cos(th0rad);
   sin0 = TMath::Sin(th0rad);

   Double_t Xbeam = 0;
   
   HrsTrkCorr *trkcorr;

   if(Which_Spectrometer == "L") {
       xsign = -1; // sign convention of tg_ph
       trkcorr = new HrsTrkCorr(HrsTrkCorr::kLeft); 
   } else {
       xsign = 1;
       trkcorr = new HrsTrkCorr(HrsTrkCorr::kRight);
   }     
   trkcorr->Init();
   trkcorr->Print("bob.txt");

   TH1F *hqsq0 = new TH1F("hqsq0","Qsq from EK",200,0.0035,0.0125);
   TH1F *hqsq1 = new TH1F("hqsq1","Qsq from tracks",200,0.0035,0.0125);
   TH1F *hqsq2 = new TH1F("hqsq2","Qsq from skewed tracks",200,0.0035,0.0125);
   TH1F *havg_th = new TH1F("havg_th","Average tg_th correction",100,-1.2*bigdelth,1.2*bigdelth);
   TH1F *havg_ph = new TH1F("havg_ph","Average tg_ph correction",100,-1.2*bigdelph,1.2*bigdelph);
   TH1F *hrast = new TH1F("hrast","Raster beam (mm)",100,-7,7);
   
   
   TRandom *rnd  = new TRandom();

   // ROOT file with Podd variables.
   sprintf(RootFile,"Afile%s.root",Which_Spectrometer.c_str());

   cout << "Rootfile "<<RootFile <<endl;
  
   TFile *file1 = TFile::Open(RootFile);

   TTree *T = (TTree *)file1->Get("T");

   if (T) {
         cout << "Found tree "<<endl;
    } else {
         cout << "No tree found ! "<<endl;
         exit(0);
    }

    sprintf(ThetaVarName, "%s.tr.tg_th", Which_Spectrometer.c_str());
    sprintf(PhiVarName, "%s.tr.tg_ph", Which_Spectrometer.c_str());
    sprintf(QsqVarName, "EK_%s.Q2", Which_Spectrometer.c_str());
    sprintf(MomVarName, "%s.gold.p", Which_Spectrometer.c_str());
    
    cout << "Var names to look for  "<<
          ThetaVarName<<"   "<<PhiVarName<<"  "<<
          QsqVarName<<"   "<<MomVarName<<endl;
  
    T->SetBranchAddress(ThetaVarName,&dtreeth);
    T->SetBranchAddress(PhiVarName,&dtreeph);
    T->SetBranchAddress(QsqVarName,&dtrqsq);
    T->SetBranchAddress(MomVarName,&dtrmom);

    numevts = (Int_t)T->GetEntries()-1;
    if(numevts > MAXEVENTS) numevts = MAXEVENTS;
    cout << "number of events "<<numevts<<endl;
   
    for (Int_t ievt=0; ievt<numevts; ievt++) {

       if (ievt>0 && (ievt%1000)==0) cout << "event "<<ievt<<endl;
 
          tent = T->GetEntry(ievt);

	  if(irast) {  // raster the beam a bit
	    Xbeam = rnd->Uniform(rastersize);
            rnd1 = rnd->Uniform(1.0);
	    rsign = 1.0;
            if(raster_pattern == -1) rsign=-1.0;
	    if(raster_pattern == 0) {
	      if(rnd1>0.5) rsign = -1.0;
	    }
	    Xbeam = rsign * Xbeam;
	  } else {
	    Xbeam = 0;
	  }

          hrast->Fill(Xbeam);
	  
	  //	  cout << "vars "<<dtreeph<<"  "<<dtreeth<<"  "<<dtrqsq<<"  "<<dtrmom<<"  "<<Xbeam<<endl;

          trkcorr->Load(Xbeam, dtreeph, dtreeth);

	  delph = trkcorr->GetDeltaTgPh();
	  delth = trkcorr->GetDeltaTgTh();
 // subtract deltas to move tracks to holes.
 // but add deltas to move ideal tracks to data.
	  dtgph = dtreeph - delph;
	  dtgth = dtreeth - delth;

// Check for huge deltas
  	  if (delph < -1.0*bigdelph || delph > bigdelph ||
	      delth < -1.0*bigdelth || delth > bigdelth ) {
	      printf("Large delta ? %f %f \n",delph,delth); // should not happen !
	  }
	      
	  if(dtreeph==0&dtreeth==0) {
	    // skip those.  not sure where they come from, but rare
	  } else {

   // cuts on angles to define acceptance
	    if(dtreeph > phcutlo && dtreeph < phcuthi &&
	       dtreeth > thcutlo && dtreeth < thcuthi) {

              havg_ph->Fill(trkcorr->GetDeltaTgPh());
              havg_th->Fill(trkcorr->GetDeltaTgTh());

	      hqsq0->Fill(dtrqsq);

  // Note: the sign "xsign" is negative (positive) on L-HRS (R-HRS). Sign convention !
	      
	      costheta1 = (cos0 + xsign* sin0*dtreeph)/TMath::Sqrt(1 + dtreeth*dtreeth + dtreeph*dtreeph);
	      qsqcalc1 = 2*Ebeam*dtrmom*(1.0 - costheta1);

              hqsq1->Fill(qsqcalc1);

	      costheta2 = (cos0 + xsign* sin0*dtgph)/TMath::Sqrt(1 + dtgth*dtgth + dtgph*dtgph);
	      qsqcalc2 = 2*Ebeam*dtrmom*(1.0 - costheta2);
            
              hqsq2->Fill(qsqcalc2);

	    }
	  }
 	    
    }  // event loop

     delete trkcorr;
     delete file1;

     
     //     hqsq0->SetLineColor(2);
     hqsq1->SetLineColor(2);
     hqsq2->SetLineColor(4);

     TCanvas *cc1 = new TCanvas();

      cc1->Divide(1,2);
     //     cc1->cd(1);
     //     hqsq0->Draw();
     cc1->cd(1);
     hqsq1->Draw();
     cc1->cd(2);
     hqsq2->Draw("");
       
     cc1->Update();

     TCanvas *c2 = new TCanvas();
     hrast->Draw();
     
  
} // end of macro




 

