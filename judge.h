#ifndef JUDGE_H
#define JUDGE_H

#include "file.h"
#include "rlimit.h"
#include "misc.h"

#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class judge_c
{
public:
	string compiler, partdir, probdir;
	judge_c();
	judge_c(const string &compiler, const string &partdir, const string &probdir);

	bool compile(const string &compiler, const string &src, const string &exec,
	             const string &log);
	bool runjudge(string &glblog, bool nolog);
};

#endif
