
// trk_script1.C: ROOT script example.

// Do this in ROOT:
// root [0] .L HrsTrkCorr.C+
// root [1] .L trk_script4.C+
// root [2]  trk_simple()

#include "HrsTrkCorr.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TRandom.h"
#include <iostream>

using namespace std;

void trk_simple() {

   HrsTrkCorr *trkcorr = new HrsTrkCorr(HrsTrkCorr::kLeft);  //or kRight

   TH2F *h2 = new TH2F("h2","test",100,-0.04,0.04,100,-0.04,0.04);

   TCanvas *c1 = new TCanvas();
   
   trkcorr->Init();  

   TRandom *rnd = new TRandom();  

   for (Int_t ievt=0; ievt<500; ievt++) {

     Double_t xbeam = rnd->Uniform(-4,4);        // millimeters
     Double_t tg_ph = rnd->Uniform(-0.02,0.02);  // horizontal tan(angle)
     Double_t tg_th = rnd->Uniform(-0.03,0.03);  // vertical tan(angle)
     
     trkcorr->Load(xbeam, tg_ph, tg_th);

     h2->Fill(tg_ph,tg_th);
     
     cout << "corrected TRANSPORT angles, tg_ph = "<<trkcorr->GetCorrTgPh();
     cout << "   tg_th = "<<trkcorr->GetCorrTgTh()<<endl;
     
   }

   h2->Draw("box");
   
}




