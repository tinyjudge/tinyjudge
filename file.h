#ifndef FILE_H
#define FILE_H

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream> 
#include <fstream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class file_c
{
private:
	string filestr;
public:
	enum file_type {
		FILE_TYPE_REG,
		FILE_TYPE_DIR,
	};

	file_c();
	file_c(string &filestr);
	file_c &setfile(string filestr);
	string tonative();
	int comp(const string &file2, bool bin);
	bool copy(const string &dest);
	bool readall(string &str);
	bool readconfig(map<string,string> &cfg);
	bool travedir(vector<string> &tree, file_type type);
};

#endif
