#ifndef MemorySnooper_H
#define MemorySnooper_H

#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <fstream>
#include <sstream>

class MemorySnooper {
public:
	MemorySnooper(std::string where)
	{
		int pid=getpid();
		std::ostringstream is;
		is<<pid;
		std::string spid=is.str();
		std::string temp="cat /proc/"+spid+"/status | grep VmSize > mem.txt";
		system(temp.c_str());
		std::ifstream in("mem.txt");
		std::string text,mem,text1;
    		in>>text>>mem>>text1;
		system("rm -f mem.txt");
		float memsize=atof(mem.c_str());
		imem=int(memsize);
		std::cout<<"*****************************************************"<<std::endl
		         <<"*                                                   *"<<std::endl
			 <<"  Memory snooper called here "<<where<<std::endl
			 <<"  process size at this point (VmMem): "<<imem<<" kBytes"<<std::endl
			 <<"*                                                   *"<<std::endl
			 <<"*****************************************************"<<std::endl;
	}
	int GetMemorySize() const {return imem;}
private:
	int imem;
};

#endif
