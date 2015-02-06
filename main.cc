#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
using namespace std;

#include "judge.h"

int main(int argc,char *argv[])
{
	map<string,string>argu;

	for(int i=1;i<argc;i++)
	{
		if(strcmp(argv[i],"-c")==0)
		{
			if(i==argc-1){
				cout<<"missing argument after -c"<<endl;
				return EXIT_FAILURE;
			}
			i++;
			argu["-c"]=argv[i];
		}
		else if(strcmp(argv[i],"-p")==0)
		{
			if(i==argc-1){
				cout<<"missing argument after -p"<<endl;
				return EXIT_FAILURE;
			}
			i++;
			argu["-p"]=argv[i];
		}
		else if(strcmp(argv[i],"-b")==0)
		{
			if(i==argc-1){
				cout<<"missing argument after -b"<<endl;
				return EXIT_FAILURE;
			}
			i++;
			argu["-b"]=argv[i];
		}
		else if(strcmp(argv[i],"-r")==0)
		{
			argu["-r"]="true";
		}
		else if(strcmp(argv[i],"-o")==0)
		{
			if(i==argc-1){
				cout<<"missing argument after -o"<<endl;
				return EXIT_FAILURE;
			}
			i++;
			argu["-o"]=argv[i];
		}
		else if(strcmp(argv[i],"-l")==0)
		{
			argu["-l"]="true";
		}
		else if(strcmp(argv[i],"--help")==0)
		{
			cout<<"TinyJudge v0.01"<<endl
				<<"  -b <dir>        specify problems\' directory"<<endl
				<<"  -c <str>        set compiler"<<endl
				<<"  -p <dir>        specify participants\' directory"<<endl
				<<"  -r              run judge"<<endl
				<<"  -o <file>       output log to file"<<endl
				<<"  -l              do not output log"<<endl
				<<"  --help          show this message"<<endl;
			return EXIT_SUCCESS;
		}
		else
		{
			cout<<"Unrecognized option \""<<argv[i]<<"\"."<<endl<<"Use --help to view help."<<endl;
			return EXIT_FAILURE;
		}
	}

	judge_c judge=judge_c(argu["-c"],argu["-p"],argu["-b"]);
	if(argu["-r"]=="true")
	{
		string log;
		if(!judge.runjudge(log,argu["-l"]=="true"?true:false))
			cout<<endl<<"Failed running judge."<<endl;
		else
		{
			if(argu["-o"]!="")
			{
				ofstream fout(argu["-o"].c_str());
				fout<<log;
			}
		}
	}
	
	return EXIT_SUCCESS;
}
