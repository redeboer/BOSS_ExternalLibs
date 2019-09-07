// File: Ext_xp_err.cc
//
// Descrition: Handle extrapolation error matrix( x, y, z, px, py, pz ).
//             The used coordinate system is the cartesian BESIII coordinate system.
//
//             This takes a base class, Ext_errmx.
//
//             The initialization has to be done by Ext_xp_err::set_err.
//             
//             
//             Modified from BELLE by Wang Liangliang
// 
// Data: 2005.3.30
// 

#include	<math.h>
#include	<stdlib.h>
#include	"TrkExtAlg/Ext_xp_err.h"



// Constants.

static const double	Cinv( 3335.640952 );// 1/c in cm*GeV/KGauss unit.
//static const double	C( 0.0002997924582 );// Light speed in KGauss/(cm*GeV) unit.
static const double   C( 0.002997924582 );// Light speed in tesla/(cm*Gev) unit. (by Wangll)
static const double	Small( 0.01 );// small number.
static const double	Eps(1.0e-12);// Infinitesimal number.
static const double	Infinite( 1.0e10 );// Infinite number.

static const int	Ndim_herr( 5 );// Dimension of helix error matrix.

// Default constructor
Ext_xp_err::Ext_xp_err() :
	m_xv(3), m_pv(3), m_xp_jcb(Ndim_err,Ndim_err,0), 
	m_h2xp_jcb(Ndim_err,Ndim_herr,0), m_q(0), m_mass2(0){}

// Copy constructor
Ext_xp_err::Ext_xp_err( const Ext_xp_err &err ) :
	Ext_errmx( (Ext_errmx)err ), m_xv(err.m_xv), m_pv(err.m_pv), 
	m_xp_jcb(err.m_xp_jcb), m_h2xp_jcb(err.m_h2xp_jcb), m_q( err.m_q ), 
	m_mass2( err.m_mass2 ){}

/*
  Set error matrix( symmetry matrix ). This initializes the error matrix.
*/
void Ext_xp_err::set_err( const HepSymMatrix &err, const Hep3Vector &xv,
			const Hep3Vector &pv, 
			const double &q, const double &mass )
// (Inputs)
// err -- Track error (symmetry) matrix for (x,y,z,px,py,pz) form.
// xv  -- coordinate(x,y,z) at the initialization.
// pv  -- momentum(px,py,pz) at the initialization.
// q   -- charge of the particle, either 1 or -1.
// mass - Mass of the particle in GeV/c**2 unit.
//
{
  Ext_errmx *errmx_ptr = (Ext_errmx *)this;
  errmx_ptr->put_err( err );

  m_xv = xv;
  m_pv = pv;
  m_q = q;
  m_mass2 = mass * mass;

// We do NOT check the validity of the error matrix here.

}



/*
  Transform the error matrix for the transformation: x -> x'
*/
bool Ext_xp_err::move( const Hep3Vector &xv1, const Hep3Vector &pv1,
		     const Hep3Vector &B, const int ms_on, 
		     const double chi_cc)
{
// (Inputs)
// xv1 -- Coordinate(x',y',z') after transformation.
// pv1 -- Momentum(px',py',pz') after transformation.
// B   -- B-field(Bx,By,Bz) at the transformation.
// ms_on -- Flag to switch on/off the multiple scattering effect.
//		ms_on = 0 for switch off. ms_on = 1 for switch on.
// chi_cc -- Constant of Moliere theory.
//
// (Outputs)
// return - = 1 for success, = 0 for failed.
//
  double dx( ( xv1 - m_xv ).mag() );
  double dp( ( pv1 - m_pv ).mag() );
  double p2( m_pv.mag2() );
  double p_abs( sqrt( p2 ) );

  double p_inv;
  if( p_abs > Small && pv1.mag() > Small ){
    p_inv = 1.0 / p_abs;
  } else {
    p_inv = Infinite;
    return 0;
  }

  double ms_coeff( 2.557 * chi_cc );
  bool with_B( ( B.mag() > Small ) ? 1 : 0 );
  double p2inv( p_inv * p_inv );
  double p3inv( p2inv * p_inv );
  double fdx( dx * p3inv );
  double cx( 100.*m_q * C * fdx );//*100 due to units problem by L.L.Wang
  double fdp( dp * p_inv );

  double px(  m_pv.x() );
  double py(  m_pv.y() );
  double pz(  m_pv.z() );
  double px2( px * px );
  double py2( py * py );
  double pz2( pz * pz );
  double pxy( px * py );
  double pyz( py * pz );
  double pzx( pz * px );
  double Bx(  B.x() );
  double By(  B.y() );
  double Bz(  B.z() );

  m_xp_jcb( 1, 1 ) = 1.0;			// dx'/dx
  m_xp_jcb( 1, 4 ) = fdx * ( py2 + pz2 );	// dx'/dpx
  m_xp_jcb( 1, 5 ) = - fdx * pxy;		// dx'/dpy
  m_xp_jcb( 1, 6 ) = - fdx * pzx;		// dx'/dpz

  m_xp_jcb( 2, 2 ) = 1.0;			// dy'/dy
  m_xp_jcb( 2, 4 ) = - fdx * pxy;		// dy'/dpx
  m_xp_jcb( 2, 5 ) = fdx * ( pz2 + px2 );	// dy'/dpy
  m_xp_jcb( 2, 6 ) = - fdx * pyz;		// dy'/dpz

  m_xp_jcb( 3, 3 ) = 1.0;			// dz'/dz
  m_xp_jcb( 3, 4 ) = - fdx * pzx;		// dz'/dpx
  m_xp_jcb( 3, 5 ) = - fdx * pyz;		// dz'/dpy
  m_xp_jcb( 3, 6 ) = fdx * ( px2 + py2 );	// dz'/dpz

  m_xp_jcb( 4, 4 ) = 1.0 - fdp;			// dx'/dx energy loss
  m_xp_jcb( 5, 5 ) = 1.0 - fdp;			// dy'/dy energy loss
  m_xp_jcb( 6, 6 ) = 1.0 - fdp;			// dz'/dz energy loss

  if( with_B && m_q!=0. ){		// B != 0  and q !=0 case
    m_xp_jcb( 4, 4 ) += - cx * ( pxy * Bz - pzx * By );	 // dpx'/dpx
    m_xp_jcb( 4, 5 ) = cx * ( ( pz2 + px2 ) * Bz + pyz * By );   // dpx'/dpy
    m_xp_jcb( 4, 6 ) = - cx * ( ( px2 + py2 ) * By + pyz * Bz ); // dpx'/dpz

    m_xp_jcb( 5, 4 ) = - cx * ( ( py2 + pz2 ) * Bz + pzx * Bx ); // dpy'/dpx
    m_xp_jcb( 5, 5 ) +=  - cx * ( pyz * Bx - pxy * Bz ); 	 // dpy'/dpy
    m_xp_jcb( 5, 6 ) = cx * ( ( px2 + py2 ) * Bx + pzx * Bz );   // dpy'/dpz

    m_xp_jcb( 6, 4 ) = cx * ( ( py2 + pz2 ) * By + pxy * Bx );   // dpz'/dpx
    m_xp_jcb( 6, 5 ) = - cx * ( ( pz2 + px2 ) * Bx + pxy * By ); // dpz'/dpy
    m_xp_jcb( 6, 6 ) += - cx * ( pzx * By - pyz * Bx ); 	 // dpz'/dpz
  }

// error transformation.

  m_err = m_err.similarity( m_xp_jcb );

// Multiple scattering.

  if( ms_on ){
    double beta_p_inv( ( m_mass2 + p2 ) / ( p2 * p2 ) ); // 1/(p*beta)**2
    double th2( 100000.0 * ms_coeff * ms_coeff * beta_p_inv * dx );//mutiply 100000.0 by L.L.Wang 
                                                                   //due to units problem 
    double m11( th2 * dx * dx /3.0);
    double m12( 0.5 * th2 * dx * p_abs );
    double m22( th2 * p2 );

    if( p_abs > Eps ){
      double c1( px*p_inv );
      double c2( py*p_inv );
      double c3( pz*p_inv );
      double c12( - c1*c2 );
      double c13( - c1*c3 );
      double c23( - c2*c3 );
      double s1s( 1 - c1*c1 );
      double s2s( 1 - c2*c2 );
      double s3s( 1 - c3*c3 );

      m_err.fast( 1, 1 ) += m11*s1s;		// error(x,x)

      m_err.fast( 2, 1 ) += m11*c12;		// error(y,x)
      m_err.fast( 2, 2 ) += m11*s2s;		// error(y,y)

      m_err.fast( 3, 1 ) += m11*c13;		// error(z,x)
      m_err.fast( 3, 2 ) += m11*c23;		// error(z,y)
      m_err.fast( 3, 3 ) += m11*s3s;		// error(z,z)

      m_err.fast( 4, 1 ) += m12*s1s;		// error(px,x)
      m_err.fast( 4, 2 ) += m12*c12;		// error(px,y)
      m_err.fast( 4, 3 ) += m12*c13;		// error(px,z)
      m_err.fast( 4, 4 ) += m22*s1s;		// error(px,px)

      m_err.fast( 5, 1 ) += m12*c12;		// error(py,x)
      m_err.fast( 5, 2 ) += m12*s2s;		// error(py,y)
      m_err.fast( 5, 3 ) += m12*c23;		// error(py,z)
      m_err.fast( 5, 4 ) += m22*c12;		// error(py,px)
      m_err.fast( 5, 5 ) += m22*s2s;		// error(py,py)

      m_err.fast( 6, 1 ) += m12*c13;		// error(pz,x)
      m_err.fast( 6, 2 ) += m12*c23;		// error(pz,y)
      m_err.fast( 6, 3 ) += m12*s3s;		// error(pz,z)
      m_err.fast( 6, 4 ) += m22*c13;		// error(pz,px)
      m_err.fast( 6, 5 ) += m22*c23;		// error(pz,py)
      m_err.fast( 6, 6 ) += m22*s3s;		// error(pz,pz)
    }
  
 }

// Now store the new xv and pv.
  m_xv = xv1;
  m_pv = pv1;
  return 1;
}

//Assign "=" operator
Ext_xp_err &Ext_xp_err::operator=( const Ext_xp_err &err )
{
  if( this != &err ){
    *((Ext_errmx *)this) = err;
    m_xv	= err.m_xv;
    m_pv	= err.m_pv;
    m_xp_jcb	= err.m_xp_jcb;
    m_h2xp_jcb	= err.m_h2xp_jcb;
    m_q		= err.m_q;
    m_mass2	= err.m_mass2;
  }
  return *this;
}

/*
 ostream "<<" operator
*/
std::ostream &operator<<( std::ostream &s, const Ext_xp_err &err )
{
  s << "m_err: " << (Ext_errmx)err
    << std::endl
    << "m_xv: " << err.m_xv << std::endl << " abs(xv)= " << err.m_xv.mag()
    << std::endl
    << "m_pv: " << err.m_pv << std::endl << " abs(pv)= " << err.m_pv.mag()
    << std::endl
    << "m_xp_jcb: " << err.m_xp_jcb
    << std::endl
    << "m_h2xp_jcb: " << err.m_h2xp_jcb
    << std::endl
    << "m_q: " << err.m_q
    << std::endl
    << "m_mass: " << sqrt(err.m_mass2)
    << std::endl;
  return s;
}
