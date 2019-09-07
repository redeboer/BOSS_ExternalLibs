extern int dbcg_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*matvec)(), int (*mttvec)(), int (*msolve)(), int (*mtsolv)(), const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *z, double *p, double *rr, double *zz, double *pp, double *dz, double *rwork, long *iwork);
extern int dsdbcg_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern int dslubc_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern long isdbcg_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*msolve)(), const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *z, double *p, double *rr, double *zz, double *pp, double *dz, double *rwork, long *iwork, double *ak, double *bk, double *bnrm, double *solnrm);
/* comlen solblk_ 8 */
/*:ref: d1mach_ 7 1 4 */
/*:ref: ddot_ 7 5 4 7 4 7 4 */
/*:ref: dcopy_ 14 5 4 7 4 7 4 */
/*:ref: daxpy_ 14 6 4 7 7 4 7 4 */
/*:ref: ds2y_ 14 6 4 4 4 4 7 4 */
/*:ref: dchkw_ 14 9 13 4 4 4 4 4 4 7 124 */
/*:ref: dsds_ 14 7 4 4 4 4 7 4 7 */
/*:ref: dsmv_ 14 :*/
/*:ref: dsmtv_ 14 :*/
/*:ref: dsdi_ 14 :*/
/*:ref: dsilus_ 14 17 4 4 4 4 7 4 4 4 4 7 7 4 4 4 7 4 4 */
/*:ref: dslui_ 14 :*/
/*:ref: dsluti_ 14 :*/
/*:ref: dnrm2_ 7 3 4 7 4 */
extern int dcg_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*matvec)(), int (*msolve)(), const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *z, double *p, double *dz, double *rwork, long *iwork);
extern int dsdcg_(const int *n, const double *b, double *x, const long *nelt, int *ia, int *ja, double *a, const long *isym, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern int dsiccg_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern long isdcg_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*msolve)(), const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *z, double *p, double *dz, double *rwork, long *iwork, double *ak, double *bk, double *bnrm, double *solnrm);
/* comlen solblk_ 8 */
/*:ref: d1mach_ 7 1 4 */
/*:ref: ddot_ 7 5 4 7 4 7 4 */
/*:ref: dcopy_ 14 5 4 7 4 7 4 */
/*:ref: daxpy_ 14 6 4 7 7 4 7 4 */
/*:ref: ds2y_ 14 6 4 4 4 4 7 4 */
/*:ref: dchkw_ 14 9 13 4 4 4 4 4 4 7 124 */
/*:ref: dsds_ 14 7 4 4 4 4 7 4 7 */
/*:ref: dsmv_ 14 :*/
/*:ref: dsdi_ 14 :*/
/*:ref: dsics_ 14 13 4 4 4 4 7 4 4 4 4 7 7 7 4 */
/*:ref: xerrwv_ 14 11 13 4 4 4 4 4 4 4 6 6 124 */
/*:ref: dsllti_ 14 :*/
/*:ref: dnrm2_ 7 3 4 7 4 */
extern int dcgn_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*matvec)(), int (*mttvec)(), int (*msolve)(), const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *z, double *p, double *atp, double *atz, double *dz, double *atdz, double *rwork, long *iwork);
extern int dsdcgn_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern int dslucn_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern long isdcgn_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*matvec)(), int (*mttvec)(), int (*msolve)(), const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *z, double *p, double *atp, double *atz, double *dz, double *atdz, double *rwork, long *iwork, double *ak, double *bk, double *bnrm, double *solnrm);
/* comlen solblk_ 8 */
/*:ref: d1mach_ 7 1 4 */
/*:ref: ddot_ 7 5 4 7 4 7 4 */
/*:ref: dcopy_ 14 5 4 7 4 7 4 */
/*:ref: daxpy_ 14 6 4 7 7 4 7 4 */
/*:ref: ds2y_ 14 6 4 4 4 4 7 4 */
/*:ref: dchkw_ 14 9 13 4 4 4 4 4 4 7 124 */
/*:ref: dsd2s_ 14 7 4 4 4 4 7 4 7 */
/*:ref: dsmv_ 14 :*/
/*:ref: dsmtv_ 14 :*/
/*:ref: dsdi_ 14 :*/
/*:ref: dsilus_ 14 17 4 4 4 4 7 4 4 4 4 7 7 4 4 4 7 4 4 */
/*:ref: dsmmti_ 14 :*/
/*:ref: dnrm2_ 7 3 4 7 4 */
extern int dcgs_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*matvec)(), int (*msolve)(), const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *r0, double *p, double *q, double *u, double *v1, double *v2, double *rwork, long *iwork);
extern int dsdcgs_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern int dslucs_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern long isdcgs_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*matvec)(), int (*msolve)(), const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *r0, double *p, double *q, double *u, double *v1, double *v2, double *rwork, long *iwork, double *ak, double *bk, double *bnrm, double *solnrm);
/* comlen solblk_ 8 */
/*:ref: d1mach_ 7 1 4 */
/*:ref: ddot_ 7 5 4 7 4 7 4 */
/*:ref: daxpy_ 14 6 4 7 7 4 7 4 */
/*:ref: ds2y_ 14 6 4 4 4 4 7 4 */
/*:ref: dchkw_ 14 9 13 4 4 4 4 4 4 7 124 */
/*:ref: dsds_ 14 7 4 4 4 4 7 4 7 */
/*:ref: dsmv_ 14 :*/
/*:ref: dsdi_ 14 :*/
/*:ref: dsilus_ 14 17 4 4 4 4 7 4 4 4 4 7 7 4 4 4 7 4 4 */
/*:ref: dslui_ 14 :*/
/*:ref: dnrm2_ 7 3 4 7 4 */
extern int dgmres_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*matvec)(), int (*msolve)(), const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *sb, double *sx, double *rgwk, long *lrgw, long *igwk, long *ligw, double *rwork, long *iwork);
extern int dsdgmr_(const int *n, const double *b, double *x, const long *nelt, int *ia, int *ja, double *a, const long *isym, long *nsave, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern int dslugm_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, long *nsave, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern int dhels_(double *a, long *lda, const long *n, double *q, double *b);
extern int dheqr_(double *a, long *lda, const long *n, double *q, long *info, long *ijob);
extern int dorth_(double *vnew, double *v, double *hes, const long *n, long *ll, long *ldhes, long *kmp, double *snormw);
extern int dpigmr_(const long *n, double *r0, double *sr, double *sz, long *jscal, long *maxl, long *maxlp1, long *kmp, long *nrsts, long *jpre, int (*matvec)(), int (*msolve)(), long *nmsl, double *z, double *v, double *hes, double *q, long *lgmr, double *rpar, long *ipar, double *wk, double *dl, double *rhol, long *nrmax, const double *b, double *bnrm, double *x, double *xl, const long *itol, const double *tol, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *iunit, long *iflag, double *err);
extern int drlcal_(const long *n, long *kmp, long *ll, long *maxl, double *v, double *q, double *rl, double *snormw, double *prod, double *r0nrm);
extern int dxlcal_(const long *n, long *lgmr, double *x, double *xl, double *zl, double *hes, long *maxlp1, double *q, double *v, double *r0nrm, double *wk, double *sz, long *jscal, long *jpre, int (*msolve)(), long *nmsl, double *rpar, long *ipar, const long *nelt, long *ia, long *ja, double *a, long *isym);
extern long isdgmr_(const long *n, const double *b, double *x, double *xl, const long *nelt, double *ia, double *ja, double *a, const long *isym, int (*msolve)(), long *nmsl, const long *itol, const double *tol, const long *itmax, long *iter, double *err, const long *iunit, double *r, double *z, double *dz, double *rwork, double *iwork, double *rnrm, double *bnrm, double *sb, double *sx, long *jscal, long *kmp, long *lgmr, long *maxl, long *maxlp1, double *v, double *q, double *snormw, double *prod, double *r0nrm, double *hes, long *jpre);
/* comlen solblk_ 8 */
/*:ref: d1mach_ 7 1 4 */
/*:ref: dcopy_ 14 5 4 7 4 7 4 */
/*:ref: dnrm2_ 7 3 4 7 4 */
/*:ref: ds2y_ 14 6 4 4 4 4 7 4 */
/*:ref: dchkw_ 14 9 13 4 4 4 4 4 4 7 124 */
/*:ref: dsds_ 14 7 4 4 4 4 7 4 7 */
/*:ref: dsmv_ 14 :*/
/*:ref: dsdi_ 14 :*/
/*:ref: dsilus_ 14 17 4 4 4 4 7 4 4 4 4 7 7 4 4 4 7 4 4 */
/*:ref: dslui_ 14 :*/
/*:ref: daxpy_ 14 6 4 7 7 4 7 4 */
/*:ref: ddot_ 7 5 4 7 4 7 4 */
/*:ref: dscal_ 14 4 4 7 7 4 */
/* Rerunning f2c -P may change prototypes or declarations. */
extern int dir_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*matvec)(), int (*msolve)(), const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *z, double *dz, double *rwork, long *iwork);
extern int dsjac_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern int dsgs_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern int dsilur_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern long isdir_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*msolve)(), const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *z, double *dz, double *rwork, long *iwork, double *bnrm, double *solnrm);
/* comlen solblk_ 8 */
/*:ref: d1mach_ 7 1 4 */
/*:ref: dchkw_ 14 9 13 4 4 4 4 4 4 7 124 */
/*:ref: ds2y_ 14 6 4 4 4 4 7 4 */
/*:ref: dsds_ 14 7 4 4 4 4 7 4 7 */
/*:ref: dsmv_ 14 :*/
/*:ref: dsdi_ 14 :*/
/*:ref: ds2lt_ 14 10 4 4 4 4 7 4 4 4 4 7 */
/*:ref: dsli_ 14 :*/
/*:ref: dsilus_ 14 17 4 4 4 4 7 4 4 4 4 7 7 4 4 4 7 4 4 */
/*:ref: dslui_ 14 :*/
/*:ref: dnrm2_ 7 3 4 7 4 */
extern int dbhin_(const long *n, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *soln, double *rhs, const long *iunit, long *job);
extern int dchkw_(char *name, long *lociw, const long *leniw, long *locw, const long *lenw, long *ierr, long *iter, double *err, int name_len);
extern int qs2i1d_(long *ia, long *ja, double *a, const long *n, long *kflag);
extern int ds2y_(const long *n, const long *nelt, long *ia, long *ja, double *a, long *isym);
extern int dcpplt_(const long *n, const long *nelt, long *ia, long *ja, double *a, const long *isym, const long *iunit);
extern int dtout_(const long *n, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *soln, double *rhs, const long *iunit, long *job);
extern int dtin_(const long *n, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *soln, double *rhs, const long *iunit, long *job);
/*:ref: d1mach_ 7 1 4 */
/*:ref: xerrwv_ 14 11 13 4 4 4 4 4 4 4 6 6 124 */
/*:ref: xerror_ 14 5 13 4 4 4 124 */
extern int dsds_(const long *n, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *dinv);
extern int dsdscl_(const long *n, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *x, const double *b, double *dinv, long *job, long *itol);
extern int dsd2s_(const long *n, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *dinv);
extern int ds2lt_(const long *n, const long *nelt, long *ia, long *ja, double *a, const long *isym, long *nel, long *iel, long *jel, double *el);
extern int dsics_(const long *n, const long *nelt, long *ia, long *ja, double *a, const long *isym, long *nel, long *iel, long *jel, double *el, double *d, double *r, long *iwarn);
extern int dsilus_(const long *n, const long *nelt, long *ia, long *ja, double *a, const long *isym, long *nl, long *il, long *jl, double *l, double *dinv, long *nu, long *iu, long *ju, double *u, long *nrow, long *ncol);
/* comlen solblk_ 8 */
/*:ref: xerrwv_ 14 11 13 4 4 4 4 4 4 4 6 6 124 */
extern int dsmv_(const long *n, double *x, double *y, const long *nelt, long *ia, long *ja, double *a, long *isym);
extern int dsmtv_(const long *n, double *x, double *y, const long *nelt, long *ia, long *ja, double *a, long *isym);
extern int dsdi_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *rwork, long *iwork);
extern int dsli_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *rwork, long *iwork);
extern int dsli2_(const long *n, const double *b, double *x, long *nel, long *iel, long *jel, double *el);
extern int dsllti_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *rwork, long *iwork);
extern int dllti2_(const long *n, const double *b, double *x, long *nel, long *iel, long *jel, double *el, double *dinv);
extern int dslui_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *rwork, long *iwork);
extern int dslui2_(const long *n, const double *b, double *x, long *il, long *jl, double *l, double *dinv, long *iu, long *ju, double *u);
extern int dsluti_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *rwork, long *iwork);
extern int dslui4_(const long *n, const double *b, double *x, long *il, long *jl, double *l, double *dinv, long *iu, long *ju, double *u);
extern int dsmmti_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, double *rwork, long *iwork);
extern int dsmmi2_(const long *n, const double *b, double *x, long *il, long *jl, double *l, double *dinv, long *iu, long *ju, double *u);
extern int domn_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*matvec)(), int (*msolve)(), long *nsave, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *z, double *p, double *ap, double *emap, double *dz, double *csav, double *rwork, long *iwork);
extern int dsdomn_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, long *nsave, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern int dsluom_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, long *nsave, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *rwork, const long *lenw, long *iwork, const long *leniw);
extern long isdomn_(const long *n, const double *b, double *x, const long *nelt, long *ia, long *ja, double *a, const long *isym, int (*msolve)(), long *nsave, const long *itol, const double *tol, const long *itmax, long *iter, double *err, long *ierr, const long *iunit, double *r, double *z, double *p, double *ap, double *emap, double *dz, double *csav, double *rwork, long *iwork, double *ak, double *bnrm, double *solnrm);
/* comlen solblk_ 8 */
/*:ref: d1mach_ 7 1 4 */
/*:ref: dcopy_ 14 5 4 7 4 7 4 */
/*:ref: ddot_ 7 5 4 7 4 7 4 */
/*:ref: daxpy_ 14 6 4 7 7 4 7 4 */
/*:ref: ds2y_ 14 6 4 4 4 4 7 4 */
/*:ref: dchkw_ 14 9 13 4 4 4 4 4 4 7 124 */
/*:ref: dsds_ 14 7 4 4 4 4 7 4 7 */
/*:ref: dsmv_ 14 :*/
/*:ref: dsdi_ 14 :*/
/*:ref: dsilus_ 14 17 4 4 4 4 7 4 4 4 4 7 7 4 4 4 7 4 4 */
/*:ref: dslui_ 14 :*/
/*:ref: dnrm2_ 7 3 4 7 4 */
