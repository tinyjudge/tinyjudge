#ifndef RLIMIT_H
#define RLIMIT_H

#include <cstdlib>
#include <cstring>
#include <string>
using namespace std;

#ifdef __WIN32
#include "Windows.h"
#else
#include "sys/resource.h"
#include "sys/signal.h"
#include "sys/time.h"
#include "sys/types.h"
#include "sys/wait.h"
#endif
#include "unistd.h" 

class rlimit_c{
private:
	string exec;
	size_t memory;
	long long time;
	int proc;
	int prior;
public:
	rlimit_c();
	rlimit_c& setexec(string exec);
	enum rlimit_type{
		RLIMIT_MEMORY,
		RLIMIT_TIME,
		RLIMIT_PROC,
		RLIMIT_PRIOR,
	};
	bool set(rlimit_type type,long long val);
	int run();
};

#endif
