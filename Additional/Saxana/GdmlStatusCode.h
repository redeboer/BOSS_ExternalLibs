//
#ifndef GDML_STATUS_CODE_H
#define GDML_STATUS_CODE_H 1

class GdmlStatusCode{
public:
  typedef enum
  {
    eError = 0,
    eOk = 1,
    eWarning,
    eParserInitFailure
  } EInfo;

public:
  GdmlStatusCode( EInfo info = GdmlStatusCode::eOk )
  : fInfo( info )
  {
  }  
  ~GdmlStatusCode()
  {
  }
  
  bool IsOk()
  {
    return( (fInfo == eOk) ? true : false );
  } 
  bool IsFailure()
  {
    return( (fInfo != eOk) ? true : false );
  }
  GdmlStatusCode::EInfo Info()
  {
    return fInfo;
  }
  
private:
  EInfo fInfo;
};

#endif // GDML_STATUS_CODE_H

