#include "db_mysql.h"

db_mysql::db_mysql()
	:m_Mysql(NULL)
	,m_Res(NULL)
	,m_bConnect(false)
	,m_bSelectDB(false)
	,m_bExecute(false)
	,m_bGetRow(false)
	,m_bInit(false)
{
	;	
}
db_mysql::db_mysql(string host,string user,string passwd,string db)
	:m_Mysql(NULL)
	,m_Res(NULL)
	,m_bConnect(false)
	,m_bSelectDB(false)
	,m_bExecute(false)
	,m_bGetRow(false)
{
	Init(host,user,passwd,db);
}	

void db_mysql::Init(string host,string user,string passwd,string db)
{
	m_strHost = host;
	m_strUser = user;
	m_strPasswd = passwd;
	m_strDB = db;
	m_bInit = true;
	if(!db.empty())
	{
		m_bSelectDB = true;
	}
}
void db_mysql::SetQuery(string query)
{
	m_strQuery=query;
}
bool db_mysql::Connect(void)
{
	if(!m_bConnect)
	{
		my_bool b = 0;
		if(!(m_Mysql=mysql_init(NULL)))
        	{
			cout<<"Init mysql error!!"<<endl;
			return false; 
        	}
//		mysql_options(m_Mysql,MYSQL_REPORT_DATA_TRUNCATION,&b);
        	if(!mysql_real_connect(m_Mysql,m_strHost.c_str(),m_strUser.c_str(),m_strPasswd.c_str(),m_strDB.c_str(),0,NULL,0))
		{
			cout<<"Connect error!!"<<endl;
			mysql_close(m_Mysql);
			return false;
		}
	}
	else
	{
		cout<<"You are reconnecting!"<<endl;
		return false;	
	}
	m_bConnect = true;
	return true;
}
bool db_mysql::SelectDB(string db)
{
	if(!m_bConnect)
	{
		cout<<"You must connect before select db!";
		return false;
	}
	if(mysql_select_db(m_Mysql,db.c_str()))
	{
		cout<<"Select database error!"<<endl;
		return false;
	}
	m_strDB = db;
	m_bSelectDB = true;
	return true;
}
	
bool db_mysql::Execute(string query)
{
	if((!m_bConnect)||(!m_bSelectDB))
	{
		cout<<"You executing query before connection or select db!"<<endl;
		return false;
	}
	if(!query.empty())
	{
		m_strQuery=query;
	}
	if(m_strQuery.empty())
	{
		cout<<"Your haven't set query string!"<<endl;
		return false;	
	}
	else
	{
		Free_Result();
		if(mysql_real_query(m_Mysql,m_strQuery.c_str(),m_strQuery.size()))
		{
			cout<<"Execute query error!"<<endl;
			return false;	
		}	
	}
//zhaohs
        Field_num=mysql_field_count(m_Mysql);
        if(Field_num==0){
	m_result=false;
	}
	else {m_result=true;}
//zhaohs
	m_Res = mysql_store_result(m_Mysql);
	m_bExecute = true;
	return true;
}
	
bool db_mysql::GetRow(my_ulonglong row)
{
	if(!m_bExecute)
	{
		cout<<"You must execute query before get row!"<<endl;
		return false;
	}
	if(row!=NEXT_ROW)
	{
		if(row>=mysql_num_rows(m_Res)||row<0)
		{
//			cout<<"Select row error! Row number must be set in range!"<<endl;
			return false;
		}
		mysql_data_seek(m_Res,row);	
	}
	m_Row=mysql_fetch_row(m_Res);
	if(m_Row==NULL)
	{
		cout<<"fetch_row error!"<<endl;	
		return false;
	}
	
	m_bGetRow = true;
	return true;
}

string db_mysql::GetField(unsigned int n)
{
        
	if(!m_bGetRow)
	{
		cout<<"You must get a row before getfield value!"<<endl;
		return "";
	}
	if(m_Row[n]!=NULL){ return (string)m_Row[n];}
	else return "";
}
//// gaizao
/*string db_mysql::GetField(string name)
{
 
         if(!m_bGetRow)
	         {cout<<"You must get a row before getfield value!"<<endl;
		  return "";
		 }
		if(m_Row[name]!=NULL){ return (string)m_Row[name];}
		else return "";
}*/
/////  myself---zhaohs
string db_mysql::GetField_name(unsigned int n)
{
        if(!m_result)
	        {
		 cout<<"error:no select!"<<endl;
		return "";
		}
        m_field = mysql_fetch_field_direct(m_Res, n);
	return string(m_field->name);


//	num_fields = mysql_num_fields(result);
//	m_field = mysql_fetch_fields(m_Res);
  //      std::cout<<m_field[n].name<<std::endl;
//	return m_field[n].name;

	   
}

////
unsigned long db_mysql::Num_Rows(void)
{
	if(!m_bExecute)
	{
		cout<<"You are get number of rows before execute!"<<endl;
		return 0;
	}
	return mysql_num_rows(m_Res);
}

void db_mysql::Free_Result(void)
{
	if(m_bExecute)
	{
		mysql_free_result(m_Res);
		m_bExecute = false;
		m_bGetRow = false;	
	}
}

void db_mysql::Close(void)
{
	if(m_bConnect)
	{
		Free_Result();
		mysql_close(m_Mysql);
		m_bConnect = false;
	}
}

db_mysql::~db_mysql()
{
	Free_Result();
	Close();
}

