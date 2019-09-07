#ifndef DatabaseRecord_h
#define DatabaseRecord_h

#include <map>
#include <string>
#include <vector>
#include <cstdlib>

class DatabaseRecord : public std::map<std::string, char*>
{
 public:
  DatabaseRecord(){}

  ~DatabaseRecord()
   {
	DeleteObject();
   }

  void clear(){
        DeleteObject();
  };
  
  void DeleteObject()
   {
    DatabaseRecord::iterator it;
    for(it=this->begin(); it != this->end(); it++)
     {
       if((*it).second!=NULL)
       {
         delete[] (*it).second;
         (*it).second = NULL;
       }
     }
    erase(this->begin(),this->end());
   }
   
   int GetInt(std::string key)
	 {
		 DatabaseRecord::iterator it = find(key);
		 if(it==end())
			 return 0;
		 return atoi((*it).second);
	 }
																      

   double GetDouble(std::string key)
     {
       DatabaseRecord::iterator it = find(key);
       if(it==end()) 
            return 0;
       return atof((*it).second);
     }

   long GetLong(std::string key)
   {
       DatabaseRecord::iterator it = find(key);
       if(it==end())
            return 0;
       return atol((*it).second);
   }

   std::string GetString(std::string key)
   {
   //  std::string str1;
     DatabaseRecord::iterator it = find(key);
     if(it==end())
           return 0;
     std::string str1((*it).second); 
     return str1;

   }
};

class DatabaseRecordVector : public std::vector <DatabaseRecord*>
{
 public:
  DatabaseRecordVector(){};
  ~DatabaseRecordVector()
  {
    clear();
  }

  void clear(){
   DatabaseRecordVector::iterator  it;
    for(it=this->begin(); it != this->end(); it++)
     {
        (*it)->DeleteObject();
     }
    erase(this->begin(),this->end());
  }
};

#endif
