#ifndef ExtBesMagneticField_H
#define ExtBesMagneticField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"
#include "MagneticField/IMagneticFieldSvc.h"

class G4FieldManager;

class ExtBesMagneticField : public G4MagneticField
{
	public:
		ExtBesMagneticField();
		~ExtBesMagneticField();
		
		G4FieldManager* GetFieldManager();
		void GetFieldValue( const  double Point[3],double *Bfield ) const;

	private:
                IMagneticFieldSvc* m_pIMF;
};

#endif
