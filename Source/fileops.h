#ifndef _FILEOPS_H
#define _FILEOPS_H

#include <fstream.h>
#include "string"
#include "getinp.h"

bool ifileopen(string&, const string&, ifstream&);
bool ofileopen(const string&, const string&, ofstream&, bool overwrite = false);

template <class T>
bool savedisk(const T& x, const char *p = "Enter filename (no extension): ") {
	bool r = false;
	if(ynquery("\nDo you wish to save the results to disk (y/n)? ")) {
		ofstream ofile;
		string f;
		if(getinpnoerr(&f, p)) {
			if(ofileopen(f, ".txt", ofile)) {
				ofile << x;
				r = true;
			}
		}
				
	}
	return r;
}

template <class T>
bool savedisk(const T& x, const string& p) {
	return savedisk<T>(x, p.c_str());
}


bool existsfile(const char *v, const char *ext, const char *mess);

inline bool existsfile(const vector<string>& v, const char *ext, const char *mess) {
	for(unsigned long i = 0; i < v.size(); i++) if(!existsfile(v[i].c_str(), ext, mess)) return false;
	return true;
}

inline bool getfiles(string *fname, const char *prompt, const char *ext) {
	do if(!getinpnoerr(fname, prompt)) return false;
	while(!existsfile((*fname).c_str(), ext, " Try again."));
	return true;
}

inline bool getfiles(vector<string> *fnames, const char *prompt, const char *ext) {
	do if(!getinpvecnoerr(fnames, prompt)) return false;
	while(!existsfile(*fnames, ext, " Try again."));
	return true;
}

#endif


