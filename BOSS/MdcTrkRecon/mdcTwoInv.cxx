/* Invert a symmetric 2x2 matrix.  Return 1 if inversion fails. */

int mdcTwoInv( double matrix[3], double invmat[3]) {

  /* Declare variables. */
  double det, detinv;

  /**************************************************************************/

  det = matrix[0] * matrix[2] - matrix[1] * matrix[1];
  if (det == 0.0) {
    invmat[0] = 0.00001;
    invmat[2] = 0.00001;
    invmat[1] = 0.00000;
    return 1;
  }
  detinv = 1./det;

  invmat[0] = matrix[2] * detinv;
  invmat[2] = matrix[0] * detinv;
  invmat[1] = -matrix[1] * detinv;

  return 0;
}
