//------------------------------------------------------------------------------|
//      [File  ]:                       MucPadChain.h  	                        |
//      [Brief ]:       Head file of class MucPadChain for MUC calibration      |
//      [Author]:       Xie Yuguang, <ygxie@mail.ihep.ac.cn>                    |
//      [Date  ]:       May 25, 2006                                            |
//      [Log   ]:       See ChangLog                                            |
//------------------------------------------------------------------------------|

#ifndef MUC_PAD_CHAIN_H
#define MUC_PAD_CHAIN_H

#include<iostream>

class MucPadChain {

	public:
		MucPadChain( int padNumber );
		MucPadChain( int padNumber, double padWidth );
		MucPadChain( int padNumber, double padWidth, double aveEff );
		MucPadChain( int part, int segment, int layer, int strip, int padNumber );
		
		MucPadChain& operator=( const MucPadChain& );
		
		double	GetEff( int padID );
		double  GetAvaEff();
		int	GetSize();
		int 	GetHit();
		int	GetHit( int padID );
		int	GetTrack();
		int 	GetTrack( int padID );
		bool 	IsEmpty();
		
		
		void	AddHit( int padID );
		void 	AddTrack( int padID );
		void	SetEff();
		void	SetEff( int padID, double eff );
		void 	SetSize( int size );
		void 	SetPadWidth( double width );
		void	SetDeathFlag( int padID );
		void	SetFiredFlag( int padID );
			
	private:
	
   static const int	m_MaxSize  = 500;
   static const int 	m_MaxWidth = 100;
   static const int 	m_DeathFlag = -1;
		int 	m_Size;
		int	m_PadWidth;
		
	vector<	int >	m_Hit;
	vector<	int >	m_Track;
	vector<	double> m_Eff;		
}

#endif
		
