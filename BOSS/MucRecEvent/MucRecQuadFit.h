//$id$
//
//$log$

/*
 *    2003/12/24   Zhengyun You      Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_QUADFIT_H
#define MUC_QUADFIT_H

class MucRecQuadFit
{
 public:
  
  /// Constructor.
  MucRecQuadFit();

  /// Destructor.
  ~MucRecQuadFit();

  int QuadFit(float x[],
	      float y[], 
	      float w[],
	      int   n,
	      float *a,
	      float *b,
	      float *c,
	      int *half, //which half parabola 1: left 2 : right
	      float *chisq,
	      float *siga, 
	      float *sigb,
	      float *sigc);
  
 private:

};

#endif  /* MUC_QUADFIT_H */
