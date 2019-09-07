// File: Ext_errmx.cc
//
// Description: Handle error matrix( x, y, z, px, py, pz ).
//              The used coordinate system is the cartesian 
//              BESIII coordinate system.The error matrix is 
//              now calculated in the Track Coordinate System( TCS ) 
//              instead of the BESIII Coordinate System( BCS )
//              in the previous version.
//              
//              Modified from BELLE:
//                                   Creation: 08-Jan-1998
//                                   Version: 05-Mar-1999
//              by Wang Liangliang
// 
// Date: 2005.3.30                          
//

#include <iostream>

#include "TrkExtAlg/Ext_errmx.h"

extern bool Ext_err_valid( bool msg, const HepSymMatrix &error, 
			  const int dimension );
static const double Eps(1.0e-12);	// Infinitesimal number.
static const double Infinite(1.0e+12);	// Infinite number.

// Default constructor

Ext_errmx::Ext_errmx() : m_err(Ndim_err,0), m_err3(3,0), m_R(3,3,0), 
	m_err2(2), m_valid(0) {}

// Copy constructor

Ext_errmx::Ext_errmx( const Ext_errmx &err ) : m_err(err.m_err),
	m_err3(err.m_err3), m_R(err.m_R), m_err2(err.m_err2), 
	m_valid(err.m_valid) {}

Ext_errmx::Ext_errmx( const HepSymMatrix &err ) : m_err(err), 
	m_err3(3,0), m_R(3,3,0), m_err2(2), m_valid(1) {}

/*
  Put the error matrix.
*/

void Ext_errmx::put_err( const double error[] )
{
  int ne = 0;

  for( int i = 1; i <= m_err.num_col(); i++ ){
    for( int j = 1; j <= i; j++ ){
      m_err.fast( i, j ) = error[ ne++ ];
    }
  }
  m_valid = 1;
}

/*
  Setup m_err3 and m_R for get_plane_err(s).
*/
void Ext_errmx::set_plane_errs( const Hep3Vector &nx, const Hep3Vector &ny, 
			       const Hep3Vector &nz ) const
{
// Setup the rotation matrix.

  m_R( 1, 1 ) = nx.x();
  m_R( 1, 2 ) = nx.y();
  m_R( 1, 3 ) = nx.z();
  m_R( 2, 1 ) = ny.x();
  m_R( 2, 2 ) = ny.y();
  m_R( 2, 3 ) = ny.z();
  m_R( 3, 1 ) = nz.x();
  m_R( 3, 2 ) = nz.y();
  m_R( 3, 3 ) = nz.z();

// Get the 3x3 sub matrix and Rotate the error matrix.

  m_err3 = (m_err.sub( 1, 3 )).similarity( m_R );

}

/*
  Get the projection of error on the plane along the readout direction. 
*/

double Ext_errmx::get_plane_err( const Hep3Vector &np, const Hep3Vector &nr ) 
	const
// ( Inputs )
// np -- Unit vector to the direction of the track.
// nr -- Readout direction unit vector on the plane.
//
// ( Outputs )
// HepVector[0] = Error( sigma ) along the readout direction.
// HepVector[1] = Error( sigma ) along the direction perp. to 
//			the readout direction.
{

// Check the validity of the error matrix.

  if( !(*this).valid( 0 ) ){
    //std::cout << "%WARNING at Ext_get_plane_err: You are trying to calculate"
//	<< " error \n           using an invalid error matrix." << std::endl;
  }

// Construct 3 TCS axes.

// x: nx --- unit vector which is perp to np and on the plane of nr and np.
// y: nz x nx
// z: nz( = np ) --- direction of the track.

  double nr_np( nr*np );
  double denom( 1.0 - nr_np*nr_np );
  double error( 0.0 );

  if( denom > Eps ){	// Track is not parallel to the readout direction.

    double fac( 1.0 / sqrt( denom ) );
    Hep3Vector nx( ( nr - nr_np * np ) * fac );
    Hep3Vector ny( np.cross( nx ) );

    (*this).set_plane_errs( nx, ny, np );

    double sigma2( m_err3( 1, 1 ) );	// Error along nx.
    if( sigma2 > 0 ){
      error = sqrt( sigma2 ) * fac;	// Error projection.
    }

  } else {	// Track is parallel to the readout direction.

    error = Infinite;

  }
  return( error );
}

/*
  Get a projection of error on the plane along the readout direction
  and its perpendicular direction on the readout plane. 
*/

const HepVector &Ext_errmx::get_plane_errs( const Hep3Vector &np, 
		const Hep3Vector &nr, const Hep3Vector &nt ) const
// ( Inputs )
// np -- Unit vector to the direction of the track.
// nr -- Readout direction unit vector on the plane.
// nt -- Unit vector that is perp. to nr on the readout plane.
//
// ( Outputs )
// HepVector[0] = Error( sigma ) along the readout direction.
// HepVector[1] = Error( sigma ) along the direction perp. to 
//			the readout direction.
{

// Check the validity of the error matrix.

  if( !(*this).valid( 1 ) ){
//    std::cout << "%WARNING at Ext_get_plane_errs: You are trying to calculate"
//	<< " error \n           using an invalid error matrix." << std::endl;
  }

  double nr_np( nr*np );
  double denom_r( 1.0 - nr_np*nr_np );

  if( denom_r > Eps ){	// nr is not parallel to the track direction: np.

    double nt_np( nt*np );
    double denom_t( 1.0 - nt_np*nt_np );
    double fac_r( 1.0 / sqrt( denom_r ) );
    Hep3Vector nx( ( nr - nr_np * np ) * fac_r );
    Hep3Vector ny( np.cross( nx ) );

    (*this).set_plane_errs( nx, ny, np );

    double sigma2( m_err3( 1, 1 ) );	// Error along nx.
    if( sigma2 > 0 ){
      m_err2( 1 ) = sqrt( sigma2 ) * fac_r;	// Error projection.
    } else {
      m_err2( 1 ) = 0.0;
    }

    if( denom_t > Eps ){   // nt is not parallel to the track direction: np.
      double fac_t( 1.0 / sqrt( denom_t ) );
      sigma2 = m_err3( 2, 2 );
      if( sigma2 > 0 ){
	m_err2( 2 ) = sqrt( sigma2 ) * fac_t;	// Error projection.
      } else {
	m_err2( 2 ) = 0.0;
      }
    } else {	// nt is parallel to the track direction: np.
      m_err2( 2 ) = (*this).get_plane_err( np, nt );
    }
  } else {	// nr is parallel to the track direction: np.
    m_err2( 1 ) = (*this).get_plane_err( np, nr );
    m_err2( 2 ) = (*this).get_plane_err( np, nt );
  }
  return( m_err2 );
}

/*
  Get the 2D projected track error perpendicular to a given vector at the 
  current point. pv: momentum vector, for example.
  view=1. Hep3Vector(1)= error(sigma) along the direction 
		which is perpendicular to pv on the xy plane.
          Hep3Vector(2)= error(sigma) along the direction 
		which is (pv) x (Hep3Vector(1) direction).
  view=2. Hep3Vector(1)= error(sigma) along the direction 
		which is perpendicular to pv on the zy plane.
          Hep3Vector(2)= error(sigma) along the direction 
		which is (pv) x (Hep3Vector(1) direction).
  view=3. Hep3Vector(1)= error(sigma) along the direction 
		which is perpendicular to pv on the zx plane.
          Hep3Vector(2)= error(sigma) along the direction 
		which is (pv) x (Hep3Vector(1) direction).
*/

const Hep3Vector * Ext_errmx::get_tvs( const int view, 
				     const Hep3Vector &pv ) const
// ( Inputs )
// view  -- 2D projection view. = 1 xy, =2 zy, =3 zx.
// pv    -- A vector for which the perpendicular error will be calculated,
//	    for example, momentum of the track.
{

  Hep3Vector	np( pv.unit() );
  Hep3Vector	nr;

  switch( view ){

  case 1:	// xy view

    if( np.x() != 0 || np.y() != 0 ){
      nr.setX(  np.y() );
      nr.setY( -np.x() );
      nr = nr.unit();
    } else {		// Pointing to z-direction.
      nr.setX( 1 );
    }
    break;

  case 2:	// zy view

    if( np.y() != 0 || np.z() != 0 ){
      nr.setY( -np.z() );
      nr.setZ(  np.y() );
      nr = nr.unit();
    } else {		// Pointing to x-direction.
      nr.setZ( 1 );
    }
    break;

  case 3:	// zx view

    if( np.z() != 0 || np.x() != 0 ){
      nr.setX( -np.z() );
      nr.setZ(  np.x() );
      nr = nr.unit();
    } else {		// Pointing to z-direction.
      nr.setZ( 1 );
    }
    break;
  }	/* End of switch */

  Hep3Vector nt( np.cross( nr ) );
  const HepVector & err_v = (*this).get_plane_errs( np, nr, nt );
  *m_nv		= err_v[0]*nr;
  *(m_nv+1)	= err_v[1]*nt;
  return( m_nv );
}

/*
  Get the 2D projected track error perpendicular to a given vector at the 
  current point. pv: momentum vector, for example.
  Hep3Vector(1)= error(sigma) along the direction which is
		perpendicular to pv on the plane formed by pv and z-axis.
  Hep3Vector(2)= error(sigma) along the direction which is
		(pv) x (Hep3Vector(1) direction).
*/

const Hep3Vector * Ext_errmx::get_tvs( const Hep3Vector &pv ) const
// ( Inputs )
// pv    -- A vector for which the perpendicular error will be calculated,
//	    for example, momentum of the track.
{

  Hep3Vector	np( pv.unit() );
  Hep3Vector	nz( 0.0, 0.0, 1.0 );
  Hep3Vector	nt( (nz.cross(np)).unit() );
  Hep3Vector	nr( nt.cross(np) );

  const HepVector & err_v = (*this).get_plane_errs( np, nr, nt );
  *m_nv		= err_v[0]*nr;
  *(m_nv+1)	= err_v[1]*nt;
  return( m_nv );

}

/*
  valid( msg ). Check the validity of the diagonal elements.
*/

bool Ext_errmx::valid( bool msg ) const 
{
  if( m_valid ){
    if( Ext_err_valid( msg, m_err, 6 ) ){
      return 1;
    } else {
      m_valid = 0;
      return 0;
    }
  } else {
    return 0;
  }
}

// Assign "=" operator

Ext_errmx &Ext_errmx::operator=( const Ext_errmx &err )
{
  if( this != &err ){
    m_err	= err.m_err;
    m_err3	= err.m_err3;
    m_err2	= err.m_err2;
    m_R		= err.m_R;
    m_valid	= err.m_valid;
    *m_nv	= *err.m_nv;
    *(m_nv+1)	= *(err.m_nv+1);
  }
  return *this;
}

/*
 ostream "<<" operator
*/
std::ostream &operator<<( std::ostream &s, const Ext_errmx &err )
{
  s << " m_valid: " << err.m_valid << '\n'
	<< "m_err: " << err.m_err << " m_err3: " << err.m_err3 
	<< " m_R: " << err.m_R << " m_err2: " << err.m_err2
	<< " *m_nv: " << *err.m_nv 
	<< " *(m_nv+1): " << *(err.m_nv+1)
	<< std::endl;
  return s;
}
