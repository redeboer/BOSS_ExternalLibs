typedef struct{
  float x;
  float y;
  float z;
} vector3;


// initialize the vector to (x,y,z)
vector3 InitV(float x, float y, float z)
{
  vector3 v;
  v.x = x;
  v.y = y;
  v.z = z;
  return v;
}

// initialize the vector to (x,y,z)
vector3 InitV1(float phi, float cosTheta, float magnitude)
{
  vector3 v;
  float sinTheta = sqrt(1.0-cosTheta*cosTheta);
  v.z = magnitude*cosTheta;
  v.y = magnitude*sinTheta*sin(phi);
  v.x = magnitude*sinTheta*cos(phi);
  return v;
}

// the magnitude of vector v
float Mag(vector3 v)
{
  return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

// the squared magnitude of vector v
float Mag2(vector3 v)
{
  return v.x*v.x+v.y*v.y+v.z*v.z;
}

// Scalar product of vector v1 and v2
float Dot(vector3 v1, vector3 v2)
{
  return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}


// Cross product of vector v1 and v2
vector3 Cross(vector3 v1, vector3 v2)
{
  vector3 v;
  v.x = v1.y*v2.z - v1.z*v2.y;
  v.y = v1.z*v2.x - v1.x*v2.z;
  v.z = v1.x*v2.y - v1.y*v2.x;
  return v;
}

// Unit vector parallel to vector v
vector3 Unit(vector3 v)
{
  vector3 vv;
  float p = Mag(v);
  vv.x = v.x/p;
  vv.y = v.y/p;
  vv.z = v.z/p;
  return vv;
}


// Calculate the intersection piont with z0 = c plane
// for vector vec which pass point pos
vector3 Intersection(float z0, vector3 vec, vector3 pos)
{ 
  vector3 vz0;
  vz0.z = z0;
  vz0.x = (vz0.z - pos.z)*vec.x/vec.z + pos.x;
  vz0.y = (vz0.z - pos.z)*vec.y/vec.z + pos.y;
  return vz0;
}


// vector v times with a constant a
vector3 TimesA(float a, vector3 v)
{
  vector3 vv;
  vv.x = a*v.x;
  vv.y = a*v.y;
  vv.z = a*v.z;
  return vv;
}

// vector v1 + vector v2
vector3 AddV(vector3 v1, vector3 v2)
{
  vector3 v;
  v.x = v1.x + v2.x;
  v.y = v1.y + v2.y;
  v.z = v1.z + v2.z;
  return v;
}

// vector v1 - vector v2
vector3 SubV(vector3 v1, vector3 v2)
{ 
  vector3 v;
  v.x = v1.x - v2.x;
  v.y = v1.y - v2.y;
  v.z = v1.z - v2.z;
  return v;
}

// Transform v from ux, uy, uz frame to (1,0,0),(0,1,0),(0,0,1) frame
vector3 TransformFrom(vector3 v, vector3 ux, vector3 uy, vector3 uz)
{
  ux = TimesA(v.x,ux);
  uy = TimesA(v.y,uy);
  uz = TimesA(v.z,uz);
  v = AddV(AddV(ux, uy), uz);
  return v;
}


// Transform v to ux, uy, uz frame from (1,0,0),(0,1,0),(0,0,1) frame
vector3 TransformTo(vector3 v, vector3 ux, vector3 uy, vector3 uz)
{
  vector3 vv;
  vv.x = Dot(v,ux);
  vv.y = Dot(v,uy);
  vv.z = Dot(v,uz);
  return vv;
}

const float pi = 3.1415926536;
const float rad = 57.29578;

typedef struct{
  float r;
  float theta;
  float phi;
} polar;

polar XYZ2Polar(vector3 v)
{
  polar s;
  float rxy;

  s.r = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
  if(s.r==0.0){
    s.theta = 0.0;
  }
  else{
    s.theta = acos(v.z/s.r);
  }

  rxy = sqrt(v.x*v.x+v.y*v.y);
  if(rxy==0.0){
    s.phi = 0.0;
  }
  else{
    if(v.y>=0.0) s.phi = acos(v.x/rxy);
    else{
      s.phi = 2.0*pi-acos(v.x/rxy);
    }
  }

  return s;
}
