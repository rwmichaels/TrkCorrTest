TrkCorrTest -- to test HrsTrkCorr
July 2021  Robert Michaels

You'll need to get HrsTrkCorr from the separate repository.

/rootfiles
   directory with root files
        sieve slit runs 2239,2240,2241,21363,21364,21365
	std Qsq runs 1983, 1984
The files you need are at JLab on the ifarm.
ifarm:/w/halla-scifs17exp/parity/disk1/bob/trkcorr_rootfiles

AfileL.root  AfileR.root
    usually a link
    
MoveHoles.C
    Test of HrsTrkCorr.  Read a sieve slit ROOT file and
    move the holes to agree with the data.
    
MoveTracks.C
    Test of HrsTrkCorr.  Read a sieve slit ROOT file and
    move the tracks to agree with the holes.

PlotResid.C
    Test of HrsTrkCorr.  Plot the residuals found

QsqDistort.C
    Read the ROOT file of a Qsq run and plot the Qsq
    with and without track distortions from HrsTrkCorr
    
trk_scriptN.C  N = 1, 2, ...
    Example of how to use HrsTrkCorr in a ROOT script.

holefiles:
   holes* = where the sieve holes are in tg_th vs tg_ph
   residuals* = residuals between track and holes

Question: How do we know that HrsTrkCorr is correct ?
Answer: Use root macros MoveHoles.C and MoveTracks.C to verify that
tracks lie on the holes for the sieve slit runs.
Also, some vetting of the class was done with QsqDistort.C

Notes on the work flow -- example on ifarm

git clone https://rwmichaels@github.com/rwmichaels/TrkCorrTest.git
 
   delete contents of rootfiles, make links.
   rootfiles -> /w/halla-scifs17exp/parity/disk1/bob/trkcorr_rootfiles

   For convenience, copy stuff here
      cp ../HrsTrkCorr/HrsTrkCorr.h .
      cp ../HrsTrkCorr/HrsTrkCorr.C .

   AfileL.root or AfileR.root is a link
   AfileR.root -> ./rootfiles/prexRHRS_21364_siyu.root

   Run MoveHoles.C, MoveTracks.C, or QsqDistort.C as per instructions
   at the top of those files.

  control the variable Which_Spectrometer (=L or R) in the macros.
  control ixbeam (=0,1,2), same macros
  AfileL(R).root is a link; make it point to the file corresponding
  to ixbeam.






