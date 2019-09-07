// This is "translation" of "subroutine  calg" from sub.f file to C++
//
// The function gausin(x) from CERNLIB replaced with
// M_SQRT2*TMath::ErfInverse(2*x-1) from ROOT
// (see http://en.wikipedia.org/wiki/Probit_function)

/*
      subroutine  calg(Px,e2)
c      real*8 Px,chi
c      Px=0.5
c      external sub
      real*8 e2
      external sub
c      chi = 0.0
      REAL Px
c      print*, "the value of Px is ",Px
      e2=gausin(Px)
c      print*, "the value of chi is ",e2
c      calg=chi
c      Px=0.1
c      print*, "the value of chi is ",e2
c      RETURN
      end
*/

#include <iostream>
#include <TMath.h>

using namespace std;

void CALG(double Px, double& e2)
{
   //    cout <<  "the value of Px is " << Px << endl;
   e2 = M_SQRT2*TMath::ErfInverse(2*Px-1);
   //    cout << "the value of chi is " << e2 << endl;
}
