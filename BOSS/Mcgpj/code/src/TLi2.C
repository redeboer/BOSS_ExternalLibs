#include <cmath>
#include "TLi2.h"

#define H00 .267652639082732605202e-0
#define H01 .287682072451780916527e-0
#define H02 .228256304407765443676e-1
#define H03 .330143155800177691582e-2
#define H04 .610346084574433927254e-3
#define H05 .129007082934408056818e-3
#define H06 .296683089332297434665e-4
#define H07 .723054765493866154886e-5
#define H08 .183840162262654437189e-5
#define H09 .482749728124851384963e-6
#define H10 .130030954762267291337e-6
#define H11 .357504457581813602892e-7
#define H12 .998842103108304614670e-8
#define H13 .283121494557618801322e-8
#define H14 .824126647619321043570e-9
#define H15 .240539930902255726013e-9
#define H16 .606886706788283640174e-10
#define H17 .171549429260100878638e-10
#define H18 .967569971597286669658e-11
#define H19 .305051035251868842991e-11

#define K00 -.448414206923646205760e-0  
#define K01  .405465108108164360846e-0
#define K02  .360658873874161845522e-1  
#define K03  .552540640642657203433e-2
#define K04  .105763505171079838462e-2  
#define K05  .228824090740860163934e-3
#define K06  .535125313340340103575e-4  
#define K07  .132073575305965864612e-4
#define K08  .339130422475175972690e-5  
#define K09  .897606968058315434299e-6
#define K10  .243351017811575051344e-6  
#define K11  .672707775328645948125e-7
#define K12  .188813993393826052532e-7  
#define K13  .537309714837069990632e-8
#define K14  .156975759629269910672e-8  
#define K15  .459579714548034691074e-9
#define K16  .116082717316582652134e-9  
#define K17  .328819236160670711131e-10
#define K18  .186463598871037633001e-10  
#define K19  .588984177266277254402e-11

double TLi2::Li2Cern(const double &x){
  static long double pi3  = M_PI*M_PI/3; 
  static long double pi6  = M_PI*M_PI/6;
  static long double pi12 = M_PI*M_PI/12;
  static long double c[20]={0.42996693560813697L, 0.40975987533077105L,
			    -0.01858843665014592L, 0.00145751084062268L,
			    -0.00014304184442340L, 0.00001588415541880L,
			    -0.00000190784959387L, 0.00000024195180854L,
			    -0.00000003193341274L, 0.00000000434545063L,
			    -0.00000000060578480L, 0.00000000008612098L,
			    -0.00000000001244332L, 0.00000000000182256L,
			    -0.00000000000027007L, 0.00000000000004042L,
			    -0.00000000000000610L, 0.00000000000000093L,
			    -0.00000000000000014L, 0.00000000000000002L};
  long double h,t,y,s,l,ll,a,alfa,b0,b1,b2,*cp;
  static long double two = 2L, one=1L, half=0.5L, zero = 0L;
  if(x == one){
    h = pi6;
  }else if(x == -one){
    h = -pi12;
  }else{
    t = -x;
    if(t <= -two){
      y =-one/(one + t);
      s = one;
      l = log(-t);
      ll = log(one + one/t);
      a =-pi3 + half*(l*l - ll*ll);
    }else if(t < -one){
      y =-one-t;
      s =-one;
      a = log(-t);
      a =-pi6 + a*(a + log(one + one/t));
    }else if(t <= -half){
      y =-(one+t)/t;
      s = one;
      a = log(-t);
      a =-pi6 + a*(-half*a+log(one + t));
    }else if(t < zero){
      y =-t/(one + t);
      s =-one;
      a = half*log(one + t)*log(one + t);
    }else if(t <= one){
      y = t;
      s = one;
      a = zero;
    }else{
      y = one/t;
      s =-one;
      a = pi6 + half*log(t)*log(t);
    }
    h    = y + y - one;
    alfa = h + h;
    b1   = zero;
    b2   = zero;
    cp   = &c[19];
    for(int i = 20; i; i--,cp--){
      b0 = *cp + alfa*b1 - b2;
      b2 = b1;
      b1 = b0;
    }
    h    = -(s*(b0-h*b2)+a);
  }
  //  std::cout<<"TLI2 "<<x<<" "<<h<<std::endl;
  return h;
}

double TLi2::Li2My(const double &x){
  const double  pi3  =  M_PI*M_PI/3; 
  const double  pi6  =  M_PI*M_PI/6;
  const double mpi12 = -M_PI*M_PI/12;
  const double two = 2, one=1, half=0.5, zero = 0;
  static double t,h,y,s,a,l,ll;
  if ( x ==  one) return  pi6;
  if ( x == -one) return mpi12;
  t = -x;
  if(t <= -two){
    // 2 < x < +inf
    y =-one/(one + t);
    s = one;
    l = log(-t);
    ll= log(one + one/t);
    a =-pi3 + half*(l*l - ll*ll);
  }else if(t < -one){
    // 1 < x < 2
    y =-one-t;
    s =-one;
    a = log(-t);
    a =-pi6 + a*(a + log(one + one/t));
  }else if(t <= -half){
    // 0.5 < x < 1
    y =-(one+t)/t;
    s = one;
    a = log(-t);
    a =-pi6 + a*(-half*a+log(one + t));
  }else if(t < zero){
    // 0 < x < 0.5
    h = 4*x - one;
    // minimax approximation
    return 
      H00+(H01+(H02+(H03+(H04+(H05+(H06+(H07+(H08+(H09+(H10+(H11+(H12+(H13+(H14+(H15+(H16+(H17+(H18+H19*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h;
  }else if(t <= one){
    // -1 < x < 0
    y = t;
    s = one;
    a = zero;
  }else{
    // -inf < x < -1
    y = one/t;
    s =-one;
    a = log(t);
    a = pi6 + half*a*a;
  }
  h = -two*y + one;
  // minimax approximation in  -1 < x < 0 range
  return
    (K00+(K01+(K02+(K03+(K04+(K05+(K06+(K07+(K08+(K09+(K10+(K11+(K12+(K13+(K14+(K15+(K16+(K17+(K18+K19*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*h)*s-a;
}
