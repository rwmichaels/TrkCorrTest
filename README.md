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

Notes on the work flow

Use TrkHoles.C (separate directory) find the residuals.
Put them into ./holefiles/resid*

control the variable Which_Spectrometer (=L or R) in the macros.
control ixbeam (=0,1,2), same macros
AfileL(R).root is a link; make it point to the file corresponding
to ixbeam.

Run MoveHoles.C or MoveTracks.C to check residuals.

Question: How do we know that HrsTrkCorr is correct ?
Answer: Use root macros MoveHoles.C and MoveTracks.C to verify that
tracks lie on the holes for the sieve slit runs.
Also, some vetting of the class was done with QsqDistort.C



