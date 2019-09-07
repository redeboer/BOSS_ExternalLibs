//$id$
//
//$log$

/*
 *    2003/12/24   Zhengyun You      Peking University
 * 
 *    2004/09/12   Zhengyun You     Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_LINEFIT_H
#define MUC_LINEFIT_H

class MucRecLineFit
{
 public:
  
  /// Constructor.
  MucRecLineFit();

  /// Destructor.
  ~MucRecLineFit();

  int LineFit(float x[],
	      float y[], 
	      float w[],
	      int   n,
	      float *a,
	      float *b,
	      float *chisq,
	      float *siga, 
        float *sigb);

  int LineFit(float x[],
      float y[],
      float w[],
      int part,
      int seg,
      int orient,
      int   n,
      float *a,
      float *b,
      float *chisq,
      float *siga,
      float *sigb);

  
 private:

};

#endif  /* MUC_LINEFIT_H */
