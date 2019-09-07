#ifndef MILLEPEDE_H 
#define MILLEPEDE_H 1

/** @class Millepede Millepede.h AlignmentTools/Millepede.h
 *  
 *
 *  @author Sebastien Viret
 *  @date   2005-07-29
 */

#include <vector>
#include "MdcAlignAlg/Alignment.h"

using namespace Alignment;

class Millepede{
public: 
  /// Standard constructor
  Millepede();
  ~Millepede( ); ///< Destructor

  /// Initialization
  bool initialize();

  bool InitMille(bool DOF[], double Sigm[], int nglo
		 , int nloc, double startfact, int nstd 
		 , double res_cut, double res_cut_init);
  bool MakeGlobalFit(double par[], double error[], double pull[]);
  bool ParGlo(int index, double param);
  bool ParSig(int index, double sigma);
  bool ConstF(double dercs[], double rhs);
  bool EquLoc(double dergb[], double derlc[], double dernl[], double rmeas, double sigma);
  bool ZerLoc(double dergb[], double derlc[], double dernl[]);
  bool FitLoc(int n, double track_params[], int single_fit);
  int  GetTrackNumber();
  void SetTrackNumber(int value);

private:

// Option set-up here 

/* the following 4 statements are moved to Alignment.h by wulh on 06/08/29 */
//   static const bool m_iteration  = true;   //  */
//   static const bool debug_mode   = false;   // More printouts */
//   static const bool verbose_mode = false;  // Definitely more printouts (matrices,...) */
//   static const bool verbose_reject = false;//\* added by wulh on 06/08/28 *\/ */


// Max. dimensions

  static const int mglobl		= 400; // Max. number of global parameters
  static const int mlocal		= 20;  // Max. number of local parameters
  static const int mcs			= 10;  // Max. number of constraint equations
  static const int mgl			= 410; // mglobl+mlocal


// Private methods 

  bool InitUn (double cutfac);
  bool PrtGlo();

  double ErrPar(int i);
  double CorPar(int i, int j);
  int  SpmInv(double v[][mgl], double b[], int n, double diag[], bool flag[]);
  int  SpmInv(double v[][mlocal], double b[], int n, double diag[], bool flag[]);
  bool SpAVAt(double v[][mlocal], double a[][mlocal], double w[][mglobl], int n, int m);
  bool SpAX(double a[][mlocal], double x[], double y[], int n, int m);
  double chindl(int n, int nd);

// Matrices

  double cgmat[mgl][mgl];
  double clmat[mlocal][mlocal];
  double clcmat[mglobl][mlocal];
  double corrm[mglobl][mglobl];
  double adercs[mcs][mglobl];


// Vectors and useful variables

  double corrv[mglobl], psigm[mglobl], pparm[mglobl], dparm[mglobl];
  double scdiag[mglobl], blvec[mlocal], arhs[mcs], diag[mgl], bgvec[mgl];

  int indgb[mglobl], nlnpa[mglobl], indnz[mglobl], indbk[mglobl], indlc[mlocal];
  
  bool scflag[mglobl];

  std::vector<int>     indst;
  std::vector<double>  arest;
  std::vector<double>  arenl;

  std::vector<int>     storeind;
  std::vector<int>     storeplace;
  std::vector<double>  storeare;
  std::vector<double>  storenl;

  int store_row_size;

  int m_track_number;
  double m_residual_cut_init;
  double m_residual_cut;

  double cfactr, cfactref;

  int itert, nst, nfl, ncs, nstdev;
  int loctot, locrej, nagb, nalc, nrank; 

};
#endif // MILLEPEDE_H 
