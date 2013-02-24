#include "ascspc.h"
#include <iostream.h>
#include "string"
#include <vector.h>
#include <fstream.h>
#include "cls.h"
#include "spectrum.h"
#include "getinp.h"
#include "fileops.h"

extern const string prmpt;

void ascspc(void) {
//	Translate files from ascii (x, y format) to spc
	
	cls();
	cout << endl << "Translate files from ASCII to SPC" << endl << endl;
	
	
	vector<string> fnames;
	//if(!getinpvecnoerr(&fnames, prmpt.c_str())) return;
	if(!getfiles(&fnames, prmpt.c_str(), ".asc")) return;

	for(size_t i=0; i<fnames.size(); i++) {
		ifstream ifile((fnames[i]+".asc").c_str());
		//if(!ifileopen(fnames[i], ".asc", ifile)) return;
		ofstream ofile;
		if(!ofileopen(fnames[i], ".spc", ofile)) return;

		cout << "Translating " << fnames[i] << endl;

		spectrum spec = readasc(ifile);

		writespc(spec, ofile);
	}
	return;
}

