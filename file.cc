#include "file.h"

file_c::file_c()
{
	this->filestr="";
}

file_c::file_c(string &filestr)
{
	setfile(filestr);
}

file_c &file_c::setfile(string filestr)
{
	this->filestr=filestr;
	this->tonative();
	return *this;
}

string file_c::tonative()
{
	for(string::iterator i=this->filestr.begin();i!=this->filestr.end();i++)
#ifdef __WIN32
		if(*i=='/')*i='\\';
#else
		if(*i=='\\')*i='/';
#endif
	return this->filestr;
}

int file_c::comp(const string &file2,bool bin)
{
	if(bin)
	{
		ifstream f1(filestr.c_str(),ios::binary);
		ifstream f2(file2.c_str(),ios::binary);
		if(f1.bad()||f1.fail()||f2.bad()||f2.fail())return -1;
		
		char cur1,cur2;
		f1.read(&cur1,1);f2.read(&cur2,1);
		while(!f1.eof()||!f2.eof())
		{
			if(f1.bad()||f1.fail()||f2.bad()||f2.fail())return -1;
			if(f1.eof()||f2.eof())return 1;
			if(cur1!=cur2)return 1;
			f1.read(&cur1,1);f2.read(&cur2,1);
		}
		return 0;
	}
	else
	{
		ifstream f1(filestr.c_str());
		ifstream f2(file2.c_str());
		if(f1.bad()||f1.fail()||f2.bad()||f2.fail())return -1;
		
		char tmp1[65536],tmp2[65536];
		string str1,str2;
		f1.getline(tmp1,65535);
		f2.getline(tmp2,65535);
		while(!f1.eof()||!f2.eof())
		{
			if(f1.bad()||f1.fail()||f2.bad()||f2.fail())return -1;
			if(f1.eof()||f2.eof())return 1;
			int i;
			for(i=0;tmp1[i]&&tmp1[i]!=10&&tmp1[i]!=13;i++);
			tmp1[i]=0;
			for(i=0;tmp2[i]&&tmp2[i]!=10&&tmp2[i]!=13;i++);
			tmp2[i]=0;
			str1=tmp1;str2=tmp2;
			if(str1!=str2)return 1;
			f1.getline(tmp1,65535);
			f2.getline(tmp2,65535);
		}
		return 0;
	}
} 

bool file_c::copy(const string &dest){
	FILE *f1=fopen(filestr.c_str(),"r");
	if(!f1)return false;
	FILE *f2=fopen(dest.c_str(),"w");
	if(!f2){
		fclose(f1);
		return false;
	}

	char cur;
	while((cur=fgetc(f1))!=EOF){
		fputc(cur,f2);
	}
	fclose(f1);
	fclose(f2);
	return true;
}

bool file_c::readall(string &str)
{
	ifstream file(this->filestr.c_str());
	if(file.bad()||file.fail())return false;
	char tmp[65536];
	str="";
	file.getline(tmp,65535);
	while(!file.eof())
	{
		if(file.bad()||file.fail())return false;
		str+=string(tmp)+'\n';
		file.getline(tmp,65535);
	}
	return true;
}

bool file_c::readconfig(map<string,string> &cfg)
{
	ifstream config(this->filestr.c_str());
	char tmp[65536];
	if(config.bad()||config.fail())return false;
	config.getline(tmp,65535);
	while(!config.eof())
	{
		if(config.bad()||config.fail())return false;
		
		string curline=tmp;
		int eq=curline.find('=');
		if(eq==string::npos)eq=curline.size();
		
		int valstart=0,valend;
		while(curline[valstart]==' '&&valstart<eq)valstart++;
		
		valend=valstart;
		while(curline[valend]!=' '&&valend<eq)valend++;
		
		cfg[curline.substr(valstart,valend-valstart)]=curline.substr(eq+1);
		
		config.getline(tmp,65535);
	}
	return true;
}

bool file_c::travedir(vector<string> &tree,file_type type)
{
	DIR *dir;
	struct dirent *ptr;
	struct stat s;
	
	if((dir=opendir(this->filestr.c_str()))==NULL)
		return false;
	
	while((ptr=readdir(dir))!=NULL)
	{
		if(ptr->d_name[0]=='.')continue;
		if(stat((this->filestr+'/'+ptr->d_name).c_str(),&s))continue;
		if(type==FILE_TYPE_REG&&S_ISREG(s.st_mode))
			tree.push_back(ptr->d_name);
		if(type==FILE_TYPE_DIR&&S_ISDIR(s.st_mode))
			tree.push_back(ptr->d_name);
	}
	
	return true;
}
