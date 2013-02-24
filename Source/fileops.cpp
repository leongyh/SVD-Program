#include "fileops.h"
#include "getinp.h"

extern string path;

bool ifileopen(string& fname, const string& ext, ifstream& ifile) {
	string temp = path + fname + ext;
	ifile.open(temp.c_str(), ios::in);
	while(!ifile) {
		string mess = string('\n') + temp + " does not exist!\nTry again, or press RETURN to exit (no extension): ";
		if(!getinpnoerr(&fname, mess.c_str()))
			return false;
		
		
		temp = path + fname + ext;
		ifile.clear();
		ifile.open(temp.c_str());
	}
	return true;
}

bool ofileopen(const string& fname, const string& ext, ofstream& ofile, bool overwrite) {
	string temp = path + fname + ext;
	if(overwrite) ofile.open(temp.c_str());
	else {
		ofile.open(temp.c_str(), ios::out|ios::noreplace);
		if(!ofile) {
			string mess = string('\n') + temp + " exists! Overwrite (y/n): ";
			if(!ynquery(mess.c_str())) return false;
			ofile.clear();
			ofile.open(temp.c_str(), ios::out);
		}
	}
	return true;
}
		

bool existsfile(const char *v, const char *ext, const char *mess) {
	string s = string(v) + ext;
	ifstream ifile(s.c_str(), ios::in|ios::nocreate);
	if(!ifile) {
		cout << endl << v << ext << " does not exist." << mess << endl;
		return false;
	}

	return true;
}


