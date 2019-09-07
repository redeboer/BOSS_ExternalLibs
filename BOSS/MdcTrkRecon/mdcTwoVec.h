/* Multiply a symmetric 2-d matrix by a vector. */

inline void mdcTwoVec(const double matrix[3], const double invect[2], 
		      double outvect[2]) {

  double buffvec[2];   /* Buffer the output in case outvect = invect. */

  /************************************************************************/

  buffvec[0] = matrix[0] * invect[0] + matrix[1] * invect[1];
  buffvec[1] = matrix[1] * invect[0] + matrix[2] * invect[1];

  outvect[0] = buffvec[0];
  outvect[1] = buffvec[1];

  return;
}
