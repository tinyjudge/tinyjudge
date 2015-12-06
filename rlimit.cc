#include "rlimit.h"

rlimit_c::rlimit_c()
{
	this->exec="";
	this->memory=this->cputime=this->proc=0;
}

rlimit_c& rlimit_c::setexec(string exec)
{
	this->exec=exec;
	return *this;
}

bool rlimit_c::set(rlimit_type type,long long val)
{
	switch(type)
	{
		case RLIMIT_MEMORY:
			this->memory=val;
			break;
		case RLIMIT_CPUTIME:
#ifdef __WIN32
			val*=10000000;
#endif
			this->cputime=val;
			break;
		case RLIMIT_PROC:
			this->proc=val;
			break;
		case RLIMIT_PRIOR:
			this->prior=val;
			break;
		case RLIMIT_TIME:
			this->time=val;
			break;
		default:
			return false;
	}
	return true;
}

int rlimit_c::run()
{
#ifndef __WIN32
	pid_t childpid=fork();
	if(childpid==-1)return -1;
	if(childpid>0)
	{
		int status,ret=0;

		waitpid(childpid,NULL,WUNTRACED);
		if(this->prior)setpriority(PRIO_PROCESS,childpid,this->prior);	//requires superuser permission, so do not check this
		struct rlimit rlimit;
		rlimit.rlim_cur=this->prior;
		rlimit.rlim_max=this->prior;
		if(prlimit(childpid,RLIMIT_NICE,&rlimit,NULL)==-1)goto killandret;
		rlimit.rlim_cur=this->proc;
		rlimit.rlim_max=this->proc;
		if(prlimit(childpid,RLIMIT_NPROC,&rlimit,NULL)==-1)goto killandret;
		rlimit.rlim_cur=this->memory;
		rlimit.rlim_max=this->memory;
		if(prlimit(childpid,RLIMIT_AS,&rlimit,NULL)==-1)goto killandret;
		rlimit.rlim_cur=this->cputime;
		rlimit.rlim_max=this->cputime+1;
		/*
		 * plus 1 to the hard limit so we can receive a SIGXCPU instead of
		 * a SIGKILL when time limit is exceeded
		 */
		if(prlimit(childpid,RLIMIT_CPU,&rlimit,NULL)==-1)goto killandret;
		if(kill(childpid,SIGCONT))goto killandret;

		for(int i=0;i<this->time*2;i++){
			ret=waitpid(childpid,&status,WNOHANG);
			if(ret!=0)break;
			usleep(500000);
		}
		if(ret==0||ret==-1)goto killandret;

		return status;
killandret:
		if(!kill(childpid,SIGKILL))
			waitpid(childpid,NULL,0);	//avoid zombie processes
		return -1;
	}
	else if(childpid==0) 
	{
		childpid=getpid();
		if(kill(childpid,SIGSTOP))exit(EXIT_FAILURE);
		//freopen("/dev/null","r",stdin);
		//freopen("/dev/null","w",stdout);
		//freopen("/dev/null","w",stderr);
		if(this->exec[1]!='/')execl(("./"+this->exec).c_str(),
			this->exec.substr(this->exec.find_last_of('/')+1).c_str(),
			(char*)NULL);
		else execl(this->exec.c_str(),
			this->exec.substr(this->exec.find_last_of('/')+1).c_str(),
			(char*)NULL);
		exit(EXIT_FAILURE);
	}
#else
	HANDLE job=CreateJobObjectA(NULL,NULL);
	if(!job)return -1;

	JOBOBJECT_BASIC_LIMIT_INFORMATION limitinfo;
	memset(&limitinfo,0,sizeof(limitinfo));
	
	limitinfo.PerProcessUserTimeLimit.QuadPart=this->cputime;
	limitinfo.MinimumWorkingSetSize=(this->memory/2>0?this->memory/2:1);
	limitinfo.MaximumWorkingSetSize=this->memory;
	limitinfo.ActiveProcessLimit=this->proc;
	limitinfo.PriorityClass=this->prior;
	if(this->cputime)limitinfo.LimitFlags|=JOB_OBJECT_LIMIT_PROCESS_TIME;
	if(this->memory)limitinfo.LimitFlags|=JOB_OBJECT_LIMIT_WORKINGSET;
	if(this->proc)limitinfo.LimitFlags|=JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
	if(this->prior)limitinfo.LimitFlags|=JOB_OBJECT_LIMIT_PRIORITY_CLASS;
	
	if(limitinfo.LimitFlags)
	{
		if(!SetInformationJobObject(job,
			JobObjectBasicLimitInformation,
			&limitinfo,sizeof(limitinfo)))
		{
			CloseHandle(job);
			return -1;
		}
	}
	
	STARTUPINFO sinfo;
	memset(&sinfo,0,sizeof(sinfo));
	sinfo.cb=sizeof(sinfo);
	PROCESS_INFORMATION pinfo;
	memset(&pinfo,0,sizeof(pinfo)); 
	if(!CreateProcessA(NULL,(char*)("\""+this->exec+'"').c_str(),NULL,
		NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,
		&sinfo,&pinfo))
	{
		CloseHandle(job);
		return -1;
	}
	
	CloseHandle(pinfo.hProcess);
	
	pinfo.hProcess=OpenProcess(PROCESS_SET_QUOTA|PROCESS_TERMINATE
		|SYNCHRONIZE|PROCESS_QUERY_INFORMATION,
		FALSE,pinfo.dwProcessId);
	
	if(!pinfo.hProcess)
	{
		CloseHandle(job);
		CloseHandle(pinfo.hThread);
		return -1;
	}
	if(!AssignProcessToJobObject(job,pinfo.hProcess))
	{
		CloseHandle(job);
		CloseHandle(pinfo.hProcess);
		CloseHandle(pinfo.hThread);
		return -1;
	}
	
	if(ResumeThread(pinfo.hThread)==-1)
	{
		CloseHandle(job);
		CloseHandle(pinfo.hProcess);
		CloseHandle(pinfo.hThread);
		return -1;
	}
	
	CloseHandle(pinfo.hThread);
	
	if(WaitForSingleObject(pinfo.hProcess,this->time*1000)==WAIT_FAILED)
	{
		CloseHandle(pinfo.hProcess);
		CloseHandle(job);
		return -1;
	}
	
	int ret;
	if(!GetExitCodeProcess(pinfo.hProcess,(LPDWORD)&ret))
	{
		CloseHandle(pinfo.hProcess);
		CloseHandle(job);
		return -1;
	}
	
	TerminateProcess(pinfo.hProcess,1);
	
	CloseHandle(pinfo.hProcess);
	CloseHandle(job);
	return ret;
#endif
}
