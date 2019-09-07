// Include files 

#include <iostream>
#include <iomanip>
#include "cstdlib"
#include "math.h"

// local
#include "MdcAlignAlg/Millepede.h"
//#include "include/MdcCosParams.h"	// added by wulh on 06/08/28

//-----------------------------------------------------------------------------
// Implementation file for class : Millepede
//
// 2005-07-29 : Sebastien Viret
//-----------------------------------------------------------------------------


using namespace std;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Millepede::Millepede()
{}
//=============================================================================
// Destructor
//=============================================================================
Millepede::~Millepede() {} 

//=============================================================================


/*
  ------------------------------------------------------
  INITMILLE: first initialization of Millepede
  this part is sub-detector independant
  ------------------------------------------------------




  ------------------------------------------------------
*/

bool Millepede::InitMille(bool DOF[], double Sigm[], int nglo
			  , int nloc, double startfact, int nstd 
			  , double res_cut, double res_cut_init)
{

     cout << "                                           " << endl;
     cout << "            * o o                   o      " << endl;
     cout << "              o o                   o      " << endl;
     cout << "   o ooooo  o o o  oo  ooo   oo   ooo  oo  " << endl;
     cout << "    o  o  o o o o o  o o  o o  o o  o o  o " << endl;
     cout << "    o  o  o o o o oooo o  o oooo o  o oooo " << endl;
     cout << "    o  o  o o o o o    ooo  o    o  o o    " << endl;
     cout << "    o  o  o o o o  oo  o     oo   ooo  oo  ++ starts" << endl;	   
     cout << "                                           " << endl;

     if (debug_mode) cout << "" << endl;
     if (debug_mode) cout << "----------------------------------------------------" << endl;
     if (debug_mode) cout << "" << endl;
     if (debug_mode) cout << "    Entering InitMille" << endl;
     if (debug_mode) cout << "" << endl;
     if (debug_mode) cout << "-----------------------------------------------------" << endl;
     if (debug_mode) cout << "" << endl;

     ncs = 0;
     loctot  = 0;                        // Total number of local fits
     locrej  = 0;                        // Total number of local fits rejected
     cfactref  = 1.0;                    // Reference value for Chi^2/ndof cut

     Millepede::SetTrackNumber(0);       // Number of local fits (starts at 0)

     m_residual_cut = res_cut;
     m_residual_cut_init = res_cut_init; 

     //   nagb	  = 6*nglo;    // Number of global derivatives
     nagb    = 3*nglo;		// modified by wulh
     nalc	  = nloc;       // Number of local derivatives
     nstdev  = nstd;     // Number of StDev for local fit chisquare cut

     if (debug_mode) cout << "Number of global parameters   : " << nagb << endl;
     if (debug_mode) cout << "Number of local parameters    : " << nalc << endl;
     if (debug_mode) cout << "Number of standard deviations : " << nstdev << endl;

     if (nagb>mglobl || nalc>mlocal)
     {
	  if (debug_mode) cout << "Two many parameters !!!!!" << endl;
	  return false;
     }

     // Global parameters initializations

     for (int i=0; i<nagb; i++)
     {
	  bgvec[i]=0.;
	  pparm[i]=0.;
	  dparm[i]=0.;
	  psigm[i]=-1.;
	  indnz[i]=-1;
	  indbk[i]=-1;
	  nlnpa[i]=0;

	  for (int j=0; j<nagb;j++)
	  {
	       cgmat[i][j]=0.;
	  }
     }

     // Local parameters initializations

     for (int i=0; i<nalc; i++)
     {
	  blvec[i]=0.;

	  for (int j=0; j<nalc;j++)
	  {
	       clmat[i][j]=0.;
	  }
     }

     // Then we fix all parameters...

     for (int j=0; j<nagb; j++)  {Millepede::ParSig(j,0.0);}

     // ...and we allow them to move if requested

     //   for (int i=0; i<3; i++)
     for (int i=0; i<3; i++)	// modified by wulh on 06/08/27
     {
	  if (verbose_mode) cout << "GetDOF(" << i << ")= " << DOF[i] << endl;

	  if (DOF[i]) 
	  {
	       for (int j=i*nglo; j<(i+1)*nglo; j++) 
	       {Millepede::ParSig(j,Sigm[i]);}
	  }
     }

     // Activate iterations (if requested)

     itert   = 0;	// By default iterations are turned off
     cfactr = 500.0;
     if (m_iteration) Millepede::InitUn(startfact);          

     arest.clear();  // Number of stored parameters when doing local fit
     arenl.clear();  // Is it linear or not?
     indst.clear(); 

     storeind.clear();
     storeare.clear();
     storenl.clear();
     storeplace.clear();

     if (debug_mode) cout << "" << endl;
     if (debug_mode) cout << "----------------------------------------------------" << endl;
     if (debug_mode) cout << "" << endl;
     if (debug_mode) cout << "    InitMille has been successfully called!" << endl;
     if (debug_mode) cout << "" << endl;
     if (debug_mode) cout << "-----------------------------------------------------" << endl;
     if (debug_mode) cout << "" << endl;

     return true;
}


/*
  -----------------------------------------------------------
  PARGLO: initialization of global parameters
  -----------------------------------------------------------

  index    = the index of the global parameter in the 
  result array (equivalent to dparm[]).

  param    = the starting value

  -----------------------------------------------------------
*/

bool Millepede::ParGlo(int index, double param)
{
     if (index<0 || index>=nagb)
     {return false;}
     else
     {pparm[index] = param;}

     return true;
}


/*
  -----------------------------------------------------------
  PARSIG: define a constraint for a single global param
  param is 'encouraged' to vary within [-sigma;sigma] 
  range
  -----------------------------------------------------------

  index    = the index of the global parameter in the 
  result array (equivalent to dparm[]).

  sigma	   = value of the constraint (sigma <= 0. will 
  mean that parameter is FIXED !!!) 

  -----------------------------------------------------------
*/

bool Millepede::ParSig(int index, double sigma)
{
     if (index>=nagb) 
     {return false;}
     else
     {psigm[index] = sigma;}

     return true;
}


/*
  -----------------------------------------------------------
  INITUN: unit for iteration
  -----------------------------------------------------------

  cutfac is used by Fitloc to define the Chi^2/ndof cut value

  A large cutfac value enables to take a wider range of tracks 
  for first iterations, which might be useful if misalignments
  are large.

  As soon as cutfac differs from 0 iteration are requested.
  cutfac is then reduced, from one iteration to the other,
  and iterations are stopped when it reaches the value 1.

  At least one more iteration is often needed in order to remove
  tracks containing outliers.

  -----------------------------------------------------------
*/

bool Millepede::InitUn(double cutfac)
{
     cfactr = std::max(1.0, cutfac);

     cout << "Initial cut factor is  " << cfactr << endl;
     itert = 1; // Initializes the iteration process
     return true;
}

/*
  -----------------------------------------------------------
  CONSTF: define a constraint equation in Millepede
  -----------------------------------------------------------

  dercs    = the row containing constraint equation 
  derivatives (put into the final matrix)

  rhs      = the lagrange multiplier value (sum of equation)	     

  -----------------------------------------------------------
*/

bool Millepede::ConstF(double dercs[], double rhs)
{  
     if (ncs>=mcs) // mcs is defined in Millepede.h
     {
	  cout << "Too many constraints !!!" << endl;
	  return false;
     }

     for (int i=0; i<nagb; i++) {adercs[ncs][i] = dercs[i];}

     arhs[ncs] = rhs;
     ncs++ ;
     cout << "Number of constraints increased to " << ncs << endl;
     return true;
}


/*
  -----------------------------------------------------------
  EQULOC: write ONE equation in the matrices
  -----------------------------------------------------------

  dergb[1..nagb]	= global parameters derivatives
  derlc[1..nalc] 	= local parameters derivatives
  rmeas  		= measured value
  sigma 		= error on measured value (nothin to do with ParSig!!!)

  -----------------------------------------------------------
*/

bool Millepede::EquLoc(double dergb[], double derlc[], double dernl[], double rmeas, double sigma)
{	

     if (sigma<=0.0) // If parameter is fixed, then no equation
     {
	  for (int i=0; i<nalc; i++)
	  {
	       derlc[i] = 0.0;
	  }
	  for (int i=0; i<nagb; i++)
	  {
	       dergb[i] = 0.0;
	  }
	  return true;
     }

     // Serious equation, initialize parameters

     double wght =  1.0/(sigma*sigma);
     int nonzer  =  0;
     int ialc    = -1;
     int iblc    = -1;
     int iagb    = -1;
     int ibgb    = -1;

     for (int i=0; i<nalc; i++) // Retrieve local param interesting indices
     {
	  if (derlc[i]!=0.0)
	  {
	       nonzer++;
	       if (ialc == -1) ialc=i;	// first index
	       iblc = i;       	     	// last index
	  }
     }

     if (verbose_mode) cout << ialc << " / " << iblc << endl;

     for (int i=0; i<nagb; i++)  // Idem for global parameters
     {
	  if (dergb[i]!=0.0)
	  {
	       nonzer++;
	       if (iagb == -1) iagb=i;	// first index
	       ibgb = i; 	     	// last index
	  }
     }

     if (verbose_mode) cout << iagb << " / " << ibgb << endl;

     indst.push_back(-1);
     arest.push_back(rmeas);
     arenl.push_back(0.);

     for (int i=ialc; i<=iblc; i++)
     {
	  if (derlc[i]!=0.0)
	  {
	       indst.push_back(i);
	       arest.push_back(derlc[i]);
	       arenl.push_back(0.0);
	       derlc[i]   = 0.0;
	  }
     }

     indst.push_back(-1);
     arest.push_back(wght);
     arenl.push_back(0.0);

     for (int i=iagb; i<=ibgb; i++)
     {
	  if (dergb[i]!=0.0)
	  {
	       indst.push_back(i);
	       arest.push_back(dergb[i]);
	       arenl.push_back(dernl[i]);
	       dergb[i]   = 0.0;
	  }
     }	

     if (verbose_mode) cout << "Out Equloc --  NST = " << arest.size() << endl;

     return true; 	
}

/*
  -----------------------------------------------------------
  ZERLOC: reset the derivative vectors
  -----------------------------------------------------------

  dergb[1..nagb]		= global parameters derivatives
  dergb[1..nalc]		= local parameters derivatives

  -----------------------------------------------------------
*/

bool Millepede::ZerLoc(double dergb[], double derlc[], double dernl[])
{
     for(int i=0; i<nalc; i++) {derlc[i] = 0.0;}
     for(int i=0; i<nagb; i++) {dergb[i] = 0.0;}
     for(int i=0; i<nagb; i++) {dernl[i] = 0.0;}

     return true;
}

/*
  -----------------------------------------------------------
  FITLOC:  perform local params fit, once all the equations
  have been written by EquLoc
  -----------------------------------------------------------

  n            = number of the fit, it is used to store 
  fit parameters and then retrieve them 
  for iterations (via STOREIND and STOREARE)

  track_params = contains the fitted track parameters and
  related errors

  single_fit   = is an option, if it is set to 1, we don't 
  perform the last loop. It is used to update 
  the track parameters without modifying global
  matrices

  -----------------------------------------------------------
*/

bool Millepede::FitLoc(int n, double track_params[], int single_fit)
{
     // Few initializations

     int i, j, k, ik, ij, ist, nderlc, ndergl, ndf;
     int ja      = -1;
     int jb      = 0;
     int nagbn   = 0;

     double rmeas, wght, rms, cutval;

     double summ  = 0.0;
     int    nsum  = 0;
     nst   = 0; 
     nst   = arest.size();


     // Fill the track store at first pass

     if (itert < 2 && single_fit != 1)  // Do it only once 
     {
	  if (debug_mode) cout << "Store equation no: " << n << endl; 

	  for (i=0; i<nst; i++)    // Store the track parameters
	  {
	       storeind.push_back(indst[i]);
	       storeare.push_back(arest[i]);
	       storenl.push_back(arenl[i]);

	       if (arenl[i] != 0.) arest[i] = 0.0; // Reset global derivatives if non linear and first iteration
	  }

	  arenl.clear();

	  storeplace.push_back(storeind.size());

	  if (verbose_mode) cout << "StorePlace size = " << storeplace[n] << endl; 
	  if (verbose_mode) cout << "StoreInd size   = " << storeind.size() << endl; 
     }	


     for (i=0; i<nalc; i++) // reset local params
     {
	  blvec[i] = 0.0;

	  for (j=0; j<nalc; j++) {clmat[i][j] = 0.0;}
     }

     for (i=0; i<nagb; i++) {indnz[i] = -1;} // reset mixed params


     /*

     LOOPS : HOW DOES IT WORKS ?	

     Now start by reading the informations stored with EquLoc.
     Those informations are in vector INDST and AREST.
     Each -1 in INDST delimits the equation parameters:

     First -1  ---> rmeas in AREST 
     Then we have indices of local eq in INDST, and derivatives in AREST
     Second -1 ---> weight in AREST
     Then follows indices and derivatives of global eq.
     ....

     We took them and store them into matrices.

     As we want ONLY local params, we substract the part of the estimated value
     due to global params. Indeed we could have already an idea of these params,
     with previous alignment constants for example (set with PARGLO). Also if there
     are more than one iteration (FITLOC could be called by FITGLO)

     */


     //
     // FIRST LOOP : local track fit
     //

     ist = 0;
     indst.push_back(-1);

     while (ist <= nst)
     {
	  if (indst[ist] == -1)
	  {
	       if (ja == -1)     {ja = ist;}  // First  0 : rmeas
	       else if (jb == 0) {jb = ist;}  // Second 0 : weight 
	       else                           // Third  0 : end of equation  
	       {
		    rmeas	= arest[ja];
		    wght 	= arest[jb];
		    if (verbose_mode) cout << "rmeas = " << rmeas << endl ;
		    if (verbose_mode) cout << "wght = " << wght << endl ;

		    for (i=(jb+1); i<ist; i++)   // Now suppress the global part   
			 // (only relevant with iterations)
		    {
			 j = indst[i];              // Global param indice
			 if (verbose_mode) cout << "dparm[" << j << "] = " << dparm[j] << endl;        
			 if (verbose_mode) cout << "Starting misalignment = " << pparm[j] << endl;        
			 rmeas -= arest[i]*(pparm[j]+dparm[j]);
		    }

		    if (verbose_mode) cout << "rmeas after global stuff removal = " << rmeas << endl ;

		    for (i=(ja+1); i<jb; i++)    // Finally fill local matrix and vector
		    {
			 j = indst[i];   // Local param indice (the matrix line) 
			 blvec[j] += wght*rmeas*arest[i];  // See note for precisions

			 if (verbose_mode) cout << "blvec[" << j << "] = " << blvec[j] << endl ;

			 for (k=(ja+1); k<=i ; k++) // Symmetric matrix, don't bother k>j coeffs
			 {
			      ik = indst[k];						
			      clmat[j][ik] += wght*arest[i]*arest[k];

			      if (verbose_mode) cout << "clmat[" << j << "][" << ik << "] = " << clmat[j][ik] << endl;
			 } 
		    }  
		    ja = -1;
		    jb = 0;
		    ist--;
	       } // End of "end of equation" operations
	  } // End of loop on equation
	  ist++;
     } // End of loop on all equations used in the fit


     //
     // Local params matrix is completed, now invert to solve...
     //

     nrank = 0;  // Rank is the number of nonzero diagonal elements 
     nrank = Millepede::SpmInv(clmat, blvec, nalc, scdiag, scflag);

     if (debug_mode) cout << "" << endl;
     if (debug_mode) cout << " __________________________________________________" << endl;
     if (debug_mode) cout << " Printout of local fit  (FITLOC)  with rank= "<< nrank << endl;
     if (debug_mode) cout << " Result of local fit :      (index/parameter/error)" << endl;

     for (i=0; i<nalc; i++)
     {
	  if (debug_mode) cout << std::setprecision(4) << std::fixed;
	  if (debug_mode) cout << std::setw(20) << i << "   /   " << std::setw(10) 
			       << blvec[i] << "   /   " << sqrt(clmat[i][i]) << endl;	
     }


     // Store the track params and errors

     for (i=0; i<nalc; i++)
     {
	  track_params[2*i] = blvec[i];
	  track_params[2*i+1] = sqrt(fabs(clmat[i][i]));
     }


     //
     // SECOND LOOP : residual calculation
     //

     ist = 0;
     ja = -1;
     jb = 0;

     while (ist <= nst)
     {
	  if (indst[ist] == -1)
	  {
	       if (ja == -1)     {ja = ist;}  // First  0 : rmeas
	       else if (jb == 0) {jb = ist;}  // Second 0 : weight 
	       else                           // Third  0 : end of equation  
	       {	
		    rmeas	= arest[ja];
		    wght 	= arest[jb];

		    nderlc = jb-ja-1;    // Number of local derivatives involved
		    ndergl = ist-jb-1;   // Number of global derivatives involved

		    // Print all (for debugging purposes)

		    if (verbose_mode) cout << "" << endl;
		    if (verbose_mode) cout << std::setprecision(4) << std::fixed;
		    if (verbose_mode) cout << ". equation:  measured value " << std::setw(13) 
					   << rmeas << " +/- " << std::setw(13) << 1.0/sqrt(wght) << endl;
		    if (verbose_mode) cout << "Number of derivatives (global, local): " 
					   << ndergl << ", " << nderlc << endl;
		    if (verbose_mode) cout << "Global derivatives are: (index/derivative/parvalue) " << endl;

		    for (i=(jb+1); i<ist; i++)
		    {if (verbose_mode) cout << indst[i] << " / " << arest[i] 
					    << " / " << pparm[indst[i]] << endl;} 

                    if (verbose_mode) cout << "Local derivatives are: (index/derivative) " << endl;

                    for (i=(ja+1); i<jb; i++) {if (verbose_mode) cout << indst[i] << " / " << arest[i] << endl;}	  

                    // Now suppress local and global parts to RMEAS;

                    for (i=(ja+1); i<jb; i++) // First the local part 
                    {
			 j = indst[i];
			 rmeas -= arest[i]*blvec[j];
                    }

                    for (i=(jb+1); i<ist; i++) // Then the global part
                    {
			 j = indst[i];
			 rmeas -= arest[i]*(pparm[j]+dparm[j]);
                    }

                    // rmeas contains now the residual value
                    // 	if (verbose_mode) cout << "Residual value : "<< rmeas << endl;
                    if (verbose_reject) cout << "Residual value : "<< rmeas << endl;

                    // reject the track if rmeas is too important (outlier)
                    if (fabs(rmeas) >= m_residual_cut_init && itert <= 1)  
                    {
			 // 	  if (verbose_mode) cout << "Rejected track !!!!!" << endl;
			 if (verbose_reject) cout << "Rejected track !!!!!" << endl;
			 locrej++;      
			 indst.clear(); // reset stores and go to the next track 
			 arest.clear();	  
			 return false;
                    }

                    if (fabs(rmeas) >= m_residual_cut && itert > 1)   
                    {
			 // 	  if (verbose_mode) cout << "Rejected track !!!!!" << endl;
			 if (verbose_reject) cout << "Rejected track !!!!!" << endl;
			 locrej++;      
			 indst.clear(); // reset stores and go to the next track 
			 arest.clear();	  
			 return false;
                    }

                    summ += wght*rmeas*rmeas ; // total chi^2
                    nsum++;                    // number of equations			
                    ja = -1;
                    jb = 0;
                    ist--;
	       } // End of "end of equation" operations
	  }   // End of loop on equation
	  ist++;
     } // End of loop on all equations used in the fit

     ndf = nsum-nrank;	
     rms = 0.0;

     if (debug_mode) cout << "Final chi square / degrees of freedom "<< summ << " / " << ndf << endl;

     if (ndf > 0) rms = summ/float(ndf);  // Chi^2/dof

     if (single_fit == 0) loctot++;

     if (nstdev != 0 && ndf > 0 && single_fit != 1) // Chisquare cut
     {
	  cutval = Millepede::chindl(nstdev, ndf)*cfactr;

	  if (debug_mode) cout << "Reject if Chisq/Ndf = " << rms << "  >  " << cutval << endl;

	  if (rms > cutval) // Reject the track if too much...
	  {
	       if (verbose_mode) cout << "Rejected track !!!!!" << endl;
	       if (single_fit == 0) locrej++;      
	       indst.clear(); // reset stores and go to the next track 
	       arest.clear();
	       return false;
	  }
     }

     if (single_fit == 1) // Stop here if just updating the track parameters
     {
	  indst.clear(); // Reset store for the next track 
	  arest.clear();
	  return true;
     }

     //  
     // THIRD LOOP: local operations are finished, track is accepted 
     // We now update the global parameters (other matrices)
     //

     ist = 0;
     ja = -1;
     jb = 0;

     while (ist <= nst)
     {
	  if (indst[ist] == -1)
	  {
	       if (ja == -1)     {ja = ist;}  // First  0 : rmeas
	       else if (jb == 0) {jb = ist;}  // Second 0 : weight 
	       else                           // Third  0 : end of equation  
	       {	
		    rmeas	= arest[ja];
		    wght 	= arest[jb];

		    for (i=(jb+1); i<ist; i++) // Now suppress the global part
		    {
			 j = indst[i];   // Global param indice
			 rmeas -= arest[i]*(pparm[j]+dparm[j]);
		    }

		    // First of all, the global/global terms (exactly like local matrix)

		    for (i=(jb+1); i<ist; i++)  
		    {
			 j = indst[i];   // Global param indice (the matrix line)          

			 bgvec[j] += wght*rmeas*arest[i];  // See note for precisions
			 if (verbose_mode) cout << "bgvec[" << j << "] = " << bgvec[j] << endl ;

			 for (k=(jb+1); k<ist ; k++)
			 {
			      ik = indst[k];						
			      cgmat[j][ik] += wght*arest[i]*arest[k];
			      if (verbose_mode) cout << "cgmat[" << j << "][" << ik << "] = " << cgmat[j][ik] << endl;
			 } 
		    }

		    // Now we have also rectangular matrices containing global/local terms.

		    for (i=(jb+1); i<ist; i++) 
		    {
			 j  = indst[i];  // Global param indice (the matrix line)
			 ik = indnz[j];  // Index of index

			 if (ik == -1)	  // New global variable
			 {
			      for (k=0; k<nalc; k++) {clcmat[nagbn][k] = 0.0;} // Initialize the row

			      indnz[j] = nagbn;
			      indbk[nagbn] = j;
			      ik = nagbn;
			      nagbn++;
			 }

			 for (k=(ja+1); k<jb ; k++) // Now fill the rectangular matrix
			 {
			      ij = indst[k];						
			      clcmat[ik][ij] += wght*arest[i]*arest[k];
			      if (verbose_mode) cout << "clcmat[" << ik << "][" << ij << "] = " << clcmat[ik][ij] << endl;
			 } 
		    }
		    ja = -1;
		    jb = 0;
		    ist--;
	       } // End of "end of equation" operations
	  }   // End of loop on equation
	  ist++;
     } // End of loop on all equations used in the fit

     // Third loop is finished, now we update the correction matrices (see notes)

     Millepede::SpAVAt(clmat, clcmat, corrm, nalc, nagbn);
     Millepede::SpAX(clcmat, blvec, corrv, nalc, nagbn);

     for (i=0; i<nagbn; i++)
     {
	  j = indbk[i];
	  bgvec[j] -= corrv[i];

	  for (k=0; k<nagbn; k++)
	  {
	       ik = indbk[k];
	       cgmat[j][ik] -= corrm[i][k];
	  }
     }

     indst.clear(); // Reset store for the next track 
     arest.clear();

     return true;
}


/*
  -----------------------------------------------------------
  MAKEGLOBALFIT:  perform global params fit, once all the 'tracks'
  have been fitted by FitLoc
  -----------------------------------------------------------

  par[]        = array containing the computed global 
  parameters (the misalignment constants)

  error[]      = array containing the error on global 
  parameters (estimated by Millepede)

  pull[]        = array containing the corresponding pulls 

  -----------------------------------------------------------
*/

bool Millepede::MakeGlobalFit(double par[], double error[], double pull[])
{
     int i, j, nf, nvar;
     int itelim = 0;

     int nstillgood;

     double sum;

     double step[150];

     double trackpars[2*mlocal];

     int ntotal_start, ntotal;

     cout << "..... Making global fit ....." << endl;

     ntotal_start = Millepede::GetTrackNumber();

     std::vector<double> track_slopes;

     track_slopes.resize(2*ntotal_start);

     for (i=0; i<2*ntotal_start; i++) track_slopes[i] = 0.;

     if (itert <= 1) itelim=10;    // Max number of iterations

     while (itert < itelim)  // Iteration for the final loop
     {
	  if (debug_mode) cout << "ITERATION --> " << itert << endl;

	  ntotal = Millepede::GetTrackNumber();
	  cout << "...using " << ntotal << " tracks..." << endl;

	  // Start by saving the diagonal elements

	  for (i=0; i<nagb; i++) {diag[i] = cgmat[i][i];}

	  //  Then we retrieve the different constraints: fixed parameter or global equation

	  nf = 0; // First look at the fixed global params

	  for (i=0; i<nagb; i++)
	  {
	       if (psigm[i] <= 0.0)   // fixed global param
	       {
		    nf++;

		    for (j=0; j<nagb; j++)
		    {
			 cgmat[i][j] = 0.0;  // Reset row and column
			 cgmat[j][i] = 0.0;
		    }			
	       }
	       else if (psigm[i] > 0.0) 
	       {
		    cgmat[i][i] += 1.0/(psigm[i]*psigm[i]);
	       }
	  }

	  nvar = nagb;  // Current number of equations	
	  if (debug_mode) cout << "Number of constraint equations : " << ncs << endl;

	  if (ncs > 0) // Then the constraint equation
	  {
	       for (i=0; i<ncs; i++)
	       {
		    sum = arhs[i];
		    for (j=0; j<nagb; j++)
		    {
			 cgmat[nvar][j] = float(ntotal)*adercs[i][j];
			 cgmat[j][nvar] = float(ntotal)*adercs[i][j];          
			 sum -= adercs[i][j]*(pparm[j]+dparm[j]);
		    }

		    cgmat[nvar][nvar] = 0.0;
		    bgvec[nvar] = float(ntotal)*sum;
		    nvar++;
	       }
	  }


	  // Intended to compute the final global chisquare

	  double final_cor = 0.0;

	  if (itert > 1)
	  {
	       for (j=0; j<nagb; j++)
	       {
		    for (i=0; i<nagb; i++)
		    {
			 if (psigm[i] > 0.0)
			 {
			      final_cor += step[j]*cgmat[j][i]*step[i]; 
			      if (i == j) final_cor -= step[i]*step[i]/(psigm[i]*psigm[i]);
			 }
		    }
	       }
	  }

	  cout << " Final coeff is " << final_cor << endl;		
	  cout << " Final NDOFs =  " << nagb << endl;

	  //  The final matrix inversion

	  nrank = Millepede::SpmInv(cgmat, bgvec, nvar, scdiag, scflag);

	  for (i=0; i<nagb; i++)
	  {
	       dparm[i] += bgvec[i];    // Update global parameters values (for iterations)
	       if (verbose_mode) cout << "dparm[" << i << "] = " << dparm[i] << endl;
	       if (verbose_mode) cout << "cgmat[" << i << "][" << i << "] = " << cgmat[i][i] << endl;
	       if (verbose_mode) cout << "err = " << sqrt(fabs(cgmat[i][i])) << endl;

	       step[i] = bgvec[i];

	       if (itert == 1) error[i] = cgmat[i][i]; // Unfitted error
	  }

	  cout << "" << endl;
	  cout << "The rank defect of the symmetric " << nvar << " by " << nvar 
	       << " matrix is " << nvar-nf-nrank << " (bad if non 0)" << endl;

	  if (itert == 0)  break;       
	  itert++;

	  cout << "" << endl;
	  cout << "Total : " << loctot << " local fits, " 
	       << locrej << " rejected." << endl;

	  // Reinitialize parameters for iteration

	  loctot = 0;
	  locrej = 0;

	  if (cfactr != cfactref && sqrt(cfactr) > 1.2*cfactref)
	  {
	       cfactr = sqrt(cfactr);
	  }
	  else
	  {
	       cfactr = cfactref;
	       //      itert = itelim;
	  }

	  if (itert == itelim)  break;  // End of story         

	  cout << "Iteration " << itert << " with cut factor " << cfactr << endl;

	  // Reset global variables

	  for (i=0; i<nvar; i++)
	  {
	       bgvec[i] = 0.0;
	       for (j=0; j<nvar; j++)
	       {
		    cgmat[i][j] = 0.0;
	       }
	  }

	  //
	  // We start a new iteration
	  //

	  // First we read the stores for retrieving the local params

	  nstillgood = 0;	

	  for (i=0; i<ntotal_start; i++)
	  {
	       int rank_i = 0;
	       int rank_f = 0;

	       (i>0) ? rank_i = abs(storeplace[i-1]) : rank_i = 0;
	       rank_f = storeplace[i];

	       if (verbose_mode) cout << "Track " << i << " : " << endl;
	       if (verbose_mode) cout << "Starts at " << rank_i << endl;
	       if (verbose_mode) cout << "Ends at " << rank_f << endl;

	       if (rank_f >= 0) // Fit is still OK
	       {

		    if (itert > 3)
		    {
			 indst.clear();
			 arest.clear();

			 for (j=rank_i; j<rank_f; j++)
			 {
			      indst.push_back(storeind[j]);

			      if (storenl[j] == 0) arest.push_back(storeare[j]);
			      if (storenl[j] == 1) arest.push_back(track_slopes[2*i]*storeare[j]);
			      if (storenl[j] == 2) arest.push_back(track_slopes[2*i+1]*storeare[j]);
			 }

			 for (j=0; j<2*nalc; j++) {trackpars[j] = 0.;}	

			 Millepede::FitLoc(i,trackpars,1);

			 track_slopes[2*i] = trackpars[2];
			 track_slopes[2*i+1] = trackpars[6];
		    }

		    indst.clear();
		    arest.clear();

		    for (j=rank_i; j<rank_f; j++)
		    {
			 indst.push_back(storeind[j]);

			 if (storenl[j] == 0) arest.push_back(storeare[j]);
			 if (storenl[j] == 1) arest.push_back(track_slopes[2*i]*storeare[j]);
			 if (storenl[j] == 2) arest.push_back(track_slopes[2*i+1]*storeare[j]);
		    }

		    for (j=0; j<2*nalc; j++) {trackpars[j] = 0.;}	

		    bool sc = Millepede::FitLoc(i,trackpars,0);

		    (sc) 
			 ? nstillgood++
			 : storeplace[i] = -rank_f;     
	       }
	  } // End of loop on fits

	  Millepede::SetTrackNumber(nstillgood);

     } // End of iteration loop

     Millepede::PrtGlo(); // Print the final results

     for (j=0; j<nagb; j++)
     {
	  par[j]   = dparm[j];
	  dparm[j] = 0.;
	  pull[j]  = par[j]/sqrt(psigm[j]*psigm[j]-cgmat[j][j]);
	  error[j] = sqrt(fabs(cgmat[j][j]));
     }

     cout << std::setw(10) << " " << endl;
     cout << std::setw(10) << "            * o o                   o      " << endl;
     cout << std::setw(10) << "              o o                   o      " << endl;
     cout << std::setw(10) << "   o ooooo  o o o  oo  ooo   oo   ooo  oo  " << endl;
     cout << std::setw(10) << "    o  o  o o o o o  o o  o o  o o  o o  o " << endl;
     cout << std::setw(10) << "    o  o  o o o o oooo o  o oooo o  o oooo " << endl;
     cout << std::setw(10) << "    o  o  o o o o o    ooo  o    o  o o    " << endl;
     cout << std::setw(10) << "    o  o  o o o o  oo  o     oo   ooo  oo ++ ends." << endl;
     cout << std::setw(10) << "                       o                   " << endl;	  

     return true;
}




/*
  -----------------------------------------------------------
  SPMINV:  obtain solution of a system of linear equations with symmetric matrix 
  and the inverse (using 'singular-value friendly' GAUSS pivot)
  -----------------------------------------------------------

  Solve the equation :  V * X = B

  V is replaced by inverse matrix and B by X, the solution vector
  -----------------------------------------------------------
*/

int Millepede::SpmInv(double v[][mgl], double b[], int n, double diag[], bool flag[])
{
     int i, j, jj, k;
     double vkk, *temp;
     double  *r, *c;
     double eps = 0.00000000000001;

     r = new double[n];
     c = new double[n];

     temp = new double[n];

     for (i=0; i<n; i++)
     {
	  r[i] = 0.0;
	  c[i] = 0.0;
	  flag[i] = true;

	  for (j=0; j<=i; j++) {if (v[j][i] == 0) {v[j][i] = v[i][j];}}
     }

     // Small loop for matrix equilibration (gives a better conditioning) 

     for (i=0; i<n; i++)
     {
	  for (j=0; j<n; j++)
	  { 
	       if (fabs(v[i][j]) >= r[i]) r[i] = fabs(v[i][j]); // Max elemt of row i
	       if (fabs(v[j][i]) >= c[i]) c[i] = fabs(v[j][i]); // Max elemt of column i
	  }
     }

     for (i=0; i<n; i++)
     {
	  if (0.0 != r[i]) r[i] = 1./r[i]; // Max elemt of row i
	  if (0.0 != c[i]) c[i] = 1./c[i]; // Max elemt of column i

	  //    if (eps >= r[i]) r[i] = 0.0; // Max elemt of row i
	  //    if (eps >= c[i]) c[i] = 0.0; // Max elemt of column i
     }

     for (i=0; i<n; i++) // Equilibrate the V matrix
     {
	  for (j=0; j<n; j++) {v[i][j] = sqrt(r[i])*v[i][j]*sqrt(c[j]);}
     }

     nrank = 0;

     // save diagonal elem absolute values 	
     for (i=0; i<n; i++) {diag[i] = fabs(v[i][i]);} 

     for (i=0; i<n; i++)
     {
	  vkk = 0.0;
	  k = -1;

	  for (j=0; j<n; j++) // First look for the pivot, ie max unused diagonal element 
	  {
	       if (flag[j] && (fabs(v[j][j])>std::max(fabs(vkk),eps*diag[j])))
	       {
		    vkk = v[j][j];
		    k = j;
	       }
	  }

	  if (k >= 0)    // pivot found
	  {      
	       if (verbose_mode) cout << "Pivot value :" << vkk << endl;
	       nrank++;
	       flag[k] = false; // This value is used
	       vkk = 1.0/vkk;
	       v[k][k] = -vkk; // Replace pivot by its inverse

	       for (j=0; j<n; j++)
	       {
		    for (jj=0; jj<n; jj++)  
		    {
			 if (j != k && jj != k) // Other elements (!!! do them first as you use old v[k][j]'s !!!)
			 {
			      v[j][jj] = v[j][jj] - vkk*v[j][k]*v[k][jj];
			 }					
		    }					
	       }

	       for (j=0; j<n; j++)
	       {
		    if (j != k) // Pivot row or column elements 
		    {
			 v[j][k] = (v[j][k])*vkk;	// Column
			 v[k][j] = (v[k][j])*vkk;	// Line
		    }
	       }	
	  }
	  else   // No more pivot value (clear those elements)
	  {
	       for (j=0; j<n; j++)
	       {
		    if (flag[j])
		    {
			 b[j] = 0.0;

			 for (k=0; k<n; k++)
			 {
			      v[j][k] = 0.0;
			      v[k][j] = 0.0;
			 }
		    }				
	       }

	       break;  // No more pivots anyway, stop here
	  }
     }

     for (i=0; i<n; i++) // Correct matrix V
     {
	  for (j=0; j<n; j++) {v[i][j] = sqrt(c[i])*v[i][j]*sqrt(r[j]);}
     }

     for (j=0; j<n; j++)
     {
	  temp[j] = 0.0;

	  for (jj=0; jj<n; jj++)  // Reverse matrix elements
	  {
	       v[j][jj] = -v[j][jj];
	       temp[j] += v[j][jj]*b[jj];
	  }					
     }

     for (j=0; j<n; j++) {b[j] = temp[j];}	// The final result				

     delete temp;
     delete r;
     delete c;

     return nrank;
}

//
// Same method but for local fit, so heavily simplified
//


int Millepede::SpmInv(double v[][mlocal], double b[], int n, double diag[], bool flag[])
{
     int i, j, jj, k;
     double vkk, *temp;
     double eps = 0.0000000000001;

     temp = new double[n];

     for (i=0; i<n; i++)
     {
	  flag[i] = true;
	  diag[i] = fabs(v[i][i]);     // save diagonal elem absolute values 	

	  for (j=0; j<=i; j++)
	  {
	       v[j][i] = v[i][j] ;
	  }
     }

     nrank = 0;

     for (i=0; i<n; i++)
     {
	  vkk = 0.0;
	  k = -1;

	  for (j=0; j<n; j++) // First look for the pivot, ie max unused diagonal element 
	  {
	       if (flag[j] && (fabs(v[j][j])>std::max(fabs(vkk),eps*diag[j])))
	       {
		    vkk = v[j][j];
		    k = j;
	       }
	  }

	  if (k >= 0)    // pivot found
	  {
	       nrank++;
	       flag[k] = false;
	       vkk = 1.0/vkk;
	       v[k][k] = -vkk; // Replace pivot by its inverse

	       for (j=0; j<n; j++)
	       {
		    for (jj=0; jj<n; jj++)  
		    {
			 if (j != k && jj != k) // Other elements (!!! do them first as you use old v[k][j]'s !!!)
			 {
			      v[j][jj] = v[j][jj] - vkk*v[j][k]*v[k][jj];
			 }					
		    }					
	       }

	       for (j=0; j<n; j++)
	       {
		    if (j != k) // Pivot row or column elements 
		    {
			 v[j][k] = (v[j][k])*vkk; // Column
			 v[k][j] = (v[k][j])*vkk; // Line
		    }
	       }					
	  }
	  else  // No more pivot value (clear those elements)
	  {
	       for (j=0; j<n; j++)
	       {
		    if (flag[j])
		    {
			 b[j] = 0.0;

			 for (k=0; k<n; k++)
			 {
			      v[j][k] = 0.0;
			 }
		    }				
	       }

	       break;  // No more pivots anyway, stop here
	  }
     }

     for (j=0; j<n; j++)
     {
	  temp[j] = 0.0;

	  for (jj=0; jj<n; jj++)  // Reverse matrix elements
	  {
	       v[j][jj] = -v[j][jj];
	       temp[j] += v[j][jj]*b[jj];
	  }					
     }

     for (j=0; j<n; j++)
     {	
	  b[j] = temp[j];
     }					

     delete temp;

     return nrank;
}


/*
  -----------------------------------------------------------
  SPAVAT
  -----------------------------------------------------------

  multiply symmetric N-by-N matrix from the left with general M-by-N
  matrix and from the right with the transposed of the same  general
  matrix  to  form  symmetric  M-by-M   matrix.

  T
  CALL SPAVAT(V,A,W,N,M)      W   =   A   *   V   *   A
  M*M     M*N     N*N     N*M

  where V = symmetric N-by-N matrix
  A = general N-by-M matrix
  W = symmetric M-by-M matrix
  -----------------------------------------------------------
*/


bool Millepede::SpAVAt(double v[][mlocal], double a[][mlocal], double w[][mglobl], int n, int m)
{
     int i,j, k, l; 

     for (i=0; i<m; i++)
     {
	  for (j=0; j<m; j++)   // Could be improved as matrix symmetric
	  {
	       w[i][j] = 0.0;      // Reset final matrix

	       for (k=0; k<n; k++)
	       {
		    for (l=0; l<n; l++)
		    {
			 w[i][j] += a[i][k]*v[k][l]*a[j][l];  // fill the matrix
		    }
	       }
	  }
     }

     return true;
}


/*
  -----------------------------------------------------------
  SPAX
  -----------------------------------------------------------

  multiply general M-by-N matrix A and N-vector X

  CALL  SPAX(A,X,Y,M,N)          Y =  A * X
  M   M*N  N

  where A = general M-by-N matrix (A11 A12 ... A1N  A21 A22 ...)
  X = N vector
  Y = M vector
  -----------------------------------------------------------
*/

bool Millepede::SpAX(double a[][mlocal], double x[], double y[], int n, int m)
{
     int i,j; 

     for (i=0; i<m; i++)
     {
	  y[i] = 0.0;	    // Reset final vector

	  for (j=0; j<n; j++)
	  {
	       y[i] += a[i][j]*x[j];  // fill the vector
	  }
     }

     return true;
}


/*
  -----------------------------------------------------------
  PRTGLO
  -----------------------------------------------------------

  Print the final results into the logfile

  -----------------------------------------------------------
*/


bool Millepede::PrtGlo()
{
     double err, gcor;

     cout << "" << endl;
     cout << "   Result of fit for global parameters" << endl;
     cout << "   ===================================" << endl;
     cout << "    I       initial       final       differ   " 
	  << "     lastcor        Error       gcor" << endl;
     cout << "-----------------------------------------" 
	  << "------------------------------------------" << endl;

     for (int i=0; i<nagb; i++)
     {
	  err = sqrt(fabs(cgmat[i][i]));
	  if (cgmat[i][i] < 0.0) err = -err;
	  gcor = 0.0;

	  if (i%(nagb/6) == 0)
	  {
	       cout << "-----------------------------------------" 
		    << "------------------------------------------" << endl;
	  }

//         cout << "cgmat[" << i << "][" << i << "] = " << cgmat[i][i]; 
//         cout << "        diag[" << i << "] = " << diag[i] << endl;
	  if (fabs(cgmat[i][i]*diag[i]) > 0)
	  {
	       cout << std::setprecision(4) << std::fixed;
	       gcor = sqrt(fabs(1.0-1.0/(cgmat[i][i]*diag[i])));
	       cout << std::setw(4) << i << "  / " << std::setw(10) << pparm[i] 
		    << "  / " << std::setw(10) << pparm[i]+ dparm[i] 
		    << "  / " << std::setw(13) << dparm[i] 
		    << "  / " << std::setw(13) << bgvec[i] << "  / " << std::setw(10) 
		    << std::setprecision(5) << err << "  / " << std::setw(10) << gcor << endl;
	  }
	  else
	  {
	       cout << std::setw(4) << i << "  / " << std::setw(10) << "OFF" 
		    << "  / " << std::setw(10) << "OFF" 
		    << "  / " << std::setw(13) << "OFF" 
		    << "  / " << std::setw(13) << "OFF" 
		    << "  / " << std::setw(10) << "OFF" 
		    << "  / " << std::setw(10) << "OFF" << endl;
	  }
     }
     return true;
}


/*
  ----------------------------------------------------------------
  CHINDL:  return the limit in chi^2/nd for n sigmas stdev authorized
  ----------------------------------------------------------------

  Only n=1, 2, and 3 are expected in input
  ----------------------------------------------------------------
*/

double Millepede::chindl(int n, int nd)
{
     int m;
     double sn[3]        =	{0.47523, 1.690140, 2.782170};
     double table[3][30] = {{1.0000, 1.1479, 1.1753, 1.1798, 1.1775, 1.1730, 1.1680, 1.1630,
			     1.1581, 1.1536, 1.1493, 1.1454, 1.1417, 1.1383, 1.1351, 1.1321,
			     1.1293, 1.1266, 1.1242, 1.1218, 1.1196, 1.1175, 1.1155, 1.1136,
			     1.1119, 1.1101, 1.1085, 1.1070, 1.1055, 1.1040},
			    {4.0000, 3.0900, 2.6750, 2.4290, 2.2628, 2.1415, 2.0481, 1.9736,
			     1.9124, 1.8610, 1.8171, 1.7791, 1.7457, 1.7161, 1.6897, 1.6658,
			     1.6442, 1.6246, 1.6065, 1.5899, 1.5745, 1.5603, 1.5470, 1.5346,
			     1.5230, 1.5120, 1.5017, 1.4920, 1.4829, 1.4742},
			    {9.0000, 5.9146, 4.7184, 4.0628, 3.6410, 3.3436, 3.1209, 2.9468,
			     2.8063, 2.6902, 2.5922, 2.5082, 2.4352, 2.3711, 2.3143, 2.2635,
			     2.2178, 2.1764, 2.1386, 2.1040, 2.0722, 2.0428, 2.0155, 1.9901,
			     1.9665, 1.9443, 1.9235, 1.9040, 1.8855, 1.8681}};

     if (nd < 1)
     {
	  return 0.0;
     }
     else
     {
	  m = std::max(1,std::min(n,3));

	  if (nd <= 30)
	  {
	       return table[m-1][nd-1];
	  }
	  else // approximation
	  {
	       return ((sn[m-1]+sqrt(float(2*nd-3)))*(sn[m-1]+sqrt(float(2*nd-3))))/float(2*nd-2);
	  }
     }
}


int    Millepede::GetTrackNumber()                      {return m_track_number;}
void   Millepede::SetTrackNumber(int value)             {m_track_number = value;}
