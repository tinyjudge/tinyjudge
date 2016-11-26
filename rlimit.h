#ifndef RLIMIT_H
#define RLIMIT_H

#ifdef __WIN32
#include <Windows.h>
#else
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif
#include <errno.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

class rlimit_c{
private:
	string exec;
	size_t memory;
	long long cputime;
	int proc;
	int prior;
	long long time;
public:
	rlimit_c();
	rlimit_c& setexec(string exec);
	enum rlimit_type{
		RLIMIT_MEMORY,
		RLIMIT_CPUTIME,
		RLIMIT_PROC,
		RLIMIT_PRIOR,
		RLIMIT_TIME,
	};
	bool set(rlimit_type type, long long val);
	int run();
};

#endif
