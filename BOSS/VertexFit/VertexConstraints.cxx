#include "VertexFit/VertexConstraints.h"
#include "VertexFit/TrackPool.h"
#include "VertexFit/BField.h"

const double VertexConstraints :: Alpha = -0.00299792458;
const int VertexConstraints::FixedVertex = 1;
const int VertexConstraints::CommonVertex = 2;

VertexConstraints::VertexConstraints()
{
	m_Ec.clear();
	m_Dc.clear();
	m_dc.clear();
	m_VD.clear();
	m_EVDE.clear();
	m_lambda0.clear();
	m_lambda.clear();
	m_covax.clear();
	m_ltrk.clear();
	m_type = 0;
}

void VertexConstraints::CommonVertexConstraints(std::vector<int> tlis) 
{
	m_ltrk = tlis;
	setType(commonVertex());
}

void VertexConstraints::FixedVertexConstraints(std::vector<int> tlis) 
{
	m_ltrk = tlis;
	setType(fixedVertex());
}

void VertexConstraints::UpdateConstraints(VertexParameter vpar, std::vector<WTrackParameter> wlis) 
{
	m_Ec.clear();
	m_Dc.clear();
	m_dc.clear();
	m_VD.clear();
	m_EVDE.clear();
	m_lambda0.clear();
	m_lambda.clear();
	m_covax.clear();

	switch(m_type) 
	{
		case FixedVertex: 
		{
			WTrackParameter wtrk;
			for (unsigned int i = 0; i < wlis.size(); i++) 
			{
				wtrk = wlis[i];
				HepLorentzVector p = wtrk.p();
				HepPoint3D x = wtrk.x();
				HepPoint3D delx = vpar.vx() - x;

				double afield = VertexFitBField::instance()->getCBz(vpar.Vx(), wtrk.X());
				double a = afield * (0.0+wtrk.charge());
				double J = a * (delx.x()*p.px() + delx.y()*p.py())/p.perp2();

				J = std::min(J, 1-1e-4);
				J = std::max(J, -1+1e-4);
				double Rx = delx.x() - 2*p.px() * (delx.x()*p.px() + delx.y()*p.py()) / p.perp2();
				double Ry = delx.y() - 2*p.py() * (delx.x()*p.px() + delx.y()*p.py()) / p.perp2();
				double S = 1.0/sqrt(1-J*J)/p.perp2();
	     
				// dc
				HepVector dc(2, 0);
				dc[0] = delx.y() * p.px() - delx.x() * p.py() -	0.5 * a * (delx.x() * delx.x() + delx.y() * delx.y());
				dc[1] = delx.z() - p.pz()/a*asin(J);
				m_dc.push_back(dc);
				// Ec
				HepMatrix Ec(2, 3, 0);
				m_Ec.push_back(Ec);
				// Dc
				HepMatrix Dc(2, 6, 0);
				Dc[0][0] = delx.y();
				Dc[0][1] = -delx.x();
				Dc[0][2] = 0;
				Dc[0][3] = p.py() + a * delx.x();
				Dc[0][4] = -p.px() + a * delx.y();
				Dc[0][5] = 0;
				Dc[1][0] = -p.pz() * S * Rx;
				Dc[1][1] = -p.pz() * S * Ry;
				Dc[1][2] = - asin(J) / a;
				Dc[1][3] = p.px() * p.pz() * S;
				Dc[1][4] = p.py() * p.pz() * S;
				Dc[1][5] = -1.0;
				m_Dc.push_back(Dc);
				// VD
				HepSymMatrix VD(2, 0);
				// int ifail;
				// VD = ((wtrk.Ew()).similarity(Dc)).inverse(ifail); // 2x2 matrix (Dc Ew Dc.T())
				m_VD.push_back(VD);
				// EVDE
				HepSymMatrix EVDE(3, 0);
				m_EVDE.push_back(EVDE);
				// lambda0
				HepVector lambda0(2, 0);
				m_lambda0.push_back(lambda0);
				// lambda
				HepVector lambda(2, 0);
				m_lambda.push_back(lambda);
				// covax
				HepMatrix covax(6, 3, 0);
				m_covax.push_back(covax);
				break;
			}
		}
		case CommonVertex:
		default: 
		{
			WTrackParameter wtrk;
			for (unsigned int i = 0; i < wlis.size(); i++)
			{
				wtrk = wlis[i];
				HepLorentzVector p = wtrk.p();
				HepPoint3D x = wtrk.x();
				HepPoint3D delx = vpar.vx() - x;
				if (wtrk.charge() == 0) 
				{
					//dc
					HepVector dc(2,0);
					dc[0] = p.pz()*delx.x() - p.px()*delx.z();
					dc[1] = p.pz()*delx.y() - p.py()*delx.z();
					m_dc.push_back(dc);
					//Ec
					HepMatrix Ec(2,3,0);
					Ec[0][0] = p.pz();
					Ec[0][1] = 0;
					Ec[0][2] = -p.px();
					Ec[1][0] = 0;
					Ec[1][1] = p.pz();
					Ec[1][2] = -p.py();
					m_Ec.push_back(Ec);
					//Dc
					HepMatrix Dc(2,6,0);
					Dc[0][0] = -delx.z();
					Dc[0][1] = 0;
					Dc[0][2] = delx.x();
					Dc[0][3] = -p.pz();
					Dc[0][4] = 0;
					Dc[0][5] = p.px();
					Dc[1][0] = 0;
					Dc[1][1] = -delx.z();
					Dc[1][2] = delx.y();
					Dc[1][3] = 0;
					Dc[1][4] = -p.pz();
					Dc[1][5] = p.py();
					m_Dc.push_back(Dc);
				} 
				else 
				{
					double afield = VertexFitBField::instance()->getCBz(vpar.Vx(), wtrk.X());
					double a = afield * (0.0+wtrk.charge());
					double J = a * (delx.x()*p.px() + delx.y()*p.py())/p.perp2();
					J = std::min(J, 1-1e-4);
					J = std::max(J, -1+1e-4);
					double Rx = delx.x() - 2*p.px() * (delx.x()*p.px() + delx.y()*p.py()) / p.perp2();
					double Ry = delx.y() - 2*p.py() * (delx.x()*p.px() + delx.y()*p.py()) / p.perp2();
					double S = 1.0/sqrt(1-J*J)/p.perp2();
					// dc
					HepVector dc(2, 0);
					dc[0] = delx.y() * p.px() - delx.x() * p.py() -	0.5 * a * (delx.x() * delx.x() + delx.y() * delx.y());
					dc[1] = delx.z() - p.pz()/a*asin(J);
					m_dc.push_back(dc);
					// Ec
					HepMatrix Ec(2, 3, 0);
					Ec[0][0] = -p.py()- a * delx.x();
					Ec[0][1] = p.px() - a * delx.y();
					Ec[0][2] = 0;
					Ec[1][0] = -p.px() * p.pz() * S ;
					Ec[1][1] = -p.py() * p.pz() * S ;
					Ec[1][2] = 1.0;
					m_Ec.push_back(Ec);
 					// Dc
					HepMatrix Dc(2, 6, 0);
					Dc[0][0] = delx.y();
					Dc[0][1] = -delx.x();
					Dc[0][2] = 0;
					Dc[0][3] = p.py() + a * delx.x();
					Dc[0][4] = -p.px() + a * delx.y();
					Dc[0][5] = 0;
					Dc[1][0] = -p.pz() * S * Rx;
					Dc[1][1] = -p.pz() * S * Ry;
					Dc[1][2] = - asin(J) / a;
					Dc[1][3] = p.px() * p.pz() * S;
					Dc[1][4] = p.py() * p.pz() * S;
					Dc[1][5] = -1.0;
					m_Dc.push_back(Dc);
				}
				// VD
				HepSymMatrix VD(2, 0);
				m_VD.push_back(VD);
				// EVDE
				HepSymMatrix EVDE(3, 0);
				m_EVDE.push_back(EVDE);
				// lambda0
				HepVector lambda0(2, 0);
				m_lambda0.push_back(lambda0);
				// lambda
				HepVector lambda(2, 0);
				m_lambda.push_back(lambda);
				// covax
				HepMatrix covax(6, 3, 0);
				m_covax.push_back(covax);
			}
			break;
		}
	}
}


