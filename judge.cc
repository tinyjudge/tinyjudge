#include "judge.h"

judge_c::judge_c()
{
	judge_c("", "", "");
}

judge_c::judge_c(const string &compiler, const string &partdir,
                 const string &probdir, const string &logfile /* = "" */ )
{
#ifdef __WIN32
	this->compiler =
		(compiler == "" ? string("g++ -O2 -o %TJ_EXEC% %TJ_SRC%") : compiler);
#else
	this->compiler =
		(compiler == "" ? string("g++ -O2 -o $TJ_EXEC $TJ_SRC") : compiler);
#endif
	this->partdir = (partdir == "" ? string("part") : partdir);
	this->probdir = (probdir == "" ? string("prob") : probdir);
	this->logfile = logfile;
}

bool judge_c::compile(const string &compiler, const string &src,
                      const string &exec, const string &log)
{
	// todo: time limit
	static char execenv[65536], srcenv[65536];
	("TJ_EXEC=" + exec).copy(execenv, 65535);
	execenv[("TJ_EXEC=" + exec).size()] = '\0';
	("TJ_SRC=" + src).copy(srcenv, 65535);
	srcenv[("TJ_SRC=" + src).size()] = '\0';
	if (putenv(execenv)) return false;
	if (putenv(srcenv)) return false;
	system(("echo invoking \"" + compiler + "\" > \"" + log + '"').c_str());
	return (!system((compiler + " 1>> \"" + log + "\" 2>&1").c_str()));
}

bool judge_c::runjudge(bool nolog)
{
	string glblog = "";
	if(logfile != "") {
		FILE *flog = fopen(logfile.c_str(), "w");
		if(flog == NULL) return false;
		fclose(flog);
	}

	vector<pair<int, string> > finalres;

	file_c file;
	file.setfile(this->probdir);

	static vector<string> probs;
	probs.clear();
	if (!file.travedir(probs, file.FILE_TYPE_DIR)) return false;

	file.setfile(this->partdir);
	static vector<string> parts;
	parts.clear();
	if (!file.travedir(parts, file.FILE_TYPE_DIR)) return false;

	static map<string, map<string, string> > config;
	config.clear();

	for (vector<string>::iterator curprob = probs.begin(); curprob != probs.end();
	     curprob++) {
		config[*curprob]["compiler"] = this->compiler;
		config[*curprob]["srcsuf"] = ".cc";
#ifdef __WIN32
		config[*curprob]["execsuf"] = ".exe";
#else
		config[*curprob]["execsuf"] = "";
#endif
		config[*curprob]["logsuf"] = ".log";
		config[*curprob]["insuf"] = ".in";
		config[*curprob]["outsuf"] = ".out";
		config[*curprob]["anssuf"] = ".ans";
		config[*curprob]["testcnt"] = "10";
		config[*curprob]["bincomp"] = "false";
		config[*curprob]["score"] = "10";
		config[*curprob]["memory"] = "134217728";
		config[*curprob]["cputime"] = "1";
		config[*curprob]["time"] = "5";
		config[*curprob]["proc"] = "0";
		config[*curprob]["prior"] = "0";
		string curcfgfull = this->probdir + '/' + *curprob + '/' + "config";
		file.setfile(curcfgfull);
		file.readconfig(config[*curprob]);
	}

	string summary = "summary:\n";

	char *orgwd = getcwd(NULL, 0);
	if (!orgwd) return false;
	char buf[20];
	for (vector<string>::iterator curpart = parts.begin(); curpart != parts.end();
	     curpart++) {
		int partscore = 0;
		for (vector<string>::iterator curprob = probs.begin();
		     curprob != probs.end(); curprob++) {
			int probscore = 0, score = atoi(config[*curprob]["score"].c_str());

			string loclog;
			loclog = "part: " + *curpart + " ("
			         + to_string(distance(parts.begin(), curpart) + 1)
			         + "/" + to_string(parts.size()) + ")\nprob: "
			         + *curprob + '\n';

			string curpartdir = this->partdir + '/' + *curpart + '/';
			string curprobdir = this->probdir + '/' + *curprob + '/';

			curpartdir = file.setfile(curpartdir).tonative();
			curprobdir = file.setfile(curprobdir).tonative();

			string compiler = config[*curprob]["compiler"];
			string src = *curprob + config[*curprob]["srcsuf"];
			string exec = *curprob + config[*curprob]["execsuf"];
			string log = *curprob + config[*curprob]["logsuf"];

			src = file.setfile(src).tonative();
			exec = file.setfile(exec).tonative();
			log = file.setfile(log).tonative();

			bool compileres;
			if (chdir(curpartdir.c_str()) == -1)
				compileres = false;
			else
				compileres = compile(compiler, src, exec, log);
			if (chdir(orgwd) == -1) compileres = false;

			glblog += loclog;
			cout << loclog;
			loclog = "";

			if (!compileres) {
				loclog += "compile: 1 cannot compile";
			} else {
				rlimit_c rlimit;
				rlimit.setexec(exec);
				rlimit.set(rlimit.RLIMIT_MEMORY,
				           atoi(config[*curprob]["memory"].c_str()));
				rlimit.set(rlimit.RLIMIT_CPUTIME,
				           atoi(config[*curprob]["cputime"].c_str()));
				rlimit.set(rlimit.RLIMIT_TIME, atoi(config[*curprob]["time"].c_str()));
				rlimit.set(rlimit.RLIMIT_PROC, atoi(config[*curprob]["proc"].c_str()));
				rlimit.set(rlimit.RLIMIT_PRIOR,
				           atoi(config[*curprob]["prior"].c_str()));

				loclog += "compile: 0 compilation completed";

				for (int i = 0; i < atoi(config[*curprob]["testcnt"].c_str()); i++) {
					glblog += loclog;
					cout << loclog;
					loclog = "";

					loclog += string("\ntest") + itoa(i, buf, 10) + ": ";
					file.setfile(curprobdir + itoa(i, buf, 10) +
					             config[*curprob]["insuf"]);

					glblog += loclog;
					cout << loclog;
					loclog = "";

					if (!file.copy(curpartdir + *curprob + config[*curprob]["insuf"])) {
						loclog += "1 cannot copy input file";
						continue;
					}
					unlink(
						(curpartdir + *curprob + config[*curprob]["outsuf"])
							.c_str());	// no verifications in case of lacking output file
					int runres;
					if (chdir(curpartdir.c_str()) == -1)
						runres = -1;
					else
						runres = rlimit.run();
					if (chdir(orgwd) == -1) runres = -1;
					
					unlink(
						(curpartdir + *curprob + config[*curprob]["insuf"])
							.c_str());
#ifdef __WIN32
					if (runres && runres != 1816) {
						loclog +=
							"2 failed to execute, run-time error or the execution took too "
							"long time, return value: ";
						loclog += itoa(runres, buf, 10);
						unlink(
							(curpartdir + *curprob + config[*curprob]["outsuf"])
								.c_str());
						continue;
					} else if (runres == 1816) {
						loclog += "3 time limit exceeded, return value: 1816";
						unlink(
							(curpartdir + *curprob + config[*curprob]["outsuf"])
								.c_str());
						continue;
					}
#else
					if (runres == -1) {
						loclog += "2 failed to execute or the execution took too long time";
						unlink(
							(curpartdir + *curprob + config[*curprob]["outsuf"])
								.c_str());
						continue;
					}
					if (WIFEXITED(runres) && WEXITSTATUS(runres)) {
						loclog += "2 return value is not 0, return value: ";
						loclog += itoa(WEXITSTATUS(runres), buf, 10);
						unlink(
							(curpartdir + *curprob + config[*curprob]["outsuf"])
								.c_str());
						continue;
					}
					if (WIFSIGNALED(runres) && WTERMSIG(runres)) {
						if (WTERMSIG(runres) == SIGXCPU) {
							loclog += "3 time limit exceeded, signal: ";
							loclog += itoa(SIGXCPU, buf, 10);
							unlink(
								(curpartdir + *curprob + config[*curprob]["outsuf"])
									.c_str());
							continue;
						}
						loclog += "2 signal: ";
						loclog += itoa(WTERMSIG(runres), buf, 10);
						unlink(
							(curpartdir + *curprob + config[*curprob]["outsuf"])
								.c_str());
						continue;
					}
#endif
					file.setfile(curprobdir + itoa(i, buf, 10) +
						config[*curprob]["anssuf"]);
					int comp_res = file.comp(curpartdir + *curprob + config[*curprob]["outsuf"],
					                         config[*curprob]["bincomp"] == "true" ? true : false);
					switch (comp_res) {
						case -1:
							loclog += "4 failed to compare output file with answer file";
							break;
						case 0:
							loclog += "0 passed";
							probscore += score;
							break;
						default:
							loclog += "5 wrong answer";
							ofstream log_out((curpartdir + log).c_str(), ofstream::app);
							if (log_out.bad() || log_out.fail()) break;
							log_out << "wrong answer for test " << itoa(i, buf, 10) << endl
							        << "***output***" << endl;
							file.setfile(curpartdir + *curprob + config[*curprob]["outsuf"]);
							string tmp;
							if (file.readall(tmp))
								log_out << tmp;
							else
								log_out << "(failed to read output file)";
							log_out << "\n***answer***" << endl;
							file.setfile(curprobdir + itoa(i, buf, 10) +
							             config[*curprob]["anssuf"]);
							if (file.readall(tmp))
								log_out << tmp;
							else
								log_out << "(failed to read answer)";
							log_out << endl;
							log_out.close();
							break;
					}
					unlink(
						(curpartdir + *curprob + config[*curprob]["outsuf"])
							.c_str());
				}
			}

			unlink(
				(curpartdir + *curprob + config[*curprob]["execsuf"])
					.c_str());

			glblog += loclog;
			cout << loclog;
			loclog = "";

			partscore += probscore;
			loclog += string("\nscore: ") + itoa(probscore, buf, 10) + string("\n");

			glblog += loclog;
			cout << loclog;
			loclog = "";

			if (!nolog) {
				loclog += "log:\n";
				if (chdir(curpartdir.c_str()) != -1) {
					file.setfile(log);
					string logstr;
					if (file.readall(logstr))
						loclog += logstr;
					else
						loclog += "(cannot fetch log)\n";
				} else
					loclog += "(cannot fetch log)\n";
				if (chdir(orgwd) == -1) return false;
			}

			loclog += '\n';
			glblog += loclog;
			cout << loclog;
		}
		glblog += string("final: ") + itoa(partscore, buf, 10) + "\n\n";
		summary += "part: " + *curpart + "\nfinal: " + buf + "\n";
		cout << string("final: ") + buf + "\n\n";
		finalres.push_back(make_pair(partscore, (string)*curpart));

		if(logfile != "") {
			FILE *flog = fopen(logfile.c_str(), "a");
			if(flog != NULL) {
				fputs(glblog.c_str(), flog);
				fclose(flog);
				glblog = "";
			} else {
				perror("writing log to file");
			}
		}
	}
	free(orgwd);

	glblog += summary;
	cout << summary;

	sort(finalres.begin(), finalres.end(), greater<pair<int, string> >());
	cout << "\nsorted:\n";
	glblog += "\nsorted:\n";
	for (int i = 0; i < finalres.size(); i++) {
		char tmp[1000];
		sprintf(tmp, "%s %d\n", finalres[i].second.c_str(), finalres[i].first);
		cout << tmp;
		glblog += tmp;
	}

	if(logfile != "") {
		FILE *flog = fopen(logfile.c_str(), "a");
		if(flog != NULL) {
			fputs(glblog.c_str(), flog);
			fclose(flog);
		} else {
			perror("writing log to file");
		}
	}

	return true;
}
