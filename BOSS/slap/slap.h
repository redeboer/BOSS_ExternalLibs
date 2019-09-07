/* this file was generated using f2c -P command */

extern int sbcg_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*matvec)(), int (*mttvec)(), int (*msolve)(), int (*mtsolv)(), const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *z, float *p, float *rr, float *zz, float *pp, float *dz, float *rwork, long *iwork);
extern int ssdbcg_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern int sslubc_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern long issbcg_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*msolve)(), const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *z, float *p, float *rr, float *zz, float *pp, float *dz, float *rwork, long *iwork, float *ak, float *bk, float *bnrm, float *solnrm);
/* comlen solblk_ 4 */
/*:ref: r1mach_ 6 1 4 */
/*:ref: sdot_ 6 5 4 6 4 6 4 */
/*:ref: scopy_ 14 5 4 6 4 6 4 */
/*:ref: saxpy_ 14 6 4 6 6 4 6 4 */
/*:ref: ss2y_ 14 6 4 4 4 4 6 4 */
/*:ref: schkw_ 14 9 13 4 4 4 4 4 4 6 124 */
/*:ref: ssds_ 14 7 4 4 4 4 6 4 6 */
/*:ref: ssmv_ 14 :*/
/*:ref: ssmtv_ 14 :*/
/*:ref: ssdi_ 14 :*/
/*:ref: ssilus_ 14 17 4 4 4 4 6 4 4 4 4 6 6 4 4 4 6 4 4 */
/*:ref: sslui_ 14 :*/
/*:ref: ssluti_ 14 :*/
/*:ref: snrm2_ 6 3 4 6 4 */
extern int scg_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*matvec)(), int (*msolve)(), const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *z, float *p, float *dz, float *rwork, long *iwork);
extern int ssdcg_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern int ssiccg_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern long isscg_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*msolve)(), const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *z, float *p, float *dz, float *rwork, long *iwork, float *ak, float *bk, float *bnrm, float *solnrm);
/* comlen solblk_ 4 */
/*:ref: r1mach_ 6 1 4 */
/*:ref: sdot_ 6 5 4 6 4 6 4 */
/*:ref: scopy_ 14 5 4 6 4 6 4 */
/*:ref: saxpy_ 14 6 4 6 6 4 6 4 */
/*:ref: ss2y_ 14 6 4 4 4 4 6 4 */
/*:ref: schkw_ 14 9 13 4 4 4 4 4 4 6 124 */
/*:ref: ssds_ 14 7 4 4 4 4 6 4 6 */
/*:ref: ssmv_ 14 :*/
/*:ref: ssdi_ 14 :*/
/*:ref: ssics_ 14 13 4 4 4 4 6 4 4 4 4 6 6 6 4 */
/*:ref: xerrwv_ 14 11 13 4 4 4 4 4 4 4 6 6 124 */
/*:ref: ssllti_ 14 :*/
/*:ref: snrm2_ 6 3 4 6 4 */
extern int scgn_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*matvec)(), int (*mttvec)(), int (*msolve)(), const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *z, float *p, float *atp, float *atz, float *dz, float *atdz, float *rwork, long *iwork);
extern int ssdcgn_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern int sslucn_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern long isscgn_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*matvec)(), int (*mttvec)(), int (*msolve)(), const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *z, float *p, float *atp, float *atz, float *dz, float *atdz, float *rwork, long *iwork, float *ak, float *bk, float *bnrm, float *solnrm);
/* comlen solblk_ 4 */
/*:ref: r1mach_ 6 1 4 */
/*:ref: sdot_ 6 5 4 6 4 6 4 */
/*:ref: scopy_ 14 5 4 6 4 6 4 */
/*:ref: saxpy_ 14 6 4 6 6 4 6 4 */
/*:ref: ss2y_ 14 6 4 4 4 4 6 4 */
/*:ref: schkw_ 14 9 13 4 4 4 4 4 4 6 124 */
/*:ref: ssd2s_ 14 7 4 4 4 4 6 4 6 */
/*:ref: ssmv_ 14 :*/
/*:ref: ssmtv_ 14 :*/
/*:ref: ssdi_ 14 :*/
/*:ref: ssilus_ 14 17 4 4 4 4 6 4 4 4 4 6 6 4 4 4 6 4 4 */
/*:ref: ssmmti_ 14 :*/
/*:ref: snrm2_ 6 3 4 6 4 */
extern int scgs_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*matvec)(), int (*msolve)(), const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *r0, float *p, float *q, float *u, float *v1, float *v2, float *rwork, long *iwork);
extern int ssdcgs_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern int sslucs_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern long isscgs_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*matvec)(), int (*msolve)(), const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *r0, float *p, float *q, float *u, float *v1, float *v2, float *rwork, long *iwork, float *ak, float *bk, float *bnrm, float *solnrm);
/* comlen solblk_ 4 */
/*:ref: r1mach_ 6 1 4 */
/*:ref: sdot_ 6 5 4 6 4 6 4 */
/*:ref: saxpy_ 14 6 4 6 6 4 6 4 */
/*:ref: ss2y_ 14 6 4 4 4 4 6 4 */
/*:ref: schkw_ 14 9 13 4 4 4 4 4 4 6 124 */
/*:ref: ssds_ 14 7 4 4 4 4 6 4 6 */
/*:ref: ssmv_ 14 :*/
/*:ref: ssdi_ 14 :*/
/*:ref: ssilus_ 14 17 4 4 4 4 6 4 4 4 4 6 6 4 4 4 6 4 4 */
/*:ref: sslui_ 14 :*/
/*:ref: snrm2_ 6 3 4 6 4 */
extern int sgmres_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*matvec)(), int (*msolve)(), const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *sb, float *sx, float *rgwk, long *lrgw, long *igwk, long *ligw, float *rwork, long *iwork);
extern int ssdgmr_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, long *nsave, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern int sslugm_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, long *nsave, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern int shels_(float *a, long *lda, const long *n, float *q, float *b);
extern int sheqr_(float *a, long *lda, const long *n, float *q, long *info, long *ijob);
extern int sorth_(float *vnew, float *v, float *hes, const long *n, long *ll, long *ldhes, long *kmp, float *snormw);
extern int spigmr_(const long *n, float *r0, float *sr, float *sz, long *jscal, long *maxl, long *maxlp1, long *kmp, long *nrsts, long *jpre, int (*matvec)(), int (*msolve)(), long *nmsl, float *z, float *v, float *hes, float *q, long *lgmr, float *rpar, long *ipar, float *wk, float *dl, float *rhol, long *nrmax, const float *b, float *bnrm, float *x, float *xl, const long *itol, const float *tol, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *iunit, long *iflag, float *err);
extern int srlcal_(const long *n, long *kmp, long *ll, long *maxl, float *v, float *q, float *rl, float *snormw, float *prod, float *r0nrm);
extern int sxlcal_(const long *n, long *lgmr, float *x, float *xl, float *zl, float *hes, long *maxlp1, float *q, float *v, float *r0nrm, float *wk, float *sz, long *jscal, long *jpre, int (*msolve)(), long *nmsl, float *rpar, long *ipar, const long *nelt, long *ia, long *ja, float *a, long *isym);
extern long issgmr_(const long *n, const float *b, float *x, float *xl, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*msolve)(), long *nmsl, const long *itol, const float *tol, const long *itmax, long *iter, float *err, const long *iunit, float *r, float *z, float *dz, float *rwork, long *iwork, float *rnrm, float *bnrm, float *sb, float *sx, long *jscal, long *kmp, long *lgmr, long *maxl, long *maxlp1, float *v, float *q, float *snormw, float *prod, float *r0nrm, float *hes, long *jpre);
/* comlen solblk_ 4 */
/*:ref: r1mach_ 6 1 4 */
/*:ref: scopy_ 14 5 4 6 4 6 4 */
/*:ref: snrm2_ 6 3 4 6 4 */
/*:ref: ss2y_ 14 6 4 4 4 4 6 4 */
/*:ref: schkw_ 14 9 13 4 4 4 4 4 4 6 124 */
/*:ref: ssds_ 14 7 4 4 4 4 6 4 6 */
/*:ref: ssmv_ 14 :*/
/*:ref: ssdi_ 14 :*/
/*:ref: ssilus_ 14 17 4 4 4 4 6 4 4 4 4 6 6 4 4 4 6 4 4 */
/*:ref: sslui_ 14 :*/
/*:ref: saxpy_ 14 6 4 6 6 4 6 4 */
/*:ref: sdot_ 6 5 4 6 4 6 4 */
/*:ref: sscal_ 14 4 4 6 6 4 */
extern int sir_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*matvec)(), int (*msolve)(), const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *z, float *dz, float *rwork, long *iwork);
extern int ssjac_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern int ssgs_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern int ssilur_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern long issir_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*msolve)(), const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *z, float *dz, float *rwork, long *iwork, float *bnrm, float *solnrm);
/* comlen solblk_ 4 */
/*:ref: r1mach_ 6 1 4 */
/*:ref: schkw_ 14 9 13 4 4 4 4 4 4 6 124 */
/*:ref: ss2y_ 14 6 4 4 4 4 6 4 */
/*:ref: ssds_ 14 7 4 4 4 4 6 4 6 */
/*:ref: ssmv_ 14 :*/
/*:ref: ssdi_ 14 :*/
/*:ref: ss2lt_ 14 10 4 4 4 4 6 4 4 4 4 6 */
/*:ref: ssli_ 14 :*/
/*:ref: ssilus_ 14 17 4 4 4 4 6 4 4 4 4 6 6 4 4 4 6 4 4 */
/*:ref: sslui_ 14 :*/
/*:ref: snrm2_ 6 3 4 6 4 */
extern int sbhin_(const long *n, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *soln, float *rhs, const long *iunit, long *job);
extern int schkw_(char *name, long *lociw, const long *leniw, long *locw, const long *lenw, long *ierr, long *iter, float *err, int name_len);
extern int qs2i1r_(long *ia, long *ja, float *a, const long *n, long *kflag);
extern int ss2y_(const long *n, const long *nelt, long *ia, long *ja, float *a, long *isym);
extern int scpplt_(const long *n, const long *nelt, long *ia, long *ja, float *a, const long *isym, long *iunit);
extern int stout_(const long *n, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *soln, float *rhs, const long *iunit, long *job);
extern int stin_(const long *n, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *soln, float *rhs, const long *iunit, long *job);
/*:ref: r1mach_ 6 1 4 */
/*:ref: xerrwv_ 14 11 13 4 4 4 4 4 4 4 6 6 124 */
/*:ref: xerror_ 14 5 13 4 4 4 124 */
extern int ssds_(const long *n, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *dinv);
extern int ssdscl_(const long *n, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *x, const float *b, float *dinv, long *job, long *itol);
extern int ssd2s_(const long *n, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *dinv);
extern int ss2lt_(const long *n, const long *nelt, long *ia, long *ja, float *a, const long *isym, long *nel, long *iel, long *jel, float *el);
extern int ssics_(const long *n, const long *nelt, long *ia, long *ja, float *a, const long *isym, long *nel, long *iel, long *jel, float *el, float *d, float *r, long *iwarn);
extern int ssilus_(const long *n, const long *nelt, long *ia, long *ja, float *a, const long *isym, long *nl, long *il, long *jl, float *l, float *dinv, long *nu, long *iu, long *ju, float *u, long *nrow, long *ncol);
/* comlen solblk_ 4 */
/*:ref: xerrwv_ 14 11 13 4 4 4 4 4 4 4 6 6 124 */
extern int ssmv_(const long *n, float *x, float *y, const long *nelt, long *ia, long *ja, float *a, long *isym);
extern int ssmtv_(const long *n, float *x, float *y, const long *nelt, long *ia, long *ja, float *a, long *isym);
extern int ssdi_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *rwork, long *iwork);
extern int ssli_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *rwork, long *iwork);
extern int ssli2_(const long *n, const float *b, float *x, long *nel, long *iel, long *jel, float *el);
extern int ssllti_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *rwork, long *iwork);
extern int dllti2_(const long *n, const float *b, float *x, long *nel, long *iel, long *jel, float *el, float *dinv);
extern int sslui_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *rwork, long *iwork);
extern int sslui2_(const long *n, const float *b, float *x, long *il, long *jl, float *l, float *dinv, long *iu, long *ju, float *u);
extern int ssluti_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *rwork, long *iwork);
extern int sslui4_(const long *n, const float *b, float *x, long *il, long *jl, float *l, float *dinv, long *iu, long *ju, float *u);
extern int ssmmti_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, float *rwork, long *iwork);
extern int ssmmi2_(const long *n, const float *b, float *x, long *il, long *jl, float *l, float *dinv, long *iu, long *ju, float *u);
extern int somn_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*matvec)(), int (*msolve)(), long *nsave, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *z, float *p, float *ap, float *emap, float *dz, float *csav, float *rwork, long *iwork);
extern int ssdomn_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, long *nsave, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern int ssluom_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, long *nsave, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *rwork, const long *lenw, long *iwork, const long *leniw);
extern long issomn_(const long *n, const float *b, float *x, const long *nelt, long *ia, long *ja, float *a, const long *isym, int (*msolve)(), long *nsave, const long *itol, const float *tol, const long *itmax, long *iter, float *err, long *ierr, const long *iunit, float *r, float *z, float *p, float *ap, float *emap, float *dz, float *csav, float *rwork, long *iwork, float *ak, float *bnrm, float *solnrm);
/* comlen solblk_ 4 */
/*:ref: r1mach_ 6 1 4 */
/*:ref: scopy_ 14 5 4 6 4 6 4 */
/*:ref: sdot_ 6 5 4 6 4 6 4 */
/*:ref: saxpy_ 14 6 4 6 6 4 6 4 */
/*:ref: ss2y_ 14 6 4 4 4 4 6 4 */
/*:ref: schkw_ 14 9 13 4 4 4 4 4 4 6 124 */
/*:ref: ssds_ 14 7 4 4 4 4 6 4 6 */
/*:ref: ssmv_ 14 :*/
/*:ref: ssdi_ 14 :*/
/*:ref: ssilus_ 14 17 4 4 4 4 6 4 4 4 4 6 6 4 4 4 6 4 4 */
/*:ref: sslui_ 14 :*/
/*:ref: snrm2_ 6 3 4 6 4 */
