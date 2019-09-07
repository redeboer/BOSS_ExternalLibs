#include "MagneticField/MucMagneticField.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "CLHEP/Units/PhysicalConstants.h"
using namespace std;
using namespace CLHEP;

#ifndef BEAN
MucMagneticField::MucMagneticField()
{
  path = std::string(getenv( "MAGNETICFIELDROOT" ));
#else
MucMagneticField::MucMagneticField(const std::string Path)
{
  path = Path;
#endif
  readPar();
}
MucMagneticField::~MucMagneticField()
{
}
void MucMagneticField::getMucField(int part,int layer,int mat,
                                     HepPoint3D& r,HepVector3D& b) 
{
  double x,y,z,bmf,emf;
  x = r.x()/m;
  y = r.y()/m;
  z = r.z()/m;
  //barrel, layer is the number of iron layer, mat=0 iron, mat=1 air
  if(part==1)
  {
    if(layer==0&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipx0[i]; }
    if(layer==0&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapx0[i]; }
    if(layer==1&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipx1[i]; }
    if(layer==1&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapx1[i]; } 
    if(layer==2&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipx2[i]; }
    if(layer==2&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapx2[i]; }
    if(layer==3&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipx3[i]; }
    if(layer==3&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapx3[i]; }
    if(layer==4&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipx4[i]; }
    if(layer==4&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapx4[i]; } 
    if(layer==5&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipx5[i]; }
    if(layer==5&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapx5[i]; }
    if(layer==6&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipx6[i]; }
    if(layer==6&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapx6[i]; }
    if(layer==7&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipx7[i]; }
    if(layer==7&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapx7[i]; }
    if(layer==8&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipx8[i]; }  
    bmf = bp[0]+bp[1]*x+bp[2]*x*x+bp[3]*z+bp[4]*z*z+bp[5]*z*z*z+bp[6]*z*z*z*z
          +bp[7]*z*z*z*z*z+bp[8]*z*z*z*z*z*z+bp[9]*z*z*z*z*z*z*z+bp[10]*y+bp[11]*y*y
          +bp[12]*y*y*y+bp[13]*x*z+bp[14]*z*y+bp[15]*x*y+bp[16]*z*y*y+bp[17]*z*z*y; 
    b[0] = bmf*tesla;

    if(layer==0&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipy0[i]; }
    if(layer==0&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapy0[i]; }
    if(layer==1&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipy1[i]; }
    if(layer==1&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapy1[i]; }
    if(layer==2&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipy2[i]; }
    if(layer==2&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapy2[i]; }
    if(layer==3&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipy3[i]; }
    if(layer==3&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapy3[i]; }
    if(layer==4&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipy4[i]; }
    if(layer==4&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapy4[i]; }
    if(layer==5&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipy5[i]; }
    if(layer==5&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapy5[i]; }
    if(layer==6&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipy6[i]; }
    if(layer==6&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapy6[i]; }
    if(layer==7&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipy7[i]; }
    if(layer==7&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapy7[i]; }
    if(layer==8&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipy8[i]; }
    bmf = bp[0]+bp[1]*x+bp[2]*x*x+bp[3]*z+bp[4]*z*z+bp[5]*z*z*z+bp[6]*z*z*z*z
          +bp[7]*z*z*z*z*z+bp[8]*z*z*z*z*z*z+bp[9]*z*z*z*z*z*z*z+bp[10]*y+bp[11]*y*y
          +bp[12]*y*y*y+bp[13]*x*z+bp[14]*z*y+bp[15]*x*y+bp[16]*z*y*y+bp[17]*z*z*y;
    b[1] = bmf*tesla;

    if(layer==0&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipz0[i]; }
    if(layer==0&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapz0[i]; }
    if(layer==1&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipz1[i]; }
    if(layer==1&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapz1[i]; }
    if(layer==2&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipz2[i]; }
    if(layer==2&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapz2[i]; }
    if(layer==3&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipz3[i]; }
    if(layer==3&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapz3[i]; }
    if(layer==4&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipz4[i]; }
    if(layer==4&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapz4[i]; }
    if(layer==5&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipz5[i]; }
    if(layer==5&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapz5[i]; }
    if(layer==6&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipz6[i]; }
    if(layer==6&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapz6[i]; }
    if(layer==7&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipz7[i]; }
    if(layer==7&&mat==1) { for(int i=0; i<18; i++) bp[i] = bapz7[i]; }
    if(layer==8&&mat==0) { for(int i=0; i<18; i++) bp[i] = bipz8[i]; }
    bmf = bp[0]+bp[1]*x+bp[2]*x*x+bp[3]*z+bp[4]*z*z+bp[5]*z*z*z+bp[6]*z*z*z*z
          +bp[7]*z*z*z*z*z+bp[8]*z*z*z*z*z*z+bp[9]*z*z*z*z*z*z*z+bp[10]*y+bp[11]*y*y
          +bp[12]*y*y*y+bp[13]*x*z+bp[14]*z*y+bp[15]*x*y+bp[16]*z*y*y+bp[17]*z*z*y;
    b[2] = bmf*tesla;
  }
  //endcaps, layer is the number of iron layer, total 9. m=0 iron, m=1 air
  if(part==0)
  {
    if(layer==0&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipx0[i]; }
    if(layer==0&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapx0[i]; }
    if(layer==1&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipx1[i]; }
    if(layer==1&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapx1[i]; }
    if(layer==2&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipx2[i]; }
    if(layer==2&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapx2[i]; }
    if(layer==3&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipx3[i]; }
    if(layer==3&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapx3[i]; }
    if(layer==4&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipx4[i]; } 
    if(layer==4&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapx4[i]; }
    if(layer==5&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipx5[i]; }
    if(layer==5&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapx5[i]; }
    if(layer==6&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipx6[i]; }
    if(layer==6&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapx6[i]; } 
    if(layer==7&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipx7[i]; }
    if(layer==7&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapx7[i]; }
    if(layer==8&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipx8[i]; }
    emf = ep[0]+ep[1]*x+ep[2]*x*x+ep[3]*x*x*x+ep[4]*x*x*x*x+ep[5]*x*x*x*x*x+ep[6]*x*x*x*x*x*x
          +ep[7]*x*x*x*x*x*x*x+ep[8]*z+ep[9]*y+ep[10]*y*y+ep[11]*y*y*y+ep[12]*x*z+ep[13]*z*y
          +ep[14]*y*x+ep[15]*y*x*x+ep[16]*y*y*x;
    b[0] = emf*tesla;

    if(layer==0&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipy0[i]; }
    if(layer==0&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapy0[i]; }
    if(layer==1&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipy1[i]; }
    if(layer==1&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapy1[i]; }
    if(layer==2&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipy2[i]; }
    if(layer==2&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapy2[i]; }
    if(layer==3&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipy3[i]; }
    if(layer==3&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapy3[i]; }
    if(layer==4&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipy4[i]; }
    if(layer==4&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapy4[i]; }
    if(layer==5&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipy5[i]; }
    if(layer==5&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapy5[i]; }
    if(layer==6&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipy6[i]; }
    if(layer==6&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapy6[i]; }
    if(layer==7&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipy7[i]; }
    if(layer==7&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapy7[i]; }
    if(layer==8&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipy8[i]; }
    emf = ep[0]+ep[1]*x+ep[2]*x*x+ep[3]*x*x*x+ep[4]*x*x*x*x+ep[5]*x*x*x*x*x+ep[6]*x*x*x*x*x*x
          +ep[7]*x*x*x*x*x*x*x+ep[8]*z+ep[9]*y+ep[10]*y*y+ep[11]*y*y*y+ep[12]*x*z+ep[13]*z*y
          +ep[14]*y*x+ep[15]*y*x*x+ep[16]*y*y*x;
    b[1] = emf*tesla;

    if(layer==0&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipz0[i]; }
    if(layer==0&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapz0[i]; }
    if(layer==1&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipz1[i]; }
    if(layer==1&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapz1[i]; }
    if(layer==2&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipz2[i]; }
    if(layer==2&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapz2[i]; }
    if(layer==3&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipz3[i]; }
    if(layer==3&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapz3[i]; }
    if(layer==4&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipz4[i]; }
    if(layer==4&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapz4[i]; }
    if(layer==5&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipz5[i]; }
    if(layer==5&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapz5[i]; }
    if(layer==6&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipz6[i]; }
    if(layer==6&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapz6[i]; }
    if(layer==7&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipz7[i]; }
    if(layer==7&&mat==1) { for(int i=0; i<17; i++) ep[i] = aapz7[i]; }
    if(layer==8&&mat==0) { for(int i=0; i<17; i++) ep[i] = aipz8[i]; }
    emf = ep[0]+ep[1]*x+ep[2]*x*x+ep[3]*x*x*x+ep[4]*x*x*x*x+ep[5]*x*x*x*x*x+ep[6]*x*x*x*x*x*x
          +ep[7]*x*x*x*x*x*x*x+ep[8]*z+ep[9]*y+ep[10]*y*y+ep[11]*y*y*y+ep[12]*x*z+ep[13]*z*y
          +ep[14]*y*x+ep[15]*y*x*x+ep[16]*y*y*x;
    b[2] = emf*tesla; 
  }
}
void MucMagneticField::readPar()
{
  filename = path +
                 std::string( "/dat/barIronpar.txt");
  ifstream infile1(filename.c_str(),ios_base::in);
  if(infile1) cout<<"*** Read field map: "<<filename<<endl;
  for(int i=0; i<18; i++)
  {
    infile1>>bipx0[i]>>bipy0[i]>>bipz0[i]>>bipx1[i]>>bipy1[i]>>bipz1[i]
           >>bipx2[i]>>bipy2[i]>>bipz2[i]>>bipx3[i]>>bipy3[i]>>bipz3[i]
           >>bipx4[i]>>bipy4[i]>>bipz4[i]>>bipx5[i]>>bipy5[i]>>bipz5[i]
           >>bipx6[i]>>bipy6[i]>>bipz6[i]>>bipx7[i]>>bipy7[i]>>bipz7[i]
           >>bipx8[i]>>bipy8[i]>>bipz8[i];
  }
  
  filename = path +
                 std::string( "/dat/barAirpar.txt");

  ifstream infile2(filename.c_str(),ios_base::in);
  if(infile2) cout<<"*** Read field map: "<<filename<<endl;
  for(int j=0; j<18; j++)
  {
    infile2>>bapx0[j]>>bapy0[j]>>bapz0[j]>>bapx1[j]>>bapy1[j]>>bapz1[j]
           >>bapx2[j]>>bapy2[j]>>bapz2[j]>>bapx3[j]>>bapy3[j]>>bapz3[j]
           >>bapx4[j]>>bapy4[j]>>bapz4[j]>>bapx5[j]>>bapy5[j]>>bapz5[j]
           >>bapx6[j]>>bapy6[j]>>bapz6[j]>>bapx7[j]>>bapy7[j]>>bapz7[j]; 
  }

  filename = path +
                 std::string( "/dat/endIronpar.txt");

  ifstream infile3(filename.c_str(),ios_base::in);
  if(infile3) cout<<"*** Read field map: "<<filename<<endl;
  for(int i=0; i<17; i++)
  {
    infile3>>aipx0[i]>>aipy0[i]>>aipz0[i]>>aipx1[i]>>aipy1[i]>>aipz1[i]
           >>aipx2[i]>>aipy2[i]>>aipz2[i]>>aipx3[i]>>aipy3[i]>>aipz3[i]
           >>aipx4[i]>>aipy4[i]>>aipz4[i]>>aipx5[i]>>aipy5[i]>>aipz5[i]
           >>aipx6[i]>>aipy6[i]>>aipz6[i]>>aipx7[i]>>aipy7[i]>>aipz7[i]
           >>aipx8[i]>>aipy8[i]>>aipz8[i];
  }

  filename = path +
                 std::string( "/dat/endAirpar.txt");

  ifstream infile4(filename.c_str(),ios_base::in);
  if(infile4) cout<<"*** Read field map: "<<filename<<endl;
  for(int j=0; j<17; j++)
  {
    infile4>>aapx0[j]>>aapy0[j]>>aapz0[j]>>aapx1[j]>>aapy1[j]>>aapz1[j]
           >>aapx2[j]>>aapy2[j]>>aapz2[j]>>aapx3[j]>>aapy3[j]>>aapz3[j]
           >>aapx4[j]>>aapy4[j]>>aapz4[j]>>aapx5[j]>>aapy5[j]>>aapz5[j]
           >>aapx6[j]>>aapy6[j]>>aapz6[j]>>aapx7[j]>>aapy7[j]>>aapz7[j];
  }
}
