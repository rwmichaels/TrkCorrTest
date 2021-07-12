
// trk_script2.C: ROOT script example.

// Do this in ROOT:
// root [0] .L HrsTrkCorr.C+
// root [1] .L trk_script2.C+
// root [2]  trk_simple()

#include "HrsTrkCorr.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TRandom.h"
#include <iostream>

using namespace std;

void trk_simple() {

   gROOT->Reset();
   gStyle->SetOptStat(0);
   gStyle->SetStatH(0.3);
   gStyle->SetStatW(0.3);
   gStyle->SetTitleH(0.09);
   gStyle->SetTitleW(0.3);
   gStyle->SetLabelSize(0.05,"x");
   gStyle->SetLabelSize(0.05,"y");
   gROOT->ForceStyle();
  
   HrsTrkCorr *trkcorr = new HrsTrkCorr(HrsTrkCorr::kLeft);  //or kRight

   trkcorr->Init();

   trkcorr->Print("check_data.txt");  // print data to file for test

   Int_t npt,kpt,idx,nhth,nhph,ngrth,ngrph;
   Float_t *xx,*yy1,*yy2,*ex,*ey;
   Float_t xmin,xmax,y1min,y1max,y2min,y2max;
   Float_t tgph,tgth;   
   TH2F *hth, *hph;
   vector<TGraphErrors* > grth,grph;
   char hname[80];
   char htitle[80];
   Int_t nmax=50;
   xx  = new Float_t[nmax];
   yy1 = new Float_t[nmax];
   yy2 = new Float_t[nmax];
   ex  = new Float_t[nmax];
   ey  = new Float_t[nmax];
   xmin=999;xmax=-999;y1min=999;y1max=-999;y2min=999;y2max=-999;

   TCanvas *c1 = new TCanvas;
   c1->Divide(1,2);
   hth = new TH2F("hth","tg_th Residuals for different X",105,25,130,100,-4,4);
   hph = new TH2F("hph","tg_ph Residuals for different X",105,25,130,100,-4,4);
   
   for (Int_t ixbeam=0; ixbeam<MAXXBEAM; ixbeam++) {

      for (Int_t i=0; i<nmax; i++) {
        xx[i]=999;yy1[i]=999;yy2[i]=999;ex[i]=999;ey[i]=999;
      }
      npt = 0; 
      for(Int_t icol=0; icol<MAXCOL; icol++) {
	for (Int_t irow=0; irow<MAXROW; irow++) {
	  idx = icol*MAXROW + irow;
          tgph = trkcorr->GetResidTgPh(ixbeam,icol,irow);
          tgth = trkcorr->GetResidTgTh(ixbeam,icol,irow);
// for defined residuals
          if (tgph != trkcorr->fNotFound && tgth != trkcorr->fNotFound) { 
  	      xx[npt]  = 1.0*idx;
	      yy1[npt] = tgth;
	      yy2[npt] = tgph;
	      ex[npt]  = 0.00001;
	      ey[npt]  = 0.2;  // mrad
	      npt++;
              if(npt > nmax) {
  		  cout << "need to increase nmax"<<endl;
		  exit(0);
	      }
	  }
	}
      }

      // finding a global min/max for the graph limits
      for (Int_t ii=0; ii<npt; ii++) {
         if (xx[ii]  < xmin)  xmin = xx[ii];
         if (xx[ii]  > xmax)  xmax = xx[ii];
         if (yy1[ii] < y1min) y1min = yy1[ii];
         if (yy1[ii] > y1max) y1max = yy1[ii];
         if (yy2[ii] < y2min) y2min = yy2[ii];
         if (yy2[ii] > y2max) y2max = yy2[ii];
      }
   
      cout << "beam "<<ixbeam<<endl;
      cout << "points "<<npt<<"  "<<xmin<<"  "<<xmax<<"  "<<y1min<<"  "<<y1max<<"  "<<y2min<<"  "<<y2max<<endl;

      grth.push_back(new TGraphErrors(nmax,xx,yy1,ex,ey));
      grph.push_back(new TGraphErrors(nmax,xx,yy2,ex,ey));
      ngrth=grth.size()-1;
      ngrph=grph.size()-1;

      grth[ngrth]->SetMarkerColor(2+ixbeam);
      grth[ngrth]->SetMarkerSize(1.2);
      grth[ngrth]->SetMarkerStyle(20);
      grph[ngrph]->SetMarkerColor(2+ixbeam);
      grph[ngrph]->SetMarkerSize(1.2);
      grph[ngrph]->SetMarkerStyle(20);

      c1->cd(1);
      if (ixbeam==0) hth->Draw();
      grth[ngrth]->Draw("P");

      c1->cd(2);
      if (ixbeam==0) hph->Draw();
      grph[ngrph]->Draw("P");

      
   } // ixbeam loop

   c1->cd(1);
   TLegend *leg1 = new TLegend(0.55,0.7,0.89,0.89);
   leg1->AddEntry(grth[0]," X = -3.2 mm","p");
   leg1->AddEntry(grth[1]," X = 0 ","p");
   leg1->AddEntry(grth[2]," X = +3.8 mm","p");
   leg1->Draw();

   c1->cd(2);
   TLegend *leg2 = new TLegend(0.55,0.7,0.89,0.89);
   leg2->AddEntry(grph[0]," X = -3.2 mm","p");
   leg2->AddEntry(grph[1]," X = 0 ","p");
   leg2->AddEntry(grph[2]," X = +3.8 mm","p");
   leg2->Draw();

		     
		      
		      
}




