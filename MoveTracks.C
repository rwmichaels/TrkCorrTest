// MoveTracks.C: Move the tracks to agree with the sieve holes.  A test.

// Need to link the root file, e.g.
//   ln -s ./rootfiles/prexLHRS_2240_siyu.root AfileL.root
//
// Do this in ROOT:
// root [0] .L HrsTrkCorr.C+
// root [1] .L MoveTracks.C+
// root [2]  move_tracks()

#include <limits>
#include "HrsTrkCorr.h"
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

void move_tracks() {

   gROOT->Reset();
   gStyle->SetOptStat(0);
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
   Int_t MAXEVENTS = 200000; // max num events
   Int_t numevts;  // = min of (MAXEVENTS, num entrees in Tree)
   Int_t verbose=0;
   Int_t movehole=0;  // normally 0
   Int_t movetrack=1;  // normally 1
   Int_t col,row,idx;
   Double_t dtreeth,dtreeph;
   Double_t dtgth,dtgph,residth,residph,delph,delth,newph,newth;
   Long64_t tent=0; 
   dtgth=0; dtgph=0; dtreeth=0; dtreeph=0;
   TH2F *hfh,*hftrk;
   
   string Which_Spectrometer="L";   // change from L to R  (left vs right HRS)

   Int_t ixbeam=2;  // choice of beam 0=(X=0)  1=(X=-3.2), 2=(X=+3.8)
   //                              run 2240       2241       2239
   //                                  21364     21365      21363

   Double_t Xbeam;
   
   HrsTrkCorr *trkcorr;

   if(Which_Spectrometer == "L") {
       trkcorr = new HrsTrkCorr(HrsTrkCorr::kLeft);  //or kRight
       hfh = new TH2F("hfh","Holes",100,-0.022,0.03,100,-0.047,0.047);
       hftrk = new TH2F("hftrk","",100,-0.022,0.03,100,-0.047,0.047);
   } else {
       trkcorr = new HrsTrkCorr(HrsTrkCorr::kRight);  
       hfh = new TH2F("hfh","Holes",100,-0.03,0.022,100,-0.047,0.047);
       hftrk = new TH2F("hftrk","",100,-0.03,0.022,100,-0.047,0.047);
   }     
   trkcorr->Init();
   trkcorr->Print("bob.txt");
   Xbeam = trkcorr->GetX(ixbeam);
   

   // Here we plot the holes
   for (col=0; col<MAXCOL; col++) {
     for (row=0; row<MAXROW; row++) {
          dtgph=trkcorr->GetHoleTgPh(ixbeam,col,row);
          dtgth=trkcorr->GetHoleTgTh(ixbeam,col,row);
          residph=trkcorr->GetResidTgPh(ixbeam,col,row);
          newph = dtgph + residph;
          residth=trkcorr->GetResidTgTh(ixbeam,col,row);
          newth = dtgth + residth;
          if (movehole) {
	     hfh->Fill(newph,newth);
	  } else {  // what we normally would do
  	     hfh->Fill(dtgph,dtgth);
	  }
     }
   }
   TCanvas *c1 = new TCanvas();
   hfh->SetMarkerStyle(20);
   hfh->SetMarkerColor(2);
   hfh->Draw();
   c1->Update();
   
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
 
    cout << "Var names to look for  "<<
                 ThetaVarName<<"   "<<PhiVarName<<endl;
  
    T->SetBranchAddress(ThetaVarName,&dtreeth);
    T->SetBranchAddress(PhiVarName,&dtreeph);


    numevts = (Int_t)T->GetEntries();
    if(numevts > MAXEVENTS) numevts = MAXEVENTS;
    cout << "number of events "<<numevts<<endl;
   
    for (Int_t ievt=0; ievt<numevts; ievt++) {

       if (ievt>0 && (ievt%1000)==0) cout << "event "<<ievt<<endl;
 
          tent = T->GetEntry(ievt);

          trkcorr->Load(Xbeam, dtreeph, dtreeth);

	  delph = trkcorr->GetDeltaTgPh();
	  delth = trkcorr->GetDeltaTgTh();

	  dtgph = dtreeph - delph;
	  dtgth = dtreeth - delth;

	  //	  cout << "vars "<<dtreeph<<"  "<<dtreeth<<"  "<<delph<<"  "<<delth<<endl;
	  if(dtreeph==0&dtreeth==0) {
	    // skip those.  not sure where they come from, but rare
	  } else {
            if (movetrack) {
   	       hftrk->Fill(dtgph,dtgth);
	    } else {
   	       hftrk->Fill(dtreeph,dtreeth);
	    }
	  }
 	    
     }  // event loop

     delete trkcorr;
     delete file1;

     TCanvas *c2 = new TCanvas();
     
     hftrk->Draw("");
     hfh->Draw("same");
     
     c2->Update();
   
  
} // end of macro




 

